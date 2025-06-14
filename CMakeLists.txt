cmake_minimum_required(VERSION 3.10)
project(LongArithmetic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основная библиотека
add_library(long_arithmetic
    src/long_arithmetic.cpp
    src/pi_calculation.cpp
)

# Заголовочные файлы
target_include_directories(long_arithmetic PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Исполняемый файл для Pi
add_executable(calculate_pi
    src/calculate_pi.cpp
)
target_link_libraries(calculate_pi long_arithmetic)

#Исполняемый файл для тестирования библиотеки
add_executable(demo
    src/demo.cpp)
target_link_libraries(demo long_arithmetic)

# Тесты 
if(EXISTS ${CMAKE_SOURCE_DIR}/test/test_long_arithmetic.cpp)
    message(STATUS "Found tests - enabling testing")
    enable_testing()
    
    add_executable(tests
        test/test_long_arithmetic.cpp
        test/main.cpp
    )
    target_link_libraries(tests long_arithmetic)
    
    find_package(GTest REQUIRED)
    target_link_libraries(tests GTest::GTest GTest::Main)
    
    add_test(NAME arithmetic_tests COMMAND tests)
else()
    message(WARNING "Test files not found - skipping tests")
endif()
