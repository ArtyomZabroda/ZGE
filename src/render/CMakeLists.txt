find_package(directxtk CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)

add_library(Renderer "renderer.h" "renderer.cpp" "render_mesh.h" "render_mesh.cpp" "render_camera.h" "render_camera.cpp" "mesh_geometry.h"  "mesh_geometry.cpp" "texture_manager.h" "texture_manager.cpp" "texture.h" "texture.cpp")
target_include_directories(Renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Renderer PUBLIC d3d11 Core PlatformInterface Components Microsoft::DirectXTK assimp::assimp Dbg)
target_include_directories(Renderer PRIVATE ${Stb_INCLUDE_DIR})

