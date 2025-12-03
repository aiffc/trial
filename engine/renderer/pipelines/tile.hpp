#pragma once

#include "base.hpp"
#include "glm/glm.hpp"
#include <SDL3/SDL_gpu.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace engine::render {

class TilePipeline final : public BasePipeline {
  friend class Renderer;

public:
  using BasePipeline::BasePipeline;
  ~TilePipeline() override = default;

  void init(const std::filesystem::path &vert,
            const std::filesystem::path &frag) override {
    if (!m_device) {
      spdlog::error("graphics pipeline初始化失败，device为空");
      return;
    }

    m_vert_config.uniform_buff_count = 2;
    m_frag_config.sample_count = 1;
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

    std::array<SDL_GPUVertexAttribute, 2> vattribute{};
    vattribute[0].buffer_slot = 0;
    vattribute[0].location = 0;
    vattribute[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vattribute[0].offset = offsetof(VertexInput, vertex_pos);

    vattribute[1].buffer_slot = 0;
    vattribute[1].location = 1;
    vattribute[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vattribute[1].offset = offsetof(VertexInput, texture_coord);

    std::vector<SDL_GPUVertexBufferDescription> vdescription{{
        .slot = 0,
        .pitch = sizeof(VertexInput),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0,

    }};
    SDL_GPUGraphicsPipelineCreateInfo create_info{
        .vertex_shader = vert_shader,
        .fragment_shader = frag_shader,
        .vertex_input_state =
            {
                .vertex_buffer_descriptions = vdescription.data(),
                .num_vertex_buffers =
                    static_cast<uint32_t>(vdescription.size()),
                .vertex_attributes = vattribute.data(),
                .num_vertex_attributes =
                    static_cast<uint32_t>(vattribute.size()),

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

  TilePipeline(TilePipeline &) = delete;
  TilePipeline(TilePipeline &&) = delete;
  TilePipeline &operator=(TilePipeline &) = delete;
  TilePipeline &operator=(TilePipeline &&) = delete;
};

} // namespace engine::render
