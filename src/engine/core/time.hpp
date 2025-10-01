#pragma once

#include <cstdint>

namespace Engine::Core {
class Time final {
private:
  uint64_t mStartTime;
  uint64_t mEndTime;
  uint32_t mFPS;
  double mFPSI;
  double mDeltaTime;

private:
  void doLimit(double delta_time);

public:
  Time();
  ~Time();

  Time(Time &) = delete;
  Time(Time &&) = delete;
  Time &operator=(Time &) = delete;
  Time &&operator=(Time &&) = delete;

  void init();
  void update();
  void setFPS(uint32_t fps);
};

} // namespace Engine::Core
