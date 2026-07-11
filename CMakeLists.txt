 cmake_minimum_required(VERSION 4.0)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project ( EFIGenieSTM32F401C C CXX ASM )

set(LIBRARIES_DIR	    ${CMAKE_SOURCE_DIR}/libs)
set(EmbeddedIOServicesHardwareAbstraction_Path ${LIBRARIES_DIR}/EmbeddedIOServices/HardwareAbstractions/Stm32Hal)

include(FetchContent)

FetchContent_Declare(
    EFIGenie
    GIT_REPOSITORY https://github.com/FL0WL0W/EFIGenie
    GIT_TAG        main
    GIT_PROGRESS   TRUE
    GIT_SHALLOW    TRUE
    SOURCE_DIR  ${LIBRARIES_DIR}/EFIGenie
)

FetchContent_MakeAvailable(EFIGenie)

include_directories(Core/Inc)
include_directories(Drivers/CMSIS/Device/ST/STM32F4xx/Include)
include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/STM32F4xx_HAL_Driver/Inc)
include_directories(Drivers/STM32F4xx_HAL_Driver/Inc/Legacy)
include_directories(Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc)
include_directories(Middlewares/ST/STM32_USB_Device_Library/Core/Inc)
include_directories(USB_DEVICE/App)
include_directories(USB_DEVICE/Target)
include_directories(${EmbeddedIOServicesHardwareAbstraction_Path}/include)

file(GLOB_RECURSE USER_CSOURCES Core/Src/*.c)
file(GLOB_RECURSE USER_CSOURCES_Templates Core/Src/*_template.c)
IF(USER_CSOURCES_Templates)
        list(REMOVE_ITEM USER_CSOURCES ${USER_CSOURCES_Templates})
ENDIF()
file(GLOB_RECURSE USER_CXXSOURCES Core/Src/*.cpp)
file(GLOB_RECURSE USER_CXXSOURCES_Templates Core/Src/*_template.cpp)
IF(USER_CXXSOURCES_Templates)
        list(REMOVE_ITEM USER_CXXSOURCES ${USER_CXXSOURCES_Templates})
ENDIF()
file(GLOB_RECURSE HAL_SOURCES Drivers/STM32F4xx_HAL_Driver/Src/*.c)
file(GLOB_RECURSE HAL_SOURCES_Templates Drivers/STM32F4xx_HAL_Driver/Src/*_template.c)
IF(HAL_SOURCES_Templates)
        list(REMOVE_ITEM HAL_SOURCES ${HAL_SOURCES_Templates})
ENDIF()
file(GLOB_RECURSE USB_CDC_SOURCES Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/*.c)
file(GLOB_RECURSE USB_CDC_SOURCES_Templates Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/*_template.c)
IF(USB_CDC_SOURCES_Templates)
        list(REMOVE_ITEM USB_CDC_SOURCES ${USB_CDC_SOURCES_Templates})
ENDIF()
file(GLOB_RECURSE USB_CORE_SOURCES Middlewares/ST/STM32_USB_Device_Library/Core/Src/*.c)
file(GLOB_RECURSE USB_CORE_SOURCES_Templates Middlewares/ST/STM32_USB_Device_Library/Core/Src/*_template.c)
IF(USB_CORE_SOURCES_Templates)
        list(REMOVE_ITEM USB_CORE_SOURCES ${USB_CORE_SOURCES_Templates})
ENDIF()
file(GLOB_RECURSE USB_TARGET_CSOURCES USB_DEVICE/Target/*.c)
file(GLOB_RECURSE USB_TARGET_CSOURCES_Templates USB_DEVICE/Target/*_template.c)
IF(USB_TARGET_CSOURCES_Templates)
        list(REMOVE_ITEM USB_TARGET_CSOURCES ${USB_TARGET_CSOURCES_Templates})
ENDIF()
file(GLOB_RECURSE USB_APP_CSOURCES USB_DEVICE/App/*.c)
file(GLOB_RECURSE USB_APP_CSOURCES_Templates USB_DEVICE/App/*_template.c)
IF(USB_APP_CSOURCES_Templates)
        list(REMOVE_ITEM USB_APP_CSOURCES ${USB_APP_CSOURCES_Templates})
ENDIF()
file(GLOB_RECURSE EmbeddedIOService_SOURCES ${EmbeddedIOServicesHardwareAbstraction_Path}/src/*.cpp)
file(GLOB_RECURSE EmbeddedIOService_SOURCES_Templates ${EmbeddedIOServicesHardwareAbstraction_Path}/src/*_template.cpp)
IF(EmbeddedIOService_SOURCES_Templates)
        list(REMOVE_ITEM EmbeddedIOService_SOURCES ${EmbeddedIOService_SOURCES_Templates})
ENDIF()

set(SRC_FILES ${USER_CSOURCES} ${USER_CXXSOURCES} ${HAL_SOURCES} ${USB_CDC_SOURCES} ${USB_CORE_SOURCES} ${USB_TARGET_CSOURCES} ${USB_APP_CSOURCES} ${EmbeddedIOService_SOURCES} startup_stm32f401xc.s STM32F401CCUx_FLASH.ld)
add_executable(firmware.elf ${SRC_FILES})

target_link_libraries(firmware.elf EFIGenie)

set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/firmware.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/firmware.bin)
add_custom_command(TARGET firmware.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex $<TARGET_FILE:firmware.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} ARGS -O binary $<TARGET_FILE:firmware.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")
