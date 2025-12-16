#pragma once

#include "SDL3_mixer/SDL_mixer.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace engine::resource {
class Audio final {
private:
  struct SoundDestroyer {
    void operator()(Mix_Chunk *sound) {
      if (sound) {
        Mix_FreeChunk(sound);
        sound = nullptr;
      }
    }
  };
  std::unordered_map<std::string, std::unique_ptr<Mix_Chunk, SoundDestroyer>>
      m_sound_map;
  struct MusicDestroyer {
    void operator()(Mix_Music *music) {
      if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
      }
    }
  };
  std::unordered_map<std::string, std::unique_ptr<Mix_Music, MusicDestroyer>>
      m_music_map;

public:
  Audio();
  ~Audio();

  void init();

  Mix_Chunk *loadOrGetSound(const std::string &);
  void removeSound(const std::string &);
  void clearSounds();

  Mix_Music *loadOrGetMusic(const std::string &);
  void removeMusic(const std::string &);
  void clearMusics();

  Audio(Audio &) = delete;
  Audio(Audio &&) = delete;
  Audio &operator=(Audio &) = delete;
  Audio &operator=(Audio &&) = delete;
};
} // namespace engine::resource
