#include <iostream>
#include <window.h>
#include <renderer.h>
#include <signal.h>
#include <log.h>
#include <entt/entity/registry.hpp>
#include <transform.h>
#include <character_controller.h>
#include <camera.h>

namespace zge {

int ZMain(int argc, char** argv) {
  InitLogging(argv[0]);
  Window window("ZGAMEENGINE", Extent2D{.width = 1600, .height = 900});
  Renderer renderer(&window);
  window.Show();
  bool window_closed = false;
  window.Closed().Connect([&window_closed]() { window_closed = true; });

  entt::registry registry{};
  entt::entity object = registry.create();
  registry.emplace<Transform>(object, glm::vec3(5, 0, 0), glm::vec3(0, 0, 0));
  entt::entity camera = registry.create();
  registry.emplace<Camera>(camera, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
  entt::entity main_character = registry.create();
  registry.emplace<Transform>(main_character, glm::vec3(-5, 0, 0),
                              glm::vec3(0, 0, 0));
  registry.emplace<CharacterController>(main_character);

  while (!window_closed) {
    window.ProcessInput();
    renderer.DrawFrame();
  }
  return 0;
}

}
