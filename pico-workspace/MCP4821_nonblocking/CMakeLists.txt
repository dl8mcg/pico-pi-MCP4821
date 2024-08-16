add_executable(mcp4821_nonblocking
        mcp4821_nonblocking.c
        )

# pull in common dependencies
target_link_libraries(mcp4821_nonblocking pico_stdlib hardware_spi)


# create map/bin/hex file etc.
pico_add_extra_outputs(mcp4821_nonblocking)


