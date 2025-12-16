#include "app.hpp"
#include "../input/input.hpp"
#include "../renderer/renderer.hpp"
#include "../resource_manager/resource_manager.hpp"
#include "../scene/manager.hpp"
#include "SDL3/SDL.h"
#include "context.hpp"
#include "spdlog/spdlog.h"
#include "time.hpp"
#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

namespace engine::core {

App::App() = default;
App::~App() = default;

void App::initAppInfo() {
  if (!SDL_SetAppMetadata("trial", "0.1", "trial")) {
    spdlog::error("设置app元数据失败{}", SDL_GetError());
    throw std::runtime_error("设置app元数据失败");
  }
  std::unordered_map<std::string, std::string> meta_data{
      {SDL_PROP_APP_METADATA_URL_STRING, "None"},
      {SDL_PROP_APP_METADATA_CREATOR_STRING, "AIF TEAM"},
      {SDL_PROP_APP_METADATA_COPYRIGHT_STRING, "AIF TEAM"},
      {SDL_PROP_APP_METADATA_TYPE_STRING, "TRIAL"}};
  for (const auto &[key, val] : meta_data) {
    if (!SDL_SetAppMetadataProperty(key.data(), val.data())) {
      spdlog::error("设置app元数据属性失败{}", SDL_GetError());
      throw std::runtime_error("设置app元数据属性失败");
    }
  }
}

void App::initSDL() {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
    spdlog::error("SDL初始化失败{}", SDL_GetError());
    throw std::runtime_error("SDL初始化失败");
  }
}

bool App::init() {
  try {
    initAppInfo();
    initSDL();

    // 初始化渲染器
    m_render = std::make_unique<engine::render::Renderer>();
    if (!m_render->init()) {
      return false;
    }
    // 初始化时间管理器
    m_time = std::make_unique<Time>(144);
    m_time->init();
    // 初始化输入管理
    m_input_manager = std::make_unique<engine::input::Manager>();
    // 初始化资源管理器
    m_resource_manager = std::make_unique<engine::resource::Manager>();
    m_resource_manager->init(*m_render);

    m_context = std::make_unique<Context>(*m_render, *m_input_manager,
                                          *m_resource_manager);
    m_scene_manager = std::make_unique<engine::scene::Manager>(*m_context);
  } catch (const std::exception &e) {
    spdlog::error("app初始化失败{}", e.what());
    return false;
  }
  return true;
}

void App::deinit() {
  // 先销毁渲染器，再退出SDL
  m_scene_manager.reset();
  m_resource_manager.reset();
  m_input_manager.reset();
  m_render.reset();
  m_time->deinit();
  SDL_Quit();
}

bool App::render() {
  if (m_render->begin()) {

    m_scene_manager->render();
    m_render->end();
  }
  return true;
}

bool App::update() {
  m_time->update();
  float dt = m_time->getDeltaTime();
  m_scene_manager->update(dt);
  return true;
}

bool App::event(const SDL_Event *event [[maybe_unused]]) {
  m_input_manager->update(*event);
  if (m_input_manager->shouldQuit()) {
    return false;
  }
  m_scene_manager->event();
  return true;
}

void App::pushScene(std::unique_ptr<engine::scene::Scene> &&scene) {
  if (scene) {
    m_scene_manager->push(std::move(scene));
  }
}
} // namespace engine::core
