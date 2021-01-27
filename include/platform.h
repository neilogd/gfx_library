#pragma once

#include <stdint.h>

#if defined(PLATFORM_STM32)
#  include "stm32f1xx.h"
#  include "stm32f1xx_hal.h"


//#  define USE_CRC_PERIPHERAL

#elif defined(PLATFORM_RPI_RP2)
#  include "pico/stdlib.h"
#  include "hardware/spi.h"
#endif

