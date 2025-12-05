find_package(SDL3 CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS log leaf url)
find_package(Vulkan)
find_package(GTest CONFIG REQUIRED)

add_library(Core "dummy.cpp" "log.h" "log.cpp" "utils.h" "window.h" "window.cpp" "input.h" "input.cpp" "resource.h" "resource.cpp")
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Core PUBLIC SDL3::SDL3 Boost::log Boost::leaf Boost::url Vulkan::Vulkan)

add_executable(
  CoreTest
  "resource_test.cpp"
)
target_link_libraries(
  CoreTest
  GTest::gtest_main
  Core
)

include(GoogleTest)
gtest_discover_tests(CoreTest)