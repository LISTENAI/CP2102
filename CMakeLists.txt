cmake_minimum_required(VERSION 3.10)

project(cp2102)

add_subdirectory(cp2102)
add_subdirectory(libcp2102_usb)
add_subdirectory(liblog)

if(NOT WIN32)
    add_subdirectory(libusb)
endif()
