add_library(Scene "dummy.cpp")
target_include_directories(Scene PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Scene PUBLIC Core Render)