#pragma once

#include <memory>
namespace Engine::Core {
class Context;

class Game final {
private:
  bool mRunning = false;             // 判断游戏是否运行
  std::unique_ptr<Context> mContext; // 游戏上下文

private:
  /* 游戏更新函数 */
  void update([[maybe_unused]] float dt);
  /* 游戏事件函数 */
  void event();
  /* 游戏渲染函数 */
  void render();
  /* 游戏内部初始化函数 */
  void init();
  /* 游戏内部退出函数 */
  void deinit();

public:
  /* 游戏构造函数 */
  Game();
  /* 游戏析构函数 */
  ~Game();

  Game(Game &) = delete;
  Game(Game &&) = delete;
  Game &operator=(Game &) = delete;
  Game &&operator=(Game &&) = delete;

  /* 游戏运行函数 */
  void run();
};
} // namespace Engine::Core
