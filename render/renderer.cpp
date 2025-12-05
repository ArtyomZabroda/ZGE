#include "renderer.h"
#include <expected>
#include <type_traits>
#include <set>

namespace render {

static VKAPI_ATTR unsigned int VKAPI_CALL
DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              vk::DebugUtilsMessageTypeFlagsEXT messageType,
              const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void* pUserData) {
  core::ILogger* logger = reinterpret_cast<core::ILogger*>(pUserData);
  switch (messageSeverity) {
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
      logger->LogDebug(pCallbackData->pMessage);
      break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
      logger->LogDebug(pCallbackData->pMessage);
      break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
      logger->LogWarning(pCallbackData->pMessage);
      break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
      logger->LogError(pCallbackData->pMessage);
  }

  return VK_FALSE;
}

boost::leaf::result<Renderer> Renderer::Create(
    core::ILogger* logger,
    core::Window* window,
    const std::span<const char* const>& extension_required_by_window) { 
  Renderer renderer;
  renderer.logger_ = logger;
  renderer.window_ = window;

  BOOST_LEAF_CHECK(renderer.CreateInstance(extension_required_by_window));

  if (kEnableValidationErrors) {
    BOOST_LEAF_CHECK(renderer.SetupDebugMessenger());
  }

  BOOST_LEAF_CHECK(renderer.CreateSurface());

  BOOST_LEAF_CHECK(renderer.PickPhysicalDevice());  
  BOOST_LEAF_CHECK(renderer.CreateLogicalDevice());
  BOOST_LEAF_CHECK(renderer.CreateSwapchain());
  BOOST_LEAF_CHECK(renderer.CreateImageViews());
  BOOST_LEAF_CHECK(renderer.CreateGraphicsPipeline());
  BOOST_LEAF_CHECK(renderer.CreateCommandPool());
  BOOST_LEAF_CHECK(renderer.CreateCommandBuffers());
  BOOST_LEAF_CHECK(renderer.CreateSyncObjects());

  return renderer;
}

boost::leaf::result<void> Renderer::CreateInstance(
    const std::span<const char* const>& extensions_required_by_window) {
  if (kEnableValidationErrors) {
    debug_utils_create_info_ = CreateDebugMessengerCreateInfo();
  }
  

  if (kEnableValidationErrors &&
      !CheckValidationLevelSupport(validation_layers_)) {
    logger_->LogError("Validation layers requested, but not available! Continuing without validation layers");
  }

  ZGE_VK_AUTO(available_extensions, context_.enumerateInstanceExtensionProperties());
  
  for (uint32_t i{}; i < extensions_required_by_window.size(); ++i)
  {
    if (std::ranges::none_of(available_extensions,
            [required_extension_name = extensions_required_by_window[i]](
                const vk::ExtensionProperties& available_extension) {
              return strcmp(required_extension_name,
                            available_extension.extensionName) == 0;
            })) {
      return boost::leaf::new_error(
          std::format("Required Vulkan extension not supported: {}",
                      extensions_required_by_window[i]));
    }
  }

  constexpr vk::ApplicationInfo appInfo{.pApplicationName = "ZGE Game",
                              .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                              .pEngineName = "ZGE",
                              .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                              .apiVersion = vk::ApiVersion14};

  // This function takes extension
  std::vector<const char*> extensions = GetRequiredExtensions(extensions_required_by_window);

  vk::InstanceCreateInfo createInfo {
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = 0,
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      
      .ppEnabledExtensionNames = extensions.data(),
  };

  if (kEnableValidationErrors) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validation_layers_.size());
    createInfo.ppEnabledLayerNames = validation_layers_.data();
    createInfo.pNext =
        (VkDebugUtilsMessengerCreateInfoEXT*)&debug_utils_create_info_;
  } else {
    createInfo.enabledLayerCount = 0;
  }

  ZGE_VK_ASSIGN(instance_, context_.createInstance(createInfo));
  std::string log_msg =
      "Vulkan instance was created successfully with following extensions:\n";
  for (int i{}; i < extensions_required_by_window.size() - 1; ++i) {
    log_msg += std::string(extensions_required_by_window[i]) + ';';
  }
  log_msg += extensions_required_by_window[extensions_required_by_window.size() - 1];
  logger_->LogDebug(log_msg);

  return {};
}

