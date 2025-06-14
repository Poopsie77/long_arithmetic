#include <gtest/gtest.h>
#include <chrono>
#include <vector>

#include "../include/long_arithmetic.hpp"
#include "../include/pi_calculation.hpp"

// Test class for all operation tests
class FixedPointTest: public ::testing::Test {
protected:
    void SetUp() override {}
};

// Тест для конструктора
TEST_F(FixedPointTest, Constructor) {
    FixedPoint num("123.456");
    EXPECT_EQ(num.to_string(3), "123.456");
}

// Тест для сложения
TEST_F(FixedPointTest, Addition) {
    FixedPoint num1("10.5");
    FixedPoint num2("20.25");
    FixedPoint result = num1 + num2;
    EXPECT_EQ(result.to_string(), "30.75");
}

// Тест для вычитания
TEST_F(FixedPointTest, Subtraction) {
    FixedPoint num1("30.75");
    FixedPoint num2("0");
    FixedPoint result = num1 - num2;
    EXPECT_EQ(result.to_string(), "30.75");
}

// Тест для умножения
TEST_F(FixedPointTest, Multiplication) {
    FixedPoint num1("10.5");
    FixedPoint num2("2.0");
    FixedPoint result = num1 * num2;
    EXPECT_EQ(result.to_string(), "21.0");
}

// Тест для деления
TEST_F(FixedPointTest, Division) {
    FixedPoint num1("21.0", 2);
    FixedPoint num2("2.0", 2);
    FixedPoint result = num1 / num2;
    EXPECT_EQ(result.to_string(), "10.5");
}

// Тест для сравнения
TEST_F(FixedPointTest, Comparison) {
    FixedPoint num1("10.5");
    FixedPoint num2("20.25");
    EXPECT_TRUE(num1 < num2);
    EXPECT_FALSE(num1 > num2);
    EXPECT_TRUE(num1 != num2);
}

// Сдвиги
//Корректность переноса битов между целой и дробной частями
//Обработку отрицательных чисел (если поддерживается)
//Краевые случаи (сдвиг на 0 бит, сдвиг больше размера числа)
TEST(FixedPointTests, LeftShift) {
    FixedPoint a("3.5");  
    FixedPoint b = a << 1;
    EXPECT_EQ(b.to_string(), "7.0");  

    FixedPoint c("0.5");  // 0.1
    FixedPoint d = c << 2;
    EXPECT_EQ(d.to_string(), "2.0");  
}

TEST(FixedPointTests, RightShift) {
    FixedPoint a("6.0");  
    FixedPoint b = a >> 1;
    EXPECT_EQ(b.to_string(), "3.0");  

    FixedPoint c("1.0");  
    FixedPoint d = c >> 2;
    EXPECT_EQ(d.to_string(), "0.25");  
}

//деление с остатком
//Деление нацело и с остатком.
//деление отрицательных чисел.
//Обработку деления на ноль
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

TEST(FixedPointTests, DivideByZero) {
    FixedPoint a("5.0"), b("0.0");
    EXPECT_THROW(a.divide_with_remainder(b), std::runtime_error);
}

//Корректность работы с разными длинами чисел.
//Игнорирование знака (если так задумано).
//Случаи с нулями.
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

// Тест для числа Pi
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