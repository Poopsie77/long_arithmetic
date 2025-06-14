#include <iostream>
#include <string>
#include "include/long_arithmetic.hpp"

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
