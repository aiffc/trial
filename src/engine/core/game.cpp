#include "game.hpp"
#include "SDL3/SDL_events.h"
#include "context.hpp"
#include "spdlog/spdlog.h"
#include "time.hpp"
#include <memory>

namespace Engine::Core {
Game::Game()
    : mContext{std::make_unique<Context>()}, mTime{std::make_unique<Time>()} {
  spdlog::trace("游戏初始化");
  mRunning = true;
}

Game::~Game() { spdlog::trace("游戏退出"); }

void Game::update([[maybe_unused]] float dt) {}

void Game::event() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      mRunning = false;
    }
  }
}

void Game::render() {}

void Game::init() {
  spdlog::trace("游戏内部初始化");
  mContext->init();
  mTime->init();
}

void Game::deinit() { spdlog::trace("游戏内部退出"); }

void Game::run() {
  init();
  while (mRunning) {
    event();
    update(0.0);
    render();
  }
  deinit();
}
} // namespace Engine::Core
