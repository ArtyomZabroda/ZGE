#ifndef DBG_DEBUG_LAYER_H_
#define DBG_DEBUG_LAYER_H_

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_sdl3.h>
#include <entt/entity/registry.hpp>
#include <functional>

namespace zge {

class DebugLayer {
 public:
  DebugLayer();
  void WindowInit(SDL_Window* window);
  void RenderInit(ID3D11Device* device, ID3D11DeviceContext* device_context);
  void ProcessEvent(SDL_Event* event);
  void MakeFrame();
  void SetUI(const std::function<void()>& imgui_ui_func) {
    imgui_ui_func_ = imgui_ui_func;
  }
  void Render();
  ~DebugLayer();

 private:
  std::function<void()> imgui_ui_func_;
};

}

#endif