boost::leaf::result<bool> Renderer::CheckValidationLevelSupport(
    std::span<const char* const> validation_layers) {
  ZGE_VK_AUTO(available_layers, context_.enumerateInstanceLayerProperties());

  for (const char* layer_name : validation_layers) {
    bool layer_found = false;

    for (const vk::LayerProperties& layer : available_layers) {
      if (strcmp(layer_name, layer.layerName) == 0) {
        layer_found = true;
        break;
      }
    }

    if (!layer_found) {
      return false;
    }
  }

  return true;
}

std::vector<const char*> Renderer::GetRequiredExtensions(
    std::span<const char* const> extension_names) {

  std::vector<const char*> extensions;
  extensions.assign_range(extension_names);

  if (kEnableValidationErrors) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

vk::DebugUtilsMessengerCreateInfoEXT Renderer::CreateDebugMessengerCreateInfo() {
  return {
      .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
      .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
      .pfnUserCallback = DebugCallback,
      .pUserData = logger_
  };
}

/**
 * @brief Selects the first suitable GPU
 * @return Suitable gpu handle
 */
boost::leaf::result<void> Renderer::PickPhysicalDevice() {
  ZGE_VK_AUTO(physical_devices, instance_.enumeratePhysicalDevices());
  if (physical_devices.empty()) {
    return boost::leaf::new_error("Failed to find GPUs with Vulkan support");
  }

  vk::raii::PhysicalDevice suitable_physical_device = nullptr;

  for (const auto& device : physical_devices) {
    if (IsDeviceSuitable(device)) {
      suitable_physical_device = device;
      break;
    }
  }

  if (suitable_physical_device == nullptr) {
    return boost::leaf::new_error("Failed to find a suitable GPU");
  }

  logger_->LogDebug(std::format("Found a suitable GPU"));
  logger_->LogDebug(std::format(
      "Name: {}", std::string(suitable_physical_device.getProperties().deviceName.data())));

  physical_device_ = suitable_physical_device;

  return {};
}

boost::leaf::result<bool> Renderer::IsDeviceSuitable(
    vk::raii::PhysicalDevice device) {
  vk::PhysicalDeviceProperties device_properties = device.getProperties();
  vk::PhysicalDeviceFeatures device_features = device.getFeatures();
  bool is_suitable =
      device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;

  QueueFamilyIndices indices = FindQueueFamilies(device);
  if (is_suitable) {
    is_suitable = indices.IsComplete();
  }
  if (is_suitable) {
    BOOST_LEAF_ASSIGN(is_suitable, CheckDeviceExtensionSupport(device));
  }
  if (is_suitable) {
    BOOST_LEAF_ASSIGN(is_suitable, IsSwapchainSuitable(device, surface_));
  }
  
  return is_suitable;
}

Renderer::QueueFamilyIndices Renderer::FindQueueFamilies(
    vk::raii::PhysicalDevice device) {
  QueueFamilyIndices indices;

  std::vector<vk::QueueFamilyProperties> queue_families = device.getQueueFamilyProperties();
  for (int i{}; i < queue_families.size(); ++i) {
    if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
      indices.graphics_family = i;
    }
    vk::ResultValue<vk::Bool32> surface_support = device.getSurfaceSupportKHR(i, surface_);
    if (surface_support.has_value() && surface_support.value == vk::True) {
      indices.present_family = i;
    }
    if (indices.IsComplete()) {
      break;
    }
  }


  return indices;
}

