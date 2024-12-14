#ifndef PLATFORM_SDL3_SDL_WINDOW_H_
#define PLATFORM_SDL3_SDL_WINDOW_H_

#include <string>
#include <extent.h>
#include <signal.h>
#include <key.h>
#include <mouse_motion.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <debug_layer.h>

namespace zge {

class Window {
 public:
  Window(const std::string& title, Extent2D client_extent, DebugLayer* debug_layer);
  void Show();
  ~Window();
  std::string Title();
  void SetTitle(const std::string& new_title);
  Extent2D Extent();
  void SetExtent(Extent2D new_extent);
#ifdef WIN32
  HWND GetNativeHandle();
#endif
  SDL_Window* GetSDLWindowHandle() { return wnd_; }
  void TrapCursor(bool enable);
  void ProcessInput();

  Signal<void()>& Closed() { return closed_; }
  Signal<void(Extent2D)>& Resized() { return resized_; }
  Signal<void(Key)>& KeyDown() { return key_down_; }
  Signal<void(Key)>& KeyPressed() { return key_pressed_; }
  Signal<void(Key)>& KeyUp() { return key_up_; }
  Signal<void(MouseMotion)>& MouseMoved() { return mouse_moved_; }
 private:
  SDL_Window* wnd_;
  DebugLayer* debug_layer_;
  Signal<void()> closed_;
  Signal<void(Extent2D)> resized_;
  Signal<void(Key)> key_down_;
  Signal<void(Key)> key_pressed_;
  Signal<void(Key)> key_up_;
  Signal<void(MouseMotion)> mouse_moved_;
};

}

#endif