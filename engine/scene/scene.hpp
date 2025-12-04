#pragma once

#include "../object/object.hpp"
#include <memory>
#include <string_view>
#include <vector>

namespace engine::core {
class Context;
}

namespace engine::scene {
class Scene {
private:
  std::string m_name;
  std::vector<std::unique_ptr<engine::object::Object>> m_objs;
  std::vector<std::unique_ptr<engine::object::Object>> m_pending;
  bool m_init{false};

private:
  void processPending();

public:
  Scene(std::string_view name) : m_name{name} {}
  virtual ~Scene() = default;

  const std::vector<std::unique_ptr<engine::object::Object>> &getObjs() const {
    return m_objs;
  }

  void addObj(std::unique_ptr<engine::object::Object> &&);
  void removeObj(engine::object::Object *);
  void removeObjByName(const std::string &);

  engine::object::Object *getObjByName(const std::string &) const;

  virtual void init(engine::core::Context &);
  virtual void update(float, engine::core::Context &);
  virtual void render(engine::core::Context &);
  virtual void event(engine::core::Context &);

  void clean() {
    if (!m_objs.empty())
      m_objs.clear();
    m_init = false;
  }

  bool isInit() const { return m_init; }
  const std::string &getName() const { return m_name; }
  void setName(const std::string &name) { m_name = name; }

  Scene(Scene &) = delete;
  Scene(Scene &&) = delete;
  Scene &operator=(Scene &) = delete;
  Scene &operator=(Scene &&) = delete;
};

} // namespace engine::scene
