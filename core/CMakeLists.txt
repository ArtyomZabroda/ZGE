find_package(SDL3 CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS log leaf)

add_library(Core "dummy.cpp" "log.h" "log.cpp" "utils.h" "window.h" "window.cpp" "input.h" "input.cpp")
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Core PUBLIC SDL3::SDL3 Boost::log Boost::leaf)