cmake_minimum_required(VERSION 3.22)

# 1. Налаштування мов та вимкнення проблемного LTO
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# Вимикаємо LTO, щоб не було помилки з liblto_plugin.dll
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-lto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-lto")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fno-lto")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_PROJECT_NAME STM32F746G_DISCO)
project(${CMAKE_PROJECT_NAME})
enable_language(C CXX ASM)
add_executable(${CMAKE_PROJECT_NAME})

# Export compile commands for IDE support (generates compile_commands.json)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 2. Підключення генерації CubeMX
add_subdirectory(cmake/stm32cubemx)

# Add touchgfx generated sources and libraries
add_subdirectory(cmake/touchgfx)

# 3. Автоматичний пошук усіх сирсів TouchGFX (вирішує undefined reference)
file(GLOB_RECURSE TOUCHGFX_SOURCES 
    "TouchGFX/gui/src/*.cpp"
    "TouchGFX/generated/gui_generated/src/*.cpp"
    "TouchGFX/generated/fonts/src/*.cpp"
    "TouchGFX/generated/images/src/*.cpp"
    "TouchGFX/generated/texts/src/*.cpp"
)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${TOUCHGFX_SOURCES}
    "STM32CubeIDE/Signal_gen/signal_gen.c" # Ваша бібліотека
)

# 4. Шляхи до заголовків (.h / .hpp)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    "STM32CubeIDE/Signal_gen"
    "TouchGFX/generated/texts/include"
    "TouchGFX/generated/fonts/include"
    "TouchGFX/generated/images/include"
    "TouchGFX/generated/gui_generated/include"
    "TouchGFX/gui/include"
    "Middlewares/ST/touchgfx/framework/include"
)

# 5. Лінковка бінарної бібліотеки TouchGFX
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    "Middlewares/ST/touchgfx/lib/core/cortex_m7/gcc"
)

# Видаляємо конфліктну libob.a
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    touchgfx-float-abi-hard
)
