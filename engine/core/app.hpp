#pragma once

#include "SDL3/SDL_events.h"
#include <memory>

namespace engine::render {
class Renderer;
class Tile;
} // namespace engine::render

namespace engine::scene {
class Manager;
}

namespace engine::core {

class Time;
class Context;

/*
 * app累需要手动进行初始化和退出
 */
class App final {
private:
  std::unique_ptr<Time> m_time;
  std::unique_ptr<engine::render::Renderer> m_render;
  std::unique_ptr<Context> m_context;
  std::unique_ptr<engine::scene::Manager> m_scene_manager;

private:
  void initAppInfo();
  void initSDL();

private:
  std::unique_ptr<render::Tile> m_tile;
  void testInit();
  void testRender();

public:
  App();
  ~App();

  [[nodiscard]] bool init();
  void deinit();
  bool render();
  bool update();
  bool event(const SDL_Event *);

  App(App &) = delete;
  App(App &&) = delete;
  App &operator=(App &) = delete;
  App &operator=(App &&) = delete;
};

} // namespace engine::core
