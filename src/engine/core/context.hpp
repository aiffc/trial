#pragma once

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
};
} // namespace Engine::Core
