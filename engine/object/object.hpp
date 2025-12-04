#pragma once

#include <string>
#include <string_view>
#include "../core/context.hpp"


namespace engine::object {

  // TODO maybe final
class Object {
private:
  std::string m_name;
  bool m_remove_flag;

public:
  Object(std::string_view name) : m_name(name) {}
  ~Object() = default;

  bool needRemove() const { return m_remove_flag; }
  void setRemove(bool flag = true) { m_remove_flag = flag; }

  void setName(const std::string &name) { m_name = name; }
  const std::string& getName() const {return m_name;}

  //TODO
  void render(engine::core::Context &context [[maybe_unused]]) {}
  void update(float dt [[maybe_unused]],
              engine::core::Context &context [[maybe_unused]]) {}
  void event(engine::core::Context &context [[maybe_unused]]) {}

  Object(Object &) = delete;
  Object(Object &&) = delete;
  Object& operator=(Object &) = delete;
  Object& operator=(Object &&) = delete;
};

}
