#include "engine/core/app.hpp"
#include "game/scenes/test_scene.hpp"
#include <SDL3/SDL_init.h>
#include <algorithm>
#include <memory>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

std::unique_ptr<engine::core::App> app;

SDL_AppResult SDL_AppInit(void **appstate [[maybe_unused]],
                          int argc [[maybe_unused]],
                          char **argv [[maybe_unused]]) {
  app = std::make_unique<engine::core::App>();
  if (!app->init()) {
    return SDL_APP_FAILURE;
  }
  auto scene = std::make_unique<game::TestScene>("test scene");
  app->pushScene(std::move(scene));
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate [[maybe_unused]], SDL_Event *event) {
  if (!app->event(event)) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate [[maybe_unused]]) {
  app->update();
  app->render();
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate [[maybe_unused]],
                 SDL_AppResult result [[maybe_unused]]) {
  app->deinit();
}
