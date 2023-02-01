/**
 * @file KeyboardSystem.hpp
 * @brief File containing classes related with keyboard inputs
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-21
 *
 * @see KeyboardKeyCode
 * @see KeyCallback
 * @see KeyboardEvent
 * @see KeyboardSystem
 */
#pragma once

#include "defines.h"
#include "Core/Event/Event.hpp"
#include "Core/Event/EventSystem.hpp"

#include <GLFW/glfw3.h>

/**
 * @enum KeyboardKeyCode
 * @brief Lists codes for different keyboard keys
 * @todo TODO: Only works under linux, so we need a platform abstraction layer for this.
 */
enum KeyboardKeyCode
{
  KEY_NULL = -1,
  KEY_ESC = GLFW_KEY_ESCAPE,
  KEY_W   = GLFW_KEY_W,
  KEY_S   = GLFW_KEY_S,
  KEY_A   = GLFW_KEY_A,
  KEY_D   = GLFW_KEY_D
};

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
 * @todo TODO: Abstract it to the platform layer...
 * @todo TODO: Int -> U64?
 */
void KeyCallback(GLFWwindow* _window,
                 int _key,
                 int _scancode,
                 int _action,
                 int _mods);

/**
 * @class KeyboardEvent
 * @brief A derived keyboard event that holds the triggered key as data
 */
class KeyboardEvent : public Event
{
public:
  /// Constructor takes the triggered key code
  KeyboardEvent(KeyboardKeyCode _code);

  /// Gets the key code from the data holder
  KeyboardKeyCode GetCode() const;
};

// TODO: Let's derive it from SystemBase object...
/**
 * @class KeyboardSystem
 * @brief Polls and dispatches key events
 *
 * @todo TODO: Re-check if needed.
 */
class KeyboardSystem
{
public:
  /// Singletin onstance getter --requires GLFWwindow
  /// @todo TODO: Split into initializer(window) getter().
  static KeyboardSystem& GetInstance(GLFWwindow* _window);

  /// Polls the window keyboard events
  /// @todo TODO: Abstract to a platform layer...
  void Update();

private:
  /// Singleton constructor
  KeyboardSystem(GLFWwindow* _window);

private:
  /// Pointer to the GLFW window
  /// @todo TODO: Should abstract it into a platform layer
  GLFWwindow* m_window;
};
