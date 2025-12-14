#include "test_scene.hpp"
#include "../../engine/object/object.hpp"
#include <algorithm>
#include <memory>
namespace game {

void TestScene::init(engine::core::Context &context) {
  engine::scene::Scene::init(context);
  auto obj = std::make_unique<engine::object::Object>("aa");
  obj->initTile(context, "../asset/trial.png", glm::vec2{512.0f, 360.0f});
  addObj(std::move(obj));

  obj = std::make_unique<engine::object::Object>("aa");
  obj->initTile(context, "../asset/trial.png", glm::vec2{100.0f, 100.0f});
  addObj(std::move(obj));

  obj = std::make_unique<engine::object::Object>("aa");
  obj->initTile(context, "../asset/trial.png", glm::vec2{700.0f, 700.0f});
  addObj(std::move(obj));
}

void TestScene::update(float dt, engine::core::Context &context) {
  engine::scene::Scene::update(dt, context);
}

void TestScene::render(engine::core::Context &context) {
  engine::scene::Scene::render(context);
  for (auto &obj : m_objs) {
    obj->render();
  }
}

void TestScene::event(engine::core::Context &context) {
  engine::scene::Scene::event(context);
}

} // namespace game
