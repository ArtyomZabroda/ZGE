find_package(SDL3)
add_library(ImGui "imgui.h" "imgui.cpp" "imgui_demo.cpp" "imgui_demo.cpp" "imgui_draw.cpp" "imgui_internal.h" "imgui_tables.cpp" "imgui_widgets.cpp" "imstb_rectpack.h" "imstb_textedit.h" "imstb_truetype.h" "backends/imgui_impl_dx11.cpp" "backends/imgui_impl_sdl3.cpp")
target_include_directories(ImGui PUBLIC . backends)
target_link_libraries(ImGui PUBLIC SDL3::SDL3 d3d11)
