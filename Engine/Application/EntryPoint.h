#pragma once

#include <iostream>

extern psge::Application* psge::CreateApplication();

int main(int argc, char** argv)
{

  auto application = psge::CreateApplication();

  application->Run();

  delete application;
}
