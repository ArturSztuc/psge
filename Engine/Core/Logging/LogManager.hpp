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
#include "Core/Logging/Logger.hpp"

class LogManager
{
public:
  static LogManager& GetInstance();

  void SetLogLevel(const std::string& _loggerName, 
                   LogLevel _logLevel);

  void Log(const std::string& _loggerName,
           LogLevel _logLevel,
           const std::string& _message);

  void WriteAndClear(const std::string& _loggerName,
                     const std::string& _filename);

  void RegisterLogFile(const std::string& _loggerName,
                       LogLevel _logLevel,
                       const std::string& _filename);

  void SaveAll(const std::string& _loggerName);

  void SaveAll();

private:
  LogManager() {};
  std::unordered_map<std::string, Logger> m_loggers;
  std::unordered_map<std::string, std::unordered_map<LogLevel, std::string>> m_logFiles;
};

#define LOG_NEW(loggerName_, logLevel_, filename_){ \
  LogManager::GetInstance().SetLogLevel(loggerName_, logLevel_); \
  LogManager::GetInstance().RegisterLogFile(loggerName_, logLevel_, filename_); }

#define LOG_SAVE(loggerName_) \
  LogManager::GetInstance().SaveAll(loggerName_)

#define LOGS_SAVE() \
  LogManager::GetInstance().SaveAll()

#define LOG_TRACE(loggerName_, message_) \
  LogManager::GetInstance().Log(loggerName_, LOG_LEVEL_TRACE, message_)
#define LOG_DEBUG(loggerName_, message_) \
  LogManager::GetInstance().Log(loggerName_, LOG_LEVEL_DEBUG, message_)
#define LOG_INFO(loggerName_, message_) \
  LogManager::GetInstance().Log(loggerName_, LOG_LEVEL_INFO, message_)
#define LOG_WARN(loggerName_, message_) \
  LogManager::GetInstance().Log(loggerName_, LOG_LEVEL_WARN, message_)
#define LOG_ERROR(loggerName_, message_) \
  LogManager::GetInstance().Log(loggerName_, LOG_LEVEL_ERROR, message_)
#define LOG_FATAL(loggerName_, message_) \
  LogManager::GetInstance().Log(loggerName_, LOG_LEVEL_FATAL, message_)

#define LTRACE(message_) \
  LOG_TRACE("global", message_)
#define LDEBUG(message_) \
  LOG_DEBUG("global", message_)
#define LINFO(message_) \
  LOG_INFO("global", message_)
#define LWARN(message_) \
  LOG_WARN("global", message_)
#define LERROR(message_) \
  LOG_ERROR("global", message_)
#define LFATAL(message_) \
  LOG_FATAL("global", message_)
