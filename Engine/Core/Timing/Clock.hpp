#pragma once

#include <chrono>
#include <string>

#include "defines.h"

using ChronoClock     = std::chrono::high_resolution_clock;
using ChronoTimePoint = std::chrono::time_point<ChronoClock>;

// TODO: Modify all the *Elapsed* function members to return in a format
//       provided with the typedef from this enum
enum ClockUnit
{
  CLOCK_UNIT_S,
  CLOCK_UNIT_MILIS,
  CLOCK_UNIT_MICROS,
  CLOCK_UNIT_NANOS,
};

class Clock
{
private:
  ChronoTimePoint m_startTimeFrame;
  ChronoTimePoint m_startTimeGlobal;

  Clock()
  {
    m_startTimeGlobal = ChronoClock::now();
    ResetFrame();
  };

public:
  static Clock& GetInstance();

  void ResetFrame();

  F32 ElapsedFrame();

  F32 Elapsed();

  std::string GetElapsedString();
};
