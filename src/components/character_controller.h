#ifndef COMPONENTS_CHARACTER_CONTROLLER_H_
#define COMPONENTS_CHARACTER_CONTROLLER_H_

#include <entt/entity/entity.hpp>

namespace zge {

struct CharacterController {
  entt::entity camera;
  float sensitivity;
};

}

#endif