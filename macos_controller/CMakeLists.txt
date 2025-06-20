cmake_minimum_required(VERSION 3.16)
project(TeensyLEDController)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to libserialport
set(LIBSERIALPORT_INCLUDE_DIR "/Users/jmillard/libdof/third-party/include")
set(LIBSERIALPORT_LIB_DIR "/Users/jmillard/libdof/third-party/runtime-libs/macos/arm64")

# Include directories
include_directories(${LIBSERIALPORT_INCLUDE_DIR})

# Find libserialport
find_library(LIBSERIALPORT_LIBRARY
    NAMES serialport libserialport
    PATHS ${LIBSERIALPORT_LIB_DIR}
    NO_DEFAULT_PATH
)

if(NOT LIBSERIALPORT_LIBRARY)
    message(FATAL_ERROR "libserialport not found in ${LIBSERIALPORT_LIB_DIR}")
endif()

message(STATUS "Found libserialport: ${LIBSERIALPORT_LIBRARY}")

# Add executable
add_executable(teensy_led_controller
    src/main.cpp
    src/teensy_controller.cpp
    src/teensy_controller.h
)

# Link libraries
target_link_libraries(teensy_led_controller ${LIBSERIALPORT_LIBRARY})

# Set RPATH for runtime library loading
set_target_properties(teensy_led_controller PROPERTIES
    INSTALL_RPATH "${LIBSERIALPORT_LIB_DIR}"
    BUILD_WITH_INSTALL_RPATH TRUE
)