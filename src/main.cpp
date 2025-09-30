#include "engine/core/game.hpp"
#include "spdlog/spdlog.h"
#include <exception>

int main() {
  try {
    spdlog::set_level(spdlog::level::trace);
    Engine::Core::Game app;
    app.run();
  } catch (const std::exception &e) {
    spdlog::error(e.what());
  }

  return 0;
}
