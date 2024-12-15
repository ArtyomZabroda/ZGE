#include <engine.h>

#include <transform.h>
#include <mesh_instance.h>
#include <character_controller.h>
#include <camera.h>
#include <lights.h>
#include <sdl3/SDL_timer.h>

zge::Engine::Engine(int argc, char** argv) 
  : registry_{} {
  InitLogging(argv[0]);
  debug_layer_ = std::make_unique<DebugLayer>();
  window_ = std::make_unique<Window>("ZGAMEENGINE",
                                     Extent2D{.width = 1600, .height = 900}, debug_layer_.get());
  window_->Closed().Connect([this]() { window_closed_ = true; });

  entt::entity object = registry_.create();
  registry_.emplace<Transform>(object, glm::vec3(0, 0, -3), glm::vec3(0, 0, 0),
                              glm::vec3(1, 1, 1));
  registry_.emplace<MeshInstance>(object, nullptr);
  entt::entity camera = registry_.create();
  main_character_ = registry_.create();
  registry_.emplace<Transform>(main_character_, glm::vec3(0, 0, 5),
                              glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
  registry_.emplace<CharacterController>(main_character_, 2.5f, 13.f);
  registry_.emplace<Camera>(camera);
  registry_.emplace<Transform>(camera, glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(1, 1, 1));
  registry_.emplace<Parent>(camera, main_character_);

  entt::entity dir_light = registry_.create();
  DirectionalLight& dir_light_component = registry_.emplace<DirectionalLight>(dir_light);
  dir_light_component.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
  dir_light_component.diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
  dir_light_component.specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
  dir_light_component.direction = glm::vec3(0.57735f, -0.57735f, 0.57735f);


  window_->TrapCursor(false);


  auto c = window_->KeyPressed().Connect([&, this](zge::Key key) { 
    if (key == zge::Key::kMouse2) {
      is_camera_moving_ = true;
      window_->TrapCursor(true);
    }
  });

  window_->KeyDown().Connect([&, this](zge::Key key) {
    if (is_camera_moving_) {
      CharacterController& controller =
          registry_.get<CharacterController>(main_character_);
      switch (key) {
        case zge::Key::kW:
          controller.movement_vec.z -= 1;
          break;
        case zge::Key::kA:
          controller.movement_vec.x -= 1;
          break;
        case zge::Key::kS:
          controller.movement_vec.z += 1;
          break;
        case zge::Key::kD:
          controller.movement_vec.x += 1;
          break;
      }
    }
  });

  window_->MouseMoved().Connect([this](MouseMotion motion) {
    if (is_camera_moving_) {
      Transform& transform = registry_.get<Transform>(main_character_);
      transform.rotation.x -= motion.y_rel;
      if (transform.rotation.x > 89)
        transform.rotation.x = 89;
      else if (transform.rotation.x < -89)
        transform.rotation.x = -89;
      transform.rotation.y -= motion.x_rel;
    }
  });

  window_->KeyUp().Connect([&](zge::Key key) {
    if (key == zge::Key::kMouse2) {
      is_camera_moving_ = false;
      window_->TrapCursor(false);
    }
  });

  renderer_ = std::make_unique<Renderer>(window_.get(), &registry_, camera, debug_layer_.get());

  debug_layer_->SetUI([this]() { 
    auto character_transform = registry_.get<Transform>(main_character_);
    ImGui::Text("FPS: %d", fps);
    ImGui::Text("Position = (%f, %f, %f)", character_transform.position.x,
                character_transform.position.y, character_transform.position.z); 
    ImGui::Text("Rotation = (%f, %f, %f)", character_transform.rotation.x,
                character_transform.rotation.y, character_transform.rotation.z);
    ImGui::Text("Camera rotation = (%f, %f, %f)", renderer_->Camera().Rotation().x,
        renderer_->Camera().Rotation().y, renderer_->Camera().Rotation().z);
    ImGui::Text("Front vec = (%f, %f, %f)", renderer_->Camera().Front().x,
                renderer_->Camera().Front().y, renderer_->Camera().Front().z);

    for (auto&& [entity, dir_light] :
         registry_.view<DirectionalLight>().each()) {
      ImGui::SliderFloat3("Light direction", &dir_light.direction[0], -1, 1);
      ImGui::SliderFloat3("Light ambient", &dir_light.ambient[0], 0, 1);
      ImGui::SliderFloat3("Light diffuse", &dir_light.diffuse[0], 0, 1);
      ImGui::SliderFloat3("Light specular", &dir_light.specular[0], 0, 1);
    }


  });

  prev_time_ = SDL_GetTicksNS();
  current_time_ = prev_time_;
  time_to_update_fps_ = current_time_ + 1000000000;
}

int zge::Engine::Run() {
  while (!window_closed_) {
    current_time_ = SDL_GetTicksNS();
    delta_time_ = current_time_ - prev_time_;

    if (current_time_ > time_to_update_fps_) {
      fps = frame_amount_in_sec_;
      frame_amount_in_sec_ = 0;
      time_to_update_fps_ = current_time_ + 1000000000;
    }

    window_->ProcessInput();
    Update();
    renderer_->DrawFrame();

    prev_time_ = current_time_;
    ++frame_amount_in_sec_;
  }
  return 0;
}

void zge::Engine::Update() {
  for (auto&& [entity, chraracter_controller, transform] :
       registry_.view<CharacterController, Transform>().each()) {
    float seconds = delta_time_ / 1000000000.f;
    glm::vec3 front_character_vec = glm::vec3(0, 0, -1);
    glm::mat4 rotation_mat = glm::mat4(1);
    rotation_mat = glm::rotate(rotation_mat, glm::radians(transform.rotation.y),
                glm::vec3(0, 1, 0));
    rotation_mat = glm::rotate(rotation_mat, glm::radians(transform.rotation.x),
                               glm::vec3(1, 0, 0));
    rotation_mat = glm::rotate(rotation_mat, glm::radians(transform.rotation.z),
                               glm::vec3(0, 0, 1));
    front_character_vec = glm::normalize(rotation_mat * glm::vec4(front_character_vec, 0.f));
    glm::vec3 right =
        glm::normalize(glm::cross(front_character_vec, glm::vec3(0, 1, 0)));
    glm::vec3 up = glm::normalize(glm::cross(right, front_character_vec));
    glm::mat4 character_view = glm::lookAt(transform.position, transform.position + front_character_vec,
                up);

    if (glm::length(chraracter_controller.movement_vec) > 1e-5) {
      chraracter_controller.movement_vec =
          glm::normalize(chraracter_controller.movement_vec);
    }

    glm::vec3 character_movement_vec = glm::inverse(character_view) * glm::vec4(chraracter_controller.movement_vec, 0.f);

    transform.position +=
        character_movement_vec *
                         chraracter_controller.speed * seconds;
    chraracter_controller.movement_vec = glm::vec3(0);
  }
  debug_layer_->MakeFrame();
}
