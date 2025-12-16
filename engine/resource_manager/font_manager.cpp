#include "font_manager.hpp"
#include "SDL3/SDL_error.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "spdlog/spdlog.h"
#include <memory>
#include <stdexcept>

namespace engine::resource {
Font::Font() = default;
Font::~Font() {
  if (TTF_Init()) {
    spdlog::trace("字体管理器退出");
    clear();
    TTF_Quit();
  }
}

void Font::init() {
  spdlog::trace("字体管理器初始化");
  if (!TTF_WasInit() && !TTF_Init()) {
    spdlog::trace("字体管理器初始化失败{}", SDL_GetError());
    throw std::runtime_error("字体管理器初始化失败");
  }
}

TTF_Font *Font::getOrLoad(const std::string &file, uint32_t size) {
  FontHashKey key{file, size};
  if (auto it = m_map.find(key); it != m_map.end()) {
    return it->second.get();
  }

  TTF_Font *raw_font = TTF_OpenFont(file.data(), static_cast<float>(size));
  if (raw_font == nullptr) {
    spdlog::error("打开字体文件失败{}", SDL_GetError());
    return nullptr;
  }
  spdlog::trace("加载字体文件{} {}", file, size);
  m_map.emplace(key, std::unique_ptr<TTF_Font, FontDestroyer>(raw_font));
  return raw_font;
}

void Font::remove(const std::string &file, uint32_t size) {
  FontHashKey key{file, size};
  if (auto it = m_map.find(key); it != m_map.end()) {
    spdlog::trace("移除字体文件{} {}", file, size);
    m_map.erase(key);
  }
}

void Font::clear() {
  if (!m_map.empty()) {
    m_map.clear();
  }
}
} // namespace engine::resource
