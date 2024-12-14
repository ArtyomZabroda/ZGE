#ifndef RENDER_RENDER_CAMERA_H_
#define RENDER_RENDER_CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <camera.h>

namespace zge {

class RenderCamera {
 public:
  RenderCamera(glm::vec3 position, glm::vec3 rotation);
  glm::mat4 View();
  glm::mat4 Proj();

  glm::vec3 Position() const { return position_; }
  glm::vec3 Rotation() const { return rotation_; }
  glm::vec3 Front() const { return front_; }

  void SetPosition(glm::vec3 position);
  void SetRotation(glm::vec3 rotation);
 private:
  glm::vec3 position_;
  glm::vec3 rotation_;

  glm::vec3 front_{0, 0, -1};
  glm::vec3 up_{0, 1, 0};
  glm::vec3 right_{1, 0, 0};

  void UpdateCameraVectors();
};

}

#endif