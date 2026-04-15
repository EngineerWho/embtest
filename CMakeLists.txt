cmake_minimum_required(VERSION 3.20)

# ── Toolchain must be set before project() ──────────────
set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER       arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER     arm-none-eabi-gcc)
set(CMAKE_OBJCOPY          arm-none-eabi-objcopy)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(embtest C ASM)

# ── Compiler flags for STM32F411 ────────────────────────
set(CPU_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -mcpu=cortex-m4 -mthumb \
    -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
    -DSTM32F411xE \
    -Wall -Wextra \
    -ffunction-sections -fdata-sections \
    -O0 -g3")

set(CMAKE_EXE_LINKER_FLAGS
    "-mcpu=cortex-m4 -mthumb \
     -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
     -specs=nosys.specs -specs=nano.specs \
     -Wl,--gc-sections \
     -Wl,-Map=${PROJECT_NAME}.map \
     -T${CMAKE_SOURCE_DIR}/boards/stm32f4/STM32F411RETX_FLASH.ld")

# ── Include paths ────────────────────────────────────────
include_directories(
    target
    boards/stm32f4
    boards/stm32f4/CMSIS/Include
    boards/stm32f4/CMSIS/Device/ST/STM32F4xx/Include
)

# ── Source files ─────────────────────────────────────────
set(EMBTEST_SOURCES
    target/runner.c
    target/uart_reporter.c
    boards/stm32f4/board.c
    boards/stm32f4/system_stm32f4xx.c
    boards/stm32f4/startup_stm32f411xe.s
)

# ── Example test executable ──────────────────────────────
add_executable(firmware.elf
    ${EMBTEST_SOURCES}
    examples/test_blink/test_blink.c
)
