#include "Core/Camera/Camera.hpp"

#define GLM_FORCE_RADIANS

namespace psge
{

Camera::Camera(F32 _nearPlane, F32 _farPlane)
  : m_viewMatrix(glm::mat4(1.0f)),
    m_projectionMatrix(glm::mat4(1.0f)),
    m_inverseViewMatrix(glm::inverse(glm::mat4(1.0f))),
    m_nearPlane(_nearPlane),
    m_farPlane( _farPlane),
    m_euler(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Camera::~Camera()
{
}

void Camera::SetPerspectiveProjection(F32 _fov, F32 _aspectRatio)
{
  m_projectionMatrix = glm::perspective(glm::radians(_fov), _aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::SetOrthographicProjection(F32 _left, F32 _right, F32 _bottom, F32 _top)
{
  m_projectionMatrix = glm::ortho(_left, _right, _bottom, _top, m_nearPlane, m_farPlane);
}

void Camera::SetViewMatrix(const glm::mat4& _viewMatrix)
{
  m_viewMatrix = _viewMatrix;
  m_inverseViewMatrix = glm::inverse(_viewMatrix);
}

void Camera::SetProjectionMatrix(const glm::mat4& _projectionMatrix)
{
  m_projectionMatrix = _projectionMatrix;
}

void Camera::SetViewDirection(const glm::vec3& _position, const glm::vec3 _direction, const glm::vec3& _up)
{
  m_viewMatrix = glm::lookAt(_position, _position + _direction, _up);
  m_inverseViewMatrix = glm::inverse(m_viewMatrix);
}

void Camera::SetViewTarget(const glm::vec3& _position, const glm::vec3& _target, const glm::vec3& _up)
{
  m_viewMatrix = glm::lookAt(_position, _target, _up);
  m_inverseViewMatrix = glm::inverse(m_viewMatrix);
}


void Camera::SetCameraPosition(const glm::vec3& _position)
{
  m_position = _position;
  m_needsUpdate = true;
}

void Camera::SetCameraYaw(F32 _yawAmount)
{ 
  m_euler.y += _yawAmount; 
  m_needsUpdate = true;
}

void Camera::SetCameraPitch(F32 _pitchAmount) { 
    m_euler.x += _pitchAmount; 

    // Clamp to avoid gimbal lock
    F32 maxPitch = glm::radians(89.0f); // Limit pitch to avoid gimbal lock
    m_euler.x = glm::clamp(m_euler.x, -maxPitch, maxPitch);

    m_needsUpdate = true; 
}

void Camera::UpdateCamera()
{
  if (!m_needsUpdate) {
    return; // No update needed
  }

  // Create a rotation matrix from the internal euler angles
  glm::mat4 rotationMatrix = glm::mat4(1.0f);
  rotationMatrix = glm::rotate(rotationMatrix, m_euler.x, glm::vec3(1.0f, 0.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, m_euler.y, glm::vec3(0.0f, 1.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, m_euler.z, glm::vec3(0.0f, 0.0f, 1.0f));

  // Translation matrix from the position
  glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_position);

  m_viewMatrix = translationMatrix * rotationMatrix ;

  // Update the inverse view matrix
  m_inverseViewMatrix = glm::inverse(m_viewMatrix);

  m_needsUpdate = false;
}

void Camera::SetClipPlanes(F32 _nearPlane, F32 _farPlane)
{
  m_nearPlane = _nearPlane;
  m_farPlane = _farPlane;
}

void Camera::MoveCameraPosition(const glm::vec3& _position)
{
  m_position += _position;
  m_needsUpdate = true;
}
};