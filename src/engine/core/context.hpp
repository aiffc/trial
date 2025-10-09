#pragma once

#include "../render/renderer.hpp"
#include <memory>
namespace Engine::Render {
class Renderer;
}

namespace Engine::Core {
class Context final {
private:
  std::unique_ptr<Engine::Render::Renderer> mRender;

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

  /* 初始化函数，用来创建游戏上下文 */
  void init();

  void begin(float r = 0.0, float g = 0.0, float b = 0.0, float a = 1.0);
  void end();
  void setViewport(float x = 0.0, float y = 0.0, float w = 1024.0,
                   float h = 720.0, float min_depth = 0.0,
                   float max_depth = 1.0);
  void setScissor(int x = 0, int y = 0, int w = 1024, int h = 720);
  void draw(uint32_t);

  template <typename T> void bindPipeline() const {
    mRender->bindPipeline<T>();
  }
};
} // namespace Engine::Core
