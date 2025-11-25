#pragma once

#include "base.hpp"
#include <SDL3/SDL_gpu.h>
#include <spdlog/spdlog.h>

namespace engine::render {

class TrianglePipeline final : public BasePipeline {
  friend class Renderer;

private:
public:
  using BasePipeline::BasePipeline;
  ~TrianglePipeline() = default;

  virtual void init(const std::filesystem::path &vert,
                    const std::filesystem::path &frag) {
    if (!m_device) {
      spdlog::error("graphics pipeline初始化失败，device为空");
      return;
    }

    SDL_GPUShader *vert_shader = loadShader(m_device, vert, m_vert_config);
    SDL_GPUShader *frag_shader = loadShader(m_device, frag, m_frag_config);
    if (!vert_shader || !frag_shader) {
      spdlog::error("创建shader失败");
      return;
    }
    SDL_GPUColorTargetDescription color_target_desc{
        .format = SDL_GetGPUSwapchainTextureFormat(m_device, m_window),
        .blend_state =
            {
                .src_color_blendfactor = SDL_GPU_BLENDFACTOR_INVALID,
                .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_INVALID,
                .color_blend_op = SDL_GPU_BLENDOP_INVALID,
                .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_INVALID,
                .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_INVALID,
                .alpha_blend_op = SDL_GPU_BLENDOP_INVALID,
                .color_write_mask = 0,
                .enable_blend = false,
                .enable_color_write_mask = false,
                .padding1 = 0,
                .padding2 = 0,
            },
    };

    SDL_GPUGraphicsPipelineCreateInfo create_info{
      .vertex_shader = vert_shader, .fragment_shader = frag_shader,
      .vertex_input_state =
          {
              .vertex_buffer_descriptions = nullptr,
              .num_vertex_buffers = 0,
              .vertex_attributes = nullptr,
              .num_vertex_attributes = 0,

          },
      .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
      .rasterizer_state =
          {
              .fill_mode = SDL_GPU_FILLMODE_FILL,
              .cull_mode = SDL_GPU_CULLMODE_BACK,
              .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
              .depth_bias_constant_factor = 0.0f,
              .depth_bias_clamp = 0.0f,
              .depth_bias_slope_factor = 0.0f,
              .enable_depth_bias = false,
              .enable_depth_clip = false,
              .padding1 = 0,
              .padding2 = 0,
          },
      .multisample_state =
          {
              .sample_count = SDL_GPU_SAMPLECOUNT_1,
              .sample_mask = 0,
              .enable_mask = false,
              .padding1 = 0,
              .padding2 = 0,
              .padding3 = 0,
          },
      .depth_stencil_state =
          {
              .compare_op = SDL_GPU_COMPAREOP_INVALID,
              .back_stencil_state =
                  {
                      .fail_op = SDL_GPU_STENCILOP_INVALID,
                      .pass_op = SDL_GPU_STENCILOP_INVALID,
                      .depth_fail_op = SDL_GPU_STENCILOP_INVALID,
                      .compare_op = SDL_GPU_COMPAREOP_INVALID,
                  },
              .front_stencil_state =
                  {
                      .fail_op = SDL_GPU_STENCILOP_INVALID,
                      .pass_op = SDL_GPU_STENCILOP_INVALID,
                      .depth_fail_op = SDL_GPU_STENCILOP_INVALID,
                      .compare_op = SDL_GPU_COMPAREOP_INVALID,
                  },
              .compare_mask = 0,
              .write_mask = 0,
              .enable_depth_test = false,
              .enable_depth_write = false,
              .enable_stencil_test = false,
              .padding1 = 0,
              .padding2 = 0,
              .padding3 = 0,
          },
      .target_info =
          {
              .color_target_descriptions = &color_target_desc,
              .num_color_targets = 1,
              .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_R8_SNORM,
              .has_depth_stencil_target = false,
              .padding1 = 0,
              .padding2 = 0,
              .padding3 = 0,
          },
      .props = 0,

    };
    m_pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &create_info);
    SDL_ReleaseGPUShader(m_device, vert_shader);
    SDL_ReleaseGPUShader(m_device, frag_shader);
  }

  TrianglePipeline(TrianglePipeline &) = delete;
  TrianglePipeline(TrianglePipeline &&) = delete;
  TrianglePipeline &operator=(TrianglePipeline &) = delete;
  TrianglePipeline &operator=(TrianglePipeline &&) = delete;
};

} // namespace engine::render
