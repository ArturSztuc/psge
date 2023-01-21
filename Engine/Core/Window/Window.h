#pragma once

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


//#include "Core/Config/InitDataStructs.h"

class Window
{
// Private data members
private:
  GLFWwindow* m_window;
  unsigned int m_width;
  unsigned int m_height;
  bool m_frameBufferResized = false;
  std::string m_windowName;

// Private member functions
private:
  static void frameBufferResizedCallback(GLFWwindow* window_, int width_, int height);

// Public data members
public:

// Public member functions
public:
  Window(const std::string& windowName_, uint32_t width_, uint32_t height);
  ~Window();

  // Remove the copy operators
  Window(const Window&)=delete;
  Window &operator=(const Window&)=delete;

  bool ShouldClose()            {return glfwWindowShouldClose(m_window);};
  bool WasWindowResized()       {return m_frameBufferResized;};
  void ResetWindowResizedFlag() {m_frameBufferResized=false;};

};
