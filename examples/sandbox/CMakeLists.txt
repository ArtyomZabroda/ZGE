add_executable(Sandbox "sandbox.cpp")
target_link_libraries(Sandbox PUBLIC Main PlatformSDL3)

add_custom_target(BuildVertexShader ALL COMMAND fxc "/T" "vs_5_0" "/Fo" "${CMAKE_CURRENT_BINARY_DIR}/vert.vso" "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/vert.hlsl")
add_custom_target(BuildPixelShader ALL COMMAND fxc "/T" "ps_5_0" "/Fo" "${CMAKE_CURRENT_BINARY_DIR}/pixel.pso" "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/pixel.hlsl")

add_dependencies(Renderer BuildVertexShader BuildPixelShader)