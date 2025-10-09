#pragma once

#include "SDL3/SDL_gpu.h"
#include <string>
#include <string_view>

namespace Engine::Render {

/* 加载并创建shader */
SDL_GPUShader *loadShader(SDL_GPUDevice *device, SDL_GPUShaderStage stage,
                          std::string_view file_name, uint32_t sample_count,
                          uint32_t uniform_buffer_count,
                          uint32_t storage_buffer_count,
                          uint32_t storage_texture_count);

class GraphicsPipeline {
public:
  SDL_GPUGraphicsPipeline *mPipeline; /* 图形管线 */
  SDL_GPUDevice *mDevice;             /* gpu设备 */
  std::string mVertPath;              /* vert shader目录 */
  std::string mFragPath;              /* frag shader目录 */

public:
  /* 获取图形管线 */
  SDL_GPUGraphicsPipeline *get() const { return mPipeline; }
  /* 获取vert shader目录 */
  std::string getv() const { return mVertPath; }
  /* 获取frag shader目录 */
  std::string getf() const { return mFragPath; }

  GraphicsPipeline(GraphicsPipeline &) = delete;
  GraphicsPipeline(GraphicsPipeline &&) = delete;
  GraphicsPipeline &operator=(GraphicsPipeline &) = delete;
  GraphicsPipeline &operator=(GraphicsPipeline &&) = delete;

  GraphicsPipeline() = default;
  // virtual void create([[maybe_unused]] SDL_GPUDevice *mDevice) {}
  // virtual void destroy([[maybe_unused]] SDL_GPUDevice *mDevice) {}
  /* 初始化函数 */
  virtual void init() {}
  virtual ~GraphicsPipeline() { mPipeline = nullptr, mDevice = nullptr; }
};

} // namespace Engine::Render
