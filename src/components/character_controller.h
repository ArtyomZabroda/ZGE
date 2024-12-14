#ifndef COMPONENTS_CHARACTER_CONTROLLER_H_
#define COMPONENTS_CHARACTER_CONTROLLER_H_

#include <entt/entity/entity.hpp>
#include <glm/glm.hpp>

namespace zge {

struct CharacterController {
  float sensitivity;
  float speed;
  glm::vec3 movement_vec;
};

}

#endif