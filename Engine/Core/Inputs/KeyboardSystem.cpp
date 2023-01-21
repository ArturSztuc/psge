#include "Core/Inputs/KeyboardSystem.hpp"


#ifndef KEYBOARDEVENTPROCESS
#define KEYBOARDEVENTPROCESS(window, key)\
{\
  if(glfwGetKey(window, key) == GLFW_PRESS) {\
    EVENT_FIRE(KeyboardEvent(EventType::EVENT_TYPE_KEYBOARD_INPUT, key));\
  }\
}
#endif

KeyboardEvent::KeyboardEvent(KeyboardKeyCode _code)
    : Event(EVENT_TYPE_KEYBOARD_INPUT, 1)
{
  m_args[0].m_U32[0] = _code;
}

KeyboardKeyCode KeyboardEvent::GetCode() const
{
  return static_cast<KeyboardKeyCode>(m_args[0].m_U32[0]);
}


KeyboardSystem::KeyboardSystem(GLFWwindow* _window)
  : m_window(_window)
{
  glfwSetKeyCallback(m_window, &KeyCallback);
};


KeyboardSystem& KeyboardSystem::GetInstance(GLFWwindow* _window)
{
  static KeyboardSystem instance(_window);
  return instance;
}

void KeyCallback(GLFWwindow* _window,
                 int _key,
                 int _scancode,
                 int _action,
                 int _mods)
{
  KeyboardEvent event(static_cast<KeyboardKeyCode>(_key));

  // TODO: Only fire if the key is within KeyboardKeyCode?
  EVENT_FIRE(event);
}

void KeyboardSystem::Update()
{
  // TODO: Need to abstract it to a platform layer
  glfwPollEvents();
}
