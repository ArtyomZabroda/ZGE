#include <debug_layer.h>

zge::DebugLayer::DebugLayer()  {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
}

void zge::DebugLayer::WindowInit(SDL_Window* window) {
  ImGui_ImplSDL3_InitForD3D(window);
}

void zge::DebugLayer::RenderInit(ID3D11Device* device,
                                 ID3D11DeviceContext* device_context) {
  ImGui_ImplDX11_Init(device, device_context);
}

void zge::DebugLayer::ProcessEvent(SDL_Event* event) {
  ImGui_ImplSDL3_ProcessEvent(event);
}

void zge::DebugLayer::MakeFrame() {
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Debug layer");
  imgui_ui_func_();
  ImGui::End();
}

void zge::DebugLayer::Render() {
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

zge::DebugLayer::~DebugLayer() {
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
}
