/**
 * @file PlatformKeyboard.hpp
 * @brief Platform abstraction for the keyboard input
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-02-02
 * @todo TODO: Include other inputs here?
 */
#pragma once
#include "defines.h"

#include "Core/Window/Window.hpp" 
#include "Core/Inputs/KeyboardSystem.hpp"

#if defined(PLATFORM_WINDOW_GLFW)
#include <GLFW/glfw3.h>
/**
 * @brief Callback function for the glfw event poller
 *
 * Callback function that is being triggered every time glfw detects any
 * keyboard input in a window.
 *
 * @param _window GLFWwindow object to poll the events from
 * @param _key Triggered key code
 * @param _scancode No idea. @todo What is this?
 * @param _action Checks if pressed, released, held etc.
 * @param _mods No idea. @todo What is this?
 *
 * @todo TODO: Update params doc
 * @todo TODO: Int -> U64?
 */
void GLFWKeyCallback(GLFWwindow* _window,
                     int _key,
                     int _scancode,
                     int _action,
                     int _mods)
{
  KeyboardEvent event(static_cast<KeyboardKeyCode>(_key));

  // TODO: Only fire if the key is within KeyboardKeyCode?
  EVENT_FIRE(event);
};
#endif


void KeyboardSystem::SetKeyboardCallback(Window* _window)
{
#if defined(PLATFORM_WINDOW_GLFW)
  glfwSetKeyCallback(_window->GetWindow(), &GLFWKeyCallback);
#endif
}


void KeyboardSystem::PollKeyboardEvents()
{
#if defined(PLATFORM_WINDOW_GLFW)
  glfwPollEvents();
#endif
}





