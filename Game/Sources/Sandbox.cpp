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
    LTRACE("Custom OnUserCreate() fired");
    EVENT_REGISTER(EVENT_TYPE_KEYBOARD_INPUT, Sandbox::OnKeyboardEvent, *this);
  };

  void OnUserUpdate(double delta)
  {
  };

  void OnKeyboardEvent(const Event& _event)
  {
    const KeyboardEvent& keyboardEvent = static_cast<const KeyboardEvent&>(_event);

    KeyboardKeyCode code = keyboardEvent.GetCode();

    if(code == KEY_ESC){
      m_shouldClose = true;
      LDEBUG("KEY_ESC fired, application will close");
    }
    LTRACE("Key: %c Fired", code);
  }

  void OnEvent(const Event& _event)
  {
  }
};

// Link to the application's entry point
psge::Application* psge::CreateApplication()
{
  return new Sandbox("");
}
