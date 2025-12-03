#pragma once

// #include "graphics_pipeline.hpp"
#include "SDL3_image/SDL_image.h"
#include "pipelines/tile.hpp"
#include "spdlog/spdlog.h"
#include "tile.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace engine::render {

template <typename T> class RenderObject;

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

  // 2d 渲染
  SDL_GPUBuffer *m_vertex_buffer{nullptr};
  SDL_GPUBuffer *m_index_buffer{nullptr};
  SDL_GPUSampler *m_sampler{nullptr};

private:
  template <typename T>
  [[nodiscard]] SDL_GPUBuffer *createBuff(const std::vector<T> &datas,
                                          SDL_GPUBufferUsageFlags usage) {
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

    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(m_device.get());
    if (!cmd) {
      spdlog::error("请求command buffer失败");
      return nullptr;
    }
    SDL_GPUCopyPass *cp = SDL_BeginGPUCopyPass(cmd);
    if (!cp) {
      spdlog::error("begin copy pass失败");
      return nullptr;
    }

    SDL_GPUTransferBufferLocation tbl{
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

  void destroyBuff(SDL_GPUBuffer *buff) {
    if (buff) {
      SDL_ReleaseGPUBuffer(m_device.get(), buff);
      buff = nullptr;
    }
  }

  // TODO 定制采样器
  [[nodiscard]] SDL_GPUSampler *createSampler() {
    SDL_GPUSamplerCreateInfo sampler_create_info{
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .mip_lod_bias = 0.0f,
        .max_anisotropy = 0.0f,
        .compare_op = SDL_GPU_COMPAREOP_GREATER_OR_EQUAL,
        .min_lod = 0.0f,
        .max_lod = 0.0f,
        .enable_anisotropy = false,
        .enable_compare = false,
        .padding1 = 0,
        .padding2 = 0,
        .props = 0,
    };
    SDL_GPUSampler *sampler =
        SDL_CreateGPUSampler(m_device.get(), &sampler_create_info);
    return sampler;
  }

  void destroySampler(SDL_GPUSampler *sampler) {
    if (sampler) {
      SDL_ReleaseGPUSampler(m_device.get(), sampler);
      sampler = nullptr;
    }
  }

public:
  Renderer() = default;
  ~Renderer() {
    destroySampler(m_sampler);
    destroyBuff(m_vertex_buffer);
    destroyBuff(m_index_buffer);
    SDL_WaitForGPUSwapchain(m_device.get(), m_window.get());
    SDL_WaitForGPUIdle(m_device.get());
    m_pipelines.clear();
    SDL_ReleaseWindowFromGPUDevice(m_device.get(), m_window.get());
    m_window.reset();
    m_device.reset();
  }

  [[nodiscard]] SDL_GPUTexture *createTexture(std::string_view path) {
    SDL_Surface *surface = IMG_Load(path.data());
    if (!surface) {
      spdlog::error("create texture失败 {}", SDL_GetError());
      return nullptr;
    }
    // 转换到rgba格式
    SDL_Surface *usurface =
        SDL_ConvertSurface(surface, SDL_PIXELFORMAT_ABGR8888);
    SDL_DestroySurface(surface);

    SDL_GPUTextureCreateInfo create_info{
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = static_cast<uint32_t>(usurface->w),
        .height = static_cast<uint32_t>(usurface->h),
        .layer_count_or_depth = 1,
        .num_levels = 1,
        .sample_count = SDL_GPU_SAMPLECOUNT_1,
        .props = 0,
    };
    SDL_GPUTexture *texture =
        SDL_CreateGPUTexture(m_device.get(), &create_info);
    if (!texture) {
      SDL_DestroySurface(usurface);
      spdlog::error("create texture失败 {}", SDL_GetError());
      return nullptr;
    }

    // 创建transfer buffer将surface数据拷贝到transfer buffer
    SDL_GPUTransferBufferCreateInfo transfer_info{
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = static_cast<uint32_t>(usurface->w * usurface->h * 4),
        .props = 0,
    };
    SDL_GPUTransferBuffer *transfer_buff =
        SDL_CreateGPUTransferBuffer(m_device.get(), &transfer_info);
    if (!transfer_buff) {
      SDL_ReleaseGPUTexture(m_device.get(), texture);
      SDL_DestroySurface(usurface);
      spdlog::error("create texture失败 {}", SDL_GetError());
      return nullptr;
    }

    void *ptr = SDL_MapGPUTransferBuffer(m_device.get(), transfer_buff, false);
    std::memcpy(ptr, usurface->pixels, usurface->w * usurface->h * 4);
    SDL_UnmapGPUTransferBuffer(m_device.get(), transfer_buff);

    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(m_device.get());
    if (!cmd) {
      SDL_ReleaseGPUTexture(m_device.get(), texture);
      SDL_DestroySurface(usurface);
      SDL_ReleaseGPUTransferBuffer(m_device.get(), transfer_buff);
      return nullptr;
    }
    SDL_GPUCopyPass *cp = SDL_BeginGPUCopyPass(cmd);
    if (!cp) {
      SDL_ReleaseGPUTexture(m_device.get(), texture);
      SDL_DestroySurface(usurface);
      SDL_ReleaseGPUTransferBuffer(m_device.get(), transfer_buff);
      spdlog::error("create texture失败 {}", SDL_GetError());
      return nullptr;
    }

    // 将transfer buffer中的数据拷贝到gpu texture
    SDL_GPUTextureTransferInfo texture_transfer_info{
        .transfer_buffer = transfer_buff,
        .offset = 0,
        .pixels_per_row = 0,
        .rows_per_layer = 0,
    };

    SDL_GPUTextureRegion region{
        .texture = texture,
        .mip_level = 0,
        .layer = 0,
        .x = 0,
        .y = 0,
        .z = 0,
        .w = static_cast<uint32_t>(usurface->w),
        .h = static_cast<uint32_t>(usurface->h),
        .d = 1,
    };
    SDL_UploadToGPUTexture(cp, &texture_transfer_info, &region, false);
    SDL_EndGPUCopyPass(cp);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(m_device.get(), transfer_buff);
    SDL_DestroySurface(usurface);
    return texture;
  }

  void destroyTexture(SDL_GPUTexture *texture) {
    if (texture) {
      SDL_ReleaseGPUTexture(m_device.get(), texture);
      texture = nullptr;
    }
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

    std::vector<engine::render::VertexInput> vertex_datas{
        {{-0.1f, -0.1f}, {0.0f, 1.0f}},
        {{0.1f, -0.1f}, {1.0f, 1.0f}},
        {{0.1f, 0.1f}, {1.0f, 0.0f}},
        {{-0.1f, 0.1f}, {0.0f, 0.0f}},
    };
    std::vector<uint32_t> index_datas{0, 1, 2, 2, 3, 0};

    m_vertex_buffer = createBuff<engine::render::VertexInput>(
        vertex_datas, SDL_GPU_BUFFERUSAGE_VERTEX);
    m_index_buffer =
        createBuff<uint32_t>(index_datas, SDL_GPU_BUFFERUSAGE_INDEX);
    m_sampler = createSampler();

    // 瓦片渲染管线
    addPipeline<TilePipeline>("../shaders/tile/vert.spv",
                              "../shaders/tile/frag.spv");
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

  template <typename T> bool bindPipeline() {
    static_assert(std::is_base_of<BasePipeline, T>::value,
                  "T类型必须继承自BasePipeline");
    std::type_index ti{typeid(T)};
    auto it = m_pipelines.find(ti);
    if (it != m_pipelines.end() && it->second->get() && m_context.render_pass) {
      SDL_BindGPUGraphicsPipeline(m_context.render_pass, it->second->get());
      // bind vertex input
      if (m_vertex_buffer) {
        SDL_GPUBufferBinding vbind{
            .buffer = m_vertex_buffer,
            .offset = 0,
        };
        SDL_BindGPUVertexBuffers(m_context.render_pass, 0, &vbind, 1);
      }
      if (m_index_buffer) {
        SDL_GPUBufferBinding ibind{
            .buffer = m_index_buffer,
            .offset = 0,
        };
        SDL_BindGPUIndexBuffer(m_context.render_pass, &ibind,
                               SDL_GPU_INDEXELEMENTSIZE_32BIT);
      }
      return true;
    }
    return false;
  }

  template <typename T> void pushVertexUniform(const T &val) {
    if (m_context.cmd) {
      SDL_PushGPUVertexUniformData(m_context.cmd, 0, &val, sizeof(T));
    }
  }

  void bindTexture(SDL_GPUTexture *texture) {
    if (texture && m_sampler && m_context.render_pass) {
      SDL_GPUTextureSamplerBinding texture_binding{.texture = texture,
                                                   .sampler = m_sampler};
      SDL_BindGPUFragmentSamplers(m_context.render_pass, 0, &texture_binding,
                                  1);
    }
  }

  void draw() {
    if (m_context.render_pass) {
      SDL_DrawGPUIndexedPrimitives(m_context.render_pass, 6, 1, 0, 0, 0);
    }
  }

  template <typename... Args>
  std::unique_ptr<Tile> createTile(std::string_view path, Args &&...args) {
    auto ret = std::make_unique<Tile>(this, std::forward<Args>(args)...);
    ret->init(path);
    return ret;
  }

  Renderer(Renderer &) = delete;
  Renderer(Renderer &&) = delete;
  Renderer &operator=(Renderer &) = delete;
  Renderer &operator=(Renderer &&) = delete;
};

} // namespace engine::render
