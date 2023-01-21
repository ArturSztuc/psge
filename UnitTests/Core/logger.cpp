#include <gtest/gtest.h>
#include <Core/Logging/LogManager.hpp>
#include <fstream>
#include <regex>

TEST(LogManagerTest, TestOneLogOneOutput)
{
  LOG_NEW("game", LOG_LEVEL_TRACE, "game_output.txt");
  LOG_TRACE("game", "this is a trace message for the game logger");
  LOG_SAVE("game");

  std::ifstream logFileGame("game_output.txt");
  std::string logMessageGame;
  std::getline(logFileGame, logMessageGame);
  std::getline(logFileGame, logMessageGame);

  EXPECT_TRUE(logMessageGame.compare("00:00:00 [TRACE]: this is a trace message for the game logger") == 0);
}

TEST(LogManagerTest, TestTwoLogsTwoOutputs)
{
  LOG_NEW("game", LOG_LEVEL_TRACE, "game_trace_output.txt");
  LOG_TRACE("game", "this is a trace message for the game logger");
  LOG_SAVE("game");

  LOG_NEW("engine", LOG_LEVEL_DEBUG, "engine_debug_output.txt");
  LOG_DEBUG("engine", "this is a debug message for the engine logger");
  LOG_SAVE("engine");

  std::ifstream logFileGame("game_output.txt");
  std::string logMessageGame;
  std::getline(logFileGame, logMessageGame);
  std::getline(logFileGame, logMessageGame);

  std::ifstream logFileEngine("engine_debug_output.txt");
  std::string logMessageEngine;
  std::getline(logFileEngine, logMessageEngine);
  std::getline(logFileEngine, logMessageEngine);

  EXPECT_TRUE(logMessageGame.compare("00:00:00 [TRACE]: this is a trace message for the game logger") == 0);
  EXPECT_TRUE(logMessageEngine.compare("00:00:00 [DEBUG]: this is a debug message for the engine logger") == 0);
}
