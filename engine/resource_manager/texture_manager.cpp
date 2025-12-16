#include "texture_manager.hpp"
#include "SDL3/SDL_error.h"
#include "spdlog/spdlog.h"

namespace engine::resource {
Texture::Texture(engine::render::Renderer &render) : m_render(render) {
  spdlog::trace("贴图管理器初始化");
}
Texture::~Texture() {
  spdlog::trace("贴图管理器退出");
  // clear();
}

SDL_GPUTexture *Texture::loadOrGet(const std::string &file) {
  if (auto it = m_map.find(file); it != m_map.end()) {
    return it->second;
  }

  SDL_GPUTexture *raw_texture = m_render.createTexture(file);
  if (raw_texture == nullptr) {
    spdlog::error("获取贴图{}失败{}", file, SDL_GetError());
    return nullptr;
  }
  spdlog::trace("加载贴图{}", file);
  m_map.emplace(file, raw_texture);
  return raw_texture;
}

void Texture::remove(const std::string &file) {
  if (auto it = m_map.find(file); it != m_map.end()) {
    spdlog::trace("移除贴图{}", file);
    if (it->second) {
      m_render.destroyTexture(it->second);
      it->second = nullptr;
      m_map.erase(it);
    }
  }
}

void Texture::clear() {
  if (!m_map.empty()) {
    for (auto &[name, val] : m_map) {
      if (val) {
        m_render.destroyTexture(val);
        val = nullptr;
      }
    }
    m_map.clear();
  }
}

} // namespace engine::resource
