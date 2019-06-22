CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
SET(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE(configs)

SET(INCLUDE_FILES
	"include/canvas.h"
	"include/commandlist.h"
	"include/commands.h"
	"include/gfxfont.h"
)

SET(SOURCE_FILES
	"src/canvas.cpp"
	"src/commandlist.cpp"
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
TARGET_LINK_LIBRARIES(gfx_library_example gfx_library SDL2)
