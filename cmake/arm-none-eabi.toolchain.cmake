# this one is important
SET(CMAKE_SYSTEM_NAME arm-bare)

# this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_C_COMPILER arm-none-eabi-gcc CACHE STRING "C Compiler")
SET(CMAKE_CXX_COMPILER arm-none-eabi-g++ CACHE STRING "C++ Compiler")
SET(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/Modules")

IF(DEFINED STM32_FAMILY)
    IF(${STM32_FAMILY} STREQUAL "F0")
        SET(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs --specs=nano.specs -mthumb -mcpu=cortex-m0plus -march=armv6-m --verbose -Wl,--gc-sections -ffunction-sections -fdata-sections -fno-exceptions")
        ADD_COMPILE_OPTIONS(-mthumb -mcpu=cortex-m0plus -march=armv6-m -ggdb -fno-exceptions)
    ELSEIF(${STM32_FAMILY} STREQUAL "F1")
        SET(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs --specs=nano.specs -mthumb -mcpu=cortex-m3 -march=armv7-m --verbose -Wl,--gc-sections -ffunction-sections -fdata-sections -fno-exceptions")
        ADD_COMPILE_OPTIONS(-mthumb -mcpu=cortex-m3 -march=armv7-m -ggdb -fno-exceptions)
    ELSEIF(${STM32_FAMILY} STREQUAL "F4")
        SET(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs --specs=nano.specs -mthumb -mcpu=cortex-m4 -march=armv7-m --verbose -Wl,--gc-sections -ffunction-sections -fdata-sections -fno-exceptions")
        ADD_COMPILE_OPTIONS(-mthumb -mcpu=cortex-m4 -march=armv7-m -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ggdb -fno-exceptions)
    ELSE()
        MESSAGE("STM32_FAMILY \"${STM32_FAMILY}\" unsupported!")
    ENDIF()
ENDIF()
