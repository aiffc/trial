#pragma once

#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_video.h"
namespace Engine::Render {
class Renderer final {
private:
  SDL_GPUDevice *mDevice; // gpu逻辑设备
  SDL_Window *mWindow;    // SDL窗口

private:
  /* 退出函数 */
  void deinit();

public:
  /* 构造函数 */
  Renderer();
  /* 析构函数 */
  ~Renderer();

  Renderer(Renderer &) = delete;
  Renderer(Renderer &&) = delete;
  Renderer &operator=(Renderer &) = delete;
  Renderer &&operator=(Renderer &&) = delete;

  /* 初始化函数，初始化SDL窗口和逻辑设备 */
  void init();
  /* 获取逻辑设备 */
  SDL_GPUDevice *getDevice() const;
  /* 获取SDL窗口 */
  SDL_Window *getWindow() const;
};

} // namespace Engine::Render
