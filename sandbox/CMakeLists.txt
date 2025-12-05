find_package(slang CONFIG REQUIRED)

add_executable(Sandbox "sandbox.cpp")
target_link_libraries(Sandbox PUBLIC Main)

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
file(GLOB SLANG_SHADERS "${SHADER_DIR}/*.slang")
set(COMPILED_SHADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/assets)
file(MAKE_DIRECTORY ${COMPILED_SHADER_DIR})

set(COMPILED_OUTPUTS "")

foreach(SHADER_FILE ${SLANG_SHADERS})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)

    # Output file name (example: shader.slang → shader.spv)
    set(OUTPUT_FILE "${COMPILED_SHADER_DIR}/${SHADER_NAME}.spv")

    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND slang::slangc
                ${SHADER_FILE}
                -target spirv -o ${OUTPUT_FILE}
        DEPENDS ${SHADER_FILE}
        COMMENT "Compiling Slang shader ${SHADER_NAME}"
    )
    list(APPEND COMPILED_OUTPUTS ${OUTPUT_FILE})
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${COMPILED_OUTPUTS})
add_dependencies(Sandbox CompileShaders)