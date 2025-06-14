/**
 * @file main.cpp
 * @brief Точка входа для запуска тестов
 * @param argc Количество аргументов
 * @param argv Аргументы командной строки
 * @return Код завершения тестов
 */

#include <gtest/gtest.h>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}