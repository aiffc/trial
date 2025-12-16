#pragma once

#include "SDL3_ttf/SDL_ttf.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

using FontHashKey = std::pair<std::string, uint32_t>;

struct FontHashFun {
  std::size_t operator()(const FontHashKey &key) const {
    std::hash<std::string> string_hash;
    std::hash<uint32_t> int_hash;
    return string_hash(key.first) ^ int_hash(key.second);
  }
};

namespace engine::resource {
class Font final {
private:
  struct FontDestroyer {
    void operator()(TTF_Font *font) {
      if (font) {
        TTF_CloseFont(font);
        font = nullptr;
      }
    }
  };
  std::unordered_map<FontHashKey, std::unique_ptr<TTF_Font, FontDestroyer>,
                     FontHashFun>
      m_map;

public:
  Font();
  ~Font();

  void init();

  TTF_Font *getOrLoad(const std::string &, uint32_t);
  void remove(const std::string &, uint32_t);
  void clear();

  Font(Font &) = delete;
  Font(Font &&) = delete;
  Font &operator=(Font &) = delete;
  Font &operator=(Font &&) = delete;
};
} // namespace engine::resource
