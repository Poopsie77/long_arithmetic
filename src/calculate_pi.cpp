/**
 * @file calculate_pi.cpp
 * @brief Программа для вычисления числа π с заданной точностью
 * 
 * Программа демонстрирует использование класса FixedPoint для вычисления
 * числа π с произвольной точностью с помощью алгоритма Бэйли-Боруэйна-Плаффа.
 */

#include <iostream>
#include <string>
#include <chrono>
#include "../include/long_arithmetic.hpp"
#include "../include/pi_calculation.hpp"


/**
 * @brief Основная функция программы
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 * @return Код завершения программы (0 - успех, 1 - ошибка)
 * 
 * @details Программа принимает один аргумент командной строки:
 * количество знаков после запятой для вывода числа π.
 * 
 * Пример использования:
 * @code{.sh}
 * ./pi_calculator 50
 * @endcode
 * 
 * Выведет число π с 50 знаками после запятой и время вычисления.
 */
int main(int argc, char** argv) {
    if (argc == 1) {
        printf("No arguments provided.\n");
        return 0;
    }
    try {
        int len = std::stoi(argv[1]);

        auto start = std::chrono::high_resolution_clock::now();
        FixedPoint pi = get_pi();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>
                       (std::chrono::high_resolution_clock::now() - start);
        std::string pi_str = pi.to_string();
        pi_str.resize(len + 2);
        std::cout << pi_str << std::endl;
        std::cout << "Total time (in ms) " << duration.count() << std::endl;

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid input." << std::endl;
    }

    return 0;
}