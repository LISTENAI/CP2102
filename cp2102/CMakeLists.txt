project(cp2102)

set(SRCS "src/main.c")

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME} log)
target_link_libraries(${PROJECT_NAME} cp2102_usb)

execute_process(COMMAND git describe --tags --abbrev=0 OUTPUT_VARIABLE GIT_TAG OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE GIT_COMMIT OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git rev-list --count HEAD OUTPUT_VARIABLE GIT_INCREMENT OUTPUT_STRIP_TRAILING_WHITESPACE)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    -DGIT_TAG="${GIT_TAG}"
    -DGIT_COMMIT="${GIT_COMMIT}"
    -DGIT_INCREMENT=${GIT_INCREMENT}
)

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE_DIR:cp2102_usb>/CP210xRuntime.dll
        ${PROJECT_BINARY_DIR}
    )
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
