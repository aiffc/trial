#include "resource_manager.hpp"
#include "SDL3_ttf/SDL_ttf.h"
#include "audio_manager.hpp"
#include "font_manager.hpp"
#include "spdlog/spdlog.h"
#include "texture_manager.hpp"
#include <memory>

namespace engine::resource {

Manager::Manager() = default;
Manager::~Manager() { spdlog::trace("资源管理器退出"); }

void Manager::init(engine::render::Renderer &render) {
  spdlog::trace("资源管理器初始化");

  m_texture = std::make_unique<Texture>(render);
  m_audio = std::make_unique<Audio>();
  m_audio->init();

  m_font = std::make_unique<Font>();
  m_font->init();
}

SDL_GPUTexture *Manager::textureGetOrLoad(const std::string &file) {
  return m_texture->loadOrGet(file);
}

void Manager::textureRemove(const std::string &file) {
  m_texture->remove(file);
}

void Manager::textureClear() { m_texture->clear(); }

Mix_Chunk *Manager::soundGetOrLoad(const std::string &file) {
  return m_audio->loadOrGetSound(file);
}
void Manager::soundRemove(const std::string &file) {
  m_audio->removeSound(file);
}
void Manager::soundClear() { m_audio->clearSounds(); }

Mix_Music *Manager::musicGetOrLoad(const std::string &file) {
  return m_audio->loadOrGetMusic(file);
}
void Manager::musicRemove(const std::string &file) {
  m_audio->removeMusic(file);
}
void Manager::musicClear() { m_audio->clearMusics(); }

TTF_Font *Manager::fontGetOrLoad(const std::string &file, uint32_t size) {
  return m_font->getOrLoad(file, size);
}
void Manager::fontRemove(const std::string &file, uint32_t size) {
  m_font->remove(file, size);
}
void Manager::fontClear() { m_font->clear(); }

} // namespace engine::resource
