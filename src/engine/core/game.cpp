#include "game.hpp"
#include "../render/pipeline/triangle.hpp"
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

void Game::update([[maybe_unused]] float dt) {
  // TODO
}

void Game::event() {
  // 当前只做退出
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      mRunning = false;
    }
  }
}

void Game::render() {
  mContext->begin();
  mContext->bindPipeline<Engine::Render::TrianglePipeline>();
  mContext->setViewport();
  mContext->setScissor();
  mContext->draw(3);
  mContext->end();
}

void Game::init() {
  spdlog::trace("游戏内部初始化");
  mContext->init(); // 初始化上下文
  mTime->init();    // 初始化时间管理器
}

void Game::deinit() { spdlog::trace("游戏内部退出"); }

void Game::run() {
  init(); // 进行内部初始化
  while (mRunning) {
    event();
    update(0.0);
    render();
  }
  deinit(); // 进行内部销毁
}
} // namespace Engine::Core
