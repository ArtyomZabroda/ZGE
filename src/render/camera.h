#ifndef RENDER_CAMERA_H_
#define RENDER_CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace zge {

class Camera {
 public:
  Camera(const glm::vec3& position, const glm::vec3& rotation);
  glm::mat4 View();
  glm::mat4 Proj();
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