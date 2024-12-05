find_package(Boost REQUIRED COMPONENTS signals2)
find_package(glog CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(Core "dummy.cpp" "extent.h" "signal.h" "log.h" "log.cpp"   "utils.h" "utils.cpp" "key.h" "mouse_motion.h")
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Core PUBLIC Boost::signals2 glog::glog EnTT::EnTT glm::glm)