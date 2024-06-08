#include "Application/Application.h"

namespace psge
{

Application::Application(const std::string& _configfile)
  : m_configFile(_configfile)
{
  m_shouldClose = false;

  // Loads the configuration file
  Configure();

  LDEBUG("Engine config loaded");
  LINFO("Config location: %s", m_config->GetLocation().data());
}

void Application::Configure()
{
  if(!m_configFile.empty())
    m_config = new JsonConfigParser(m_configFile);
  else
    m_config = JsonConfigParser::LoadDefaultConfig();

  // Initialising global log
  LOG_NEW("global", LOG_LEVEL_TRACE, std::string(m_config->Get<std::string>("log_location")) + "global_log.txt");

  CreateWindow();
}

void Application::CreateWindow()
{
  m_window = new Window(m_config->Get<std::string>("game_title", "DefaultName"),
                        m_config->Get<int>("window_width", 800),
                        m_config->Get<int>("window_height", 600));
}

Application::~Application()
{
  LINFO("Destroying the application's renderer");
  m_renderer.reset();

  LINFO("Application deconstructed gracefully");
  LOGS_SAVE();
};

void Application::Run(bool _shouldRun)
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
    RenderGame();

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
  KeyboardSystem::GetInstance(m_window);

  LDEBUG("Initializing the TaskManager");
  TaskManager::GetInstance().Initialize(m_config->Get<int>("task_manager_threads", 1));

  LDEBUG("Initializing the renderer");
  InitializeRenderer();
}

void Application::InitializeRenderer()
{
  RendererConfig config{};
  config.m_applicationName = (m_config->Get<std::string>("game_title")).data();

  m_renderer = std::make_unique<VulkanRenderer>();
  m_renderer->Initialize(config, m_window);
}

void Application::LoadPlugins()
{
  m_pluginManager = new PluginManager(*m_config);

  if (m_pluginManager->FindPlugins())
    LINFO("Found plugins");
  else
    LINFO("Did not find any plugins");

  //std::unique_ptr<void*> examplePlugn = m_pluginManager->GetPlugin("RendererPluginInterface");
}

void Application::ShouldLoopClose()
{
  if(m_window->ShouldClose())
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

  // Update the task system
  TaskManager::GetInstance().Update(m_deltaTime);
}

void Application::RenderGame()
{
  m_renderer->Render(m_deltaTime);
}

void Application::UpdateInput()
{
  // TODO: Need to change the behaviour of this singleton to have GLFW initializer and normal GetInstance.
  KeyboardSystem::GetInstance(m_window).Update(m_deltaTime);
}

};
