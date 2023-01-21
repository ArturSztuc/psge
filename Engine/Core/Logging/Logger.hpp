/**
 * @file Logger.hpp
 * @brief File defining a single logger class
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-21
 *
 * @see LogLevel
 * @see Logger
 */
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

/** 
 * @enum LogLevel
 * @brief Log level enum
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

/**
 * @class Logger
 * @brief Logger class that holds it's own output location
 */
class Logger
{
public:
  /// Default constructor
  Logger();

  /**
   * @brief Sets the log level
   * @param _logLevel Level of this logger
   */
  void SetLogLevel(LogLevel _logLevel);

  /**
   * @brief logs a message
   * @param _logLevel level of the log message
   * @param _message message to log
   */
  void Log(LogLevel _logLevel, const std::string& _message);

  /**
   * @brief Writes all the log messages into a file
   * @param _filename file to write the message to
   * @todo TODO: Check if writing is efficient enough...
   * @todo TODO: Should this be private?
   */
  void WriteToFile(const std::string& _filename);

  /**
   * @brief Writes all the log messages and clears the buffer
   * @param _filename file to write the messages to
   */
  void WriteAndClear(const std::string& _filename);

private:
  /// Map of log level and message prefixes 
  /// @todo TODO: Should this be somewhere else?
  std::map<LogLevel, std::string> m_logLevelNameMap = 
                 {{LOG_LEVEL_TRACE, "TRACE"},
                  {LOG_LEVEL_DEBUG, "DEBUG"},
                  {LOG_LEVEL_INFO,  "INFO"},
                  {LOG_LEVEL_WARN,  "WARN"},
                  {LOG_LEVEL_ERROR, "ERROR"},
                  {LOG_LEVEL_FATAL, "FATAL"}};

  /// Level of this logger
  LogLevel m_logLevel;

  /// Message buffer
  std::vector<std::string> m_messages;
};
