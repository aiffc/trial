#include "tile.hpp"
#include "spdlog/spdlog.h"
#include "renderer.hpp"

namespace engine::render {
  Tile::~Tile() {
    if (m_init && m_texture && m_owner) {
      m_owner->destroyTexture(m_texture);
    }
  }

  void Tile::init(std::string_view texture_path) {
    if (m_owner) {
      m_texture =  m_owner->createTexture(texture_path);
      if (!m_texture) {
        spdlog::error("创建gpu texture失败");
        return;
      }
      m_init = true;
    }
  }

  void Tile::render() {
    if (m_init) {
      m_owner->bindPipeline<TilePipeline>();
      // TODO 每次都要获取？
      RenderInfo rinfo { m_owner->getWindowSize() };
      m_owner->pushVertexUniform<RenderInfo>(rinfo);
      m_owner->pushVertexUniform<TileInfo>(m_tile_info);
      m_owner->bindTexture(m_texture);
      m_owner->draw();
    }
  }
}
