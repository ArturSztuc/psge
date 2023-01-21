#include "Core/Logging/Logger.hpp"

Logger::Logger()
  : m_logLevel(LOG_LEVEL_INFO)
{
}

void Logger::SetLogLevel(LogLevel _logLevel)
{
  m_logLevel = _logLevel;
}

void Logger::Log(LogLevel _logLevel, const std::string& _message)
{
  if(_logLevel < m_logLevel)
    return;

  std::stringstream ss;
  ss << Clock::GetInstance().GetElapsedString() << " [" << m_logLevelNameMap[_logLevel] << "]: " << _message;
  m_messages.push_back(ss.str());
}

void Logger::WriteToFile(const std::string& _filename)
{
  std::ofstream file;
  file.open(_filename, std::ios_base::app);
  for(const auto& message : m_messages){
    file << message << std::endl;
    //std::cout << message << std::endl;
  }
}

void Logger::WriteAndClear(const std::string& _filename)
{
  WriteToFile(_filename);
  m_messages.clear();
};
