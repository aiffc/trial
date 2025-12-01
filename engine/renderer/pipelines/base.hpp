#pragma once
#include <SDL3/SDL_error.h>
#include <iostream>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <spdlog/spdlog.h>

namespace engine::render {

struct ShaderConfig {
  uint32_t sample_count{0};
  uint32_t uniform_buff_count{0};
  uint32_t storage_buff_count{0};
  uint32_t storage_texture_count{0};
  SDL_GPUShaderStage shader_stage{SDL_GPU_SHADERSTAGE_VERTEX};
};

static SDL_GPUShader *loadShader(SDL_GPUDevice *device,
                                 const std::filesystem::path &file,
                                 const ShaderConfig &config) {
  size_t code_size;
  void *code = SDL_LoadFile(file.string().data(), &code_size);
  if (!code) {
    spdlog::error("加载shader文件 {}失败 {}", file.string(), SDL_GetError());
    return nullptr;
  }
  SDL_GPUShaderCreateInfo create_info{
      .code_size = code_size,
      .code = static_cast<uint8_t *>(code),
      .entrypoint = "main",
      .format = SDL_GPU_SHADERFORMAT_SPIRV, // 只考虑用spirv
      .stage = config.shader_stage,
      .num_samplers = config.sample_count,
      .num_storage_textures = config.storage_texture_count,
      .num_storage_buffers = config.storage_buff_count,
      .num_uniform_buffers = config.uniform_buff_count,
      .props = 0,
  };
  SDL_GPUShader *shader = SDL_CreateGPUShader(device, &create_info);
  if (!shader) {
    spdlog::error("加载shader文件 {}失败 {}", file.string(), SDL_GetError());
    SDL_free(code);
    return nullptr;
  }

  SDL_free(code);
  return shader;
}

class BasePipeline {
  friend class Renderer;
protected:
  SDL_GPUDevice *m_device;
  SDL_Window *m_window;
  SDL_GPUGraphicsPipeline *m_pipeline;

  // shader配置
  ShaderConfig m_vert_config{.shader_stage = SDL_GPU_SHADERSTAGE_VERTEX};
  ShaderConfig m_frag_config{.shader_stage = SDL_GPU_SHADERSTAGE_FRAGMENT};

public:
  BasePipeline(SDL_GPUDevice *device, SDL_Window* window) : m_device{device}, m_window{window} {}
  virtual ~BasePipeline() {
    if (m_pipeline && m_device) {
      SDL_ReleaseGPUGraphicsPipeline(m_device, m_pipeline);
      m_pipeline = nullptr;
    }
  }

  virtual void init(const std::filesystem::path &vert,
                    const std::filesystem::path &frag) = 0;

  SDL_GPUGraphicsPipeline *get() const { return m_pipeline; }

  BasePipeline(BasePipeline &) = delete;
  BasePipeline(BasePipeline &&) = delete;
  BasePipeline &operator=(BasePipeline &) = delete;
  BasePipeline &operator=(BasePipeline &&) = delete;
};

} // namespace engine::render
