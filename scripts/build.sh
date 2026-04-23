#!/bin/bash
set -e

echo "[embtest] Building firmware..."

rm -rf build

cmake -B build -DCMAKE_C_FLAGS="-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -DSTM32F411xE -Wall -ffunction-sections -fdata-sections -O0 -g3"

cmake --build build

echo "[embtest] Build complete:"
ls -lh build/firmware.elf
arm-none-eabi-size build/firmware.elf
