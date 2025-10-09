#include "context.hpp"
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

void Context::begin(float r, float g, float b, float a) {
  mRender->begin(r, g, b, a);
}
void Context::end() { mRender->end(); }
void Context::setViewport(float x, float y, float w, float h, float min_depth,
                          float max_depth) {
  mRender->setViewport(x, y, w, h, min_depth, max_depth);
}
void Context::setScissor(int x, int y, int w, int h) {
  mRender->setScissor(x, y, w, h);
}
void Context::draw(uint32_t vertices) { mRender->draw(vertices); }
} // namespace Engine::Core
