#pragma once

#include <GLFW/glfw3.h>
#include "Core/Event/Event.hpp"
#include "Core/Event/EventSystem.hpp"


// TODO: Only works under linux, so we need a platform abstraction layer for this.
enum KeyboardKeyCode
{
  KEY_NULL = -1,
  KEY_ESC = GLFW_KEY_ESCAPE,
  KEY_W   = GLFW_KEY_W,
  KEY_S   = GLFW_KEY_S,
  KEY_A   = GLFW_KEY_A,
  KEY_D   = GLFW_KEY_D
};

void KeyCallback(GLFWwindow* _window,
                 int _key,
                 int _scancode,
                 int _action,
                 int _mods);

class KeyboardEvent : public Event
{
public:
  KeyboardEvent(KeyboardKeyCode _code);
  KeyboardKeyCode GetCode() const;
};

// TODO: Let's derive it from SystemBase object...
class KeyboardSystem
{
public:
  static KeyboardSystem& GetInstance(GLFWwindow* _window);

  void Update();

private:
  KeyboardSystem(GLFWwindow* _window);

private:
  /// Pointer to the GLFW window
  /// @todo TODO: Should abstract it into a platform layer
  GLFWwindow* m_window;

  //static KeyboardSystem& m_instance;

};
