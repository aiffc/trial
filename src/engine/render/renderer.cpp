#include "renderer.hpp"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_video.h"
#include "pipeline/triangle.hpp"
#include "spdlog/spdlog.h"
#include <stdexcept>
#include <string>

namespace Engine::Render {

Renderer::Renderer() = default;

Renderer::~Renderer() { deinit(); }

void Renderer::deinit() {
  spdlog::trace("销毁Renderer");

  // 先销毁管线
  for (auto &[key, value] : pipelines) {
    spdlog::trace("销毁{}", key.name());
    value.reset();
  }

  SDL_ReleaseWindowFromGPUDevice(mDevice, mWindow);
  if (mDevice) {
    spdlog::trace("销毁SDL GPU逻辑设备");
    SDL_DestroyGPUDevice(mDevice);
  }
  if (mWindow) {
    spdlog::trace("销毁SDL窗口");
    SDL_DestroyWindow(mWindow);
  }
}

void Renderer::init() {
  spdlog::trace("创建SDL GPU逻辑设备");
  mDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV |
                                    SDL_GPU_SHADERFORMAT_DXIL |
                                    SDL_GPU_SHADERFORMAT_MSL,
                                true, "vulkan");
  if (mDevice == nullptr) {
    throw std::runtime_error("SDL创建GPU逻辑设备失败" +
                             std::string(SDL_GetError()));
  }

  spdlog::trace("创建SDL窗口");
  mWindow = SDL_CreateWindow("triled", 1024, 720, SDL_WINDOW_RESIZABLE);
  if (mWindow == nullptr) {
    throw std::runtime_error("SDL创建窗口失败: " + std::string(SDL_GetError()));
  }

  spdlog::trace("窗口关联gpu");
  if (!SDL_ClaimWindowForGPUDevice(mDevice, mWindow)) {
    throw std::runtime_error("SDL逻辑设备绑定窗口失败: " +
                             std::string(SDL_GetError()));
  }
  addPipeline<TrianglePipeline>(
      mDevice, SDL_GetGPUSwapchainTextureFormat(mDevice, mWindow),
      "../shader/triangle/vert.spv", "../shader/triangle/frag.spv");
}

SDL_GPUDevice *Renderer::getDevice() const { return mDevice; }

SDL_Window *Renderer::getWindow() const { return mWindow; }

// SDL_GPUCommandBuffer *Renderer::getCmd() const { return mCmd; }

// SDL_GPUTexture *Renderer::getSwapchainTexture() const {
//   return mSwapchainTexture;
// }

// SDL_GPURenderPass *Renderer::getRenderPass() const { return mRenderPass; }

void Renderer::begin(float r, float g, float b, float a) {
  mCmd = SDL_AcquireGPUCommandBuffer(mDevice);
  if (mCmd == nullptr) {
    spdlog::error("请求命令缓冲失败");
    throw std::runtime_error("acquire cmd buffer failed");
  }

  if (!SDL_WaitAndAcquireGPUSwapchainTexture(mCmd, mWindow, &mSwapchainTexture,
                                             NULL, NULL)) {
    spdlog::error("请求交换链图像失败");
    throw std::runtime_error("acquire swapchain texture failed");
  }

  SDL_GPUColorTargetInfo color_info{
      .texture = mSwapchainTexture,
      .clear_color = SDL_FColor{r, g, b, a},
      .load_op = SDL_GPU_LOADOP_CLEAR,
      .store_op = SDL_GPU_STOREOP_STORE,
  };

  mRenderPass = SDL_BeginGPURenderPass(mCmd, &color_info, 1, NULL);
  if (mCmd == nullptr) {
    spdlog::error("请求RenderPass失败");
    throw std::runtime_error("begin render pass failed");
  }
}

void Renderer::end() {
  SDL_EndGPURenderPass(mRenderPass);
  SDL_SubmitGPUCommandBuffer(mCmd);
}

void Renderer::setViewport(float x, float y, float w, float h, float min_depth,
                           float max_depth) {
  SDL_GPUViewport view_port{
      .x = x,
      .y = y,
      .w = w,
      .h = h,
      .min_depth = min_depth,
      .max_depth = max_depth,
  };
  SDL_SetGPUViewport(mRenderPass, &view_port);
}

void Renderer::setScissor(int x, int y, int w, int h) {
  SDL_Rect scissor{
      .x = x,
      .y = y,
      .w = w,
      .h = h,
  };
  SDL_SetGPUScissor(mRenderPass, &scissor);
}

void Renderer::draw(uint32_t vertivces) {
  SDL_DrawGPUPrimitives(mRenderPass, vertivces, 1, 0, 0);
}

} // namespace Engine::Render
