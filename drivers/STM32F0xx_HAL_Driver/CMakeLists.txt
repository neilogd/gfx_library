SET(SOURCE_FILES 
    Src/stm32f0xx_hal.c
    Src/stm32f0xx_hal_adc.c
    Src/stm32f0xx_hal_adc_ex.c
    Src/stm32f0xx_hal_comp.c
    Src/stm32f0xx_hal_cortex.c
    Src/stm32f0xx_hal_crc.c
    Src/stm32f0xx_hal_crc_ex.c
    Src/stm32f0xx_hal_dac.c
    Src/stm32f0xx_hal_dac_ex.c
    Src/stm32f0xx_hal_dma.c
    Src/stm32f0xx_hal_flash.c
    Src/stm32f0xx_hal_flash_ex.c
    Src/stm32f0xx_hal_gpio.c
    Src/stm32f0xx_hal_i2c.c
    Src/stm32f0xx_hal_i2c_ex.c
    Src/stm32f0xx_hal_i2s.c
    Src/stm32f0xx_hal_irda.c
    Src/stm32f0xx_hal_iwdg.c
    #Src/stm32f0xx_hal_msp_template.c
    Src/stm32f0xx_hal_pcd.c
    Src/stm32f0xx_hal_pcd_ex.c
    Src/stm32f0xx_hal_pwr.c
    Src/stm32f0xx_hal_pwr_ex.c
    Src/stm32f0xx_hal_rcc.c
    Src/stm32f0xx_hal_rcc_ex.c
    Src/stm32f0xx_hal_rtc.c
    Src/stm32f0xx_hal_rtc_ex.c
    Src/stm32f0xx_hal_smartcard.c
    Src/stm32f0xx_hal_smartcard_ex.c
    Src/stm32f0xx_hal_smbus.c
    Src/stm32f0xx_hal_spi.c
    Src/stm32f0xx_hal_tim.c
    Src/stm32f0xx_hal_tim_ex.c
    Src/stm32f0xx_hal_tsc.c
    Src/stm32f0xx_hal_uart.c
    Src/stm32f0xx_hal_uart_ex.c
    Src/stm32f0xx_hal_usart.c
    Src/stm32f0xx_hal_wwdg.c
    Src/stm32f0xx_ll_adc.c
    Src/stm32f0xx_ll_comp.c
    Src/stm32f0xx_ll_crc.c
    Src/stm32f0xx_ll_crs.c
    Src/stm32f0xx_ll_dac.c
    Src/stm32f0xx_ll_dma.c
    Src/stm32f0xx_ll_exti.c
    Src/stm32f0xx_ll_gpio.c
    Src/stm32f0xx_ll_i2c.c
    Src/stm32f0xx_ll_pwr.c
    Src/stm32f0xx_ll_rcc.c
    Src/stm32f0xx_ll_rtc.c
    Src/stm32f0xx_ll_spi.c
    Src/stm32f0xx_ll_tim.c
    Src/stm32f0xx_ll_usart.c
    Src/stm32f0xx_ll_utils.c
)

ADD_LIBRARY(STM32F0xx_HAL_Driver ${SOURCE_FILES})
TARGET_INCLUDE_DIRECTORIES(STM32F0xx_HAL_Driver PUBLIC
    Inc
    ${ROOT_PATH}/include
    ${ROOT_PATH}/drivers/CMSIS/Device/ST/STM32F0xx/Include
    ${ROOT_PATH}/drivers/CMSIS/Include
)
