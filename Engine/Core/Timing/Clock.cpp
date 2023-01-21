#include "Core/Timing/Clock.hpp"

Clock& Clock::GetInstance()
{
  static Clock clock;
  return clock;
};

void Clock::ResetFrame()
{
  m_startTimeFrame = ChronoClock::now();
};

F32 Clock::ElapsedFrame()
{
  auto endTime = ChronoClock::now();
  return std::chrono::duration<float>(endTime - m_startTimeFrame).count();
};

F32 Clock::Elapsed()
{
  auto endTime = ChronoClock::now();
  auto diff = endTime - m_startTimeFrame;
  return std::chrono::duration<F32>(diff).count();
}

std::string Clock::GetElapsedString()
{
  int  elapsed   = static_cast<int>(Elapsed());
  auto hours    = (elapsed / 3600) % 24;
  auto minutes  = (elapsed / 60) % 60;
  auto seconds  = elapsed % 60;
  char buffer[9];

  std::sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  return std::string(buffer);
};
