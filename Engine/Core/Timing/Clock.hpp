/**
 * @file Clock.hpp
 * @brief File containing a counter/clock implementation
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-21
 *
 * @see ClockUnit
 * @see Clock
 */
#pragma once

#include <chrono>
#include <string>

#include "defines.h"

using ChronoClock     = std::chrono::high_resolution_clock;
using ChronoTimePoint = std::chrono::time_point<ChronoClock>;

/**
 * @enum ClockUnit
 * @brief unit format for the clock/time counter
 * @todo TODO: Modify all the *Elapsed* function members to return in a format provided with the typedef from this enum
 * @todo TODO: This is not used yet.
 */
enum ClockUnit
{
  CLOCK_UNIT_S,
  CLOCK_UNIT_MILIS,
  CLOCK_UNIT_MICROS,
  CLOCK_UNIT_NANOS,
};

/**
 * @class Clock
 * @brief Clock class for measuring the frame's elapsed time
 * @todo TODO: Implement a time unit to be used in Update functions?
 */
class Clock
{
private:
  /// Frame's starting time
  ChronoTimePoint m_startTimeFrame;

  /// Application's starting time
  ChronoTimePoint m_startTimeGlobal;

  /// Private singleton constructor
  Clock()
  {
    m_startTimeGlobal = ChronoClock::now();
    ResetFrame();
  };

public:
  /// Get the clock's singleton instance
  static Clock& GetInstance();

  /// Resets the frame's time at the beginning of the frame
  void ResetFrame();

  /// Measures the elapsed time in milliseconds
  /// @todo TODO: Don't use F32? Time unit class?
  F32 ElapsedFrame();

  /// Elapsed frame time...
  /// @todo TODO: The same as above? Change to ElapsedGlobal?
  F32 Elapsed();

  /// Returns the elapsed time in a form of a string
  std::string GetElapsedString();
};
