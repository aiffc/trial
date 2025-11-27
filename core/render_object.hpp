#pragma once

#include <SDL3/SDL_gpu.h>
#include <cstddef>
#include <cstdint>
#include <spdlog/spdlog.h>
#include <utility>
#include <vector>

namespace engine::render {

class Renderer;

template <typename T> class RenderObject final {
  friend class Renderer;
private:
  Renderer *m_owner;
  std::vector<T> m_vertex_data;
  SDL_GPUBuffer *m_vertex_buff{nullptr};
  std::vector<uint32_t> m_index_data;
  SDL_GPUBuffer *m_index_buff{nullptr};
  bool m_init{false};

public:
  RenderObject(Renderer *renderer) : m_owner(renderer) {}
  RenderObject(Renderer *renderer, const std::vector<T> &vdata)
      : m_owner(renderer), m_vertex_data(vdata) {}
  RenderObject(Renderer *renderer, const std::vector<T> &vdata,
               const std::vector<uint32_t> &idata)
      : m_owner(renderer), m_vertex_data(vdata), m_index_data(idata) {}
  ~RenderObject();

  size_t getSize() const { return sizeof(T); }
  size_t getVertexSize() const { return m_vertex_data.size(); }
  size_t getIndexSize() const { return m_index_data.size(); }

  const std::vector<T> getVertexData() const { return m_vertex_data; }
  const std::vector<uint32_t> getIndexData() const { return m_index_data; }

  void pushVertexData(T &&v) { m_vertex_data.push_back(std::move(v)); }
  void pushVertexData(const T &v) { m_vertex_data.push_back(v); }
  void insertVertexData(const std::vector<T> &v) {
    m_vertex_data.insert(m_vertex_data.end(), v.begin(), v.end());
  }
  void pushIndexData(uint32_t v) { m_index_data.push_back(v); }
  void insertIndexData(const std::vector<uint32_t> &v) {
    m_index_data.insert(m_index_data.end(), v.begin(), v.end());
  }

  void init();

  void render();

  RenderObject(RenderObject &) = delete;
  RenderObject(RenderObject &&) = delete;
  RenderObject &operator=(RenderObject &) = delete;
  RenderObject &operator=(RenderObject &&) = delete;
};

} // namespace engine::render
