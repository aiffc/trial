#pragma once

#include "../../engine/scene/scene.hpp"

namespace game {

class TestScene final : public engine::scene::Scene {
public:
  using engine::scene::Scene::Scene;
  ~TestScene() override = default;

  void init(engine::core::Context &) override;
  void update(float, engine::core::Context &) override;
  void render(engine::core::Context &) override;
  void event(engine::core::Context &) override;
};
} // namespace game
