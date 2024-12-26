#cmake_minimum_required(VERSION 3.12)

project(subtone_detect_int64 C)

# Setze die C-Standardversion auf C11
#set(CMAKE_C_STANDARD 11)

add_executable(subtone_detect_int64 subtone_detect_int64.c sampleprocessing.c detect_subtone.c)

# pull in common dependencies
target_link_libraries(subtone_detect_int64 pico_stdlib hardware_spi hardware_adc)

# create map/bin/hex file etc.
pico_add_extra_outputs(subtone_detect_int64)



