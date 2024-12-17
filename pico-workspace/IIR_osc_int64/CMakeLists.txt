add_executable(IIR_osc_int64 IIR_osc_int64.c)

# pull in common dependencies
target_link_libraries(IIR_osc_int64 pico_stdlib hardware_spi)


# create map/bin/hex file etc.
pico_add_extra_outputs(IIR_osc_int64)


