#include "Core/Camera/Camera.hpp"

namespace psge
{

Camera::Camera(F32 _nearPlane, F32 _farPlane)
  : m_viewMatrix(glm::mat4(1.0f)),
    m_projectionMatrix(glm::mat4(1.0f)),
    m_inverseViewMatrix(glm::inverse(glm::mat4(1.0f))),
    m_nearPlane(_nearPlane),
    m_farPlane( _farPlane)
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

void Camera::SetCameraTranslation(const glm::vec3& _position)
{
  // Update the view matrix to translate the camera to the new position
  m_viewMatrix = glm::translate(m_viewMatrix, _position);
  m_inverseViewMatrix = glm::inverse(m_viewMatrix);
}

void Camera::SetClipPlanes(F32 _nearPlane, F32 _farPlane)
{
  m_nearPlane = _nearPlane;
  m_farPlane = _farPlane;
}

};