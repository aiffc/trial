#pragma once

#include "SDL3/SDL_gpu.h"
#include <string>
#include <string_view>

namespace Engine::Render {

SDL_GPUShader *loadShader(SDL_GPUDevice *device, SDL_GPUShaderStage stage,
                          std::string_view file_name, uint32_t sample_count,
                          uint32_t uniform_buffer_count,
                          uint32_t storage_buffer_count,
                          uint32_t storage_texture_count);

class GraphicsPipeline {
public:
  SDL_GPUGraphicsPipeline *mPipeline;
  SDL_GPUDevice *mDevice;
  std::string mVertPath;
  std::string mFragPath;

public:
  SDL_GPUGraphicsPipeline *get() const { return mPipeline; }
  std::string getv() const { return mVertPath; }
  std::string getf() const { return mFragPath; }

  GraphicsPipeline(GraphicsPipeline &) = delete;
  GraphicsPipeline(GraphicsPipeline &&) = delete;
  GraphicsPipeline &operator=(GraphicsPipeline &) = delete;
  GraphicsPipeline &operator=(GraphicsPipeline &&) = delete;

  GraphicsPipeline() = default;
  virtual void create([[maybe_unused]] SDL_GPUDevice *mDevice) {}
  virtual void destroy([[maybe_unused]] SDL_GPUDevice *mDevice) {}
  virtual ~GraphicsPipeline() { mPipeline = nullptr, mDevice = nullptr; }
};

} // namespace Engine::Render
