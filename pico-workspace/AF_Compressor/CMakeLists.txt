add_executable(af_compressor
        af_compressor.c
        )

# pull in common dependencies
target_link_libraries(af_compressor pico_stdlib hardware_spi hardware_adc)


# create map/bin/hex file etc.
pico_add_extra_outputs(af_compressor)


