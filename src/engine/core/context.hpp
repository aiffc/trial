#pragma once
#include "SDL3/SDL.h"
#include "SDL3/SDL_video.h"

namespace Engine::Core {
class Context final {
private:
  SDL_Window *window; // SDL窗口

private:
  /* 上下文退出函数 */
  void deinit();

public:
  /* 构造函数   */
  Context();
  /* 析构函数 */
  ~Context();

  Context(Context &) = delete;
  Context(Context &&) = delete;
  Context &operator=(Context &) = delete;
  Context &&operator=(Context &&) = delete;

  /*
   * 初始化函数，用来创建游戏上下文
   */
  void init();
  /*
   * 获取SDL窗口
   */
  SDL_Window *getWindow() const;
};
} // namespace Engine::Core
