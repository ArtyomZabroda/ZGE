#include <camera.h>
#include <numbers>


zge::Camera::Camera(const glm::vec3& position, const glm::vec3& rotation)
    : position_{position}, rotation_{rotation} {
  UpdateCameraVectors();
}

glm::mat4 zge::Camera::View() {
  return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 zge::Camera::Proj() {
  return glm::perspectiveZO(float(std::numbers::pi / 2), 16.f / 9.f, 0.1f,
                            100.f);
}

void zge::Camera::UpdateCameraVectors() {
  glm::vec4 front{0, 0, -1, 0};
  glm::mat4 mat = glm::rotate(glm::mat4(1), glm::radians(-rotation_.y), glm::vec3(0, 1, 0));
  mat = glm::rotate(mat, glm::radians(rotation_.x), glm::vec3(1, 0, 0));
  mat = glm::rotate(mat, glm::radians(rotation_.z), glm::vec3(0, 0, 1));
  front_ = glm::normalize(mat * front);
  right_ = glm::normalize(glm::cross(front_, glm::vec3(0, 1, 0)));
  up_ = glm::normalize(glm::cross(right_, front_));
}
