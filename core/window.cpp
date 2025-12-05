#include <window.h>
#include <limits>
#include <SDL3/SDL_vulkan.h>

namespace core {

boost::leaf::result<Window> Window::Create(core::ILogger* logger,
                                           Extent2D extent,
                                           const std::string& title) {
  logger->LogDebug("Window::Create begin");
  Window window;
  window.logger_ = logger;
  if (extent.width > std::numeric_limits<int>::max() ||
      extent.height > std::numeric_limits<int>::max()) {
    return boost::leaf::new_error(WindowError::kExtentOutOfBounds);
  }
  window.sdl_wnd_.reset(SDL_CreateWindow(title.c_str(), extent.width, extent.height, SDL_WINDOW_VULKAN));
  if (window.sdl_wnd_ == nullptr) {
    return boost::leaf::new_error(WindowError::kFailedToCreateWindow, std::string(SDL_GetError()));
  }
  logger->LogDebug("SDL window was successfully created");

  logger->LogDebug("Window::Create end");
  return window;
}

std::tuple<const char* const*, std::size_t>
Window::GetVulkanInstanceExtensions() {
  uint32_t count;
  const char* const* extensions_ptr = SDL_Vulkan_GetInstanceExtensions(&count);
  return std::tuple<const char* const*, std::size_t>(extensions_ptr,
                                                          static_cast<size_t>(count));
}

boost::leaf::result<VkSurfaceKHR> Window::CreateSurface(VkInstance instance) {
  VkSurfaceKHR surface;
  bool is_successful = SDL_Vulkan_CreateSurface(sdl_wnd_.get(), instance,
                           nullptr,
                           &surface);
  if (!is_successful) return boost::leaf::new_error(std::string(SDL_GetError()));
  return surface;
}

}

