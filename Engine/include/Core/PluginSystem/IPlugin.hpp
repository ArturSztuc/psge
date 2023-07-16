#pragma once

#include "defines.h"

//namespace psge
//{

class IPlugin
{
public:
  IPlugin()
  {};

  ~IPlugin()
  {};

  virtual const char* GetPluginName()           const = 0;
  virtual const char* GetPluginInterfaceName()  const = 0;
  virtual const char* GetPluginCapabilities()   const = 0;
  virtual const char* GetPluginDelete()         const = 0;
};

//};