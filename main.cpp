#include "core/pipelines/triangle.hpp"
#include "core/renderer.hpp"
#include <memory>

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

std::unique_ptr<engine::render::Renderer> renderer;

SDL_AppResult SDL_AppInit(void **appstate [[maybe_unused]],
                          int argc [[maybe_unused]],
                          char **argv [[maybe_unused]]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }
  renderer = std::make_unique<engine::render::Renderer>();
  if (!renderer->init()) {
    return SDL_APP_FAILURE;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate [[maybe_unused]], SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate [[maybe_unused]]) {

  if (renderer->begin()) {
    if (renderer->bind<engine::render::TrianglePipeline>()) {
      renderer->drawPrimitives();
    }
    renderer->end();
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate [[maybe_unused]],
                 SDL_AppResult result [[maybe_unused]]) {
  renderer.reset();
}
