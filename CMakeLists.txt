CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
SET(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
SET(CMAKE_CXX_STANDARD 17)

INCLUDE(configs)

IF(DEFINED STM32_FAMILY)
	ADD_DEFINITIONS(-DSTM32F103xB)
	ADD_SUBDIRECTORY(drivers/STM32F1xx_HAL_Driver)
	ADD_SUBDIRECTORY(drivers/CMSIS)
	ADD_SUBDIRECTORY(drivers/stm32_startup)
	# Linker setup
	ADD_SECTION(flash rx 0x08000000 64K)
	ADD_SECTION(ram rwx 0x20000000 20K)
	#eventually replace this with Add_flash_section
	SET_RAM_SECTION_NAME(ram)
	SET_FLASH_SECTION_NAME(flash)

	SET(LINKER_FILE "")
	GENERATE_LINKER_FILE(LINKER_FILE)

	ADD_DEFINITIONS(-DPLATFORM_STM32)
ELSE()
	ADD_DEFINITIONS(-DPLATFORM_PC)
ENDIF()

SET(INCLUDE_FILES
"include/canvas.h"
	"include/commandlist.h"
	"include/commands.h"
	"include/gfxfont.h"
	"include/tile_canvas.h"
	"src/display_st7735.h"
	"src/profiling.h"
)

SET(SOURCE_FILES
	"src/canvas.cpp"
	"src/commandlist.cpp"
	"src/tile_canvas.cpp"
	"src/display_st7735.cpp"
	"src/profiling.cpp"
)

SET(EXAMPLE_TESTBED_FILES
	"examples/main.cpp"
	"examples/testbed.cpp"
)

SET(EXAMPLE_00_FILES
	"examples/main.cpp"
	"examples/00-canvas-usage.cpp"
)

SET(EXAMPLE_01_FILES
	"examples/main.cpp"
	"examples/01-cmdlist-usage.cpp"
)

SET(EXAMPLE_02_FILES
	"examples/main.cpp"
	"examples/02-tilecanvas-usage.cpp"
)

INCLUDE_DIRECTORIES(
	"include"
	"drivers/stm32_startup"

	// temp
	"examples/ssre/Source/Library"
)

# Add gfx_library
ADD_LIBRARY(gfx_library ${SOURCE_FILES})

# Add examples
ADD_EXECUTABLE(gfx_library_testbed ${EXAMPLE_TESTBED_FILES})
ADD_EXECUTABLE(00-canvas-usage ${EXAMPLE_00_FILES})
ADD_EXECUTABLE(01-cmdlist-usage ${EXAMPLE_01_FILES})
ADD_EXECUTABLE(02-tilecanvas-usage ${EXAMPLE_02_FILES})

SET_PROPERTY(TARGET gfx_library_testbed
             PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

IF(DEFINED STM32_FAMILY)
	TARGET_LINK_LIBRARIES(gfx_library STM32${STM32_FAMILY}xx_HAL_Driver cmsis startup)

	TARGET_LINK_OPTIONS(gfx_library_testbed PUBLIC -T ${LINKER_FILE})
	TARGET_LINK_LIBRARIES(gfx_library_testbed gfx_library)

	TARGET_LINK_OPTIONS(00-canvas-usage PUBLIC -T ${LINKER_FILE})
	TARGET_LINK_LIBRARIES(00-canvas-usage gfx_library)

	TARGET_LINK_OPTIONS(01-cmdlist-usage PUBLIC -T ${LINKER_FILE})
	TARGET_LINK_LIBRARIES(01-cmdlist-usage gfx_library)

	TARGET_LINK_OPTIONS(02-tilecanvas-usage PUBLIC -T ${LINKER_FILE})
	TARGET_LINK_LIBRARIES(02-tilecanvas-usage gfx_library)

ELSE()
	TARGET_LINK_LIBRARIES(gfx_library_testbed gfx_library SDL2)
	TARGET_LINK_LIBRARIES(00-canvas-usage gfx_library SDL2)
	TARGET_LINK_LIBRARIES(01-cmdlist-usage gfx_library SDL2)
	TARGET_LINK_LIBRARIES(02-tilecanvas-usage gfx_library SDL2)
ENDIF()
