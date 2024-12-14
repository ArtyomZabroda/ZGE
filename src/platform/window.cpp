#include <window.h>
#include <log.h>

zge::Window::Window(const std::string& title, Extent2D client_extent, DebugLayer* debug_layer)
    : debug_layer_{debug_layer} {
  
  LOG(INFO) << "Creating a window";
  wnd_ = SDL_CreateWindow(title.c_str(), client_extent.width,
                          client_extent.height, SDL_WINDOW_RESIZABLE);
  CHECK(wnd_ != nullptr) << SDL_GetError();
  debug_layer_->WindowInit(wnd_);
  LOG(INFO) << "Window has been created successfully";
}

void zge::Window::Show() {
  SDL_ShowWindow(wnd_);
}

zge::Window::~Window() {
  SDL_DestroyWindow(wnd_);
}

std::string zge::Window::Title() {
  return SDL_GetWindowTitle(wnd_);
}

void zge::Window::SetTitle(const std::string& new_title) {
  SDL_SetWindowTitle(wnd_, new_title.c_str());
}

zge::Extent2D zge::Window::Extent() {
  int width, height;
  SDL_GetWindowSize(wnd_, &width, &height);
  Extent2D extent{.width = (uint32_t)width, .height = (uint32_t)height};
  return extent;
}

void zge::Window::SetExtent(Extent2D new_extent) {
  if (!SDL_SetWindowSize(wnd_, new_extent.width, new_extent.height)) {
    LOG(ERROR) << SDL_GetError();
  }
}

#ifdef WIN32
HWND zge::Window::GetNativeHandle() {
  return (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(wnd_), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
}
#endif

void zge::Window::ProcessInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    debug_layer_->ProcessEvent(&event);
    switch (event.type) {
      case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        closed_.Emit();
        break;
      case SDL_EVENT_WINDOW_RESIZED:
        resized_.Emit(Extent2D{.width = (unsigned int)event.window.data1,
                               .height = (unsigned int)event.window.data2});
        break;
      case SDL_EVENT_MOUSE_MOTION:
        mouse_moved_.Emit(MouseMotion{
            .x_pos = (unsigned int)event.motion.x,
            .y_pos = (unsigned int)event.motion.y,
            .x_rel = (int)event.motion.xrel,
            .y_rel = (int)event.motion.yrel
        });
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == SDL_BUTTON_RIGHT) {
          key_pressed_.Emit(zge::Key::kMouse2);
        }
        break;
      case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == SDL_BUTTON_RIGHT) {
          key_up_.Emit(zge::Key::kMouse2);
        }
        break;
    }
  }
  const bool* keys_state = SDL_GetKeyboardState(NULL);
  if (keys_state[SDL_SCANCODE_W]) {
    key_down_.Emit(zge::Key::kW);
  }
  if (keys_state[SDL_SCANCODE_A]) {
    key_down_.Emit(zge::Key::kA);
  }
  if (keys_state[SDL_SCANCODE_S]) {
    key_down_.Emit(zge::Key::kS);
  }
  if (keys_state[SDL_SCANCODE_D]) {
    key_down_.Emit(zge::Key::kD);
  }

  float a, b;
  SDL_MouseButtonFlags f = SDL_GetMouseState(&a, &b);
  if (f & SDL_BUTTON_RMASK) {
    key_down_.Emit(zge::Key::kMouse2);
  }

}

void zge::Window::TrapCursor(bool enable) {
  SDL_SetWindowRelativeMouseMode(wnd_, enable);
}
