#include "defines.h"

#if defined(PLATFORM_WINDOW_GLFW)
#include "Core/Window/Window.hpp"

Window::Window(const std::string& _windowName,
               U32 _width,
               U32 _height)
  : m_windowName(_windowName),
    m_extent{_width, _height}
   //m_width(_width), m_height(_height) 
{
  glfwInit();

  // Give hints to the GLFW
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  // The last two are glfw monitoring and glfw window share
  m_window = glfwCreateWindow(m_extent.width, m_extent.height, m_windowName.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(m_window, this);
  glfwSetFramebufferSizeCallback(m_window, frameBufferResizedCallback);
}

Window::~Window()
{
  glfwDestroyWindow(m_window);
}

void Window::frameBufferResizedCallback(GLFWwindow* _window,
                                        int _width,
                                        int _height)
{
  auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
  window->m_frameBufferResized = true;
  window->m_extent.width = _width;
  window->m_extent.height = _height;
}

B8 Window::ShouldClose()
{
  return glfwWindowShouldClose(m_window);
}
#endif // Platform linux or windows (does glfw work under MacOS too?)
