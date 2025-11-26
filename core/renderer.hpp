#pragma once

// #include "graphics_pipeline.hpp"
#include "pipelines/base.hpp"
#include "pipelines/triangle.hpp"
#include "spdlog/spdlog.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>
#include <algorithm>
#include <complex>
#include <iostream>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
namespace engine::render {

struct RenderContext {
  SDL_GPUCommandBuffer *cmd;
  SDL_GPUTexture *swapchain_texture;
  SDL_GPURenderPass *render_pass;
};

class Renderer final {
private:
  struct DeviceDeleter {
    void operator()(SDL_GPUDevice *device) {
      if (device) {
        SDL_DestroyGPUDevice(device);
        device = nullptr;
      }
    }
  };
  std::unique_ptr<SDL_GPUDevice, DeviceDeleter> m_device;

  struct WindowDelter {
    void operator()(SDL_Window *window) {
      if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
      }
    }
  };
  std::unique_ptr<SDL_Window, WindowDelter> m_window;

  RenderContext m_context;

  std::unordered_map<std::type_index, std::unique_ptr<BasePipeline>>
      m_pipelines;

public:
  Renderer() = default;
  ~Renderer() {
    SDL_WaitForGPUSwapchain(m_device.get(), m_window.get());
    SDL_WaitForGPUIdle(m_device.get());
    m_pipelines.clear();
    SDL_ReleaseWindowFromGPUDevice(m_device.get(), m_window.get());
    m_window.reset();
    m_device.reset();
  }

  template <typename T>
  T *addPipeline(const std::filesystem::path &vert,
                 const std::filesystem::path &frag) {
    static_assert(std::is_base_of<BasePipeline, T>::value,
                  "T类型必须继承自BasePipeline");
    std::type_index ti{typeid(T)};
    if (m_pipelines.contains(ti)) {
      T *ret = static_cast<T *>(m_pipelines.find(ti)->second.get());
      return ret;
    }
    auto new_pipeline = std::make_unique<T>(m_device.get(), m_window.get());
    new_pipeline->init(vert, frag);
    T *ptr = new_pipeline.get();
    m_pipelines.emplace(ti, std::move(new_pipeline));
    return ptr;
  }

  template <typename T> void removePipeline() {
    static_assert(std::is_base_of<BasePipeline, T>::value,
                  "T类型必须继承自BasePipeline");
    std::type_index ti{typeid(T)};
    auto it = m_pipelines.find(ti);
    if (it != m_pipelines.end()) {
      m_pipelines.erase(it);
    }
  }

  template <typename T> T *getPipeline() {
    static_assert(std::is_base_of<BasePipeline, T>::value,
                  "T类型必须继承自BasePipeline");
    std::type_index ti{typeid(T)};
    if (m_pipelines.contains(ti)) {
      return static_cast<T *>(m_pipelines.find(ti)->second.get());
    }
    return nullptr;
  }

  bool init() {
    auto *device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL, true, "vulkan");
    if (!device) {
      spdlog::error("创建gpu设备失败 {}", SDL_GetError());
      return false;
    }
    // TODO 手动设置窗口大小
    auto *window = SDL_CreateWindow("trial", 1024, 720, SDL_WINDOW_RESIZABLE);
    if (!window) {
      spdlog::error("创建SDL窗口失败{}", SDL_GetError());
      return false;
    }
    if (!SDL_ClaimWindowForGPUDevice(device, window)) {
      spdlog::error("窗口关联gpu失败{}", SDL_GetError());
      return false;
    }
    m_window = std::unique_ptr<SDL_Window, WindowDelter>(window);
    m_device = std::unique_ptr<SDL_GPUDevice, DeviceDeleter>(device);
    addPipeline<TrianglePipeline>("../shaders/triangle/vert.spv",
                                  "../shaders/triangle/frag.spv");
    return true;
  }

  bool begin(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) {
    m_context.cmd = nullptr;
    m_context.swapchain_texture = nullptr;
    m_context.render_pass = nullptr;

    if (!m_window) {
      return  false;
    }

    m_context.cmd = SDL_AcquireGPUCommandBuffer(m_device.get());
    if (!m_context.cmd) {
      spdlog::error("请求命令失败{}", SDL_GetError());
      return false;
    }

    if (!SDL_WaitAndAcquireGPUSwapchainTexture(m_context.cmd, m_window.get(),
                                                &m_context.swapchain_texture, nullptr,
                                                nullptr)) {
      spdlog::error("请求交换链图像失败{}", SDL_GetError());
      return false;
    }
    if (!m_context.swapchain_texture) {
      spdlog::error("请求交换链图像失败2{}", SDL_GetError());
      return false;
    }

    SDL_GPUColorTargetInfo info{
        .texture = m_context.swapchain_texture,
        .mip_level = 0,
        .layer_or_depth_plane = 0,
        .clear_color = {r, g, b, a},
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
        .resolve_texture = nullptr,
        .resolve_mip_level = 0,
        .resolve_layer = 0,
        .cycle = false,
        .cycle_resolve_texture = false,
        .padding1 = 0,
        .padding2 = 0,
    };
    m_context.render_pass =
        SDL_BeginGPURenderPass(m_context.cmd, &info, 1, nullptr);
    if (!m_context.render_pass) {
      spdlog::error("render失败{}", SDL_GetError());
      return false;
    }
    return true;
  }

  void end() {
    if (m_context.render_pass && m_context.cmd) {
      SDL_EndGPURenderPass(m_context.render_pass);
      SDL_SubmitGPUCommandBuffer(m_context.cmd);
    }
  }

  void drawPrimitives() {
    if (m_context.render_pass) {
      SDL_DrawGPUPrimitives(m_context.render_pass, 3, 1, 0, 0);
    }
  }

  template <typename T> bool bind() {
    static_assert(std::is_base_of<BasePipeline, T>::value,
                  "T类型必须继承自BasePipeline");
    std::type_index ti{typeid(T)};
    auto it = m_pipelines.find(ti);
    if (it != m_pipelines.end() && it->second->get() && m_context.render_pass) {
      SDL_BindGPUGraphicsPipeline(m_context.render_pass, it->second->get());
      return true;
    }
    return false;
  }

  Renderer(Renderer &) = delete;
  Renderer(Renderer &&) = delete;
  Renderer &operator=(Renderer &) = delete;
  Renderer &operator=(Renderer &&) = delete;
};

} // namespace engine::render
