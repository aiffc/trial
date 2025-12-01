#pragma once

#include <cstdint>
namespace engine::core {
class Time final {
private:
  uint64_t m_last_frame_time{0};
  uint64_t m_start_frame_time{0};
  double m_delta_time{0.0};
  uint32_t m_fps{0};
  double m_frame_interval{0.0};

private:
  void limit(uint64_t);

public:
  Time(uint32_t fps = 144);
  ~Time();

  void init();
  void deinit();
  void setfps(uint32_t);
  void update();
  float getDeltaTime() const { return static_cast<float>(m_delta_time); }
  uint32_t getfps() const { return m_fps; }

  Time(Time &) = delete;
  Time(Time &&) = delete;
  Time &operator=(Time &) = delete;
  Time &operator=(Time &&) = delete;
};

} // namespace engine::core
