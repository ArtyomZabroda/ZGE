add_library(Editor "editor.cpp")
target_include_directories(Editor PUBLIC .)
target_link_libraries(Editor PUBLIC ImGui Main)