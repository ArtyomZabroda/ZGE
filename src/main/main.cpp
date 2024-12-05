#include <iostream>
#include <window.h>
#include <renderer.h>
#include <signal.h>
#include <log.h>
#include <entt/entity/registry.hpp>
#include <transform.h>
#include <mesh_instance.h>
#include <parent.h>
#include <character_controller.h>
#include <camera.h>

namespace zge {

int ZMain(int argc, char** argv) {
  InitLogging(argv[0]);
  Window window("ZGAMEENGINE", Extent2D{.width = 1600, .height = 900});
  entt::registry registry{};
  window.Show();
  bool window_closed = false;
  window.Closed().Connect([&window_closed]() { window_closed = true; });


  entt::entity object = registry.create();
  registry.emplace<Transform>(object, glm::vec3(0, 0, -3), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
  registry.emplace<MeshInstance>(object, nullptr);
  entt::entity camera = registry.create();
  entt::entity main_character = registry.create();
  registry.emplace<Transform>(main_character, glm::vec3(0, 0, 0),
                              glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
  registry.emplace<CharacterController>(main_character, 2.5f);
  registry.emplace<Camera>(camera, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
  registry.emplace<Parent>(camera, main_character);

  glm::vec3 movement_vec(0,0,0);
  window.KeyDown().Connect([&](zge::Key key) { 
    zge::Transform main_character_transform =
        registry.get<Transform>(main_character);
  });

  Renderer renderer(&window, &registry, camera);

  while (!window_closed) {
    window.ProcessInput();
    renderer.DrawFrame();
  }
  return 0;
}

}
