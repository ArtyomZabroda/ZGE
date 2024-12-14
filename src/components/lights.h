#ifndef COMPONENTS_LIGHT_H_
#define COMPONENTS_LIGHT_H_

#include <glm/glm.hpp>

namespace zge {
struct DirectionalLight {
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
  glm::vec3 direction;
  float pad;
};

struct PointLight {
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;

  glm::vec3 position;
  float range;

  glm::vec3 attenuation;
  float pad;
};

struct SpotLight {
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;

  glm::vec3 position;
  float range;

  glm::vec3 direction;
  float spot;

  glm::vec3 attenuation;
  float pad;
};

}



#endif