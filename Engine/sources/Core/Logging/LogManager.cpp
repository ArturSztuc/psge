#include "Core/Logging/LogManager.hpp"

LogManager& LogManager::GetInstance()
{
  static LogManager instance;
  return instance;
};

void LogManager::SetLogLevel(const std::string& _loggerName, 
                             LogLevel _logLevel)
{
  m_loggers[_loggerName].SetLogLevel(_logLevel);
};

void LogManager::Log(const std::string& _loggerName,
                     LogLevel _logLevel,
                     const std::string& _message)
{
  m_loggers[_loggerName].Log(_logLevel, _message);
};


void LogManager::WriteAndClear(const std::string& _loggerName,
                              const std::string& _filename)
{
  m_loggers[_loggerName].WriteAndClear(_filename);
}

void LogManager::RegisterLogFile(const std::string& _loggerName,
                                 LogLevel _logLevel,
                                 const std::string& _filename)
{
  m_logFiles[_loggerName][_logLevel] = _filename;
  Log(_loggerName, LOG_LEVEL_INFO, "Logger initialized");
}

void LogManager::SaveAll(const std::string& _loggerName)
{
  for(const auto& kv : m_logFiles[_loggerName])
    WriteAndClear(_loggerName, kv.second);
}

void LogManager::SaveAll()
{
  for(const auto& kv : m_logFiles)
    SaveAll(kv.first);
};
