/**
 * @file EntryPoint.h
 * @brief Entry point for the game/application
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-21
 *
 * @see CreateApplication
 */

#pragma once

#include <iostream>

/// Function to be defined by user elsewhere in the code to attach the
/// application to the entry point
extern psge::Application* psge::CreateApplication();

// Entry point!
int main(int argc, char** argv)
{
  auto application = psge::CreateApplication();

  application->Run();

  delete application;
}
