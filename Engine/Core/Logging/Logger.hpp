#pragma once

#include <chrono>
#include <fstream> 
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include "Core/Timing/Clock.hpp"

/*! \enum LogLevel
 *
 *  Detailed description
 */
enum LogLevel 
{  
  LOG_LEVEL_TRACE,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_FATAL
};


class Logger
{
public:
  Logger ();

  void SetLogLevel(LogLevel _logLevel);

  void Log(LogLevel _logLevel, const std::string& _message);

  void WriteToFile(const std::string& _filename);

  void WriteAndClear(const std::string& _filename);

private:
  std::map<LogLevel, std::string> m_logLevelNameMap = 
                 {{LOG_LEVEL_TRACE, "TRACE"},
                  {LOG_LEVEL_DEBUG, "DEBUG"},
                  {LOG_LEVEL_INFO,  "INFO"},
                  {LOG_LEVEL_WARN,  "WARN"},
                  {LOG_LEVEL_ERROR, "ERROR"},
                  {LOG_LEVEL_FATAL, "FATAL"}};
  LogLevel m_logLevel;
  std::vector<std::string> m_messages;
};
