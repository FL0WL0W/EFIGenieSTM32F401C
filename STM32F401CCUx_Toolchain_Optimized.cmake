SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
SET(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)
add_definitions ( -DUSE_HAL_DRIVER )
add_definitions ( -DSTM32F401xC )
add_definitions ( -DALLOW_TASK_TO_SCHEDULE_IN_CALLBACK )
set ( CMAKE_C_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O3 -g -gdwarf-2 -Wall -fdata-sections -ffunction-sections -mno-unaligned-access" )
set ( CMAKE_CXX_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O3 -g -gdwarf-2 -Wall -fdata-sections -ffunction-sections -fno-exceptions -fno-rtti -mno-unaligned-access" )
set ( CMAKE_ASM_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O3 -Wall -fdata-sections -ffunction-sections -x assembler-with-cpp -mno-unaligned-access" )
set ( CMAKE_EXE_LINKER_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O3 --specs=nano.specs --specs=nosys.specs -lc -lm -lnosys -T\"${PROJECT_SOURCE_DIR}/STM32F401CCUx_FLASH.ld\" -Wl,--cref -Wl,--gc-sections" )
