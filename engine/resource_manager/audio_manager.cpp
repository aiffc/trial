#include "audio_manager.hpp"
#include "SDL3/SDL_error.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "spdlog/spdlog.h"
#include <memory>
#include <stdexcept>

namespace engine::resource {

Audio::Audio() = default;
Audio::~Audio() {
  spdlog::trace("音频管理器退出");
  Mix_HaltChannel(-1);
  Mix_HaltMusic();

  clearSounds();
  clearMusics();

  Mix_CloseAudio();
  Mix_Quit();
}

void Audio::init() {
  MIX_InitFlags init_flag = MIX_INIT_MP3 | MIX_INIT_OGG;
  if (init_flag != (Mix_Init(init_flag) & init_flag)) {
    spdlog::error("音频管理器初始化失败{}", SDL_GetError());
    throw std::runtime_error("音频管理器初始化失败");
  }
  if (!Mix_OpenAudio(0, nullptr)) {
    Mix_Quit();
    spdlog::error("音频管理器打开设备失败{}", SDL_GetError());
    throw std::runtime_error("音频管理器打开设备失败");
  }
  spdlog::trace("音频管理器初始化");
}

Mix_Chunk *Audio::loadOrGetSound(const std::string &file) {
  if (auto it = m_sound_map.find(file); it != m_sound_map.end()) {
    return it->second.get();
  }

  Mix_Chunk *raw_chunk = Mix_LoadWAV(file.data());
  if (raw_chunk == nullptr) {
    spdlog::error("加载音效失败");
    return nullptr;
  }
  spdlog::trace("加载音效{}", file);
  m_sound_map.emplace(file,
                      std::unique_ptr<Mix_Chunk, SoundDestroyer>(raw_chunk));
  return raw_chunk;
}

void Audio::removeSound(const std::string &file) {
  if (auto it = m_sound_map.find(file); it != m_sound_map.end()) {
    m_sound_map.erase(it);
  }
}

void Audio::clearSounds() {
  if (!m_sound_map.empty()) {
    m_sound_map.clear();
  }
}

Mix_Music *Audio::loadOrGetMusic(const std::string &file) {
  if (auto it = m_music_map.find(file); it != m_music_map.end()) {
    return it->second.get();
  }

  Mix_Music *raw_music = Mix_LoadMUS(file.data());
  if (raw_music == nullptr) {
    spdlog::error("加载音乐失败");
    return nullptr;
  }
  spdlog::trace("加载音乐{}", file);
  m_music_map.emplace(file,
                      std::unique_ptr<Mix_Music, MusicDestroyer>(raw_music));
  return raw_music;
}
void Audio::removeMusic(const std::string &file) {
  if (auto it = m_music_map.find(file); it != m_music_map.end()) {
    m_music_map.erase(it);
  }
}
void Audio::clearMusics() {
  if (!m_music_map.empty()) {
    m_music_map.clear();
  }
}
} // namespace engine::resource
