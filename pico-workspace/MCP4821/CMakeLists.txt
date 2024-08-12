add_executable(mcp4821
        mcp4821.c
        )

# pull in common dependencies
target_link_libraries(mcp4821 pico_stdlib hardware_i2c hardware_spi)


# create map/bin/hex file etc.
pico_add_extra_outputs(mcp4821)


