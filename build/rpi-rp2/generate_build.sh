#!/bin/bash
rm CMakeCache.txt
rm -r CMakeFiles/
rm -r elf2uf2/
rm -r generated/
rm -r pico_sdk/

cmake ../.. -DRPI_RP2=1 -DCMAKE_BUILD_TYPE=RELEASE -DPICO_PLATFORM=rp2040