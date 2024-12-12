#cmake_minimum_required(VERSION 3.12)

project(SubtoneDetector_Goertzel_Win C)

# Setze die C-Standardversion auf C11
#set(CMAKE_C_STANDARD 11)

add_executable(SubtoneDetector_Goertzel_Win subtonedetector_goertzel_win.c goertzelwin.c)

# pull in common dependencies
target_link_libraries(SubtoneDetector_Goertzel_Win pico_stdlib hardware_adc)

# create map/bin/hex file etc.
pico_add_extra_outputs(SubtoneDetector_Goertzel_Win)


