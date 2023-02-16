#include "Application/Application.h"

namespace psge
{

Application::Application(const std::string& _configfile)
  : m_configFile(_configfile)
{
  m_shouldClose = false;

  // Loads the configuration file
  Configure();

  // Initialising global log
  LOG_NEW("global", LOG_LEVEL_TRACE, m_engineConfig.m_logLocation + "global_log.txt");

  LDEBUG("Engine config loaded");
  LINFO((std::string("Config location: ") +  m_config->GetLocation()).c_str());
}

void Application::Configure()
{
  /// @todo TODO: Need to rid of this dumb EngineInit, and only use our config...
  if(!m_configFile.empty())
    m_config = new JsonConfigParser(m_configFile);
  else
    m_config = JsonConfigParser::LoadDefaultConfig();
  
  m_engineConfig  = EngineInit::Fill(*m_config);
  m_pluginManager = new PluginManager(*m_config);
}

Application::~Application()
{
  LINFO("Application deconstructed gracefully");
  LOGS_SAVE();
};

void Application::Run()
{
  LINFO("Running the application");

  Initialize();

  while(!m_shouldClose){

    // Check if should close
    ShouldLoopClose();

    // Update user input
    UpdateInput();

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
  LINFO("Application loop completed gracefully");
  LOGS_SAVE();
}

void Application::Initialize()
{
  LDEBUG("Initializing the plugins");
  LoadPlugins();

  LDEBUG("Running OnUserCreate");
  OnUserCreate();

  LDEBUG("Initializing the clock");
  UpdateClock();

  LDEBUG("Initializing the keyboard system");
  KeyboardSystem::GetInstance(&m_window);
}

void Application::LoadPlugins()
{
  if (m_pluginManager->FindPlugins())
    LINFO("Found plugins");
  else
    LINFO("Did not find any plugins");
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
  ECSManager::GetInstance().Update(m_deltaTime);

  // Update the event system
  EventSystem::GetInstance().Update();
}

void Application::UpdateInput()
{
  // TODO: Need to change the behaviour of this singleton to have GLFW initializer and normal GetInstance.
  KeyboardSystem::GetInstance(&m_window).Update();
}

};
