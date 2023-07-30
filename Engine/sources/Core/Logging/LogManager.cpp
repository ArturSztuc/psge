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

// Helper function to format the log message using vsnprintf. This is awful, but
// I'm not sure what else I can do here...
std::string FormatLogMessage(const std::string& _message, va_list args) {
    char buffer[MAX_LOG_BUFFER_SIZE];
    vsnprintf(buffer, MAX_LOG_BUFFER_SIZE, _message.c_str(), args);
    return buffer;
}

void LogManager::Log(const std::string& _loggerName,
                     LogLevel _logLevel,
                     const std::string& _message,
                     ...)
{
  // pointer to the arguments
  __builtin_va_list args;
  va_start(args, _message);

  // Use the ugly helper function :(
  std::string message = FormatLogMessage(_message, args);

  // Clean up the va_list pointer
  va_end(args);

  m_loggers[_loggerName].Log(_logLevel, message);
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
