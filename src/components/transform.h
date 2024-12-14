#ifndef COMPONENTS_TRANSFORM_H_
#define COMPONENTS_TRANSFORM_H_

#include <glm/glm.hpp>
#include <entt/entity/registry.hpp>
#include <parent.h>

namespace zge {

struct Transform {
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
};

inline glm::mat4 CalculateGlobalModelTransform(entt::registry& registry, entt::entity entity){
  entt::entity processed_entity = entity;
  glm::mat4 model = glm::mat4(1);
  while (processed_entity != entt::null) {
    if (Transform* t = registry.try_get<zge::Transform>(processed_entity)) {
      model = glm::translate(model, t->position);
      model =
          glm::rotate(model, glm::radians(t->rotation.y), glm::vec3(0, 1, 0));
      model =
          glm::rotate(model, glm::radians(t->rotation.x), glm::vec3(1, 0, 0));
      model =
          glm::rotate(model, glm::radians(t->rotation.z), glm::vec3(0, 0, 1));
      model = glm::scale(model, t->scale);
    }
    zge::Parent* parent = registry.try_get<zge::Parent>(processed_entity);
    processed_entity = parent ? parent->entity : entt::null;
  }
  return model;
}

}



#endif