boost::leaf::result<void> Renderer::CreateLogicalDevice() {
  QueueFamilyIndices indices = FindQueueFamilies(physical_device_);

  // picked physical device should have all indices
  // from QueueFamilyIndices initialized and not empty
  assert(indices.IsComplete());

  float queue_priority = 1.0f;

  vk::DeviceQueueCreateInfo device_queue_create_info{
      .queueFamilyIndex = indices.graphics_family.value(),
      .queueCount = 1,
      .pQueuePriorities = &queue_priority
  };

  vk::StructureChain<vk::PhysicalDeviceFeatures2,
                     vk::PhysicalDeviceVulkan11Features,
                     vk::PhysicalDeviceVulkan13Features,
                     vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
      feature_chain{
          vk::PhysicalDeviceFeatures2{
          },
                    vk::PhysicalDeviceVulkan11Features{
                .shaderDrawParameters = true
          },
                    vk::PhysicalDeviceVulkan13Features{
                        .synchronization2 = true, .dynamicRendering = true},
          vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT{
              .extendedDynamicState = true}};

  std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
  std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(),
                                            indices.present_family.value()};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : unique_queue_families) {
    vk::DeviceQueueCreateInfo queue_create_info{};
    queue_create_info.queueFamilyIndex = queueFamily;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queuePriority;
    queue_create_infos.push_back(queue_create_info);
  }

  vk::DeviceCreateInfo device_create_info{
      .pNext = feature_chain.get<vk::PhysicalDeviceFeatures2>(),
      .queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
      .pQueueCreateInfos = queue_create_infos.data(),
      .enabledExtensionCount =
          static_cast<uint32_t>(required_device_extensions_.size()),
      .ppEnabledExtensionNames = required_device_extensions_.data()};

  if (kEnableValidationErrors) {
    device_create_info.enabledLayerCount =
        static_cast<uint32_t>(validation_layers_.size());
    device_create_info.ppEnabledLayerNames = validation_layers_.data();
  } else {
    device_create_info.enabledLayerCount = 0;
  }

  ZGE_VK_ASSIGN(device_, physical_device_.createDevice(device_create_info));

  graphics_queue_ = device_.getQueue(indices.graphics_family.value(), 0);
  present_queue_ = device_.getQueue(indices.present_family.value(), 0);

  return {};
}

boost::leaf::result<void> Renderer::SetupDebugMessenger() {
  ZGE_VK_ASSIGN(debug_messenger_,
                instance_.createDebugUtilsMessengerEXT(
                    debug_utils_create_info_));

  return {};
}

boost::leaf::result<void> Renderer::CreateSurface() {
  BOOST_LEAF_AUTO(surface, window_->CreateSurface(static_cast<vk::Instance>(instance_)));
  surface_ = vk::raii::SurfaceKHR(instance_, surface);
  return {};
}

boost::leaf::result<bool> Renderer::CheckDeviceExtensionSupport(
    vk::raii::PhysicalDevice device) {
  ZGE_VK_AUTO(available_extensions, device.enumerateDeviceExtensionProperties());

  for (const char* required_extension : required_device_extensions_) {
    if (std::find_if(available_extensions.begin(), available_extensions.end(),
                     [&required_extension](vk::ExtensionProperties available_extension) {
              return strcmp(available_extension.extensionName, required_extension) == 0;
                     }) ==
        available_extensions.end()) {
      return false;
    }
  }
  return true;
}

boost::leaf::result<Renderer::SwapchainSupportDetails> Renderer::QuerySwapchainSupport(vk::raii::PhysicalDevice& device, vk::raii::SurfaceKHR& surface) {
  SwapchainSupportDetails details;

  ZGE_VK_ASSIGN(details.capabilities, device.getSurfaceCapabilitiesKHR(surface));
  ZGE_VK_ASSIGN(details.formats,
                device.getSurfaceFormatsKHR(surface));
  ZGE_VK_ASSIGN(details.present_modes, device.getSurfacePresentModesKHR(surface));
  return details;
}

