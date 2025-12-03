#include "app.hpp"
#include "../renderer/renderer.hpp"
#include "SDL3/SDL.h"
#include "spdlog/spdlog.h"
#include "time.hpp"
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
  if (!SDL_SetAppMetadata("Example Snake game", "1.0", "com.example.Snake")) {
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

    // TEST
    testInit();
  } catch (const std::exception &e) {
    spdlog::error("app初始化失败{}", e.what());
    return false;
  }
  return true;
}

void App::deinit() {
  // TEST
  m_tile.reset();
  // 先销毁渲染器，再退出SDL
  m_render.reset();
  m_time->deinit();
  SDL_Quit();
}
bool App::render() {
  if (m_render->begin()) {
    // TEST
    testRender();
    m_render->end();
  }
  return true;
}
bool App::update() {
  m_time->update();
  // float dt = m_time->getDeltaTime();
  return true;
}
bool App::event(const SDL_Event *event [[maybe_unused]]) { return true; }

void App::testInit() {
  if (m_render) {
    m_tile = m_render->createTile("../asset/trial.png", glm::vec2{512.0f, 360.0f});
  }
}

void App::testRender() { m_tile->render(); }
} // namespace engine::core
