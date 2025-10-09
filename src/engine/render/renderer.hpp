#pragma once

#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_video.h"
#include <cstdint>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace Engine::Render {
class GraphicsPipeline;

class Renderer final {
private:
  SDL_GPUDevice *mDevice = nullptr; // gpu逻辑设备
  SDL_Window *mWindow = nullptr;    // SDL窗口
  SDL_GPUCommandBuffer *mCmd = nullptr;
  SDL_GPUTexture *mSwapchainTexture = nullptr;
  SDL_GPURenderPass *mRenderPass = nullptr;
  std::unordered_map<std::type_index, std::unique_ptr<GraphicsPipeline>>
      pipelines;

private:
  /* 退出函数 */
  void deinit();

  template <typename T, typename... Args> T *addPipeline(Args &&...args) {
    static_assert(std::is_base_of<GraphicsPipeline, T>::value,
                  "T类型必须是GraphicsPipeline");
    auto type_index = std::type_index(typeid(T));
    if (pipelines.contains(type_index)) {
      T *ret = static_cast<T *>(pipelines.find(type_index)->second.get());
      return ret;
    }
    auto new_pipeline = std::make_unique<T>(std::forward<Args>(args)...);
    new_pipeline->init();
    T *ret = new_pipeline.get();
    pipelines[type_index] = std::move(new_pipeline);

    return ret;
  }

  template <typename T> T *getPipeline() const {
    static_assert(std::is_base_of<GraphicsPipeline, T>::value,
                  "T类型必须是GraphicsPipeline");
    auto type_index = std::type_index(typeid(T));
    auto pipeline = pipelines.find(type_index);
    if (pipeline != pipelines.end()) {
      return static_cast<T *>(pipeline->second.get());
    }
    return nullptr;
  }

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

  /* 初始化函数，用来创建游戏上下文 */
  /* 开始渲染 */
  void begin(float r = 0.0, float g = 0.0, float b = 0.0, float a = 1.0);
  /* 结束渲染 */
  void end();
  /* 设置视口 */
  void setViewport(float x = 0.0, float y = 0.0, float w = 1024.0,
                   float h = 720.0, float min_depth = 0.0,
                   float max_depth = 1.0);
  /* 设置视口裁剪 */
  void setScissor(int x = 0, int y = 0, int w = 1024, int h = 720);
  /* 开始绘制 */
  void draw(uint32_t);

  /* 绑定渲染管线 */
  template <typename T> void bindPipeline() const {
    auto pipeline = getPipeline<T>();
    if (pipeline) {
      SDL_BindGPUGraphicsPipeline(mRenderPass, pipeline->get());
    }
  }
};

} // namespace Engine::Render
