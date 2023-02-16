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
#include "Core/Window/Window.hpp"
#include "Core/EntityComponentSystem/System.hpp"

/**
 * @enum KeyboardKeyCode
 * @brief Lists codes for different keyboard keys
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

/**
 * @class KeyboardSystem
 * @brief Polls and dispatches key events
 *
 * @todo TODO: Re-check if needed.
 */
class KeyboardSystem : public SystemBase
{
public:
  /// Singletin onstance getter --requires GLFWwindow
  /// @todo TODO: Split into initializer(window) getter().
  static KeyboardSystem& GetInstance(Window* _window);

  /// Polls the window keyboard events
  void Update(F32 _deltaTime);

  /// Shuts the KeyboardSystem down
  void Shutdown();

private:
  /// Singleton constructor
  KeyboardSystem(Window* _window);

  /// Platform abstrction for setting the Keyboard callback
  void SetKeyboardCallback(Window* _window);

  /// Platform abstraction for polling for the keyboard events
  void PollKeyboardEvents();

private:
  /// Pointer to the GLFW window
  Window* m_window;
};