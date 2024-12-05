#ifndef COMPONENTS_TRANSFORM_H_
#define COMPONENTS_TRANSFORM_H_

#include <glm/glm.hpp>
#include <entt/entity/entity.hpp>
#include <parent.h>

namespace zge {

struct Transform {
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
};

}



#endif