boost::leaf::result<bool>
Renderer::IsSwapchainSuitable(vk::raii::PhysicalDevice& device,
                              vk::raii::SurfaceKHR& surface) {
  BOOST_LEAF_AUTO(swapchain_support, QuerySwapchainSupport(device, surface));
  return !swapchain_support.formats.empty() &&
         !swapchain_support.present_modes.empty();
}

vk::SurfaceFormatKHR Renderer::ChooseSwapSurfaceFormat(
  const std::vector<vk::SurfaceFormatKHR>& formats) {
  for (const vk::SurfaceFormatKHR& format : formats) {
    if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
      return format;
    }
  }
  logger_->LogError(
      "Can't find a preferred surface format. Falling back to the first "
      "exposed format. Rendering may be incorrect");
  return formats[0];
}

vk::PresentModeKHR Renderer::ChooseSwapPresentMode(
  const std::vector<vk::PresentModeKHR>& present_modes) {
  for (const auto& availablePresentMode : present_modes) {
    if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
      return availablePresentMode;
    }
  }

  return vk::PresentModeKHR::eFifo;
}

vk::Extent2D Renderer::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
  return capabilities.currentExtent;
  // TODO: add high dpi display support
}

boost::leaf::result<void> Renderer::CreateSwapchain() { 
  BOOST_LEAF_AUTO(swapchain_support, QuerySwapchainSupport(physical_device_, surface_));
  vk::SurfaceFormatKHR surface_format =
      ChooseSwapSurfaceFormat(swapchain_support.formats);
  vk::PresentModeKHR present_mode =
      ChooseSwapPresentMode(swapchain_support.present_modes);
  vk::Extent2D extent = ChooseSwapExtent(swapchain_support.capabilities);

  uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;

  // maxImageCount == 0 means that there is no maximum
  if (swapchain_support.capabilities.maxImageCount > 0 &&
      image_count > swapchain_support.capabilities.maxImageCount) {
    image_count = swapchain_support.capabilities.maxImageCount;
  }

  vk::SwapchainCreateInfoKHR create_info{
      .surface = surface_,
      .minImageCount = image_count,
      .imageFormat = surface_format.format,
      .imageColorSpace = surface_format.colorSpace,
      .imageExtent = extent,
      .imageArrayLayers = 1,
      .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
      .preTransform = swapchain_support.capabilities.currentTransform,
      .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
      .presentMode = present_mode,
      .clipped = vk::True,
      .oldSwapchain = nullptr};


  QueueFamilyIndices indices = FindQueueFamilies(physical_device_);
  uint32_t queue_family_indices[] = {indices.graphics_family.value(),
                                     indices.present_family.value()};
  if (indices.graphics_family != indices.present_family) {
    create_info.imageSharingMode = vk::SharingMode::eConcurrent;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    create_info.imageSharingMode = vk::SharingMode::eExclusive;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
  }

  ZGE_VK_ASSIGN(swapchain_, device_.createSwapchainKHR(create_info));

  ZGE_VK_ASSIGN(swapchain_images_, swapchain_.getImages());
  swapchain_surface_format_ = surface_format;
  swapchain_extent_ = extent;
  return {};
}

boost::leaf::result<void> Renderer::CreateImageViews() {
  swapchain_image_views_.reserve(swapchain_images_.size());
  for (std::size_t i{}; i < swapchain_images_.size(); ++i) {
    vk::ImageViewCreateInfo create_info{
        .image = swapchain_images_[i],
        .viewType = vk::ImageViewType::e2D,
        .format = swapchain_surface_format_.format,
        .components = {
          .r = vk::ComponentSwizzle::eIdentity,
          .g = vk::ComponentSwizzle::eIdentity,
          .b = vk::ComponentSwizzle::eIdentity,
          .a = vk::ComponentSwizzle::eIdentity},
        .subresourceRange = {
          .aspectMask = vk::ImageAspectFlagBits::eColor,
          .baseMipLevel = 0,
          .levelCount = 1,
          .baseArrayLayer = 0,
          .layerCount = 1
        }};
    ZGE_VK_AUTO(image_view, device_.createImageView(create_info));
    swapchain_image_views_.emplace_back(std::move(image_view));
  }
  return {};
}

