#cmake_minimum_required(VERSION 3.12)

project(HP_IIR_int64 C)

# Setze die C-Standardversion auf C11
#set(CMAKE_C_STANDARD 11)

add_executable(HP_IIR_int64 HP_IIR_int64.c sampleprocessing.c hpiir.c dac.c)

# pull in common dependencies
target_link_libraries(HP_IIR_int64 pico_stdlib hardware_spi hardware_adc)

# create map/bin/hex file etc.
pico_add_extra_outputs(HP_IIR_int64)



