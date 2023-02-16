#include "Core/Inputs/KeyboardSystem.hpp"
#include "Platform/PlatformKeyboard.hpp"


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


KeyboardSystem::KeyboardSystem(Window* _window)
  : SystemBase(SYSTEM_TYPE_INPUT), m_window(_window)
{
  SetKeyboardCallback(m_window);
};


KeyboardSystem& KeyboardSystem::GetInstance(Window* _window)
{
  static KeyboardSystem instance(_window);
  return instance;
}

void KeyboardSystem::Update(F32 _deltaTime)
{
  PollKeyboardEvents();
}

void KeyboardSystem::Shutdown()
{

}