/**
 * @file Camera.hpp
 * @brief A camera class for rendering scenes
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2025-06-28
 * 
 * @todo Need to think how to integrate this with the ECS system.
 * Perhaps this could be integrated with ECS by having a CameraComponent that
 * would be used to define camera position, orientation, and other properties.
 * CameraComponent could then be attached to entities in the ECS system.
 */

#pragma once

#include "defines.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace psge
{

/**
 * @class Camera
 * @brief Camera class for rendering scenes
 */
class Camera
{
public:
    /** 
     * @brief Constructor for the Camera class
     * 
     * @param _nearPlane Near clipping plane distance
     * @param _farPlane Far clipping plane distance
     */
    Camera(F32 _nearPlane = 0.1f,
           F32 _farPlane = 1000.0f);

    ~Camera();

    NOCOPY(Camera);

    /**
     * @brief Sets the Perspective Projection 
     * 
     * @param _fov field of view in degrees
     * @param _aspectRatio aspect ratio of the viewport (width / height)
     */
    void SetPerspectiveProjection(F32 _fov,
                                  F32 _aspectRatio);

    /**
     * @brief Sets the Orthographic Projection
     * 
     * @param _left Left clipping plane
     * @param _right Right clipping plane
     * @param _bottom Bottom clipping plane
     * @param _top Top clipping plane
     */
    void SetOrthographicProjection(F32 _left,
                                   F32 _right,
                                   F32 _bottom,
                                   F32 _top);

    /**
     * @brief Sets the View Matrix
     * @param _viewMatrix The view matrix to set
     * @note This will also update the inverse view matrix
     */
    void SetViewMatrix(const glm::mat4& _viewMatrix);

    /**
     * @brief Sets the Projection Matrix
     * @param _projectionMatrix The projection matrix to set
     * @note This will not update the inverse view matrix
     */
    void SetProjectionMatrix(const glm::mat4& _projectionMatrix);

    /**
     * @brief Sets the View Direction
     * 
     * @param _position Camera position
     * @param _direction Camera direction
     * @param _up Up vector, defaults to (0, 1, 0)
     */
    void SetViewDirection(const glm::vec3& _position,
                          const glm::vec3 _direction,
                          const glm::vec3& _up = glm::vec3(0.0f, 1.0f, 0.0f));
    void SetViewTarget(const glm::vec3& _position, const glm::vec3& _target, const glm::vec3& _up = glm::vec3(0.0f, 1.0f, 0.0f));

    /**
     * @brief Sets the clip planes for the camera
     * 
     * @param _nearPlane Near clipping plane distance
     * @param _farPlane Far clipping plane distance
     */
    void SetClipPlanes(F32 _nearPlane,
                       F32 _farPlane);

    /**
     * @brief Sets the camera position
     * 
     * @param _position Camera position in world space
     */
    void SetCameraPosition(const glm::vec3& _position);

    void UpdateCamera();

    /**
     * @brief Set the Camera Yaw
     * 
     * Modifies the camera's yaw, rotation around the Y-axis (looking left and
     * right)
     * 
     * @param _yawAmount The amount to change the yaw by
     */
    void SetCameraYaw(F32 _yawAmount);

    /**
     * @brief Set the Camera Pitch
     * 
     * Modifies the camera's pitch, rotation around the X-axis (looking up and
     * down)
     * 
     * @param _pitchAmount  The amount to change the pitch by
     */
    void SetCameraPitch(F32 _pitchAmount);

    void MoveCameraPosition(const glm::vec3& _position);

    const glm::mat4& GetViewMatrix() const { return m_viewMatrix; }
    const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }
    const glm::mat4& GetInverseViewMatrix() const { return m_inverseViewMatrix; }

    const glm::vec3 GetPosition() const { return glm::vec3(m_inverseViewMatrix[3]); }

    const glm::vec3 GetEuler() const { return m_euler; }

private:
  /// @brief The view matrix of the camera
  glm::mat4 m_viewMatrix;

  /// @brief The inverse view matrix of the camera
  glm::mat4 m_inverseViewMatrix;

  /// @brief The projection matrix of the camera
  glm::mat4 m_projectionMatrix;

  /// @brief The near clipping plane distance
  F32 m_nearPlane;

  /// @brief The far clipping plane distance
  F32 m_farPlane;

  /// @brief The position of the camera in world space
  glm::vec3 m_position;

  /// @brief The direction of the camera in world space
  glm::vec3 m_euler;

  /// @brief Flag to indicate if the camera needs to be updated
  B8 m_needsUpdate;
};
};