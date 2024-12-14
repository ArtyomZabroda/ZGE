add_library(Main "main.cpp" "engine.h" "engine.cpp"   )
target_include_directories(Main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Main PUBLIC PlatformSDL3 Renderer Components ImGui Dbg)