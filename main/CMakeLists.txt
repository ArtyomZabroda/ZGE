find_package(SDL3 CONFIG REQUIRED)

add_library(Main "main.cpp")
target_link_libraries(Main PUBLIC Core SDL3::SDL3)