#include "context.hpp"
#include "../render/renderer.hpp"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "spdlog/spdlog.h"
#include <memory>
#include <stdexcept>
#include <string>

namespace Engine::Core {
Context::Context() : mRender{std::make_unique<Engine::Render::Renderer>()} {}

Context::~Context() { deinit(); }

void Context::init() {
  spdlog::trace("SDL初始化");
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    throw std::runtime_error("SDL初始化失败: " + std::string(SDL_GetError()));
  }

  mRender->init();
}

void Context::deinit() {
  // 先将renderer销毁再退出SDL
  mRender.reset();
  spdlog::trace("退出SDL");
  SDL_Quit();
}
} // namespace Engine::Core
