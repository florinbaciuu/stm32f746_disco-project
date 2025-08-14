# -----------------------------------------------------------------------------
# @author    Baciu Aurel Florin
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.20)
# Core project settings
project("stm32f746_disco-project")
enable_language(C CXX ASM)
add_compile_options(-fdiagnostics-color=always)
message("Build type: " ${CMAKE_BUILD_TYPE})
message("Florin Aurel Baciu")

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Core MCU flags, CPU type, instruction set and FPU setup
set(cpu_PARAMS
    ${CPU_FLAGS}
    -mcpu=cortex-m7
    -mthumb
    #-mfpu=fpv5-d16
    #-mfloat-abi=hard
    # -mcpu, -mfloat, -mfloat-abi, ...
)

################SOURCES

# Sources
set(sources_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Sys/syscall.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Sys/sysmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Sys/stm32f7xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Sys/system_stm32f7xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Startup/startup_stm32f746xx.S
    ${CMAKE_CURRENT_SOURCE_DIR}/Sys/stm32f7xx_hal_tick.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Sys/stm32f7xx_freertos_tick.c
)


################INCLUDE

# Include directories for all compilers
set(include_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Startup
    ${CMAKE_CURRENT_SOURCE_DIR}/Sys
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F7xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/STM32746G-Discovery/
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/Components
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F7xx_HAL_Driver/Inc
)
#####=============================================================####
## FREERTOS
list(APPEND sources_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/stream_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/portable/GCC/ARM_CM7/r0p1/port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/portable/MemMang/heap_4.c
)
list(APPEND include_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/include
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/portable/GCC/ARM_CM7/r0p1
    #${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/portable/GCC/ARM_CM4F/
)
## END FREERTOS
#####=============================================================####
### HAL
#   1   listeaza toate fisierele HAL
file(GLOB HAL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c
)
#   2   Elimini timebase-urile template din listă: ca sa elimini erorile de first defined here and etc
list(REMOVE_ITEM HAL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_timebase_tim_template.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_timebase_rtc_alarm_template.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_timebase_rtc_wakeup_template.c
)
#   3   Appendezi la sources_SRCS toate fisierele C din HAL
list(APPEND sources_SRCS
    ${HAL_SRCS} # HAL core
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.c # BSP Discovery (LED)
)
### END HAL
#####=============================================================####
###BSP
file(GLOB BSP_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.c
)
list(APPEND sources_SRCS
    ${BSP_SRCS} # HAL core
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/Components/ft5336/ft5336.c
)
list(APPEND include_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/STM32746G-Discovery/
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/Components/ft5336
)


###END BSP
#####=============================================================####
### Utilities
# list(APPEND sources_SRCS
#     ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/Fonts/font8.c
#     ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/Fonts/font12.c
#     ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/Fonts/font16.c
#     ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/Fonts/font20.c
#     ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/Fonts/font24.c
# )
# nu trebuie incluse si C pt ca le include fonts.h
list(APPEND include_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/Fonts/
)
#####=============================================================####

#####=============================================================####
# Include directories for each compiler
set(include_c_DIRS)
set(include_cxx_DIRS)
set(include_asm_DIRS)

# Symbols definition for all compilers
set(symbols_SYMB STM32F746xx
    USE_HAL_DRIVER)

# Symbols definition for each compiler
set(symbols_c_SYMB
    RTE_DEVICE_STARTUP_STM32F7XX
)
set(symbols_cxx_SYMB)
set(symbols_asm_SYMB)

# Link directories and names of libraries
set(link_DIRS)
set(link_LIBS)

# Linker script
set(linker_script_SRC ${CMAKE_CURRENT_SOURCE_DIR}/linker-heap_4.ld)

# Compiler options
set(compiler_OPTS)

# Linker options
set(linker_OPTS)

# Link directories setup
# Must be before executable is added
link_directories(${CMAKE_PROJECT_NAME} ${link_DIRS})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add support for blank char as part of project name
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_ORIGINAL_NAME})

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${sources_SRCS})

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${include_DIRS}
    $<$<COMPILE_LANGUAGE:C>: ${include_c_DIRS}>
    $<$<COMPILE_LANGUAGE:CXX>: ${include_cxx_DIRS}>
    $<$<COMPILE_LANGUAGE:ASM>: ${include_asm_DIRS}>
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    ${symbols_SYMB}
    $<$<COMPILE_LANGUAGE:C>: ${symbols_c_SYMB}>
    $<$<COMPILE_LANGUAGE:CXX>: ${symbols_cxx_SYMB}>
    $<$<COMPILE_LANGUAGE:ASM>: ${symbols_asm_SYMB}>

    # Configuration specific
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>: >
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME} ${link_LIBS})

# Compiler options
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    ${cpu_PARAMS}
    ${compiler_OPTS}
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
    $<$<COMPILE_LANGUAGE:C>: >
    $<$<COMPILE_LANGUAGE:CXX>:

    # -Wno-volatile
    # -Wold-style-cast
    # -Wuseless-cast
    # -Wsuggest-override
    >
    $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp -MMD -MP>
    $<$<CONFIG:Debug>:-O0 -g3 -ggdb>
    $<$<CONFIG:Release>:-Os>
)

# Linker options
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -T${linker_script_SRC}
    ${cpu_PARAMS}
    ${linker_OPTS}
    -Wl,--gc-sections
    -Wl,-Map=${PROJECT_ORIGINAL_NAME}.map
    -u _printf_float # STDIO float formatting support (remove if not used)
    --specs=nosys.specs
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -lsupc++
    -Wl,--end-group
    -Wl,-z,max-page-size=8 # Allow good software remapping across address space (with proper GCC section making)
    -Wl,--print-memory-usage
)

### LVGL
# add_compile_definitions(LV_CONF_SKIP)
# add_compile_definitions(LV_CONF_INCLUDE_SIMPLE)
# set(LVGL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/lvgl)
# add_subdirectory(${LVGL_DIR})
# target_link_libraries(${PROJECT_NAME} PRIVATE lvgl)
# target_compile_options(lvgl PRIVATE -DSTM32F7)
# target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
#     ${LVGL_DIR}
#     ${LVGL_DIR}/src
# )

# Execute post-build to print size, generate hex and bin
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${PROJECT_ORIGINAL_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${PROJECT_ORIGINAL_NAME}.bin
)


