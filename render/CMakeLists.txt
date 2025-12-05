find_package(Vulkan)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Render "renderer.h" "renderer.cpp"   "render_minimal.h")
target_link_libraries(Render PUBLIC Vulkan::Vulkan Core)
target_compile_definitions(Render PUBLIC "VULKAN_HPP_NO_EXCEPTIONS" "VULKAN_HPP_NO_STRUCT_CONSTRUCTORS")
target_include_directories(Render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})