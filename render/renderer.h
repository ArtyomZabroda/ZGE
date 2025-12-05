#ifndef RENDERER_RENDER_H_
#define RENDERER_RENDER_H_

#include <optional>
#include <set>
#include "render_minimal.h"
#include "window.h"
#include <fstream>

namespace render {

class Renderer {
 public:
  ~Renderer();
  Renderer(Renderer&& other) = default;
  Renderer& operator=(Renderer&& other) = default;
  static boost::leaf::result<Renderer> Create(core::ILogger* logger, core::Window* window, const std::span<const char* const>& extension_names);
  boost::leaf::result<void> DrawFrame();

 private:
  struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool IsComplete() { 
      return graphics_family.has_value() &&
             present_family.has_value();
    }
  };

  struct SwapchainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> present_modes;
  };

  Renderer()
      : logger_(nullptr),
        instance_(nullptr),
        debug_messenger_(nullptr),
        physical_device_(nullptr),
        device_(nullptr),
        graphics_queue_(nullptr),
        present_queue_(nullptr),
        surface_(nullptr),
        window_(nullptr),
        swapchain_(nullptr),
        pipeline_layout_(nullptr),
        graphics_pipeline_(nullptr),
        command_pool_(nullptr),
        current_frame_(0) {}

  boost::leaf::result<void> CreateInstance(
      const std::span<const char* const>& extension_names);
  boost::leaf::result<void> SetupDebugMessenger();
  boost::leaf::result<void> PickPhysicalDevice();
  boost::leaf::result<void> CreateLogicalDevice();
  boost::leaf::result<void> CreateSurface();
  boost::leaf::result<void> CreateSwapchain();
  boost::leaf::result<void> CreateImageViews();
  boost::leaf::result<void> CreateGraphicsPipeline();
  boost::leaf::result<void> CreateCommandPool();
  boost::leaf::result<void> CreateCommandBuffers();
  boost::leaf::result<void> CreateSyncObjects();

  boost::leaf::result<void> RecordCommandBuffer(uint32_t image_index);

  

  std::vector<const char*> GetRequiredExtensions(
      std::span<const char* const> extension_names);
  boost::leaf::result<bool> CheckValidationLevelSupport(
      std::span<const char* const> validation_layers);
  vk::DebugUtilsMessengerCreateInfoEXT CreateDebugMessengerCreateInfo();
  boost::leaf::result<bool> IsDeviceSuitable(vk::raii::PhysicalDevice device);
  QueueFamilyIndices FindQueueFamilies(vk::raii::PhysicalDevice device);
  boost::leaf::result<bool> CheckDeviceExtensionSupport(
      vk::raii::PhysicalDevice device);
  boost::leaf::result<SwapchainSupportDetails> QuerySwapchainSupport(
      vk::raii::PhysicalDevice& device, vk::raii::SurfaceKHR& surface);
  boost::leaf::result<bool> IsSwapchainSuitable(
      vk::raii::PhysicalDevice& device,
                           vk::raii::SurfaceKHR& surface);
  vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<vk::SurfaceFormatKHR>& formats);
  vk::PresentModeKHR ChooseSwapPresentMode(
      const std::vector<vk::PresentModeKHR>& present_modes);
  vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
  void TransitionImageLayout(
      uint32_t image_index, vk::ImageLayout old_layout,
      vk::ImageLayout new_layout, vk::AccessFlags2 src_access_mask,
      vk::AccessFlags2 dst_access_mask, vk::PipelineStageFlags2 src_stage_mask,
      vk::PipelineStageFlags2 dst_stage_mask);

  core::ILogger* logger_;
  core::Window* window_;

  vk::raii::Context context_;
  vk::raii::Instance instance_;
  vk::raii::DebugUtilsMessengerEXT debug_messenger_;
  vk::raii::PhysicalDevice physical_device_;
  vk::raii::Device device_;
  vk::raii::Queue graphics_queue_;
  vk::raii::Queue present_queue_;
  vk::raii::SurfaceKHR surface_;
  vk::raii::SwapchainKHR swapchain_;
  std::vector<vk::Image> swapchain_images_;
  vk::SurfaceFormatKHR swapchain_surface_format_;
  vk::Extent2D swapchain_extent_;
  std::vector<vk::raii::ImageView> swapchain_image_views_;
  vk::raii::PipelineLayout pipeline_layout_;
  vk::raii::Pipeline graphics_pipeline_;
  vk::raii::CommandPool command_pool_;
  std::vector<vk::raii::CommandBuffer> command_buffers_;
  std::vector<vk::raii::Semaphore> present_complete_semaphores_;
  std::vector<vk::raii::Semaphore> render_finished_semaphores_;
  std::vector<vk::raii::Fence> in_flight_fences_;
  uint32_t current_frame_;
  vk::DebugUtilsMessengerCreateInfoEXT debug_utils_create_info_;

#ifdef NDEBUG
  static constexpr bool kEnableValidationErrors = false;
#else
  static constexpr bool kEnableValidationErrors = true;
#endif

  const std::vector<const char*> validation_layers_ = {
      "VK_LAYER_KHRONOS_validation"};
  const std::vector<const char*> required_device_extensions_ = {
      vk::KHRSwapchainExtensionName, vk::KHRSpirv14ExtensionName, vk::KHRSynchronization2ExtensionName, vk::KHRCreateRenderpass2ExtensionName};

  constexpr static uint32_t kMaxFramesInFlight = 2;
};

inline std::vector<char> ReadFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }
  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

}
#endif