#pragma once

#include "SDL3/SDL_gpu.h"
#include "spdlog/spdlog.h"
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
  SDL_GPUDevice *&mDevice;            /* gpu设备, 引用 */
  std::string mVertPath;              /* vert shader目录 */
  std::string mFragPath;              /* frag shader目录 */
  SDL_GPUTextureFormat mFormat;       /* 交换链图像格式 */

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

  GraphicsPipeline() = delete;
  /* 基类构造函数，子类可以用此构造函数进行构造 */
  explicit GraphicsPipeline(SDL_GPUDevice *&device, SDL_GPUTextureFormat format,
                            std::string_view vpath, std::string_view fpath)
      : mPipeline{nullptr}, mDevice{device}, mVertPath{vpath}, mFragPath{fpath},
        mFormat{format} {}
  /* 初始化函数，创建图形管线 */
  virtual void init() = 0;
  virtual ~GraphicsPipeline() {
    spdlog::trace("销毁图形管线");
    SDL_ReleaseGPUGraphicsPipeline(mDevice, mPipeline);
    mPipeline = nullptr;
  }
};

} // namespace Engine::Render
