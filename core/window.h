#ifndef CORE_WINDOW_H_
#define CORE_WINDOW_H_

#include <SDL3/SDL.h>
#include <memory>
#include <system_error>
#include "utils.h"
#include "log.h"
#include <vulkan/vulkan_raii.hpp>

namespace core {

class Window {
 public:
  static boost::leaf::result<Window> Create(core::ILogger* logger, Extent2D extent,
                                                       const std::string& title);
  std::tuple<const char* const*, std::size_t> GetVulkanInstanceExtensions();
  boost::leaf::result<VkSurfaceKHR> CreateSurface(VkInstance instance);
 private:
  Window() : sdl_wnd_{nullptr, SDL_DestroyWindow} {};
  std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> sdl_wnd_;
  core::ILogger* logger_;
  
};

enum class WindowError {
  kSuccess,
  kExtentOutOfBounds,
  kFailedToCreateWindow
};

}

#endif