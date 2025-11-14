add_executable(Sandbox "sandbox.cpp")
target_link_libraries(Sandbox PUBLIC Main)
target_compile_options(Sandbox PUBLIC "/fsanitize=address")