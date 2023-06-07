cmake_minimum_required(VERSION 3.13)

# include pico-sdk
include(lib/pico-sdk/pico_sdk_init.cmake)

project(calcOS)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

set(SOURCES
	"src/main.c"
	"src/tft.c"
)

set(HEADERS
	"src/tft.h"
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
	pico_stdlib
	hardware_spi
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_add_extra_outputs(${PROJECT_NAME})
