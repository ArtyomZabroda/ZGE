add_library(PlatformInterface "dummy.cpp" "window.h")
target_include_directories(PlatformInterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(PlatformInterface PUBLIC Core)

add_subdirectory("windows")