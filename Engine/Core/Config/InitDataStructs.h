#pragma once

#include <string>
#include "Core/Config/ConfigParser.h"

struct EngineInit
{
  std::string m_title         = "Application";
  int         m_windowWidth   = 800;
  int         m_windowHeight  = 600;
  bool        m_fullScreen    = false;
  std::string m_logLocation   = "./";

  static EngineInit Fill(JsonConfigParser& _parser)
  {
    EngineInit engineInit{};
    engineInit.m_title        = _parser.Get<std::string>("game_title");
    engineInit.m_windowWidth  = _parser.Get<int>("window_width");
    engineInit.m_windowHeight = _parser.Get<int>("window_height");
    engineInit.m_fullScreen   = _parser.Get<int>("window_full_screen");
    engineInit.m_logLocation  = _parser.Get<int>("log_location");

    return engineInit;
  }
};
