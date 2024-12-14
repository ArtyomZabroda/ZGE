find_package(SDL3 CONFIG REQUIRED)
add_library(Dbg "debug_layer.h" "debug_layer.cpp")
target_include_directories(Dbg PUBLIC .)
target_link_libraries(Dbg PUBLIC ImGui SDL3::SDL3 d3d11 Core)