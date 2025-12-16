#pragma once

#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_render.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "texture_manager.hpp"
#include <memory>
#include <string>

namespace engine::render {
class Renderer;
}

namespace engine::resource {
class Texture;
class Audio;
class Font;

class Manager final {
private:
  std::unique_ptr<Texture> m_texture;
  std::unique_ptr<Audio> m_audio;
  std::unique_ptr<Font> m_font;

public:
  Manager();
  ~Manager();

  void init(engine::render::Renderer &);

  SDL_GPUTexture *textureGetOrLoad(const std::string &);
  void textureRemove(const std::string &);
  void textureClear();

  Mix_Chunk *soundGetOrLoad(const std::string &);
  void soundRemove(const std::string &);
  void soundClear();

  Mix_Music *musicGetOrLoad(const std::string &);
  void musicRemove(const std::string &);
  void musicClear();

  TTF_Font *fontGetOrLoad(const std::string &, uint32_t);
  void fontRemove(const std::string &, uint32_t);
  void fontClear();

  Manager(Manager &) = delete;
  Manager(Manager &&) = delete;
  Manager &operator=(Manager &) = delete;
  Manager &operator=(Manager &&) = delete;
};
} // namespace engine::resource
