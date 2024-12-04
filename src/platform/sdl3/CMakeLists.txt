find_package(SDL3 CONFIG REQUIRED)

add_library(PlatformSDL3 "sdl_main.cpp" "../window.h" "../window.cpp")
target_include_directories(PlatformSDL3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(PlatformSDL3 PUBLIC PlatformInterface SDL3::SDL3)