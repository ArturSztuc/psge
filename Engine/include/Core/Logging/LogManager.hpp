/**
 * @file LogManager.hpp
 * @brief File defining the class that manages loggers
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-21
 *
 * @see LogManager
 */
#pragma once

#include "defines.h"
#include "Core/Timing/Clock.hpp"
#include "Core/Logging/Logger.hpp"

#include <chrono>
#include <fstream> 
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <format>
#include <stdarg.h>
#include <filesystem>


/**
 * @class LogManager
 * @brief Manages multiple loggers and outputs
 */
class LogManager
{
public:
  /// Singleton instance getter
  static LogManager& GetInstance();

  /// Makes log manager non-copyable
  NOCOPY(LogManager);

  /**
   * @brief Sets level of a given logger
   * @param _loggerName name of the logger to set the level for
   * @param _logLevel Log level to set for the logger
   */
  void SetLogLevel(const std::string& _loggerName, 
                   LogLevel _logLevel);

  /**
   * @brief Logs the message into a specific logger
   * @param _loggerName name of the logger to send the lof message to
   * @param _logLevel Log level of the message
   * @param _message message to log
   */
  void Log(const std::string& _loggerName,
           LogLevel _logLevel,
           const std::string& _location,
           const int& _line,
           const std::string& _message,
           ...);

  /**
   * @brief Writes the messages into file and clears the buffers
   * @param _loggerName name of the logger to save messages for
   * @param _filename name of the file to save the logger's message
   */
  void WriteAndClear(const std::string& _loggerName,
                     const std::string& _filename);

  /**
   * @brief Registers a new logger with the manager
   * @param _loggerName name of the new logger register
   * @param _logLevel level of the logger to register
   * @param _filename name of the file that corresponds to the logger
   */
  void RegisterLogFile(const std::string& _loggerName,
                       LogLevel _logLevel,
                       const std::string& _filename);

  /**
   * @brief Saves all the messages for a specific logger
   * @param _loggerName name of the logger to save the messages for
   */
  void SaveAll(const std::string& _loggerName);

  /**
   * @brief Saves all the messages
   */
  void SaveAll();

private:
  /// Private songleton constructor 
  LogManager() {};

  /// Map of the loggers, each with different logger name
  std::unordered_map<std::string, Logger> m_loggers;

  /// Map of the files, different for each logger and each level of each logger
  std::unordered_map<std::string, std::unordered_map<LogLevel, std::string>> m_logFiles;
};

/// Reguisters a new logger
#define LOG_NEW(loggerName_, logLevel_, filename_){ \
  LogManager::GetInstance().SetLogLevel(loggerName_, logLevel_); \
  LogManager::GetInstance().RegisterLogFile(loggerName_, logLevel_, filename_); }

/// Saves a specific log file
#define LOG_SAVE(loggerName_) \
  LogManager::GetInstance().SaveAll(loggerName_)

/// Saves all the logs
#define LOGS_SAVE() \
  LogManager::GetInstance().SaveAll()

/// Logs traces for different logger names and messages
#define LOG_TRACE(loggerName_, message_, ...) \
  LogManager::GetInstance().Log(loggerName_, LOG_LEVEL_TRACE, __FILE__, __LINE__, message_, ##__VA_ARGS__)
#define LOG_DEBUG(loggerName_, message_, ...) \
  LogManager::GetInstance().Log(loggerName_, LOG_LEVEL_DEBUG, __FILE__, __LINE__, message_, ##__VA_ARGS__)
#define LOG_INFO(loggerName_, message_, ...) \
  LogManager::GetInstance().Log(loggerName_, LOG_LEVEL_INFO,  __FILE__, __LINE__, message_, ##__VA_ARGS__)
#define LOG_WARN(loggerName_, message_, ...) \
  LogManager::GetInstance().Log(loggerName_, LOG_LEVEL_WARN,  __FILE__, __LINE__, message_, ##__VA_ARGS__)
#define LOG_ERROR(loggerName_, message_, ...) \
  LogManager::GetInstance().Log(loggerName_, LOG_LEVEL_ERROR, __FILE__, __LINE__, message_, ##__VA_ARGS__)
#define LOG_FATAL(loggerName_, message_, ...) \
  LogManager::GetInstance().Log(loggerName_, LOG_LEVEL_FATAL, __FILE__, __LINE__, message_, ##__VA_ARGS__)

/// General logger macros
#define LTRACE(message_, ...) \
  LOG_TRACE("global", message_, ##__VA_ARGS__)
#define LDEBUG(message_, ...) \
  LOG_DEBUG("global", message_, ##__VA_ARGS__)
#define LINFO(message_, ...) \
  LOG_INFO("global", message_, ##__VA_ARGS__)
#define LWARN(message_, ...) \
  LOG_WARN("global", message_, ##__VA_ARGS__)
#define LERROR(message_, ...) \
  LOG_ERROR("global", message_, ##__VA_ARGS__)
#define LFATAL(message_, ...) \
  LOG_FATAL("global", message_, ##__VA_ARGS__)
