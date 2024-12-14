add_library(Components "dummy.cpp" "transform.h" "character_controller.h" "camera.h" "mesh_instance.h" "parent.h" "lights.h")
target_include_directories(Components PUBLIC .)
target_link_libraries(Components PUBLIC Core Renderer)