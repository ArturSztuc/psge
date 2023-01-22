#include "defines.h"

#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS)
#include "Core/Window/Window.hpp"

Window::Window(const std::string& windowName_, uint32_t width_, uint32_t height_)
  : m_windowName(windowName_), m_width(width_), m_height(height_) 
{
  glfwInit();

  // Give hints to the GLFW
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  // The last two are glfw monitoring and glfw window share
  m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(m_window, this);
  glfwSetFramebufferSizeCallback(m_window, frameBufferResizedCallback);
}

Window::~Window()
{
  glfwDestroyWindow(m_window);
}

void Window::frameBufferResizedCallback(GLFWwindow* window_,
                                        int width_,
                                        int height_)
{
  auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window_));
  window->m_frameBufferResized = true;
  window->m_width  = width_;
  window->m_height = height_;
}

bool Window::ShouldClose()
{
  return glfwWindowShouldClose(m_window);
}
#endif // Platform linux or windows (does glfw work under MacOS too?)
