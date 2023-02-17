/**
 * @file PlatformFileManager.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @date 2023-02-17
 */
#pragma once

// std includes
#include <iostream>

// Load platform-specific library-loading lib
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#elif defined(PLATFORM_LINUX)
#include <dlfcn.h>
#endif

// internal includes
#include "defines.h"
#include "Core/PluginSystem/PluginManager.hpp"
#include "Core/Logging/LogManager.hpp"

namespace psge
{

void* PluginManager::LoadSharedLibrary(S64 _file)
{
#ifdef PLATFORM_WINDOWS
  HMODULE handle = LoadLibrary(_file.Data());
  if(!handle){
    LERROR("Failed to load a shared library!");
    LERROR(GetLastError());
    return nullptr;
  }
#elif defined(PLATFORM_LINUX)
  void* handle = dlopen(_file.Data(), RTLD_LAZY);
  if(!handle){
    LERROR("Failed to load a shared library!");
    LERROR(dlerror());
    return nullptr;
  }
#endif
  return handle;
}

void* PluginManager::GetFunctionAddress(void* _handle, const S64& _functionName)
{
  void* address = nullptr;
#ifdef PLATFORM_WINDOWS
  address = (void*)GetProcAddress((HMODULE)_handle, _functionName.Data());
  if(!address){
    LERROR("Failed to load exported function from a shared library!");
    LERROR(GetLastError());
  }
#elif defined(PLATFORM_LINUX)
  address = dlsym(_handle, _functionName.Data());
  if(!address){
    LERROR("Failed to load exported function from a shared library!");
    LERROR(dlerror());
  }
#endif
  return address;
}

};