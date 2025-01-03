#cmake_minimum_required(VERSION 3.12)

project(fsk_rx C)

# Setze die C-Standardversion auf C11
#set(CMAKE_C_STANDARD 11)

add_executable(fsk_rx fsk_rx.c sampleprocessing.c fsk_demod.c dac.c fsk_decode.c)

# pull in common dependencies
target_link_libraries(fsk_rx pico_stdlib hardware_adc hardware_spi)

# create map/bin/hex file etc.
pico_add_extra_outputs(fsk_rx)



