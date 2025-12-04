#pragma once

namespace engine::render {
  class Renderer;
}

namespace engine::core {
class Context {
private:
  engine::render::Renderer &m_renderer;

public:
  Context(engine::render::Renderer &renderer) : m_renderer(renderer) {}
  ~Context() = default;

  engine::render::Renderer& getRenderer() {return m_renderer;}

  Context(Context &) = delete;
  Context(Context &&) = delete;
  Context& operator=(Context &) = delete;
  Context& operator=(Context &&) = delete;
};
}
