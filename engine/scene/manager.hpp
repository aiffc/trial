#pragma once

#include "scene.hpp"
#include "spdlog/spdlog.h"
#include <memory>
#include <vector>

namespace engine::core {
class Context;
}

namespace engine::scene {
class Scene;

class Manager final {
private:
  enum class PendingActions {
    None,
    Push,
    Pop,
    Replace,
  };
  engine::core::Context &m_context;
  std::vector<std::unique_ptr<Scene>> m_scenes;
  std::unique_ptr<Scene> m_pending;
  PendingActions m_action{PendingActions::None};

private:
  void processPending();

public:
  Manager(engine::core::Context &context) : m_context{context} {
    spdlog::trace("初始化场景管理器");
  }
  ~Manager() = default;

  void render();
  void update(float);
  void event();

  void push(std::unique_ptr<Scene> &&);
  void replace(std::unique_ptr<Scene> &&);
  void pop();

  Manager(Manager &) = delete;
  Manager(Manager &&) = delete;
  Manager &operator=(Manager &) = delete;
  Manager &operator=(Manager &&) = delete;
};

} // namespace engine::scene