boost::leaf::result<void> Renderer::CreateGraphicsPipeline() {
  std::vector<char> shader_code = ReadFile("assets/default.spv");

  ZGE_VK_AUTO(
      vertex_shader,
      device_.createShaderModule(
          {.codeSize = shader_code.size(),
           .pCode = reinterpret_cast<const uint32_t*>(shader_code.data())}));


  vk::PipelineShaderStageCreateInfo vert_shader_stage_create_info{
      .stage = vk::ShaderStageFlagBits::eVertex,
      .module = vertex_shader,
      .pName = "vertexMain"
  };
  vk::PipelineShaderStageCreateInfo frag_shader_stage_create_info{
      .stage = vk::ShaderStageFlagBits::eFragment,
      .module = vertex_shader,
      .pName = "fragmentMain"};

  vk::PipelineShaderStageCreateInfo shader_stages_create_info[] = {
      vert_shader_stage_create_info, frag_shader_stage_create_info};

  std::vector<vk::DynamicState> dynamic_states = {vk::DynamicState::eViewport,
                                                  vk::DynamicState::eScissor};
  vk::PipelineDynamicStateCreateInfo dynamic_state_create_info{
      .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
      .pDynamicStates = dynamic_states.data()};

  vk::PipelineVertexInputStateCreateInfo vertex_input_info{
      .vertexBindingDescriptionCount = 0,
      .pVertexBindingDescriptions = nullptr,
      .vertexAttributeDescriptionCount = 0,
      .pVertexAttributeDescriptions = nullptr};

  vk::PipelineInputAssemblyStateCreateInfo input_assembly_create_info{
      .topology = vk::PrimitiveTopology::eTriangleList,
      .primitiveRestartEnable = vk::False};

  vk::Viewport viewport{
      .x = 0.0f,
      .y = 0.0f,
      .width = static_cast<float>(swapchain_extent_.width),
      .height = static_cast<float>(swapchain_extent_.height),
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };

  vk::Rect2D scissor{.offset = {0, 0}, .extent = swapchain_extent_};

  vk::PipelineViewportStateCreateInfo viewport_state_create_info{
      .viewportCount = 1, .pViewports = &viewport, .scissorCount = 1, .pScissors = &scissor};

  vk::PipelineRasterizationStateCreateInfo rasterizer_create_info{
    .depthClampEnable = vk::False,
    .rasterizerDiscardEnable = vk::False,
    .polygonMode = vk::PolygonMode::eFill,
    .cullMode = vk::CullModeFlagBits::eBack,
    .frontFace = vk::FrontFace::eClockwise,
    .depthBiasConstantFactor = 0.0f,
    .depthBiasClamp = 0.0f,
    .depthBiasSlopeFactor = 0.0f,
    .lineWidth = 1.0f
  };

  vk::PipelineMultisampleStateCreateInfo multisampling_create_info{
      .rasterizationSamples = vk::SampleCountFlagBits::e1,
      .sampleShadingEnable = vk::False,
      .minSampleShading = 1.0f,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = vk::False,
      .alphaToOneEnable = vk::False};

  vk::PipelineColorBlendAttachmentState color_blend_attachemnt_create_info{
      .blendEnable = vk::False,
      .colorWriteMask =
          vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
      };

  vk::PipelineColorBlendStateCreateInfo color_blending_create_info{
      .logicOpEnable = vk::False,
      .attachmentCount = 1,
      .pAttachments = &color_blend_attachemnt_create_info};

  vk::PipelineLayoutCreateInfo pipeline_layout_create_info{};

  ZGE_VK_ASSIGN(pipeline_layout_,
                device_.createPipelineLayout(pipeline_layout_create_info));

  vk::PipelineRenderingCreateInfo pipeline_rendering_create_info{
      .colorAttachmentCount = 1,
      .pColorAttachmentFormats = &swapchain_surface_format_.format};

  vk::GraphicsPipelineCreateInfo create_info{
      .pNext = &pipeline_rendering_create_info,
      .stageCount = 2,
      .pStages = shader_stages_create_info,
      .pVertexInputState = &vertex_input_info,
      .pInputAssemblyState = &input_assembly_create_info,
      .pViewportState = &viewport_state_create_info,
      .pRasterizationState = &rasterizer_create_info,
      .pMultisampleState = &multisampling_create_info,
      .pDepthStencilState = nullptr,
      .pColorBlendState = &color_blending_create_info,
      .pDynamicState = &dynamic_state_create_info,
      .layout = pipeline_layout_,
      .renderPass = nullptr,
      .subpass = 0,
      .basePipelineHandle = nullptr,
      .basePipelineIndex = -1};

  ZGE_VK_ASSIGN(graphics_pipeline_,
                device_.createGraphicsPipeline(nullptr, create_info));

  return {};
}

