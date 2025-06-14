/**
 * @file long_arithmetic.hpp
 * @brief Заголовочный файл класса FixedPoint для работы с числами фиксированной точки
 */

#ifndef LONG_NUM_H
#define LONG_NUM_H

#include <vector>
#include <string>
#include <cstdint>
#include <utility>

/**
 * @enum Op_behavior
 * @brief Поведение операций для сравнения чисел
 */
enum class Op_behavior {
    PLUS_FST,  ///< Первое число положительное, второе положительное
    PLUS_SND,  ///< Первое число положительное, второе отрицательное
    SUB_FST,   ///< Первое число отрицательное, второе положительное
    SUB_SND    ///< Первое число отрицательное, второе отрицательное
};

/**
 * @class FixedPoint
 * @brief Класс для работы с числами фиксированной точки произвольной точности
 * 
 * Класс реализует арифметические операции, сравнения и преобразования
 * для чисел с фиксированной точкой в двоичном представлении.
 */
class FixedPoint {
public:
    /**
     * @brief Конструктор из строки
     * @param num_str Строковое представление числа
     * @param frac_bits Количество бит дробной части (по умолчанию 32)
     */
    FixedPoint(const std::string &num_str, int frac_bits = 32);

    /**
     * @brief Конструктор из числа с плавающей точкой
     * @param num Число для инициализации
     * @param frac_bits Количество бит дробной части (по умолчанию 32)
     */
    FixedPoint(const double &num, int frac_bits = 32);

    /// @name Битвые операции
    
    /**
     * @brief Оператор битового сдвига влево
     * @param n Количество бит для сдвига
     * @return Число со сдвинутыми битами
     */
    FixedPoint operator<<(int n) const;
    
    /**
     * @brief Оператор битового сдвига вправо
     * @param n Количество бит для сдвига
     * @return Число со сдвинутыми битами
     */
    FixedPoint operator>>(int n) const;
    
    /**
     * @brief Оператор побитового XOR
     * @param other Число для операции
     * @return Результат XOR
     */
    FixedPoint operator^(const FixedPoint &other) const;
    
    /// @}
    
    /**
     * @brief Конструктор копирования
     * @param other Число для копирования
     */
    FixedPoint(const FixedPoint& other);
    
    /**
     * @brief Деструктор
     */
    ~FixedPoint();

    /**
     * @brief Оператор присваивания
     * @param other Число для присваивания
     * @return Ссылка на текущий объект
     */
    FixedPoint& operator=(const FixedPoint& other);

    /// @name Арифметические операторы
    /// @{
    
    /**
     * @brief Оператор сложения
     * @param other Слагаемое
     * @return Сумма чисел
     */
    FixedPoint operator+(const FixedPoint &other) const;
    
    /**
     * @brief Оператор вычитания
     * @param other Вычитаемое
     * @return Разность чисел
     */
    FixedPoint operator-(const FixedPoint &other) const;
    
    /**
     * @brief Оператор умножения
     * @param other Множитель
     * @return Произведение чисел
     */
    FixedPoint operator*(const FixedPoint &other) const;
    
    /**
     * @brief Оператор деления
     * @param other Делитель
     * @return Частное от деления
     */
    FixedPoint operator/(const FixedPoint &other) const;
    
    /**
     * @brief Деление с остатком
     * @param other Делитель
     * @return Пара: частное и остаток
     */
    std::pair<FixedPoint, FixedPoint> divide_with_remainder(const FixedPoint &other) const;
    
    /// @}
    
    /// @name Операторы сравнения
    /// @{
    
    bool operator>(const FixedPoint &other) const;
    bool operator<(const FixedPoint &other) const;
    bool operator==(const FixedPoint &other) const;
    bool operator<=(const FixedPoint &other) const;
    bool operator>=(const FixedPoint &other) const;
    bool operator!=(const FixedPoint &other) const;
    
    /// @}
    
    /// @name Составные операторы присваивания
    /// @{
    
    FixedPoint& operator+=(const FixedPoint &other);
    FixedPoint& operator*=(const FixedPoint &other);
    FixedPoint& operator-=(const FixedPoint &other);
    FixedPoint& operator/=(const FixedPoint &other);
    
    /// @}
    
    /**
     * @brief Устанавливает точность дробной части
     * @param precision Количество бит дробной части
     */
    void set_precision(size_t precision);

