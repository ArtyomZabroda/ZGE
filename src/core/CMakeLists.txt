add_library(Core "dummy.cpp" "util/extent.h")
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})