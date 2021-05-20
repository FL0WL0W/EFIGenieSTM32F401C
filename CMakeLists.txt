cmake_minimum_required ( VERSION 3.9 )
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project ( EFIGenie C CXX ASM )

include_directories(Drivers/CMSIS/Device/ST/STM32F4xx/Include)
include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/STM32F4xx_HAL_Driver/Inc)
include_directories(Drivers/STM32F4xx_HAL_Driver/Inc/Legacy)
include_directories(Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc)
include_directories(Middlewares/ST/STM32_USB_Device_Library/Core/Inc)
include_directories(Core/Inc)
include_directories(USB_DEVICE/App)
include_directories(USB_DEVICE/Target)

file(GLOB_RECURSE USER_CSOURCES Core/Src/*.c)
file(GLOB_RECURSE USER_CXXSOURCES Core/Src/*.cpp)
file(GLOB_RECURSE USB_CDC_SOURCES Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/*.c)
file(GLOB_RECURSE USB_CORE_SOURCES Middlewares/ST/STM32_USB_Device_Library/Core/Src/*.c)
file(GLOB_RECURSE HAL_SOURCES Drivers/STM32F4xx_HAL_Driver/Src/*.c)
file(GLOB_RECURSE USB_TARGET_CSOURCES USB_DEVICE/Target/*.c)
file(GLOB_RECURSE USB_APP_CSOURCES USB_DEVICE/App/*.c)

set(SOURCE_FILES ${USER_CSOURCES} ${USER_CXXSOURCES} ${HAL_SOURCES} ${USB_TARGET_CSOURCES} ${USB_APP_CSOURCES} ${USB_CDC_SOURCES} ${USB_CORE_SOURCES} startup_stm32f401xc.s STM32F401CCUx_FLASH.ld)
add_executable(firmware.elf ${SOURCE_FILES})

set(LIBRARIES	libs)
add_subdirectory (${LIBRARIES}/OperationArchitecture)
add_subdirectory (${LIBRARIES}/EmbeddedIOServices)
add_subdirectory (${LIBRARIES}/EmbeddedIOOperations)
add_subdirectory (${LIBRARIES}/ReluctorOperations)
add_subdirectory (${LIBRARIES}/EFIGenie)
target_link_libraries(firmware.elf OperationArchitecture)
target_link_libraries(firmware.elf EmbeddedIOServices)
target_link_libraries(firmware.elf EmbeddedIOOperations)
target_link_libraries(firmware.elf ReluctorOperations)
target_link_libraries(firmware.elf EFIGenie)

set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/firmware.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/firmware.bin)
add_custom_command(TARGET firmware.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex $<TARGET_FILE:firmware.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} ARGS -O binary $<TARGET_FILE:firmware.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")
