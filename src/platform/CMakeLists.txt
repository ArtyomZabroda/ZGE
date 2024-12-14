add_library(PlatformInterface "dummy.cpp" )
target_include_directories(PlatformInterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(PlatformInterface PUBLIC Core Dbg)
add_subdirectory("sdl3")