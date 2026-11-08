# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.0)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(DCM120 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

set(DCM120_SOURCES
    DCM120.cpp
    src/app/app.cpp
    src/drive/drive_controller.cpp
    src/input/pwm_input.cpp
    src/esc/esc_output.cpp
    src/esc/esc_direction_output.cpp
    src/can/can_manager.cpp
    src/dcm/dcm_command_dispatcher.cpp
    src/dcm/dcm_commands.cpp
    src/dcm/dcm_commands_lookup_table.cpp
    src/mcp2515/mcp2515.cpp
)

add_executable(DCM120 ${DCM120_SOURCES})

pico_set_program_name(DCM120 "DCM120")
pico_set_program_version(DCM120 "0.1.0")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(DCM120 0)
pico_enable_stdio_usb(DCM120 0)

# Add the standard library to the build
target_link_libraries(DCM120
        pico_stdlib
        hardware_clocks
        hardware_gpio
        hardware_irq
        hardware_pwm
        hardware_spi
        hardware_sync
        hardware_timer
        hardware_watchdog
)

# Add the standard include files to the build
target_include_directories(DCM120 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include
)

set(DCM120_SPEED_INPUT_PIN 15 CACHE STRING "GPIO used for RC speed PWM input")
set(DCM120_ESC_OUTPUT_PIN 14 CACHE STRING "GPIO used for ESC PWM output")
set(DCM120_ESC_DIRECTION_PIN 13 CACHE STRING "GPIO used for ESC direction PWM output")
set(DCM120_CAN_RX_PIN 16 CACHE STRING "MCP2515 SPI MISO GPIO")
set(DCM120_CAN_CS_PIN 17 CACHE STRING "MCP2515 SPI chip-select GPIO")
set(DCM120_CAN_SCK_PIN 18 CACHE STRING "MCP2515 SPI clock GPIO")
set(DCM120_CAN_TX_PIN 19 CACHE STRING "MCP2515 SPI MOSI GPIO")

target_compile_definitions(DCM120 PRIVATE
    DCM120_SPEED_INPUT_PIN=${DCM120_SPEED_INPUT_PIN}
    DCM120_ESC_OUTPUT_PIN=${DCM120_ESC_OUTPUT_PIN}
    DCM120_ESC_DIRECTION_PIN=${DCM120_ESC_DIRECTION_PIN}
    DCM120_CAN_RX_PIN=${DCM120_CAN_RX_PIN}
    DCM120_CAN_CS_PIN=${DCM120_CAN_CS_PIN}
    DCM120_CAN_SCK_PIN=${DCM120_CAN_SCK_PIN}
    DCM120_CAN_TX_PIN=${DCM120_CAN_TX_PIN}
)

set_source_files_properties(${DCM120_SOURCES} PROPERTIES
    COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic"
)

pico_add_extra_outputs(DCM120)

