#cmake_minimum_required(VERSION 3.12)

project(SubtoneDetector_DFT C)

# Setze die C-Standardversion auf C11
#set(CMAKE_C_STANDARD 11)

add_executable(SubtoneDetector_DFT subtonedetector_DFT.c dft.c)

# pull in common dependencies
target_link_libraries(SubtoneDetector_DFT pico_stdlib hardware_adc)

# create map/bin/hex file etc.
pico_add_extra_outputs(SubtoneDetector_DFT)


