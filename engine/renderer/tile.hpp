#pragma once

#include "glm/glm.hpp"
#include "SDL3/SDL_gpu.h"
#include <string_view>

namespace engine::render {

class Renderer;

  struct TileInfo{
    glm::vec2 pos;
  };

class Tile final {
  friend class Renderer;

private:
  Renderer* m_owner{nullptr};
  TileInfo m_tile_info;
  SDL_GPUTexture *m_texture;
  bool m_init{false};

public:
  Tile(Renderer *renderer, const glm::vec2 &pos = {0.0f, 0.0f}) : m_owner(renderer), m_tile_info{.pos = pos} {}
  ~Tile();

  void init(std::string_view texture_path);
  void render();

  Tile(Tile &) = delete;
  Tile(Tile &&) = delete;
  Tile& operator=(Tile &) = delete;
  Tile& operator=(Tile &&) = delete;
  };

    }
