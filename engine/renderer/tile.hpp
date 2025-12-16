#pragma once

#include "../core/context.hpp"
#include "SDL3/SDL_gpu.h"
#include "glm/glm.hpp"
#include <glm/ext/vector_int2.hpp>
#include <locale>
#include <string_view>

namespace engine::render {

class Renderer;

struct RenderInfo {
  glm::vec2 window_size;
};

struct TileInfo {
  glm::vec2 pos;
  glm::vec2 size{200.0f, 200.0f};
  // TODO
  // glm::vec2 scale{1.0f, 1.0f};
  // float rotation{0.0f};
  // glm::vec2 center{0.0f, 0.0f};
  // bool flipv{false};
  // bool fliph{false};
};

class Tile final {
  friend class Renderer;

private:
  Renderer *m_owner{nullptr};
  TileInfo m_tile_info;
  SDL_GPUTexture *m_texture;
  bool m_init{false};

public:
  Tile(Renderer *renderer, const glm::vec2 &pos = {0.0f, 0.0f})
      : m_owner(renderer), m_tile_info{.pos = pos} {}
  ~Tile();

  void init(std::string_view texture_path);
  void render();

  void init(engine::core::Context &context, std::string_view texture_path);

  void move(const glm::vec2 &val) { m_tile_info.pos += val; }
  void setPos(const glm::vec2 &val) { m_tile_info.pos = val; }
  const glm::vec2 &getPos() const { return m_tile_info.pos; }
  void setSize(const glm::vec2 &val) { m_tile_info.size = val; }
  const glm::vec2 &getSize() const { return m_tile_info.size; }

  Tile(Tile &) = delete;
  Tile(Tile &&) = delete;
  Tile &operator=(Tile &) = delete;
  Tile &operator=(Tile &&) = delete;
};

} // namespace engine::render