    /**
     * @brief Выводит двоичное представление числа
     */
    void print_bin() const;

    /**
     * @brief Преобразует число в строку
     * @param len Желаемая длина строки (-1 для автоматической)
     * @return Строковое представление числа
     */
    std::string to_string(int len = -1) const;

private:
    std::vector<uint32_t> integer;    ///< Двоичное представление целой части
    std::vector<uint32_t> fractional; ///< Двоичное представление дробной части
    uint32_t fractional_bits;         ///< Количество бит дробной части
    bool is_negative = false;         ///< Флаг отрицательного числа

    /**
     * @brief Проверяет, является ли число нулём
     * @return true если число равно нулю
     */
    bool is_zero() const;

    /**
     * @brief Вспомогательная функция для определения поведения операций
     */
    Op_behavior helper(const FixedPoint &a, const FixedPoint &b, char op) const;

    /**
     * @brief Сравнивает модули чисел
     * @return true если |a| > |b|
     */
    bool bigger_abs(const FixedPoint &a, const FixedPoint &b) const;

    /**
     * @brief Сравнивает модули чисел
     * @return true если |a| < |b|
     */
    bool less_abs(const FixedPoint &a, const FixedPoint &b) const;

    /**
     * @brief Выводит битовое представление 32-битного числа
     */
    void printBits(uint32_t value) const;

    /**
     * @brief Складывает дробные части чисел
     * @return Пара: результат и флаг переноса
     */
    std::pair<std::vector<uint32_t>, bool> add_frac(const std::vector<uint32_t> &a,
                                                  const std::vector<uint32_t> &b,
                                                  uint32_t carry = 0) const;

    /**
     * @brief Складывает целые части чисел
     * @return Пара: результат и флаг переноса
     */
    std::pair<std::vector<uint32_t>, bool> add_int(const std::vector<uint32_t> &a,
                                                 const std::vector<uint32_t> &b,
                                                 uint32_t carry = 0) const;

    /**
     * @brief Вычитает два числа
     * @return Пара: целая и дробная части результата
     */
    std::pair<std::vector<uint32_t>, std::vector<uint32_t>>
    subtract_nums(const FixedPoint &a, const FixedPoint &b) const;

    /**
     * @brief Вычитает два 32-битных числа с учётом заёма
     * @return Флаг заёма
     */
    int subtract(uint32_t &res, uint32_t val_a, uint32_t val_b, uint32_t borrow = 0) const;

    /**
     * @brief Вычитает два вектора чисел
     */
    std::vector<uint32_t> subtract_vec(const std::vector<uint32_t> &a, 
                                     const std::vector<uint32_t> &b) const;

    /**
     * @brief Делит два числа
     * @return Пара: целая и дробная части результата
     */
    std::pair<std::vector<uint32_t>, std::vector<uint32_t>>
    divide(const FixedPoint &a, const FixedPoint &b) const;

    /**
     * @brief Сравнивает два вектора чисел
     * @return true если a >= b
     */
    bool not_less_vec(const std::vector<uint32_t> &a, 
                     const std::vector<uint32_t> &b) const;

    /**
     * @brief Добавляет бит при делении
     */
    void add_bit_div(std::vector<uint32_t> &vec, uint32_t bit_added, bool is_one) const;

    /**
     * @brief Преобразует целую часть из строки в двоичный вид
     */
    std::vector<uint32_t> int_part_to_bin(const std::string& num_str) const;

    /**
     * @brief Умножает десятичную строку на 2
     */
    std::string mult_by_two(const std::string &num_str) const;

    /**
     * @brief Преобразует дробную часть из строки в двоичный вид
     */
    std::vector<uint32_t> frac_to_binary(const std::string &frac_str, int frac_bits = 32) const;

    /**
     * @brief Преобразует десятичную строку в двоичное представление
     * @return Пара: целая и дробная части в двоичном виде
     */
    std::pair<std::vector<uint32_t>, std::vector<uint32_t>>
    decimal_to_binary(const std::string &num_str, int frac_bits = 32) const;
};

/**
 * @brief Пользовательский литерал для создания FixedPoint
 * @param number Число с плавающей точкой
 * @return Объект FixedPoint
 */
FixedPoint operator""_long(long double number);

#endif // LONG_NUM_H