boost::leaf::result<void> Renderer::CreateCommandPool() {
  QueueFamilyIndices queue_family_indices = FindQueueFamilies(physical_device_);
  vk::CommandPoolCreateInfo create_info{
      .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      .queueFamilyIndex = queue_family_indices.graphics_family.value()};
  ZGE_VK_ASSIGN(command_pool_, device_.createCommandPool(create_info));
  return {};
}

boost::leaf::result<void> Renderer::CreateCommandBuffers() {
  vk::CommandBufferAllocateInfo alloc_info{
    .commandPool = command_pool_,
    .level = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = kMaxFramesInFlight
  };
  ZGE_VK_ASSIGN(command_buffers_, device_.allocateCommandBuffers(alloc_info));
  return {};
}

boost::leaf::result<void> Renderer::RecordCommandBuffer(uint32_t image_index) {
  vk::Result begin_result = command_buffers_[current_frame_].begin({});
  if (begin_result != vk::Result::eSuccess) {
    return boost::leaf::new_error(begin_result);
  }

  TransitionImageLayout(image_index, vk::ImageLayout::eUndefined,
                        vk::ImageLayout::eColorAttachmentOptimal, {},
                        vk::AccessFlagBits2::eColorAttachmentWrite,
                        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                        vk::PipelineStageFlagBits2::eColorAttachmentOutput);
  vk::ClearValue clear_color = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
  vk::RenderingAttachmentInfo attachment_info{
      .imageView = swapchain_image_views_[image_index],
      .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = clear_color};

  vk::RenderingInfo rendering_info{
      .renderArea = {.offset = {0, 0}, .extent = swapchain_extent_},
      .layerCount = 1,
      .colorAttachmentCount = 1,
      .pColorAttachments = &attachment_info};

  command_buffers_[current_frame_].beginRendering(rendering_info);

  command_buffers_[current_frame_].bindPipeline(
      vk::PipelineBindPoint::eGraphics,
                               graphics_pipeline_);
  command_buffers_[current_frame_].setViewport(
      0,
      vk::Viewport{0.0f, 0.0f, static_cast<float>(swapchain_extent_.width),
                   static_cast<float>(swapchain_extent_.height), 0.0f, 1.0f});
  command_buffers_[current_frame_].setScissor(
      0,
                             vk::Rect2D{vk::Offset2D{0, 0}, swapchain_extent_});
  command_buffers_[current_frame_].draw(3, 1, 0, 0);

  command_buffers_[current_frame_].endRendering();

  TransitionImageLayout(image_index, vk::ImageLayout::eColorAttachmentOptimal,
                        vk::ImageLayout::ePresentSrcKHR,
                        vk::AccessFlagBits2::eColorAttachmentWrite, {},
                        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                        vk::PipelineStageFlagBits2::eBottomOfPipe);

  vk::Result end_result = command_buffers_[current_frame_].end();
  if (end_result != vk::Result::eSuccess) {
    return boost::leaf::new_error(end_result);
  }
}

