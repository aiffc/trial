#pragma once

namespace engine::render {
class Renderer;
}

namespace engine::input {
class Manager;
}

namespace engine::resource {
class Manager;
}

namespace engine::core {
class Context {
private:
  engine::render::Renderer &m_renderer;
  engine::input::Manager &m_input_manager;
  engine::resource::Manager &m_resource_manager;

public:
  Context(engine::render::Renderer &renderer,
          engine::input::Manager &input_manager,
          engine::resource::Manager &resource_manager)
      : m_renderer(renderer), m_input_manager(input_manager),
        m_resource_manager(resource_manager) {}
  ~Context() = default;

  engine::render::Renderer &getRenderer() { return m_renderer; }
  engine::input::Manager &getInput() { return m_input_manager; }
  engine::resource::Manager &getResource() { return m_resource_manager; }

  Context(Context &) = delete;
  Context(Context &&) = delete;
  Context &operator=(Context &) = delete;
  Context &operator=(Context &&) = delete;
};
} // namespace engine::core
