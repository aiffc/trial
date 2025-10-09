#include "time.hpp"
#include "SDL3/SDL_timer.h"
#include "spdlog/spdlog.h"
#include <cstdint>

namespace Engine::Core {
Time::Time() = default;
Time::~Time() = default;

void Time::doLimit(double delta_time) {
  if (delta_time < mFPSI) {
    uint64_t delay_time =
        static_cast<uint64_t>((mFPSI - delta_time) * 1000000000.0);
    SDL_DelayNS(delay_time);
    mDeltaTime =
        static_cast<double>(SDL_GetTicksNS() - mEndTime) / 1000000000.0;
  }
}

void Time::init() {
  mStartTime = SDL_GetTicksNS();
  mEndTime = mStartTime;
  spdlog::trace("时间初始化 {}", mStartTime);
}

void Time::update() {
  mStartTime = SDL_GetTicksNS();
  double current_delta_time =
      static_cast<double>(mEndTime - mStartTime) / 1000000000.0;

  if (mFPSI > 0.0) {
    doLimit(current_delta_time);
  } else {
    mDeltaTime = current_delta_time;
  }

  mEndTime = SDL_GetTicksNS();
}

void Time::setFPS(uint32_t fps) {
  mFPS = fps;

  if (mFPS == 0) {
    mFPSI = 0.0;
  } else {
    mFPSI = 1.0 / mFPS;
  }
}
} // namespace Engine::Core
