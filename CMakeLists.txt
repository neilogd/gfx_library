CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
SET(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE(configs)

IF(DEFINED STM32_FAMILY)
	ADD_DEFINITIONS(-DSTM32F103x6)
	ADD_SUBDIRECTORY(drivers/STM32F1xx_HAL_Driver)
	ADD_SUBDIRECTORY(drivers/CMSIS)
	ADD_SUBDIRECTORY(drivers/stm32_startup)
	# Linker setup
	ADD_SECTION(flash rw 0x08000000 192K)
	ADD_SECTION(ram rwx 0x20000000 20K)
	#eventually replace this with Add_flash_section
	SET_RAM_SECTION_NAME(ram)
	SET_FLASH_SECTION_NAME(flash)
	GENERATE_LINKER_FILE()
ENDIF()

SET(INCLUDE_FILES
	"include/canvas.h"
	"include/commandlist.h"
	"include/commands.h"
	"include/gfxfont.h"
	"include/tile_canvas.h"
)

SET(SOURCE_FILES
	"src/canvas.cpp"
	"src/commandlist.cpp"
	"src/tile_canvas.cpp"
)

SET(EXAMPLE_FILES
	"examples/main.cpp"
)

INCLUDE_DIRECTORIES(
	"include"
)

# Add gfx_library
ADD_LIBRARY(gfx_library ${SOURCE_FILES})


# Add gfx_library_example
ADD_EXECUTABLE(gfx_library_example ${EXAMPLE_FILES})

IF(DEFINED STM32_FAMILY)
	TARGET_LINK_LIBRARIES(gfx_library_example gfx_library STM32${STM32_FAMILY}xx_HAL_Driver cmsis startup)
ELSE()
	TARGET_LINK_LIBRARIES(gfx_library_example gfx_library SDL2)
ENDIF()