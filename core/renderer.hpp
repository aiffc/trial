#pragma once

// #include "graphics_pipeline.hpp"
#include "pipelines/base.hpp"
#include "pipelines/vertex.hpp"
#include "spdlog/spdlog.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>
namespace engine::render {

struct RenderContext {
  SDL_GPUCommandBuffer *cmd;
  SDL_GPUTexture *swapchain_texture;
  SDL_GPURenderPass *render_pass;
};

struct BuffInfo {
  uint32_t size;
  uint32_t type_size;
  SDL_GPUBuffer *vbuff{nullptr};
  SDL_GPUBuffer *ibuff{nullptr};
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

  std::unordered_map<std::string, BuffInfo> m_vertex_buffs;

private:
  template <typename T>
  SDL_GPUBuffer *createBuff(size_t type_size,
                            const std::vector<T>& datas, SDL_GPUBufferUsageFlags usage) {
    SDL_GPUBufferCreateInfo vbuff_info{
        .usage = usage,
        .size = static_cast<uint32_t>(sizeof(T) * datas.size()),
        .props = 0,
    };
    SDL_GPUTransferBufferCreateInfo tbuff_info{
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = static_cast<uint32_t>(sizeof(T) * datas.size()),
        .props = 0,
    };
    SDL_GPUBuffer *buff = SDL_CreateGPUBuffer(m_device.get(), &vbuff_info);
    if (!buff) {
      spdlog::error("创建vertex buff失败");
      return nullptr;
    }
    SDL_GPUTransferBuffer *tbuff =
        SDL_CreateGPUTransferBuffer(m_device.get(), &tbuff_info);
    if (!tbuff) {
      spdlog::error("创建transfer buff失败");
      return nullptr;
    }

    void *data = SDL_MapGPUTransferBuffer(m_device.get(), tbuff, false);
    std::memcpy(data, datas.data(), sizeof(T) * datas.size());
    SDL_UnmapGPUTransferBuffer(m_device.get(), tbuff);

    SDL_GPUCommandBuffer *cmd =
        SDL_AcquireGPUCommandBuffer(m_device.get());
    if (!cmd) {
      spdlog::error("请求command buffer失败");
      return nullptr;
    }
    SDL_GPUCopyPass *cp = SDL_BeginGPUCopyPass(cmd);
    if (!cp) {
      spdlog::error("begin copy pass失败");
      return nullptr;
    }

    SDL_GPUTransferBufferLocation tbl {
      .transfer_buffer = tbuff,
      .offset = 0,
    };
    SDL_GPUBufferRegion br{
        .buffer = buff,
        .offset = 0,
        .size = static_cast<uint32_t>(sizeof(T) * datas.size()),
    };
    SDL_UploadToGPUBuffer(cp, &tbl, &br, false);
    SDL_EndGPUCopyPass(cp);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(m_device.get(), tbuff);
    return buff;
  }

public:
  Renderer() = default;
  ~Renderer() {
    removeAllVBuff();
    SDL_WaitForGPUSwapchain(m_device.get(), m_window.get());
    SDL_WaitForGPUIdle(m_device.get());
    m_pipelines.clear();
    SDL_ReleaseWindowFromGPUDevice(m_device.get(), m_window.get());
    m_window.reset();
    m_device.reset();
  }
  /*********************** pipeline ***********************/
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

  /*********************** buff ***********************/
  template <typename T>
  void addBuff(const std::string &key, const std::vector<T> &datas, const std::vector<uint32_t>& idatas = {}) {
    if (auto it = m_vertex_buffs.find(key); it != m_vertex_buffs.end()) {
      return;
    }
    SDL_GPUBuffer *vbuff =
      createBuff<T>(sizeof(T), datas, SDL_GPU_BUFFERUSAGE_VERTEX);
    SDL_GPUBuffer *ibuff = nullptr;
    if (!idatas.empty()) {
      ibuff = createBuff<uint32_t>(sizeof(uint32_t), idatas,
                         SDL_GPU_BUFFERUSAGE_INDEX);
    }
    m_vertex_buffs.emplace(key,
                           BuffInfo{
                               .size = static_cast<uint32_t>(datas.size()),
                               .type_size = static_cast<uint32_t>(sizeof(T)),
                               .vbuff = std::move(vbuff),
                               .ibuff = std::move(ibuff),
                           });
  }

