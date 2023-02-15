#pragma once

#include "defines.h"

//namespace psge
//{

class IPlugin
{
private:
  S32 m_pluginName;

public:
  IPlugin(S32 _pluginName)
    : m_pluginName(_pluginName)
  {
  };

  ~IPlugin()
  {
  };
};

//};