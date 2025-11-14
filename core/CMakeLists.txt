find_package(SDL3 CONFIG REQUIRED)

add_library(Core "dummy.cpp")
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Core PUBLIC SDL3::SDL3)