  SDL_GPUBuffer *getVBuff(const std::string &key) {
    if (auto it = m_vertex_buffs.find(key); it != m_vertex_buffs.end()) {
      return it->second.vbuff;
    }
    return nullptr;
  }

  SDL_GPUBuffer *getIBuff(const std::string &key) {
    if (auto it = m_vertex_buffs.find(key); it != m_vertex_buffs.end()) {
      return it->second.ibuff;
    }
    return nullptr;
  }

  uint32_t getVertexSize(const std::string &key) {
    if (auto it = m_vertex_buffs.find(key); it != m_vertex_buffs.end()) {
      return it->second.size;
    }
    return 0;
  }

  void removeBuff(const std::string &key) {
    if (auto it = m_vertex_buffs.find(key); it != m_vertex_buffs.end()) {
      if (it->second.vbuff)
        SDL_ReleaseGPUBuffer(m_device.get(), it->second.vbuff);
      if (it->second.ibuff)
        SDL_ReleaseGPUBuffer(m_device.get(), it->second.ibuff);
      m_vertex_buffs.erase(it);
    }
  }

  void removeAllVBuff() {
    for (auto &[key, val] : m_vertex_buffs) {
      if (val.vbuff)
        SDL_ReleaseGPUBuffer(m_device.get(), val.vbuff);
      if (val.ibuff)
        SDL_ReleaseGPUBuffer(m_device.get(), val.ibuff);
    }
  }

  /*********************** renderer ***********************/
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
    addPipeline<VertexBuff>("../shaders/vertex_buff/vert.spv",
                            "../shaders/vertex_buff/frag.spv");
    return true;
  }

  bool begin(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) {
    m_context.cmd = nullptr;
    m_context.swapchain_texture = nullptr;
    m_context.render_pass = nullptr;

    if (!m_window) {
      return false;
    }

    m_context.cmd = SDL_AcquireGPUCommandBuffer(m_device.get());
    if (!m_context.cmd) {
      spdlog::error("请求命令失败{}", SDL_GetError());
      return false;
    }

    if (!SDL_WaitAndAcquireGPUSwapchainTexture(m_context.cmd, m_window.get(),
                                               &m_context.swapchain_texture,
                                               nullptr, nullptr)) {
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

  template <typename T> bool bindPipeline() {
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

  void drawBuff(const std::string &key) {
    if (!m_context.render_pass) {
      return;
    }
    SDL_GPUBuffer *vbuff = getVBuff(key);
    SDL_GPUBuffer *ibuff = getIBuff(key);
    if (!vbuff) {return ;}

    SDL_GPUBufferBinding vbind {
      .buffer = vbuff,
      .offset = 0,
    };
    SDL_BindGPUVertexBuffers(m_context.render_pass, 0, &vbind, 1);
    if (ibuff) {
      SDL_GPUBufferBinding ibind{
        .buffer = ibuff,
          .offset = 0,
      };
      SDL_BindGPUIndexBuffer(m_context.render_pass, &ibind,
                             SDL_GPU_INDEXELEMENTSIZE_32BIT);
    }
    SDL_DrawGPUPrimitives(m_context.render_pass, getVertexSize(key), 1, 0, 0);
  }

  Renderer(Renderer &) = delete;
  Renderer(Renderer &&) = delete;
  Renderer &operator=(Renderer &) = delete;
  Renderer &operator=(Renderer &&) = delete;
};

} // namespace engine::render
