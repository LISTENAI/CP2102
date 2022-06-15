project(cp2102_usb)

set(SRCS)

if(WIN32)
    list(APPEND SRCS "src/cp2102_win32.c")
else()
    list(APPEND SRCS "src/cp2102_libusb.c")
endif()

if(APPLE)
    list(APPEND SRCS "src/tty_utils_darwin.c")
elseif(UNIX)
    list(APPEND SRCS "src/tty_utils_linux.c")
elseif(NOT WIN32)
    list(APPEND SRCS "src/tty_utils_dummy.c")
endif()

add_library(${PROJECT_NAME} STATIC ${SRCS})

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} log)

if(WIN32)
    set(CP210XRT_DIR ${PROJECT_SOURCE_DIR}/cp210xrt)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CP210XRT_DIR}/CP210xRuntime.dll
        ${PROJECT_BINARY_DIR}
    )

    target_include_directories(${PROJECT_NAME} PRIVATE ${CP210XRT_DIR})
    target_link_libraries(${PROJECT_NAME} ${CP210XRT_DIR}/CP210xRuntime.dll)
else()
    target_link_libraries(${PROJECT_NAME} usb)
endif()

if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework CoreFoundation")
endif()
