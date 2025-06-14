#include <gtest/gtest.h>
#include <chrono>
#include <vector>

#include "../include/long_arithmetic.hpp"
#include "../include/pi_calculation.hpp"


class FixedPointTest: public ::testing::Test {
protected:
    void SetUp() override {}
};

/**
 * @test Тест конструктора FixedPoint
 * @brief Проверка корректности создания объекта из строки
 */
TEST_F(FixedPointTest, Constructor) {
    FixedPoint num("123.456");
    EXPECT_EQ(num.to_string(3), "123.456");
}

/**
 * @test Тест операции сложения
 * @brief Проверка корректности работы оператора +
 */
TEST_F(FixedPointTest, Addition) {
    FixedPoint num1("10.5");
    FixedPoint num2("20.25");
    FixedPoint result = num1 + num2;
    EXPECT_EQ(result.to_string(), "30.75");
}

/**
 * @test Тест операции вычитания
 * @brief Проверка корректности работы оператора -
 */
TEST_F(FixedPointTest, Subtraction) {
    FixedPoint num1("30.75");
    FixedPoint num2("0");
    FixedPoint result = num1 - num2;
    EXPECT_EQ(result.to_string(), "30.75");
}

/**
 * @test Тест операции умножения
 * @brief Проверка корректности работы оператора *
 */
TEST_F(FixedPointTest, Multiplication) {
    FixedPoint num1("10.5");
    FixedPoint num2("2.0");
    FixedPoint result = num1 * num2;
    EXPECT_EQ(result.to_string(), "21.0");
}

/**
 * @test Тест операции деления
 * @brief Проверка корректности работы оператора /
 */
TEST_F(FixedPointTest, Division) {
    FixedPoint num1("21.0", 2);
    FixedPoint num2("2.0", 2);
    FixedPoint result = num1 / num2;
    EXPECT_EQ(result.to_string(), "10.5");
}

/**
 * @test Тест операций сравнения
 * @brief Проверка корректности работы операторов сравнения
 */
TEST_F(FixedPointTest, Comparison) {
    FixedPoint num1("10.5");
    FixedPoint num2("20.25");
    EXPECT_TRUE(num1 < num2);
    EXPECT_FALSE(num1 > num2);
    EXPECT_TRUE(num1 != num2);
}

/**
 * @test Тест битового сдвига влево
 * @brief Проверка корректности работы оператора <<
 * @details Проверяет:
 * - Корректность переноса битов
 * - Обработку граничных случаев
 */
TEST(FixedPointTests, LeftShift) {
    FixedPoint a("3.5");  
    FixedPoint b = a << 1;
    EXPECT_EQ(b.to_string(), "7.0");  

    FixedPoint c("0.5");  // 0.1
    FixedPoint d = c << 2;
    EXPECT_EQ(d.to_string(), "2.0");  
}

/**
 * @test Тест битового сдвига вправо
 * @brief Проверка корректности работы оператора >>
 * @details Проверяет:
 * - Корректность переноса битов
 * - Обработку дробных чисел
 */
TEST(FixedPointTests, RightShift) {
    FixedPoint a("6.0");  
    FixedPoint b = a >> 1;
    EXPECT_EQ(b.to_string(), "3.0");  

    FixedPoint c("1.0");  
    FixedPoint d = c >> 2;
    EXPECT_EQ(d.to_string(), "0.25");  
}

/**
 * @test Тест деления с остатком
 * @brief Проверка корректности divide_with_remainder()
 * @details Проверяет:
 * - Деление нацело и с остатком
 * - Обработку отрицательных чисел
 */
TEST(FixedPointTests, DivideWithRemainder) {
    FixedPoint a("10.0"), b("3.0");
    auto [quotient, remainder] = a.divide_with_remainder(b);
    EXPECT_EQ(quotient.to_string(), "3.0");   
    EXPECT_EQ(remainder.to_string(), "1.0");   

    FixedPoint c("-7.0"), d("2.0");
    auto [q, r] = c.divide_with_remainder(d);
    EXPECT_EQ(q.to_string(), "-3.0");        
    EXPECT_EQ(r.to_string(), "-1.0");          
}

/**
 * @test Тест деления на ноль
 * @brief Проверка обработки ошибки деления на ноль
 */
TEST(FixedPointTests, DivideByZero) {
    FixedPoint a("5.0"), b("0.0");
    EXPECT_THROW(a.divide_with_remainder(b), std::runtime_error);
}


/**
 * @test Тест побитового XOR
 * @brief Проверка корректности работы оператора ^
 * @details Проверяет:
 * - Корректность работы с разными длинами чисел
 * - Обработку дробных частей
 */
TEST(FixedPointTests, BitwiseXOR) {
    FixedPoint a("5.0");    
    FixedPoint b("3.0");    
    FixedPoint c = a ^ b;
    EXPECT_EQ(c.to_string(), "6.0");  

    FixedPoint d("0.5");    
    FixedPoint e("0.25");  
    FixedPoint f = d ^ e;
    EXPECT_EQ(f.to_string(), "0.75");  
}

/**
 * @test Тест расчета числа π
 * @brief Проверка корректности и производительности
 * @details Проверяет:
 * - Корректность вычисления π
 * - Время выполнения (должно быть < 1 сек)
 */
TEST_F(FixedPointTest, PiCalculation) {
    auto start = std::chrono::high_resolution_clock::now();
    FixedPoint pi = get_pi();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>
                   (std::chrono::high_resolution_clock::now() - start);
    std::string pi_str = pi.to_string();
    pi_str.resize(102);

    EXPECT_EQ(pi_str, pi_right);
    EXPECT_TRUE(duration.count() < 1000);

}