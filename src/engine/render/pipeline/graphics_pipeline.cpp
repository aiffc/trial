#include "graphics_pipeline.hpp"
#include "SDL3/SDL_iostream.h"
#include "spdlog/spdlog.h"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string_view>

namespace Engine::Render {

SDL_GPUShader *loadShader(SDL_GPUDevice *device, SDL_GPUShaderStage stage,
                          std::string_view file_name, uint32_t sample_count,
                          uint32_t uniform_buffer_count,
                          uint32_t storage_buffer_count,
                          uint32_t storage_texture_count) {
  spdlog::trace("创建 shader {}", file_name);
  size_t code_size;
  auto path = std::filesystem::path(file_name);

  void *code = SDL_LoadFile(path.c_str(), &code_size);
  if (nullptr == code) {
    spdlog::error("加载shader文件{}失败", path.c_str());
    return nullptr;
  }

  SDL_GPUShaderCreateInfo info{
      .code = static_cast<uint8_t *>(code),
      .code_size = code_size,
      .entrypoint = "main",
      .format = SDL_GPU_SHADERFORMAT_SPIRV,
      .stage = stage,
      .num_samplers = sample_count,
      .num_storage_buffers = storage_buffer_count,
      .num_storage_textures = storage_texture_count,
      .num_uniform_buffers = uniform_buffer_count,
  };
  SDL_GPUShader *shader = SDL_CreateGPUShader(device, &info);
  if (nullptr == shader) {
    spdlog::error("创建shader {}失败", path.c_str());
    return nullptr;
  }
  return shader;
}
} // namespace Engine::Render
