/**
 * @file interactive_mode.cpp
 * @brief Интерактивный режим работы с длинной арифметикой
 * Программа предоставляет интерактивный интерфейс для выполнения операций
 * с числами произвольной точности с использованием класса FixedPoint.
 */


#include <iostream>
#include <string>
#include "../include/long_arithmetic.hpp"


/**
 * @brief Выводит список доступных операций
 * 
 * Отображает все поддерживаемые арифметические и битовые операции,
 * которые можно выполнять с числами FixedPoint.
 */
void print_operations() {
    std::cout << "\nДоступные операции:\n"
              << "  +  : сложение\n"
              << "  -  : вычитание\n"
              << "  *  : умножение\n"
              << "  /  : деление\n"
              << "  << : сдвиг влево\n"
              << "  >> : сдвиг вправо\n"
              << "  ^  : XOR\n"
              << "  %  : деление с остатком\n"
              << "  cmp : сравнение\n"
              << "  q  : выход\n\n";
}

/**
 * @brief Основная функция интерактивного режима
 * @return Код завершения программы
 * 
 * @details Реализует интерактивный цикл для выполнения операций:
 * 1. Запрос первого числа
 * 2. Выбор операции
 * 3. Для бинарных операций - запрос второго числа
 * 4. Вывод результата
 * 
 * Поддерживает обработку ошибок и выход по команде.
 * 
 * Пример использования:
 * @code{.sh}
 * Введите первое число (или 'q' для выхода): 123.456
 * Введите операцию: +
 * Введите второе число: 789.123
 * Результат: 912.579
 * @endcode
 */
int main() {
    std::cout << "=== Интерактивный режим long_arithmetic ===\n";
    print_operations();

    while(true) {
        try {
            std::cout << "Введите первое число (или 'q' для выхода): ";
            std::string input;
            std::getline(std::cin, input);
            
            if(input == "q") break;
            if(input.empty()) continue;

            FixedPoint num1(input);

            std::cout << "Введите операцию: ";
            std::string op;
            std::getline(std::cin, op);

            if(op == "q") break;
            if(op.empty()) continue;

            // Обработка унарных операций
            if(op == "<<" || op == ">>") {
                std::cout << "Введите количество битов: ";
                int bits;
                std::cin >> bits;
                std::cin.ignore();
                
                auto result = (op == "<<") ? num1 << bits : num1 >> bits;
                std::cout << "Результат: " << result.to_string() << "\n";
                continue;
            }

            // Бинарные операции
            std::cout << "Введите второе число: ";
            std::getline(std::cin, input);
            if(input == "q") break;

            FixedPoint num2(input);

            if(op == "+") {
                std::cout << "Результат: " << (num1 + num2).to_string() << "\n";
            }
            else if(op == "-") {
                std::cout << "Результат: " << (num1 - num2).to_string() << "\n";
            }
            else if(op == "*") {
                std::cout << "Результат: " << (num1 * num2).to_string() << "\n";
            }
            else if(op == "/") {
                std::cout << "Результат: " << (num1 / num2).to_string() << "\n";
            }
            else if(op == "^") {
                std::cout << "Результат: " << (num1 ^ num2).to_string() << "\n";
            }
            else if(op == "%") {
                auto [quotient, remainder] = num1.divide_with_remainder(num2);
                std::cout << "Частное: " << quotient.to_string() 
                          << ", Остаток: " << remainder.to_string() << "\n";
            }
            else if(op == "cmp") {
                std::cout << num1.to_string() << " == " << num2.to_string() << ": " << (num1 == num2) << "\n";
                std::cout << num1.to_string() << " != " << num2.to_string() << ": " << (num1 != num2) << "\n";
                std::cout << num1.to_string() << " < "  << num2.to_string() << ": " << (num1 < num2)  << "\n";
                std::cout << num1.to_string() << " > "  << num2.to_string() << ": " << (num1 > num2)  << "\n";
            }
            else {
                std::cout << "Неизвестная операция!\n";
                print_operations();
            }
        }
        catch(const std::exception& e) {
            std::cout << "Ошибка: " << e.what() << "\n";
        }
    }

    std::cout << "Работа завершена.\n";
    return 0;
}
