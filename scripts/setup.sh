#!/bin/bash
# Downloads STM32F411RE vendor files
# Run once after cloning: ./scripts/setup.sh

set -e
echo "[embtest] Downloading STM32F411RE CMSIS files..."

cd boards/stm32f4

# CMSIS device headers
wget -q https://raw.githubusercontent.com/STMicroelectronics/cmsis_device_f4/master/Include/stm32f4xx.h
wget -q https://raw.githubusercontent.com/STMicroelectronics/cmsis_device_f4/master/Include/stm32f411xe.h
wget -q https://raw.githubusercontent.com/STMicroelectronics/cmsis_device_f4/master/Include/system_stm32f4xx.h

# Startup file
wget -q https://raw.githubusercontent.com/STMicroelectronics/cmsis_device_f4/master/Source/Templates/gcc/startup_stm32f411xx.s

# Linker script
wget -q https://raw.githubusercontent.com/STMicroelectronics/STM32CubeF4/master/Projects/STM32F411RE-Nucleo/Templates/STM32CubeIDE/STM32F411RETX_FLASH.ld

# CMSIS core
mkdir -p CMSIS/Include
wget -q -P CMSIS/Include https://raw.githubusercontent.com/ARM-software/CMSIS_5/develop/CMSIS/Core/Include/cmsis_compiler.h
wget -q -P CMSIS/Include https://raw.githubusercontent.com/ARM-software/CMSIS_5/develop/CMSIS/Core/Include/cmsis_gcc.h
wget -q -P CMSIS/Include https://raw.githubusercontent.com/ARM-software/CMSIS_5/develop/CMSIS/Core/Include/cmsis_version.h
wget -q -P CMSIS/Include https://raw.githubusercontent.com/ARM-software/CMSIS_5/develop/CMSIS/Core/Include/core_cm4.h

cd ../..
echo "[embtest] Setup complete."
