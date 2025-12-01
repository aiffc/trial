#include "time.hpp"
#include "SDL3/SDL_timer.h"
#include "spdlog/spdlog.h"
#include <cstdint>

namespace engine::core {

Time::Time(uint32_t fps) : m_fps{fps} {
  if (fps == 0) {
    spdlog::trace("时间管理器初始化：不做帧限制");
    m_frame_interval = 0.0;
  } else {
    spdlog::trace("时间管理器初始化：锁定帧数{}", fps);
    m_frame_interval = 1.0 / fps;
  }
}
Time::~Time() = default;

void Time::init() {
  spdlog::trace("时间管理器初始化");
  m_last_frame_time = SDL_GetTicksNS();
  m_start_frame_time = m_last_frame_time;
}
void Time::deinit() { spdlog::trace("时间管理器退出"); }

void Time::limit(uint64_t l2s_interval) {

  uint64_t interval = static_cast<uint64_t>(m_frame_interval * 1000000000);
  if (l2s_interval < interval) {
    uint64_t delay_time = interval - l2s_interval;
    SDL_DelayNS(delay_time);
    m_delta_time = (SDL_GetTicksNS() - m_last_frame_time) / 1000000000.0;
  }
}

void Time::setfps(uint32_t fps) {
  m_fps = fps;
  if (fps == 0) {
    spdlog::trace("时间管理器初始化：不做帧限制");
    m_frame_interval = 0.0;
  } else {
    spdlog::trace("时间管理器初始化：锁定帧数{}", fps);
    m_frame_interval = 1.0 / fps;
  }
}

void Time::update() {
  m_start_frame_time = SDL_GetTicksNS();
  uint64_t last_to_start_interval = m_start_frame_time - m_last_frame_time;
  if (m_frame_interval > 0.0) {
    limit(last_to_start_interval);
  } else {
    m_delta_time = last_to_start_interval / 1000000000.0;
  }
  m_last_frame_time = SDL_GetTicksNS();
}

} // namespace engine::core
