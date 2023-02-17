/**
 * @file PlatformFileManager.hpp
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @brief Macros to make the library loading and unloadong platform-agnostic
 * @date 2023-02-17
 */
#pragma once

#ifdef PLATFORM_WINDOWS
  #include <windows.h>
  #define HANDLE HMODULE
  #define CLOSE_LIB FreeLibrary 
  #define GET_LIB_FUNCTION GetProcAddress
  #define GET_LIB(_file) LoadLibrary(_file)
  #define GET_LIB_ERROR GetLastError()
#elif defined(PLATFORM_LINUX)
  #include <dlfcn.h>
  #define HANDLE void*
  #define CLOSE_LIB dlclose
  #define GET_LIB_FUNCTION dlsym
  #define GET_LIB(_file) dlopen(_file, RTLD_LAZY)
  #define GET_LIB_ERROR dlerror()
#else
  #define HANDLE
  #define CLOSE_LIB
  #define GET_LIB_FUNCTION
  #define GET_LIB(_file)
#endif