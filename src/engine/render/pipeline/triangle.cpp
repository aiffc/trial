#include "triangle.hpp"
#include "SDL3/SDL_gpu.h"
#include "spdlog/spdlog.h"
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace Engine::Render {

TrianglePipeline::TrianglePipeline(SDL_GPUDevice *device,
                                   const SDL_GPUTextureFormat format,
                                   std::string_view vert_path,
                                   std::string_view frag_path)
    : mFormat{format} {
  mVertPath = vert_path;
  mFragPath = frag_path;
  mDevice = device;
  mPipeline = nullptr;
}

void TrianglePipeline::init() {
  spdlog::trace("创建三角形管线");
  SDL_GPUShader *vshader =
      loadShader(mDevice, SDL_GPU_SHADERSTAGE_VERTEX, getv(), 0, 0, 0, 0);
  SDL_GPUShader *fshader =
      loadShader(mDevice, SDL_GPU_SHADERSTAGE_FRAGMENT, getf(), 0, 0, 0, 0);

  std::vector<SDL_GPUColorTargetDescription> target_color{{
      .format = mFormat,
  }};

  SDL_GPUGraphicsPipelineCreateInfo info{
      .target_info =
          {
              .num_color_targets = static_cast<uint32_t>(target_color.size()),
              .color_target_descriptions = target_color.data(),
          },
      .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
      .vertex_shader = vshader,
      .fragment_shader = fshader,
      .rasterizer_state = {
          .fill_mode = SDL_GPU_FILLMODE_FILL,
      }};

  mPipeline = SDL_CreateGPUGraphicsPipeline(mDevice, &info);
  SDL_ReleaseGPUShader(mDevice, vshader);
  SDL_ReleaseGPUShader(mDevice, fshader);
  if (mPipeline == nullptr) {
    throw std::runtime_error("创建三角形管线失败");
  }
}

TrianglePipeline::~TrianglePipeline() {
  spdlog::trace("销毁三角形管线");
  SDL_ReleaseGPUGraphicsPipeline(mDevice, mPipeline);
  mPipeline = nullptr;
  mDevice = nullptr;
}

} // namespace Engine::Render
