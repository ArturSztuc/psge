/**
 * @file Mesh.hpp
 * @brief A mesh object, and a part of ECS
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2024-05-11
 */
#pragma once

#include "defines.h"

#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

using MeshVertex = glm::vec3;
using MeshNormal = glm::vec3;
using MeshColor = glm::vec3;
using MeshUV = glm::vec2;

class Mesh
{
public:
  struct Vertex3D
  {
    MeshVertex position{};
  };

  Mesh LoadFromObj(const S128& _fileLocation);
  Mesh LoadFromBin(const S128& _fileLocation);

private:
  std::vector<MeshVertex> m_Vertices;
  std::vector<MeshNormal> m_Normals;
  std::vector<MeshVertex> m_UVs;
  U32 m_VertexCount;
  U32 m_TriangleCount;
  U32 m_EdgeCount;
};

inline void LoadFromObj(const S128& _fileLocation, Mesh& _mesh);
inline void LoadFromBin(const S128& _fileLocation, Mesh& _mesh);