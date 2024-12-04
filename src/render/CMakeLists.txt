find_package(directxtk CONFIG REQUIRED)

add_library(Renderer "renderer.h" "renderer.cpp" "render_mesh.h" "render_mesh.cpp")
target_include_directories(Renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Renderer PUBLIC d3d11 Core PlatformInterface Microsoft::DirectXTK)

