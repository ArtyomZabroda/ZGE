find_package(SDL3 CONFIG REQUIRED)

add_library(Main "main.cpp" "engine.h" "engine.cpp")
target_link_libraries(Main PUBLIC Core Render SDL3::SDL3)