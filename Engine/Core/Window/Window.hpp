/**
 * @file Window.hpp
 * @brief File containing implementation of the window
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-21
 *
 * @see Window
 */
#pragma once

#include <string>

#include "Platform/PlatformWindow.hpp"

/**
 * @class Window
 * @brief Class that creates a window we can draw on with an API
 * @todo TODO: Need to abstract most of it into a platform layer...
 * @todo TODO: Write a json-based constructor
 */
class Window
{
private:
  /// Window to draw on
  /// @todo TODO: Abstract this to a platform layer...
  PlatformWindow* m_window;

  /// Window width
  unsigned int m_width;

  /// Window height
  unsigned int m_height;

  /// Was the frame recently resized?
  bool m_frameBufferResized = false;

  /// Name of the window to print/draw on titlebar
  std::string m_windowName;

private:
  /// Function callback to resize the window
  static void frameBufferResizedCallback(PlatformWindow* _window, int _width, int _height);

public:
  /**
   * @brief Window constructor
   * @param _windowName name of the window to show as a title
   * @param _width window width
   * @param _height window height
   */
  Window(const std::string& _windowName,
         uint32_t _width,
         uint32_t _height);

  /// Default deconstructor
  ~Window();

  // Remove the copy operators
  Window(const Window&)=delete;
  Window &operator=(const Window&)=delete;

  /// Returns the platform-abstracted window class
  PlatformWindow* GetWindow() {return m_window;};

  /// Returns if the window should close for whatever reason
  bool ShouldClose();

  /// Returns if the window was resized
  bool WasWindowResized()       {return m_frameBufferResized;};

  /// Resets the window resized flag 
  void ResetWindowResizedFlag() {m_frameBufferResized=false;};
};
