add_library(cmsis INTERFACE)

target_include_directories(cmsis INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Include)
target_include_directories(cmsis INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Device/ST/STM32${STM32_FAMILY}xx/Include)
target_compile_definitions(cmsis INTERFACE -DSTM32F103xB)
target_compile_definitions(cmsis INTERFACE -DUSE_HAL_DRIVER)

