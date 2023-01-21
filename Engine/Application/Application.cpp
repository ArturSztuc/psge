#include "Application/Application.h"


namespace psge
{

Application::Application(const std::string& _configfile)
  :m_configFile(_configfile)
{
  m_shouldClose = false;
  if(!_configfile.empty()){
    JsonConfigParser jsonConfig(m_configFile);
    m_engineConfig = EngineInit::Fill(jsonConfig);
  }
  else
    m_engineConfig = EngineInit{};

  // Initialising global log
  LOG_NEW("global", LOG_LEVEL_TRACE, m_engineConfig.m_logLocation + "global_log.txt");

  LINFO("Initializing the application");
};

Application::~Application()
{
};

void Application::Run()
{
  LINFO("Running the application");

  Initialize();

  while(!m_shouldClose){

    // Check if should close
    ShouldLoopClose();

    // Update user input
    //processInput();

    // Time update
    UpdateClock();

    // Update game state
    UpdateGameState();

    // Update the camera
    //UpdateCamera();

    // Render the gmae
    //RenderGame();

    LOGS_SAVE();
  }
}

void Application::Initialize()
{
  LDEBUG("Running OnUserCreate");
  OnUserCreate();

  UpdateClock();
}

void Application::ShouldLoopClose()
{
  if(m_window.ShouldClose())
    m_shouldClose = true;
}

void Application::UpdateClock()
{
  m_deltaTime = Clock::GetInstance().ElapsedFrame();
}

void Application::UpdateGameState()
{
  // Update user-defined game state
  OnUserUpdate(m_deltaTime);

  // Update EntityComponentSystem
  ECS().Update(m_deltaTime);
}

};
