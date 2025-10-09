#pragma once

#include "SDL3/SDL_gpu.h"
#include "graphics_pipeline.hpp"
#include <string_view>

namespace Engine::Render {

class TrianglePipeline final : public GraphicsPipeline {
private:
  const SDL_GPUTextureFormat mFormat;

public:
  TrianglePipeline(SDL_GPUDevice *device, const SDL_GPUTextureFormat format,
                   std::string_view vert_path, std::string_view frag_path);
  ~TrianglePipeline();

  void init();

  TrianglePipeline(TrianglePipeline &) = delete;
  TrianglePipeline(TrianglePipeline &&) = delete;
  TrianglePipeline &operator=(TrianglePipeline &) = delete;
  TrianglePipeline &&operator=(TrianglePipeline &&) = delete;
};

} // namespace Engine::Render
