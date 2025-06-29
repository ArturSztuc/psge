/**
 * @file Application.h
 * @brief Base class for our application/game to be derived from
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-14
 *
 * @see Application
 * @see CreateApplication
 */

#pragma once

#include "Core/Core.h"
#include "Graphics/Graphics.h"

namespace psge
{

/**
 * @class Application
 * @brief Base application class used in the entry point
 *
 * This base class is supposed to be used to derive a new game. The derived
 * game class has to overwrite few functions, and needs to have a
 * CreateApplication() function to be actually used in the entry point.  It
 * also needs to have OnUserCreate(), used for creating the game, and a custom
 * OnUserUpdate to progress the game simulation at every time step.
 *
 * @see OnUserCreate
 * @see OnUserUpdate
 * @see CreateApplication
 * @see OnEvent
 */
class Application
{
public:
  /**
   * @brief Application's base class constructor
   * 
   * Base class used for deriving a game application. Takes config file
   * location.
   *
   * @param _configfile Location of the config file to load with the app
   */
  Application(const std::string& configfile_);

  /**
   * @brief Default deconstructor
   */
  ~Application();

  Application(const Application&) = delete;
  void operator=(const Application&) = delete;

  /**
   * @brief Contains the main game loop
   *
   * This function contains the main game loop that updates
   * the game mechanics (and runs e.g. OnUserUpdate and Update
   * functions) at every time step.
   */
  virtual void Run(bool _shouldRun = true);

  /** 
   * @brief To be ran in the initialiser. To-overwrite.
   */
  virtual void OnUserCreate() = 0;

  /** 
   * @brief User-defined to update game sim at every step. To-overwrite
   *
   * @param _delta time step from the last frame for simulation
   *
   * @todo TODO: Use a timestep class instead of F64!
   */
  virtual void OnUserUpdate(F64 _delta) = 0;

  /** 
   * @brief Executes application's response to an event
   *
   * A callback function that executes the Application's response to an event
   * that the application might subscribe to. 
   *
   * @todo TODO: Make it overwriteable
   *
   * @param _event Event/message that the Application subscribes to
   */
  void OnEvent(const Event& _event);

  /** 
   * @brief Executes application's key response to an event
   *
   * A callback function that executes the Application's response to an event
   * that the application might subscribe to. 
   *
   * @todo TODO: Remove this? We can have it in OnEvent etc.
   *
   * @param _event Event/message that the Application subscribes to
   */
  void OnEventKey(const Event& _event);

protected:
  /**
   * @brief Initializes the game engine
   */
  void Initialize();

  /**
   * @brief Configures the engine given the JsonConfigParser object
   */
  void Configure();

  /**
   * @brief Create a Window object
   */
  void CreateWindow();

  /**
   * @brief Create a Camera object
   */
  void CreateCamera();

  /**
   * @brief Loads and selects the plugins based on our config
   */
  void LoadPlugins();

  void InitializeRenderer();

  /**
   * @brief Checks whether the game simulation should exit
   */
  void ShouldLoopClose();

  /**
   * @brief Updates the clock
   */
  void UpdateClock();

  /**
   * @brief Updates the game state
   */
  void UpdateGameState();

  /** 
   * @brief Polls for the input events
   */
  void UpdateInput();

  void RenderGame();

protected:
  /// Checks if the game simulation should close
  bool        m_shouldClose;

  /// Stores the location of the game engine initializer config
  std::string m_configFile;

  /// Stores the config object
  JsonConfigParser* m_config;

  /// Stores the window to render the game upon
  Window* m_window;

  /// Stores the camera object
  std::shared_ptr<Camera> m_camera;

  //Renderer* m_renderer;
  std::unique_ptr<Renderer> m_renderer;

  PluginManager* m_pluginManager = nullptr;

  /// Stores the time
  /// @todo TODO: Change to a time object
  F64 m_deltaTime;
};

/**
 * @brief Creates the application in the entry point
 *
 * A function that creates the application/game, to be used in the entry point.
 * This function needs to be written in the derived class.
 *
 * @return Application object that is used in the entry point 
 */
Application* CreateApplication();
};
