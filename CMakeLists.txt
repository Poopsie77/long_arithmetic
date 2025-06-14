cmake_minimum_required(VERSION 3.10)
project(long_arithmetic)

# Настройки компилятора
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -O3")

# Путь к GTest
set(GTEST_ROOT "/wsl.localhost/Ubuntu/usr")
find_package(GTest REQUIRED)

# Длина по умолчанию для π
set(DEFAULT_LEN_PI 100 CACHE STRING "Default digits for pi calculation")

# Основные исходники библиотеки
set(LIB_SOURCES
    src/long_arithmetic.cpp
    src/pi_calculation.cpp
)

# 1. Основная библиотека
add_library(long_arithmetic ${LIB_SOURCES})
target_include_directories(long_arithmetic PUBLIC include)

# 2. Тесты
add_executable(tests
    src/test_long_arithmetic.cpp
    src/main.cpp
)
target_link_libraries(tests long_arithmetic GTest::GTest GTest::Main pthread)

# 3. Вычисление π
add_executable(pi_calculator
    src/calculate_pi.cpp
)
target_link_libraries(pi_calculator long_arithmetic)

# 4. Интерактивный демо-режим (новый файл)
add_executable(interactive_demo
    interactive_demo.cpp
)
target_link_libraries(interactive_demo long_arithmetic)

# Кастомные команды
add_custom_command(TARGET tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Запуск тестов..."
    COMMAND $<TARGET_FILE:tests>
)

add_custom_target(run_pi
    COMMAND ${CMAKE_COMMAND} -E echo "Вычисление π с длиной ${DEFAULT_LEN_PI}"
    COMMAND $<TARGET_FILE:pi_calculator> ${DEFAULT_LEN_PI}
    DEPENDS pi_calculator
)

add_custom_target(run_demo
    COMMAND ${CMAKE_COMMAND} -E echo "Запуск интерактивного демо..."
    COMMAND $<TARGET_FILE:interactive_demo>
    DEPENDS interactive_demo
)

# Основные цели
add_custom_target(all DEPENDS tests pi_calculator interactive_demo)
add_custom_target(clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E echo "Очистка завершена"
)

# Функция для установки длины π
function(set_pi_length)
    set(oneValueArgs LENGTH)
    cmake_parse_arguments(PI "" "${oneValueArgs}" "" ${ARGN})
    
    if(DEFINED PI_LENGTH)
        add_custom_target(pi_${PI_LENGTH}
            COMMAND ${CMAKE_COMMAND} -E echo "Вычисление π с длиной ${PI_LENGTH}"
            COMMAND $<TARGET_FILE:pi_calculator> ${PI_LENGTH}
            DEPENDS pi_calculator
        )
    endif()
endfunction()

set_pi_length(LENGTH ${DEFAULT_LEN_PI})