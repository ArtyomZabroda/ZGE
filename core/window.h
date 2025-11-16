#ifndef CORE_WINDOW_H_
#define CORE_WINDOW_H_

#include <SDL3/SDL.h>
#include <memory>
#include <system_error>
#include "utils.h"

namespace core {

class Window {
 public:
  static boost::leaf::result<Window> Create(Extent2D extent,
                                                       const std::string& title);
 private:
  Window() : sdl_wnd_{nullptr, SDL_DestroyWindow} {};
  std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> sdl_wnd_;
};

enum class WindowError {
  kSuccess,
  kExtentOutOfBounds,
  kFailedToCreateWindow
};

}

#endif