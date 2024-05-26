/**
 * @file PlatformFileManager.hpp
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @brief Macros to make the library loading and unloadong platform-agnostic
 * @date 2023-02-17
 */
#pragma once
#include "defines.h"

#ifdef PLATFORM_WINDOWS
  #include <windows.h>
  #define HANDLE HMODULE
  #define CLOSE_LIB FreeLibrary 
  #define GET_LIB_FUNCTION GetProcAddress
  #define GET_LIB(_file) LoadLibrary(_file)
  #define GET_LIB_ERROR GetLastError()
  #define PSGE_API __declspec(dllexport)
#elif defined(PLATFORM_LINUX)
  #include <dlfcn.h>
  #define HANDLE void*
  #define CLOSE_LIB dlclose
  #define GET_LIB_FUNCTION dlsym
  #define GET_LIB(_file) dlopen(_file, RTLD_LAZY)
  #define GET_LIB_ERROR dlerror()
  #define PSGE_API __attribute__((visibility("default")))
#else
  #define HANDLE
  #define CLOSE_LIB
  #define GET_LIB_FUNCTION
  #define GET_LIB(_file)
  #define PSGE_API
#endif