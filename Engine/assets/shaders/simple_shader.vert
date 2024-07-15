#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colour;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColour;
layout(location = 1) out vec3 fragPositionWorld;
layout(location = 2) out vec3 fragNormalWorld;

struct PointLight{
  vec4 position; // ignore w
  vec4 colour;   // w is intensity
};

// Uniform buffer object 
layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionMatrix;
  mat4 viewMatrix;
  mat4 inverseViewMatrix;
  // mat4 projectionViewMatrix; // Add this when we have tons of objects
  vec4 ambientLightColour; // w is intensity

  // TODO: use specialization constants instead of this dumb hard-coded value... at pipeline createion!
  PointLight pointLights[10];
  int numLights;
} ubo;

// Push constant
layout(push_constant) uniform Push {
  mat4 modelMatrix; 
  mat4 normalMatrix;
} push;

void main(){
  // Position in the world space
  vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);

  // Vertex position for drawing
  // ordering matters! Projection first, then view...
  gl_Position = ubo.projectionMatrix * ubo.viewMatrix * positionWorld;

  fragNormalWorld   = normalize(mat3(push.normalMatrix) * normal);
  fragPositionWorld = positionWorld.xyz;
  fragColour        = colour;
}