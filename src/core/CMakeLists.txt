find_package(Boost REQUIRED COMPONENTS signals2)
find_package(glog CONFIG REQUIRED)
find_package(directxmath CONFIG REQUIRED)

add_library(Core "dummy.cpp" "extent.h" "signal.h" "log.h" "log.cpp"  )
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Core PUBLIC Boost::signals2 glog::glog Microsoft::DirectXMath)