void Renderer::TransitionImageLayout(
  uint32_t image_index, vk::ImageLayout old_layout,
  vk::ImageLayout new_layout, vk::AccessFlags2 src_access_mask,
  vk::AccessFlags2 dst_access_mask, vk::PipelineStageFlags2 src_stage_mask,
  vk::PipelineStageFlags2 dst_stage_mask) {
  vk::ImageMemoryBarrier2 barrier = {
      .srcStageMask = src_stage_mask,
      .srcAccessMask = src_access_mask,
      .dstStageMask = dst_stage_mask,
      .dstAccessMask = dst_access_mask,
      .oldLayout = old_layout,
      .newLayout = new_layout,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = swapchain_images_[image_index],
      .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                           .baseMipLevel = 0,
                           .levelCount = 1,
                           .baseArrayLayer = 0,
                           .layerCount = 1}};
  vk::DependencyInfo dependencyInfo = {.dependencyFlags = {},
                                       .imageMemoryBarrierCount = 1,
                                       .pImageMemoryBarriers = &barrier};
  command_buffers_[current_frame_].pipelineBarrier2(dependencyInfo);
}

boost::leaf::result<void> Renderer::CreateSyncObjects() {
  present_complete_semaphores_.clear();
  render_finished_semaphores_.clear();
  in_flight_fences_.clear();

  for (size_t i{}; i < kMaxFramesInFlight; ++i) {
    ZGE_VK_AUTO(present_complete_semaphore,
                device_.createSemaphore(vk::SemaphoreCreateInfo()));
    present_complete_semaphores_.push_back(
        std::move(present_complete_semaphore));
    ZGE_VK_AUTO(render_finished_semaphore,
                device_.createSemaphore(vk::SemaphoreCreateInfo()));
    render_finished_semaphores_.push_back(std::move(render_finished_semaphore));
    ZGE_VK_AUTO(in_flight_fence,
                  device_.createFence(vk::FenceCreateInfo{
                      .flags = vk::FenceCreateFlagBits::eSignaled}));
    in_flight_fences_.push_back(std::move(in_flight_fence));
  }

  return {};
}

boost::leaf::result<void> Renderer::DrawFrame() {
  while (vk::Result::eTimeout ==
         device_.waitForFences(*in_flight_fences_[current_frame_], vk::True, UINT64_MAX));
  ZGE_VK_AUTO(image_index, swapchain_.acquireNextImage(
                               UINT64_MAX, *present_complete_semaphores_[current_frame_]));
  device_.resetFences(*in_flight_fences_[current_frame_]);
  command_buffers_[current_frame_].reset();
  RecordCommandBuffer(image_index);
  

  vk::PipelineStageFlags wait_destination_stage_mask =
      vk::PipelineStageFlagBits::eColorAttachmentOutput;
  vk::SubmitInfo submit_info{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &*present_complete_semaphores_[current_frame_],
    .pWaitDstStageMask = &wait_destination_stage_mask,
      .commandBufferCount = 1,
      .pCommandBuffers = &*command_buffers_[current_frame_],
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &*render_finished_semaphores_[current_frame_]
  };
  vk::Result submit_result =
      graphics_queue_.submit(submit_info, *in_flight_fences_[current_frame_]);
  if (submit_result != vk::Result::eSuccess) {
    return boost::leaf::new_error(submit_result);
  }

  vk::PresentInfoKHR present_info{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &*render_finished_semaphores_[current_frame_],
      .swapchainCount = 1,
      .pSwapchains = &*swapchain_,
      .pImageIndices = &image_index};

  vk::Result present_result = present_queue_.presentKHR(present_info);
  if (present_result != vk::Result::eSuccess) {
    return boost::leaf::new_error(present_result);
  }

  current_frame_ = (current_frame_ + 1) % kMaxFramesInFlight;

  return {};
}

Renderer::~Renderer() {
  if (device_ != nullptr) { // after move constructor it can be null
    device_.waitIdle();
  }
  
}

}