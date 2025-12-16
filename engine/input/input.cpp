#include "input.hpp"
#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace engine::input {
void Manager::updateActionState(const std::string &action, bool is_down,
                                bool repeat) {
  auto it = m_action_status.find(action);
  if (it == m_action_status.end()) {
    return;
  }

  if (is_down) {
    if (repeat)
      it->second = ActionState::Held;
    else
      it->second = ActionState::Press;
  } else {
    it->second = ActionState::Release;
  }
}

void Manager::update(const SDL_Event &event) {
  for (auto &[action, state] : m_action_status) {
    if (state == ActionState::Press) {
      state = ActionState::Held;
    } else if (state == ActionState::Release) {
      state = ActionState::None;
    }
  }

  switch (event.type) {
  case SDL_EVENT_KEY_DOWN:
  case SDL_EVENT_KEY_UP: {
    SDL_Scancode keycode = event.key.scancode;
    bool is_down = event.key.down;
    bool is_repeat = event.key.repeat;
    auto it = std::find_if(
        m_key_binding.begin(), m_key_binding.end(),
        [&keycode](const std::pair<std::string, std::vector<std::string>> &p) {
          return getScancode(p.first) == keycode;
        });
    if (it != m_key_binding.end()) {
      for (const auto &action : it->second) {
        updateActionState(action, is_down, is_repeat);
      }
    }
    break;
  }
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
  case SDL_EVENT_MOUSE_BUTTON_UP: {
    uint32_t button = event.button.button;
    bool is_down = event.button.down;
    auto it = std::find_if(
        m_key_binding.begin(), m_key_binding.end(),
        [&button](const std::pair<std::string, std::vector<std::string>> &p) {
          return getMouseButtonUin32FromString(p.first) == button;
        });
    if (it != m_key_binding.end()) {
      for (const auto &action : it->second) {
        updateActionState(action, is_down, false);
      }
    }
  } break;
  case SDL_EVENT_MOUSE_MOTION:
    m_mouse_pos = glm::vec2{event.motion.x, event.motion.y};
    break;
  case SDL_EVENT_QUIT:
    m_quit = true;
    break;
  default:
    break;
  }
}

bool Manager::isActionPress(const std::string &action) const {
  const auto &it = m_action_status.find(action);
  if (it != m_action_status.end()) {
    return it->second == ActionState::Press ||
           it->second == ActionState::Held;
  }
  return false;
}
bool Manager::isActionHeld(const std::string &action) const {
  const auto &it = m_action_status.find(action);
  if (it != m_action_status.end()) {
    return it->second == ActionState::Held;
  }
  return false;
}
bool Manager::isActionRelease(const std::string &action) const {
  const auto &it = m_action_status.find(action);
  if (it != m_action_status.end()) {
    return it->second == ActionState::Release;
  }
  return false;
  }

} // namespace engine::input
