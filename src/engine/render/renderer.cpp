#include "renderer.hpp"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_video.h"
#include "spdlog/spdlog.h"
#include <stdexcept>
#include <string>

namespace Engine::Render {

Renderer::Renderer() = default;

Renderer::~Renderer() { deinit(); }

void Renderer::deinit() {
  spdlog::trace("销毁Renderer");
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
                                true, NULL);
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
}

SDL_GPUDevice *Renderer::getDevice() const { return mDevice; }
SDL_Window *Renderer::getWindow() const { return mWindow; }
} // namespace Engine::Render
