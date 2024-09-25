add_executable(mcp4821_ADCtoDAC
        mcp4821_ADCtoDAC.c
        )

# pull in common dependencies
target_link_libraries(mcp4821_ADCtoDAC pico_stdlib hardware_spi hardware_adc)


# create map/bin/hex file etc.
pico_add_extra_outputs(mcp4821_ADCtoDAC)


