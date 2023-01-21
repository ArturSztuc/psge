#include <psge.h>

class Sandbox : public psge::Application
{
public:
  Sandbox(const std::string& configfile_)
    : psge::Application(configfile_)
  {
  };

  ~Sandbox()
  {
  };

  void OnUserCreate()
  {
  };

  void OnUserUpdate(double delta)
  {
  };
};

// Link to the application's entry point
psge::Application* psge::CreateApplication()
{
  return new Sandbox("");
}
