#pragma once

#include "../renderer/renderer.hpp"
#include "glm/glm.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace engine::resource {
class Texture final {
private:
  engine::render::Renderer &m_render;
  // TODO 封装成智能指针？
  std::unordered_map<std::string, SDL_GPUTexture *> m_map;

public:
  Texture(engine::render::Renderer &render);
  ~Texture();

  SDL_GPUTexture *loadOrGet(const std::string &);
  void remove(const std::string &);
  void clear();

  Texture(Texture &) = delete;
  Texture(Texture &&) = delete;
  Texture &operator=(Texture &) = delete;
  Texture &operator=(Texture &&) = delete;
};
} // namespace engine::resource
