#include "scene.hpp"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <memory>

namespace engine::scene {
void Scene::processPending() {
  if (!m_pending.empty()) {
    for (auto &obj : m_pending) {
      m_objs.push_back(std::move(obj));
    }
    m_pending.clear();
  }
}

void Scene::addObj(std::unique_ptr<engine::object::Object> &&obj) {
  if (obj) {
    m_pending.push_back(std::move(obj));
  } else {
    spdlog::warn("尝试添加空对象");
  }
}

void Scene::removeObj(engine::object::Object *obj) {
  if (obj)
    obj->setRemove();
}

engine::object::Object *Scene::getObjByName(const std::string &name) const {
  auto it =
      std::find_if(m_objs.begin(), m_objs.end(),
                   [name](const std::unique_ptr<engine::object::Object> &obj) {
                     return name == obj->getName();
                   });
  if (it != m_objs.end()) {
    return (*it).get();
  }
  return nullptr;
}

void Scene::removeObjByName(const std::string &name) {
  auto obj = getObjByName(name);
  if (obj)
    removeObj(obj);
}

void Scene::init(engine::core::Context &) {
  m_init = true;
  spdlog::trace("场景{}初始化成功", m_name);
}

void Scene::update(float dt [[maybe_unused]],
                   engine::core::Context &context [[maybe_unused]]) {
  // 调用对象跟新
  for (auto it = m_objs.begin(); it != m_objs.end();) {
    if (*it) {
      // 安全的删除对象
      if ((*it)->needRemove()) {
        it = m_objs.erase(it);
      } else {
        //  (*it)->update(dt);
        it++;
      }
    }
  }
  processPending();
}

void Scene::render(engine::core::Context &context [[maybe_unused]]) {
  // 调用对象渲染
  for (const auto &obj : m_objs) {
    obj->render();
  }
}

void Scene::event(engine::core::Context &context [[maybe_unused]]) {}
} // namespace engine::scene
