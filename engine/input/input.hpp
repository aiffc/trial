#pragma once

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_render.h"
#include "glm/glm.hpp"
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace engine::input {
enum class ActionState {
  None,
  Press,
  Held,
  Release,
};

class Manager final {
private:
  bool m_quit{false};
  glm::vec2 m_mouse_pos;
  const std::unordered_map<std::string, std::vector<std::string>> m_key_binding{
      {"w", {"move up"}},
      {"s", {"move down"}},
      {"a", {"move left"}},
      {"d", {"move right"}},
      {"q", {"show menu"}},
      {"e", {"show info"}},
      {"mouse left", {"attack", "select", "click"}},
      {"mouse right", {"cancle"}}};

  std::unordered_map<std::string, ActionState> m_action_status{
      {"move down", ActionState::None}, {"move up", ActionState::None},
      {"move left", ActionState::None}, {"move right", ActionState::None},
      {"select", ActionState::None},    {"show menu", ActionState::None},
      {"show info", ActionState::None}, {"attack", ActionState::None},
      {"cancle", ActionState::None},    {"click", ActionState::None}};

private:
  static inline SDL_Scancode getScancode(std::string_view key) {
    return SDL_GetScancodeFromName(key.data());
  }
  static inline uint32_t getMouseButtonUin32FromString(std::string_view key) {
    // 只需要鼠标左键
    if (key == "mouse left")
      return SDL_BUTTON_LEFT;
    else if (key == "mouse right")
      return SDL_BUTTON_RIGHT;
    return 0;
  }
  void updateActionState(const std::string &, bool, bool repeat = false);

public:
  Manager() = default;
  ~Manager() = default;

  bool shouldQuit() const { return m_quit; }
  void setQuit(bool p = true) { m_quit = p; }

  void update(const SDL_Event &);

  bool isActionPress(const std::string &) const;
  bool isActionHeld(const std::string &) const;
  bool isActionRelease(const std::string &) const;

  const glm::vec2 &getMousePos() const { return m_mouse_pos; }

  Manager(Manager &) = delete;
  Manager(Manager &&) = delete;
  Manager &operator=(Manager &) = delete;
  Manager &operator=(Manager &&) = delete;
};

} // namespace engine::input
