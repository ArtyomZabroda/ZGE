#ifndef COMPONENTS_CAMERA_H_
#define COMPONENTS_CAMERA_H_

#include <glm/glm.hpp>

namespace zge {

struct Camera {
  glm::vec3 position;
  glm::vec3 rotation;
};

}

#endif