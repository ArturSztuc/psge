#include <gtest/gtest.h>
#include <Core/Timing/Clock.hpp>


TEST(TimingTests, Clock)
{
  // Initializes the clock!
  Clock::GetInstance();

  // No seconds passed yet
  EXPECT_EQ(Clock::GetInstance().GetElapsedString(), "00:00:00");

  Clock::GetInstance().Sleep(1, ClockUnit::CLOCK_UNIT_S);
  
  // One second passed, expect that as a string
  EXPECT_EQ(Clock::GetInstance().GetElapsedString(), "00:00:01");

  // Reset, test float elapsed. Should be close to zero
  Clock::GetInstance().ResetFrame();
  EXPECT_LT(Clock::GetInstance().Elapsed(), 0.0001);

  // Sleep for one second. Float should be just above 1
  Clock::GetInstance().Sleep(1, ClockUnit::CLOCK_UNIT_S);
  EXPECT_GT(Clock::GetInstance().Elapsed(), 1.000);
  EXPECT_LT(Clock::GetInstance().Elapsed(), 1.01);
}


