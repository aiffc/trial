#include "render_object.hpp"
#include "renderer.hpp"
#include <spdlog/spdlog.h>
#include <string_view>

namespace engine::render {

template <typename T> RenderObject<T>::~RenderObject() {
  if (m_init && m_owner) {
    m_owner->destroyBuff(m_vertex_buff);
    m_owner->destroyBuff(m_index_buff);
    m_owner->destroySampler(m_sampler);
    m_owner->destroyTexture(m_texture);
    m_owner = nullptr;
  }
}

template <typename T>
void RenderObject<T>::init(std::string_view texture_path) {
  if (!m_owner) {
    spdlog::error("无法初始化无效的渲染对象");
    return;
  }

  // 无顶点数据直接返回,渲染对象无效
  if (m_vertex_data.empty()) {
    spdlog::error("无顶点数据无效渲染对象");
    return;
  }
  m_vertex_buff =
      m_owner->createBuff<T>(m_vertex_data, SDL_GPU_BUFFERUSAGE_VERTEX);
  if (!m_vertex_buff) {
    spdlog::error("无法初始化无效的渲染对象");
    return;
  }
  if (!m_index_data.empty()) {
    m_index_buff =
        m_owner->createBuff<uint32_t>(m_index_data, SDL_GPU_BUFFERUSAGE_INDEX);
    if (!m_index_buff) {
      spdlog::error("无法初始化无效的渲染对象");
      m_owner->destroyBuff(m_vertex_buff);
      return;
    }
  }
  // 创建对象贴图
  m_texture = m_owner->createTexture(texture_path);
  if (!m_texture) {
    spdlog::error("无法初始化无效的渲染对象");
    m_owner->destroyBuff(m_vertex_buff);
    if (m_index_buff) {
      m_owner->destroyBuff(m_index_buff);
    }
    return;
  }
  m_sampler = m_owner->createSampler();
  if (!m_sampler) {
    spdlog::error("无法初始化无效的渲染对象");
    m_owner->destroyBuff(m_vertex_buff);
    if (m_index_buff) {
      m_owner->destroyBuff(m_index_buff);
    }
    m_owner->destroyTexture(m_texture);
    return;
  }

  m_init = true;
}

template <typename T> void RenderObject<T>::render() {
  if (m_init) {
    if (m_texture && m_sampler && m_index_buff) {
      m_owner->drawBuff(m_vertex_buff, m_index_buff,
                        static_cast<uint32_t>(m_index_data.size()), m_texture,
                        m_sampler);
    } else if (m_index_buff) {
      m_owner->drawBuff(m_vertex_buff, m_index_buff,
                        static_cast<uint32_t>(m_index_data.size()));

    } else {
      m_owner->drawBuff(m_vertex_buff,
                        static_cast<uint32_t>(m_vertex_data.size()));
    }
  }
}
} // namespace engine::render
