#include "manager.hpp"
#include "../core/context.hpp"

namespace engine::scene {
void Manager::render() {
  if (!m_scenes.empty()) {
    for (const auto &scene : m_scenes) {
      scene->render(m_context);
    }
  }
}

void Manager::update(float dt) {
  if (!m_scenes.empty()) {
    auto &last_scene = m_scenes.back();
    last_scene->update(dt, m_context);
  }
  processPending();
}

void Manager::event() {
  // 只考虑顶层
  if (!m_scenes.empty()) {
    auto &last_scene = m_scenes.back();
    last_scene->event(m_context);
  }
}

void Manager::processPending() {
  if (!m_pending && (m_action == PendingActions::Push ||
                     m_action == PendingActions::Replace)) {
    spdlog::warn("尝试处理空场景");
    return;
  }
  switch (m_action) {
  case PendingActions::Pop:
    if (!m_scenes.empty()) {
      m_scenes.pop_back();
    } else {
      spdlog::warn("当前为空场景");
    }
    break;

  case PendingActions::Push:
    if (!m_pending->isInit())
      m_pending->init(m_context);
    spdlog::trace("压入场景{}", m_pending->getName());
    m_scenes.push_back(std::move(m_pending));
    break;

  case PendingActions::Replace: {
    spdlog::trace("用场景{}替换场景{}", m_pending->getName(),
                  m_scenes.back()->getName());
    m_scenes.clear();
    if (!m_pending->isInit())
      m_pending->init(m_context);

    m_scenes.push_back(std::move(m_pending));
  } break;

  case PendingActions::None:
  default:
    break;
  }
  m_action = PendingActions::None;
}

void Manager::push(std::unique_ptr<Scene> &&scene) {
  m_pending = std::move(scene);
  m_action = PendingActions::Push;
}

void Manager::replace(std::unique_ptr<Scene> &&scene) {
  m_pending = std::move(scene);
  m_action = PendingActions::Replace;
}

void Manager::pop() { m_action = PendingActions::Pop; }
} // namespace engine::scene
