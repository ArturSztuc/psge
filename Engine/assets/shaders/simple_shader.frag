#version 450

layout(location = 0) in vec3 fragColour;
layout(location = 1) in vec3 fragPositionWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout(location = 0) out vec4 outColor;

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


void main() {
  vec3 ambientLight   = ubo.ambientLightColour.xyz * ubo.ambientLightColour.w;
  vec3 diffuseLight   = vec3(0.0);
  vec3 specularLight  = vec3(0.0);
  vec3 surfaceNormal  = normalize(fragNormalWorld);

  // Extract camera position in the world view
  vec3 cameraPosWorld = ubo.inverseViewMatrix[3].xyz;
  // Camera point direction in world view coordinates
  vec3 viewDirection = normalize(cameraPosWorld - fragPositionWorld);

  for(int i = 0; i < ubo.numLights; ++i){
    PointLight light = ubo.pointLights[i];
    // Calculate vectex distance w.r.t. to the light
    vec3 directionToLight = light.position.xyz - fragPositionWorld;
    // Need to do this before we normalize directionToLight, otherwise attenuate will always be 0
    float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
    directionToLight = normalize(directionToLight);

    float cosAngleIncidence = max(dot(surfaceNormal, directionToLight), 0);
    vec3 intensity = light.colour.xyz * light.colour.w * attenuation;

    // Add to the total diffuse light
    diffuseLight += intensity * cosAngleIncidence;

    // Specular lighting using blinn-phong model
    vec3 halfAngle  = normalize(directionToLight + viewDirection);
    float blinnTerm = dot(surfaceNormal, halfAngle);
    // Ignore if light or camera on opposite side of the object surface
    blinnTerm = clamp(blinnTerm, 0, 1);
    blinnTerm = pow(blinnTerm, 512); // Higher values = sharper highlight
    specularLight += intensity * blinnTerm;
  }

  outColor = vec4((ambientLight + diffuseLight + specularLight) * fragColour, 1.0);
}