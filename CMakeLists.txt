cmake_minimum_required(VERSION 3.13)

# include pico-sdk
include(lib/pico-sdk/pico_sdk_init.cmake)

project(calcOS)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

set(SOURCES
	"src/main.c"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} pico_stdlib)

pico_add_extra_outputs(${PROJECT_NAME})

