project(cp2102_usb)

set(SRCS "src/cp2102_libusb.c")

if(APPLE)
    list(APPEND SRCS "src/tty_utils_darwin.c")
elseif(UNIX)
    list(APPEND SRCS "src/tty_utils_linux.c")
else()
    list(APPEND SRCS "src/tty_utils_dummy.c")
endif()

add_library(${PROJECT_NAME} STATIC ${SRCS})

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} log)
target_link_libraries(${PROJECT_NAME} usb)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework CoreFoundation")
endif()
