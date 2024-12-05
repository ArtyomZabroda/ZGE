#include <render_camera.h>
#include <numbers>
#include <parent.h>

zge::RenderCamera::RenderCamera(glm::vec3 position, glm::vec3 rotation)
    : position_{position}, rotation_{rotation} {
  UpdateCameraVectors();
}

glm::mat4 zge::RenderCamera::View() {
  return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 zge::RenderCamera::Proj() {
  return glm::perspectiveZO(float(std::numbers::pi / 2), 16.f / 9.f, 0.1f,
                            100.f);
}

void zge::RenderCamera::SetPosition(glm::vec3 position) {
  position_ = position;
}

void zge::RenderCamera::SetRotation(glm::vec3 rotation) {
  rotation_ = rotation;
  UpdateCameraVectors();
}

void zge::RenderCamera::UpdateCameraVectors() {
  glm::vec4 front{0, 0, -1, 0};
  glm::mat4 mat =
      glm::rotate(glm::mat4(1), glm::radians(-rotation_.y), glm::vec3(0, 1, 0));
  mat = glm::rotate(mat, glm::radians(rotation_.x), glm::vec3(1, 0, 0));
  mat = glm::rotate(mat, glm::radians(rotation_.z), glm::vec3(0, 0, 1));
  front_ = glm::normalize(mat * front);
  right_ = glm::normalize(glm::cross(front_, glm::vec3(0, 1, 0)));
  up_ = glm::normalize(glm::cross(right_, front_));
}
