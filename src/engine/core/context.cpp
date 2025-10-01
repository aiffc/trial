#include "context.hpp"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include "spdlog/spdlog.h"
#include <stdexcept>
#include <string>

namespace Engine::Core {
Context::Context() = default;

Context::~Context() { deinit(); }

void Context::init() {
  spdlog::trace("SDL初始化");
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    throw std::runtime_error("SDL init failed: " + std::string(SDL_GetError()));
  }

  spdlog::trace("创建SDL窗口");
  window = SDL_CreateWindow("triled", 1024, 720, SDL_WINDOW_RESIZABLE);
  if (window == nullptr) {
    throw std::runtime_error("SDL create window failed: " +
                             std::string(SDL_GetError()));
  }
}

void Context::deinit() {
  if (window) {
    spdlog::trace("销毁SDL窗口");
    SDL_DestroyWindow(window);
  }

  spdlog::trace("退出SDL");
  SDL_Quit();
}

SDL_Window *Context::getWindow() const { return window; }
} // namespace Engine::Core
