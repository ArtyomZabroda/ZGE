#ifndef MAIN_ENGINE_H_
#define MAIN_ENGINE_H_

#include <entt/entity/registry.hpp>
#include <window.h>
#include <log.h>
#include <memory>
#include <renderer.h>
#include <debug_layer.h>

namespace zge {

class Engine {
 public:
  Engine(int argc, char** argv);
  int Run();
  void Update();
 private:
  entt::registry registry_;
  std::unique_ptr<Window> window_;
  bool window_closed_ = false;
  std::unique_ptr<Renderer> renderer_;
  std::unique_ptr<DebugLayer> debug_layer_;

  entt::entity main_character_;
  bool is_camera_moving_ = false;

  uint64_t prev_time_;
  uint64_t current_time_;
  uint64_t delta_time_;
  uint64_t time_to_update_fps_;

  unsigned int frame_amount_in_sec_ = 0;
  unsigned int fps = 0;

};

}

#endif