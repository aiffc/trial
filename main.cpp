#include "core/pipelines/base_obj.hpp"
#include "core/renderer.hpp"
#include "core/render_object.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <iostream>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

std::unique_ptr<engine::render::Renderer> renderer;
std::unique_ptr<engine::render::RenderObject<engine::render::BaseObject>> triangle;

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
  std::vector<engine::render::BaseObject> datas{
    {{-0.5f, -0.5f},{1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f}},
  };
  std::vector<uint32_t> indexdatas{0, 1, 2, 2, 3, 0};
  
  triangle = renderer->createRenderObject<engine::render::BaseObject>("11.PNG", datas, indexdatas);
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
    if (renderer->bindPipeline<engine::render::BaseObjectPipeline>()) {
      triangle->render();
    }
    renderer->end();
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate [[maybe_unused]],
                 SDL_AppResult result [[maybe_unused]]) {
  triangle.reset();
  renderer.reset();
}
