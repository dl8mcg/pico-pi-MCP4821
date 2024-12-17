cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

include(pico_extras_import_optional.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()



# Initialize the SDK
pico_sdk_init()



# Add projects

add_subdirectory(mcp4821)
add_subdirectory(mcp4821_nonblocking)
add_subdirectory(mcp4821_nonblocking_timer_irq_sine_wave)
add_subdirectory(mcp4821_IIR_osc)
add_subdirectory(mcp4821_ADCtoDAC)
add_subdirectory(AF_Compressor)
add_subdirectory(SubtoneDetector_DFT)
add_subdirectory(SubtoneDetector_Goertzel_Win)
add_subdirectory(IIR_osc_int64)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        )
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()

# Hardware-specific examples in subdirectories:

