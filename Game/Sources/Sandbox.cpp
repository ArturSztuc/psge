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
    ConfigureCamera();
  };

  void OnUserUpdate(double delta)
  {
    UpdateCamera(delta);

  };

  void OnKeyboardEvent(const Event& _event)
  {
    const KeyboardEvent& keyboardEvent = static_cast<const KeyboardEvent&>(_event);

    KeyboardKeyCode code = keyboardEvent.GetCode();

    if (code == KEY_ESC) {
      m_shouldClose = true;
      LDEBUG("KEY_ESC fired, application will close");
    }
    LTRACE("Key: %c Fired", code);
  }

  void OnEvent(const Event& _event)
  {
  }

  private:
    void ConfigureCamera()
    {
      m_camera->SetClipPlanes(0.1f, 1000.0f);
      m_camera->SetPerspectiveProjection(45.0f, m_window->GetAspectRatio());
      m_camera->SetCameraPosition(glm::vec3(0.0f, 0.0f, 30.0f));
    }

    void UpdateCamera(F64 _delta)
    {
      m_camera->UpdateCamera();

      F32 tempSpeed = 50.0f;

      if (KeyboardSystem::GetInstance(m_window).IsKeyPressed(KeyboardKeyCode::KEY_A) ||
          KeyboardSystem::GetInstance(m_window).IsKeyPressed(KeyboardKeyCode::KEY_LEFT)) {
          m_camera->SetCameraYaw(_delta * 1.0f);
      }
      if (KeyboardSystem::GetInstance(m_window).IsKeyPressed(KeyboardKeyCode::KEY_D) ||
          KeyboardSystem::GetInstance(m_window).IsKeyPressed(KeyboardKeyCode::KEY_RIGHT)) {
          m_camera->SetCameraYaw(-_delta * 1.0f);
      }
      if (KeyboardSystem::GetInstance(m_window).IsKeyPressed(KeyboardKeyCode::KEY_UP)) {
          m_camera->SetCameraPitch(_delta * 1.0f);
      }
      if (KeyboardSystem::GetInstance(m_window).IsKeyPressed(KeyboardKeyCode::KEY_DOWN)) {
          m_camera->SetCameraPitch(-_delta * 1.0f);
      }

      glm::vec3 velocity(0.0f);

      if (KeyboardSystem::GetInstance(m_window).IsKeyPressed(KeyboardKeyCode::KEY_S)) {
        glm::vec3 backward = glm::normalize(m_camera->GetViewMatrix() * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
        velocity += backward;
      }
      if (KeyboardSystem::GetInstance(m_window).IsKeyPressed(KeyboardKeyCode::KEY_W)) {
        glm::vec3 forward = glm::normalize(m_camera->GetViewMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
        velocity += forward;
      }
      if (KeyboardSystem::GetInstance(m_window).IsKeyPressed(KeyboardKeyCode::KEY_Q)) {
        glm::vec3 left = glm::normalize(m_camera->GetViewMatrix() * glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f));
        velocity += left;
      }
      if (KeyboardSystem::GetInstance(m_window).IsKeyPressed(KeyboardKeyCode::KEY_E)) {
        glm::vec3 right = glm::normalize(m_camera->GetViewMatrix() * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
        velocity += right;
      }
      if (KeyboardSystem::GetInstance(m_window).IsKeyPressed(KeyboardKeyCode::KEY_SPACE)) {
        velocity.y += 1.0f;
      }
      if (KeyboardSystem::GetInstance(m_window).IsKeyPressed(KeyboardKeyCode::KEY_X)) {
        velocity.y -= 1.0f;
      }

      if (glm::length(velocity) > 0.0002f) {
        velocity = glm::normalize(velocity) * tempSpeed * static_cast<F32>(_delta);
        m_camera->MoveCameraPosition(velocity);
      }
    }
};

// Link to the application's entry point
psge::Application* psge::CreateApplication()
{
  return new Sandbox("");
}
