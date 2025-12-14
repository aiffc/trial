#pragma once

#include "../core/context.hpp"
#include "../renderer/renderer.hpp"
#include "../renderer/tile.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace engine::object {

// TODO maybe final
class Object final {
private:
  std::string m_name;
  bool m_remove_flag;
  std::unique_ptr<engine::render::Tile> m_tile;

public:
  Object(std::string_view name) : m_name(name) {}
  ~Object() = default;

  bool needRemove() const { return m_remove_flag; }
  void setRemove(bool flag = true) { m_remove_flag = flag; }

  void setName(const std::string &name) { m_name = name; }
  const std::string &getName() const { return m_name; }

  template <typename... Args>
  void initTile(engine::core::Context &context, Args &&...args) {
    m_tile = context.getRenderer().createTile(std::forward<Args>(args)...);
  }

  void render() {
    if (m_tile)
      m_tile->render();
  }

  Object(Object &) = delete;
  Object(Object &&) = delete;
  Object &operator=(Object &) = delete;
  Object &operator=(Object &&) = delete;
};

} // namespace engine::object
