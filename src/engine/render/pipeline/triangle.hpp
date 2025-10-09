#pragma once

#include "graphics_pipeline.hpp"

namespace Engine::Render {

class TrianglePipeline final : public GraphicsPipeline {
public:
  /* 构造函数 */
  using GraphicsPipeline::GraphicsPipeline;
  /* 析构函数 */
  ~TrianglePipeline() override;

  void init() override;

  TrianglePipeline(TrianglePipeline &) = delete;
  TrianglePipeline(TrianglePipeline &&) = delete;
  TrianglePipeline &operator=(TrianglePipeline &) = delete;
  TrianglePipeline &&operator=(TrianglePipeline &&) = delete;
};

} // namespace Engine::Render
