/**
 * @file long_arithmetic.cpp
 * @brief Реализация класса FixedPoint для арифметики фиксированной точки
 */

#include <iostream>
#include <algorithm>
#include <stdexcept>

#include <chrono>

#include "../include/long_arithmetic.hpp"

/**
 * @brief Конструктор из строки с указанием бит дробной части
 * @param num_str Строковое представление числа
 * @param frac_bits Количество бит дробной части
 */
FixedPoint::FixedPoint(const std::string &num_str, int frac_bits) : fractional_bits(frac_bits) {
    auto binary_result = decimal_to_binary(num_str, fractional_bits);

    integer = binary_result.first;     // Сохранение целой части в двоичном формате
    fractional = binary_result.second; // Сохранение дробной части в двоичном формате
    is_negative = num_str[0] == '-';
}

/**
 * @brief Конструктор из числа double с указанием бит дробной части
 * @param num Число для инициализации
 * @param frac_bits Количество бит дробной части
 */
FixedPoint::FixedPoint(const double &num, int frac_bits) : fractional_bits(frac_bits) {
    auto binary_result = decimal_to_binary(std::to_string(num), fractional_bits);

    integer = binary_result.first;     
    fractional = binary_result.second;
    is_negative = num < 0;
}


// Конструктор и деструктор копирования 
FixedPoint::FixedPoint(const FixedPoint& other) = default;
FixedPoint::~FixedPoint() = default;

// Оператор присваивания 
FixedPoint& FixedPoint::operator=(const FixedPoint& other) = default;

/**
 * @brief Оператор сложения
 * @param other Слагаемое
 * @return Результат сложения
 * @details Реализует сложение с учетом знаков чисел и переносов
 */
FixedPoint FixedPoint::operator+(const FixedPoint &other) const {

    Op_behavior behavior = helper(*this, other, '+');

    
    FixedPoint result("0.0", std::max(fractional_bits, other.fractional_bits));

    switch (behavior) {
        case Op_behavior::PLUS_FST: {
            bool res_compare = !less_abs(*this, other);
            const FixedPoint &a = (res_compare ? *this : other);
            const FixedPoint &b = (res_compare ? other : *this);

            auto sub_res = subtract_nums(a, b);

            result.integer = sub_res.first;     
            result.fractional = sub_res.second; 
            result.is_negative = a.is_negative;
            break;
        }
        case Op_behavior::PLUS_SND: {

            
            auto add_res = add_frac(fractional, other.fractional);
            result.fractional = add_res.first;

            
            add_res = add_int(integer, other.integer, add_res.second);
            result.integer = add_res.first;

            
            if (add_res.second) {
                result.integer.push_back(1);
            }
            result.is_negative = is_negative;
            break;
        }
        default:
            throw std::invalid_argument("Unexpected behavior encountered");
    }

    while (result.fractional.size() > 1 && result.fractional.front() == 0) {
        result.fractional.erase(result.fractional.begin());
    }
    while (result.integer.size() > 1 && result.integer.back() == 0) {
        result.integer.erase(result.integer.end() - 1);
    }
    result.fractional_bits = result.fractional.size() * 32;

    return result;
}


/**
 * @brief Оператор вычитания
 * @param other Вычитаемое
 * @return Результат вычитания
 * @details Реализует вычитание с учетом знаков чисел и заемов
 */
FixedPoint FixedPoint::operator-(const FixedPoint &other) const {

    Op_behavior behavior = helper(*this, other, '-');
    
    FixedPoint result("0.0", std::max(fractional_bits, other.fractional_bits));

    switch (behavior) {
        case Op_behavior::SUB_FST: {
            
            auto add_res = add_frac(fractional, other.fractional);
            result.fractional = add_res.first;

            
            add_res = add_int(integer, other.integer, add_res.second);
            result.integer = add_res.first;

            
            if (add_res.second) {
                result.integer.push_back(1);
            }
            result.is_negative = is_negative;
            break;
        }
        case Op_behavior::SUB_SND: {
            bool res_compare = !less_abs(*this, other);
            const FixedPoint &a = (res_compare ? *this : other);
            const FixedPoint &b = (res_compare ? other : *this);

            auto sub_res = subtract_nums(a, b);

            result.integer = sub_res.first;     
            result.fractional = sub_res.second;
            result.is_negative = (is_negative ? res_compare : !res_compare);

            break;
        }
        default:
            throw std::invalid_argument("Unexpected behavior encountered");
    }

    while (result.fractional.size() > 1 && result.fractional.front() == 0) {
        result.fractional.erase(result.fractional.begin());
    }
    while (result.integer.size() > 1 && result.integer.back() == 0) {
        result.integer.erase(result.integer.end() - 1);
    }
    result.fractional_bits = result.fractional.size() * 32;

    return result;
}

/**
 * @brief Оператор умножения
 * @param other Множитель
 * @return Результат умножения
 * @details Реализует побитовое умножение с учетом переносов
 */
FixedPoint FixedPoint::operator*(const FixedPoint &other) const {
    
    FixedPoint result("0.0", (fractional_bits / 32 + 1) * 32 + (other.fractional_bits / 32 +  1) * 32);

    
    uint32_t this_sz = integer.size() + fractional.size();
    uint32_t other_sz = other.integer.size() + other.fractional.size();

    
    uint32_t mid_mult_sz = (this_sz + other_sz) * 32 + 1;
    std::vector<uint32_t> mid_mult(mid_mult_sz);

    
    for (uint32_t this_i = 0; this_i < this_sz; this_i++) {
        for (uint32_t bit_this = 0; bit_this < 32; bit_this++) {
            for (uint32_t other_i = 0; other_i < other_sz; other_i++) {
                for (uint32_t bit_other = 0; bit_other < 32; bit_other++) {
                    if (this_i < fractional.size() && other_i < other.fractional.size()) {
                        mid_mult[32 * (this_i + other_i) + bit_this + bit_other] +=
                        (fractional[this_i] >> bit_this) & (other.fractional[other_i] >> bit_other) & 0x00000001;
                    }
                    if (this_i < fractional.size() && other_i >= other.fractional.size()) {
                        mid_mult[32 * (this_i + other_i) + bit_this + bit_other] +=
                        (fractional[this_i] >> bit_this) & (other.integer[other_i - other.fractional.size()] >> bit_other) & 0x00000001;
                    }
                    if (this_i >= fractional.size() && other_i < other.fractional.size()) {
                        mid_mult[32 * (this_i + other_i) + bit_this + bit_other] +=
                        (integer[this_i - fractional.size()] >> bit_this) & (other.fractional[other_i] >> bit_other) & 0x00000001;
                    }
                    if (this_i >= fractional.size() && other_i >= other.fractional.size()) {
                        mid_mult[32 * (this_i + other_i) + bit_this + bit_other] +=
                        (integer[this_i - fractional.size()] >> bit_this) & (other.integer[other_i - other.fractional.size()] >> bit_other) & 0x00000001;
                    }
                }
            }
        }
    }

    std::vector<uint32_t> mult_int_result;
    std::vector<uint32_t> mult_frac_result;
    int bit_added = 0;

    for (size_t i = 0; i < mid_mult_sz - 1; i++) {
        if (i < 32 * (fractional.size() + other.fractional.size())) {
            if (bit_added == 0) mult_frac_result.push_back(0);
            mult_frac_result.back() = mult_frac_result.back() | ((mid_mult[i] % 2) << bit_added);
            mid_mult[i + 1] += mid_mult[i] / 2;
            bit_added = (bit_added + 1) % 32;
        } else {
            if (bit_added == 0) mult_int_result.push_back(0);
            mult_int_result.back() = mult_int_result.back() | ((mid_mult[i] % 2) << bit_added);
            mid_mult[i + 1] += mid_mult[i] / 2;
            bit_added = (bit_added + 1) % 32;
        }
    }

    result.integer = mult_int_result;     
    result.fractional = mult_frac_result; 
    result.is_negative = is_negative ^ other.is_negative;

    while (result.fractional.size() > 1 && result.fractional.front() == 0) {
        result.fractional.erase(result.fractional.begin());
    }
    while (result.integer.size() > 1 && result.integer.back() == 0) {
        result.integer.erase(result.integer.end() - 1);
    }
    result.fractional_bits = result.fractional.size() * 32;

    return result;
}

/**
 * @brief Оператор деления
 * @param other Делитель
 * @return Результат деления
 * @throw std::runtime_error при делении на ноль
 * @details Реализует алгоритм деления с восстановлением остатка
 */
FixedPoint FixedPoint::operator/(const FixedPoint &other) const {
    
    FixedPoint result("0.0", std::max(fractional_bits, other.fractional_bits));

    auto div_res = divide(*this, other);

    result.integer.clear();
    result.fractional.clear();

    result.integer = div_res.first;
    result.fractional = div_res.second;

    if (result.integer.empty()) result.integer.push_back(0);
    if (result.fractional.empty()) result.fractional.push_back(0);

    result.is_negative = is_negative ^ other.is_negative;

    while (result.fractional.size() > 1 && result.fractional.front() == 0) {
        result.fractional.erase(result.fractional.begin());
    }
    while (result.integer.size() > 1 && result.integer.back() == 0) {
        result.integer.erase(result.integer.end() - 1);
    }

    result.fractional_bits = result.fractional.size() * 32;

    return result;
}


bool FixedPoint::operator>(const FixedPoint &other) const {
    bool abs_compare = bigger_abs(*this, other);
    if (!is_negative && !other.is_negative) return abs_compare;
    if (is_negative && other.is_negative) return !abs_compare;
    return !is_negative;
}

bool FixedPoint::operator<(const FixedPoint &other) const {
    bool abs_compare = less_abs(*this, other);
    if (!is_negative && !other.is_negative) return abs_compare;
    if (is_negative && other.is_negative) return !abs_compare;
    return is_negative;
}

bool FixedPoint::operator==(const FixedPoint &other) const {
    for (int i = std::max(integer.size(), other.integer.size()) - 1; i >= 0; i--) {
        uint32_t val_a = ((uint32_t) i) < integer.size() ? integer[i] : 0;
        uint32_t val_b = ((uint32_t) i) < other.integer.size() ? other.integer[i] : 0;
        if (val_a != val_b) {
            return false;
        }
    }

    int this_i = fractional.size() - 1, other_i = other.fractional.size() - 1;
    for (; this_i >= 0 && other_i >= 0; this_i--, other_i--) {

        if (fractional[this_i] != other.fractional[other_i]) {
            return false;
        }
    }

    if (this_i == -1 && other_i == -1) return true;
    if (this_i == -1) {
        for (; other_i >= 0; other_i--) {
            if (other.fractional[other_i] != 0) return false;
        }
    }
    if (other_i == -1) {
        for (; this_i >= 0; this_i--) {
            if (fractional[this_i] != 0) return false;
        }
    }
    return false;
}

bool FixedPoint::operator<=(const FixedPoint &other) const {
    return !(*this > other);
}

bool FixedPoint::operator>=(const FixedPoint &other) const {
    return !(*this < other);
}

bool FixedPoint::operator!=(const FixedPoint &other) const {
    return !(*this == other);
}

FixedPoint& FixedPoint::operator+=(const FixedPoint &other) {
    *this = *this + other;
    return *this;
}

FixedPoint& FixedPoint::operator*=(const FixedPoint &other) {
    *this = *this * other;
    return *this;
}

FixedPoint& FixedPoint::operator-=(const FixedPoint &other) {
    *this = *this - other;
    return *this;
}

FixedPoint& FixedPoint::operator/=(const FixedPoint &other) {
    *this = *this / other;
    return *this;
}


/**
 * @brief Устанавливает точность дробной части
 * @param precision Новое количество бит дробной части
 * @details Удаляет младшие биты дробной части для уменьшения точности
 */
void FixedPoint::set_precision(size_t precision) {
    if (precision > fractional_bits) {
        std::cout << "You can just set less value" << std::endl;
        return;
    }

    if (precision == 0) {
        fractional.clear();
        fractional_bits = 0;
        return;
    }

    int need_to_del = fractional_bits - precision;
    int low_order_bits = (fractional_bits % 32 ? fractional_bits % 32 : 32);
    if (need_to_del >= low_order_bits) {
        uint32_t q_del = (need_to_del - low_order_bits) / 32 + 1;
        fractional.erase(fractional.begin(), fractional.begin() + q_del);

        if (!fractional.empty())
            fractional[0] &= 0xFFFFFFFF << ((need_to_del - low_order_bits) % 32);
        else
            fractional.push_back(0);
    } else {
        fractional[0] &= 0xFFFFFFFF << need_to_del;
    }
    fractional_bits = precision;
}

void FixedPoint::print_bin() const {
    std::cout << "Sign: ";
    std::cout << (is_negative ? "-" : "+") << std::endl;
    std::cout << "Fractional_bits: " << fractional_bits << std::endl;
    std::cout << "Integer bits:    ";
    for (uint32_t value : integer) {
        printBits(value);
        std::cout << " ";
    }
    std::cout << std::endl;

    std::cout << "Fractional bits: ";
    for (uint32_t value : fractional) {
        printBits(value);
        std::cout << " ";
    }
    std::cout << std::endl;
}

std::string FixedPoint::to_string(int len) const {
    FixedPoint ten = FixedPoint{"10"};

    FixedPoint before = *this;

    before.set_precision(0);

    FixedPoint after = *this - before;

    std::string before_res;
    while (!before.is_zero()) {
        FixedPoint cur = before / ten;
        cur.set_precision(0);
        FixedPoint rem = before - (cur * ten);
        before_res.push_back('0' + rem.integer[0]);

        before = cur;
    }

    if (before_res == "") {
        before_res = "0";
    }

    std::reverse(before_res.begin(), before_res.end());

    std::string after_res;
    int stop = after.fractional_bits;
    while (!after.is_zero() && stop > 0) {
        FixedPoint cur = after * ten;
        FixedPoint rem = after * ten;
        rem.set_precision(0);

        after_res.push_back('0' + rem.integer[0]);

        after = cur - rem;
        stop -= 4;
    }

    if (after_res == "") {
        after_res = "0";
    }

    if (len != -1 && after_res.size() > (uint32_t) len) {
        after_res.resize(len + 1);
        after_res[after_res.size() - 2] += static_cast<uint32_t>(after_res[after_res.size() - 1] >= '5');
        after_res.pop_back();
    }

    if (is_negative) {
        return "-" + before_res + "." + after_res;
    }

    return before_res + "." + after_res;
}

/**
 * @brief Проверяет, является ли число нулем
 * @return true если число равно нулю
 */
bool FixedPoint::is_zero() const {
    if (integer.size() == 0 && fractional.size() == 0) return true;
    for (uint32_t val : integer) {
        if (val != 0) return false;
    }
    for (uint32_t val : fractional) {
        if (val != 0) return false;
    }
    return true;
}

Op_behavior FixedPoint::helper(const FixedPoint &a, const FixedPoint &b, char op) const {
    bool sign_xor = a.is_negative ^ b.is_negative;
    switch (op) {
    case '+':
        if (sign_xor)
            return Op_behavior::PLUS_FST;
        else
            return Op_behavior::PLUS_SND;
    case '-':
        if (sign_xor)
            return Op_behavior::SUB_FST;
        else
            return Op_behavior::SUB_SND;
    }
    throw std::invalid_argument("Invalid argument in FixedPoint::helper");
}


void FixedPoint::printBits(uint32_t value) const {
    for (int i = 31; i >= 0; --i) {
        std::cout << ((value >> i) & 1);
    }
}

/**
 * @brief Сравнивает абсолютные значения двух чисел
 * @param a Первое число
 * @param b Второе число
 * @return true если |a| > |b|
 */
bool FixedPoint::bigger_abs(const FixedPoint &a, const FixedPoint &b) const {
    for (int i = std::max(a.integer.size(), b.integer.size()) - 1; i >= 0; i--) {
        uint32_t val_a = ((uint32_t) i) < a.integer.size() ? a.integer[i] : 0;
        uint32_t val_b = ((uint32_t) i) < b.integer.size() ? b.integer[i] : 0;
        if (val_a > val_b) {
            return true;
        }
        if (val_a < val_b) {
            return false;
        }
    }

    int a_i = a.fractional.size() - 1, b_i = b.fractional.size() - 1;
    for (; a_i >= 0 && b_i >= 0; a_i--, b_i--) {
        if (a.fractional[a_i] > b.fractional[b_i]) {
            return true;
        }
        if (a.fractional[a_i] < b.fractional[b_i]) {
            return false;
        }
    }

    if (a_i == -1 && b_i == -1) return false;
    if (a_i == -1) return false;
    return true;
}

/**
 * @brief Сравнивает абсолютные значения двух чисел
 * @param a Первое число
 * @param b Второе число
 * @return true если |a| < |b|
 */
bool FixedPoint::less_abs(const FixedPoint &a, const FixedPoint &b) const {
    for (int i = std::max(a.integer.size(), b.integer.size()) - 1; i >= 0; i--) {
        uint32_t val_a = ((uint32_t) i) < a.integer.size() ? a.integer[i] : 0;
        uint32_t val_b = ((uint32_t) i) < b.integer.size() ? b.integer[i] : 0;
        if (val_a < val_b) {
            return true;
        }
        if (val_a > val_b) {
            return false;
        }
    }

    int a_i = a.fractional.size() - 1, b_i = b.fractional.size() - 1;
    for (; a_i >= 0 && b_i >= 0; a_i--, b_i--) {
        if (a.fractional[a_i] < b.fractional[b_i]) {
            return true;
        }
        if (a.fractional[a_i] > b.fractional[b_i]) {
            return false;
        }
    }
    if (a_i == -1 && b_i == -1) return false;
    if (b_i == -1) return false;
    return true;
}

/**
 * @brief Выполняет сложение дробных частей
 * @param a Первая дробная часть
 * @param b Вторая дробная часть
 * @param carry Флаг переноса
 * @return Пару: результат и новый флаг переноса
 */
std::pair<std::vector<uint32_t>, bool> FixedPoint::add_frac(const std::vector<uint32_t> &a,
                                                            const std::vector<uint32_t> &b,
                                                            uint32_t carry) const {
    std::vector<uint32_t> result;
    size_t max_sz = std::max(a.size(), b.size());

    size_t a_i = 0, b_i = 0;

    // Perform addition bit by bit
    while (a_i < max_sz && b_i < max_sz) {
        if (b_i < max_sz - a.size()) {
            result.push_back(b[b_i++]);
            continue;
        }
        if (a_i < max_sz - b.size()) {
            result.push_back(a[a_i++]);
            continue;
        }

        result.push_back(0);

        for (size_t j = 0; j < 32; ++j) {
            uint32_t addition = (carry == 1 ? 0xFFFFFFFF : 0);
            result.back() = result.back() | ((a[a_i] ^ b[b_i] ^ addition) & (1 << j));
            carry = (((a[a_i] & (1 << j)) && (b[b_i] & (1 << j))) ||
                    (((a[a_i] ^ b[b_i]) & (1 << j)) && addition) ? 1 : 0);
        }
        a_i++;
        b_i++;
    }
    return std::make_pair(result, carry);
}

/**
 * @brief Выполняет сложение целых частей
 * @param a Первая целая часть
 * @param b Вторая целая часть
 * @param carry Флаг переноса
 * @return Пару: результат и новый флаг переноса
 */
std::pair<std::vector<uint32_t>, bool> FixedPoint::add_int(const std::vector<uint32_t> &a,
                                                           const std::vector<uint32_t> &b,
                                                           uint32_t carry) const {
    std::vector<uint32_t> result;
    size_t max_sz = std::max(a.size(), b.size());

    for (size_t i = 0; i < max_sz; ++i) {
        if (i >= a.size()) {
            result.push_back(b[i] + carry);
            carry = (b[i] + carry) == 0;
            continue;
        }
        if (i >= b.size()) {
            result.push_back(a[i] + carry);
            carry = (a[i] + carry) == 0;
            continue;
        }

        result.push_back(0);

        for (size_t j = 0; j < 32; ++j) {
            uint32_t addition = (carry == 1 ? 0xFFFFFFFF : 0);
            result.back() = result.back() | ((a[i] ^ b[i] ^ addition) & (1 << j));
            carry = (((a[i] & (1 << j)) && (b[i] & (1 << j))) ||
                    (((a[i] ^ b[i]) & (1 << j)) && addition) ? 1 : 0);
        }
    }
    return std::make_pair(result, carry);
}

std::pair<std::vector<uint32_t>, std::vector<uint32_t>>
FixedPoint::subtract_nums(const FixedPoint &a, const FixedPoint &b) const {
    std::vector<uint32_t> result_int;
    std::vector<uint32_t> result_frac;

    size_t a_int_sz = a.integer.size();
    size_t a_frac_sz = a.fractional.size();
    size_t b_int_sz = b.integer.size();
    size_t b_frac_sz = b.fractional.size();


    size_t max_sz = std::max(a_int_sz + a_frac_sz, b_int_sz + b_frac_sz);
    size_t max_frac_sz = std::max(a_frac_sz, b_frac_sz);

    uint32_t borrow = 0; 

    size_t a_i = 0, b_i = 0;

    
    while (a_i < max_sz && b_i < max_sz) {
        if (a_i < max_frac_sz && b_i < max_frac_sz) {
            result_frac.push_back(0); 
        } else {
            result_int.push_back(0); 
        }

       
        if (b_i < max_frac_sz - a_frac_sz) {
            borrow = subtract(result_frac.back(), 0, b.fractional[b_i++], borrow);
            continue;
        }

        if (a_i < max_frac_sz - b_frac_sz) {
            borrow = subtract(result_frac.back(), a.fractional[a_i++], 0, borrow);
            continue;
        }

      
        if (a_i < max_frac_sz && b_i < max_frac_sz) {
            borrow = subtract(result_frac.back(), a.fractional[a_i++], b.fractional[b_i++], borrow);
            continue;
        }

        
        uint32_t val_a = (a_i - a_frac_sz < a_int_sz) ? a.integer[a_i++ - a_frac_sz] : 0;
        uint32_t val_b = (b_i - b_frac_sz < b_int_sz) ? b.integer[b_i++ - b_frac_sz] : 0;
        borrow = subtract(result_int.back(), val_a, val_b, borrow);
    }

    return std::make_pair(result_int, result_frac);
}


int FixedPoint::subtract(uint32_t &res, uint32_t val_a, uint32_t val_b, uint32_t borrow) const {
    uint32_t int_res = val_a ^ val_b;
    for (size_t bit_i = 0; bit_i < 32; bit_i++) {
        switch (borrow) {
        case 0:
            res = res | (int_res & (1 << bit_i));
            if ((val_a & (1 << bit_i)) == 0 && (val_b & (1 << bit_i)) != 0) {
                borrow = 1; 
            }
            break;
        case 1:
            res = res | ((~int_res) & (1 << bit_i));
            if ((val_a & (1 << bit_i)) != 0 && (val_b & (1 << bit_i)) == 0) {
                borrow = 0; 
            }
            break;
        }
    }
    return borrow;
}

std::vector<uint32_t> FixedPoint::subtract_vec(const std::vector<uint32_t> &a, const std::vector<uint32_t> &b) const {
    uint32_t borrow = 0; 

    uint32_t a_i = 0, b_i = 0;
    uint32_t max_sz = std::max(a.size(), b.size());

    std::vector<uint32_t> result;

  
    while (a_i < max_sz && b_i < max_sz) {
        result.push_back(0);

        uint32_t val_a = (a_i < a.size()) ? a[a_i++] : 0;
        uint32_t val_b = (b_i < b.size()) ? b[b_i++] : 0;
        borrow = subtract(result.back(), val_a, val_b, borrow);
    }
    return result;
}

std::pair<std::vector<uint32_t>, std::vector<uint32_t>>
FixedPoint::divide(const FixedPoint &a, const FixedPoint &b) const {

    std::vector<uint32_t> result_int;
    std::vector<uint32_t> result_frac;

    uint32_t a_int_sz  = a.integer.size();
    uint32_t a_frac_sz = a.fractional.size();
    uint32_t b_frac_sz = b.fractional.size();

    uint32_t a_sz = a_int_sz + a_frac_sz;

    FixedPoint Remainder{0, 0};
    uint32_t bit_taken = 0;

    FixedPoint Divider{0, 0};
    std::vector<uint32_t> divider(b.fractional);
    divider.insert(divider.end(), b.integer.begin(), b.integer.end());

    if (divider.empty()) {
        throw std::runtime_error("Attempted division by zero");
    }

    Divider.integer = divider;

    for (uint32_t bit_i = 0; bit_i < (a_sz + 2 * b_frac_sz) * 32; bit_i++) {

        uint32_t addition;

        if (bit_i < a_int_sz * 32) {
            addition = ((a.integer[a_int_sz - 1 - (bit_i / 32)] >> (31 - bit_taken)) & 0x00000001);
            add_bit_div(Remainder.integer, bit_i, addition);
        } else if (bit_i < (a_int_sz + a_frac_sz) * 32) {
            addition = ((a.fractional[a_frac_sz - 1 - ((bit_i - a_int_sz * 32) / 32)] >> (31 - bit_taken)) & 0x00000001);
            add_bit_div(Remainder.integer, bit_i, addition);
        } else {
            add_bit_div(Remainder.integer, bit_i, false);
        }

        bit_taken = (bit_taken + 1) % 32;

        if (Remainder >= Divider) {
            Remainder -= Divider;
            if (bit_i / 32 < a_int_sz + b_frac_sz)
                add_bit_div(result_int, bit_i, true);
            else
                add_bit_div(result_frac, bit_i, true);
        } else {
            if (bit_i / 32 < a_int_sz + b_frac_sz)
                add_bit_div(result_int, bit_i, false);
            else
                add_bit_div(result_frac, bit_i, false);
        }
    }

    return std::make_pair(result_int, result_frac);
}

bool FixedPoint::not_less_vec(const std::vector<uint32_t> &a, const std::vector<uint32_t> &b) const {

    for (int i = std::max(a.size(), b.size()) - 1; i >= 0; i--) {
        uint32_t val_a = (((uint32_t) i) < a.size() ? a[i] : 0);
        uint32_t val_b = (((uint32_t) i) < b.size() ? b[i] : 0);
        if (val_a > val_b) {
            return true;
        }
        if (val_a < val_b) {
            return false;
        }
    }

    return true;
}

void FixedPoint::add_bit_div(std::vector<uint32_t> &vec, uint32_t bit_added, bool is_one) const {
    if (bit_added % 32 == 0 || vec.back() & 0x80000000) vec.push_back(0);
    for (uint32_t i = vec.size() - 1; i > 0; i--) {
        vec[i] <<= 1;
        vec[i] |= vec[i - 1] >> 31;
    }
    vec[0] <<= 1;
    vec[0] |= (is_one ? 1 : 0);
}

/**
 * @brief Преобразует целую часть из десятичной строки в двоичную
 * @param num_str Десятичная строка
 * @return Двоичное представление целой части
 */
std::vector<uint32_t> FixedPoint::int_part_to_bin(const std::string &num_str) const {
    std::string cur_num_str = num_str;  
    std::vector<uint32_t> binary_result; 

    if (cur_num_str == "0") {
        binary_result.push_back(0);
        return binary_result;
    }

    int bit_added = 0;

  
    while (cur_num_str != "0") {
        int remainder = 0;
        std::string result;

        for (char digit_c : cur_num_str) {
            int cur_digit = (digit_c - '0') + remainder * 10;
            int quotient = cur_digit / 2;
            remainder = cur_digit % 2;

            result += std::to_string(quotient);
        }

    
        result.erase(0, result.find_first_not_of('0'));
        if (result.empty()) {
            result = "0";
        }

        if (bit_added == 0) binary_result.push_back(0);

        binary_result.back() = binary_result.back() | (remainder << bit_added);

        cur_num_str = result;
        bit_added = (bit_added + 1) % 32;
    }

    return binary_result;
}

/**
 * @brief Умножает десятичную строку на 2
 * @param num_str Входная строка
 * @return Результат умножения в виде строки
 */
std::string FixedPoint::mult_by_two(const std::string &num_str) const {
    std::string result = "";
    int carry = 0;

    for (int i = num_str.size() - 1; i >= 0; i--) {
        int value = (num_str[i] - '0') * 2 + carry;
        carry = value / 10;
        result = std::to_string(value % 10) + result;
    }

    if (carry > 0) {
        result = std::to_string(carry) + result;
    }

    return result;
}

/**
 * @brief Преобразует дробную часть из десятичной строки в двоичную
 * @param frac_str Дробная часть в виде строки
 * @param frac_bits Требуемое количество бит
 * @return Двоичное представление дробной части
 */
std::vector<uint32_t> FixedPoint::frac_to_binary(const std::string &frac_str, int frac_bits) const {
    std::vector<uint32_t> binary;
    std::string frac_part = frac_str;
    uint32_t frac_part_size = frac_part.size();

    for (int i = 0, bit_added = 0; i < frac_bits && !frac_part.empty(); ++i, bit_added = (bit_added + 1) % 32) {
     
        std::string multiplied = mult_by_two(frac_part);

        if (bit_added == 0) binary.insert(binary.begin(), 0);

        if (multiplied.size() == frac_part_size) {
            binary[0] = (binary[0] << 1) | 0;
            frac_part = multiplied;
        } else {
            binary[0] = (binary[0] << 1) | 1;
            frac_part = multiplied.substr(1);
        }

        if (multiplied.size() == 0) {
            frac_part = "0";
        }
    }
    if (!binary.empty())
        binary[0] <<= 32 - (frac_bits % 32);

    return binary;
}

/**
 * @brief Преобразует десятичную строку в двоичное представление
 * @param num_str Входная строка
 * @param frac_bits Количество бит дробной части
 * @return Пару: целая и дробная части в двоичном виде
 */
std::pair<std::vector<uint32_t>, std::vector<uint32_t>>
FixedPoint::decimal_to_binary(const std::string& num_str, int frac_bits) const {
    uint32_t is_sign = (num_str[0] == '-' || num_str[0] == '+' ? 1 : 0);

   
    size_t dot_pos = num_str.find('.');

  
    if (dot_pos == std::string::npos) {
        std::vector<uint32_t> binary_integer = int_part_to_bin(num_str.substr(is_sign));
        std::vector<uint32_t> binary_fraction;
        uint32_t frac_sz = (frac_bits % 32 == 0 ? frac_bits / 32 : frac_bits / 32 + 1);
        for (uint32_t i = 0; i < frac_sz; i++) {
            binary_fraction.push_back(0);
        }
        return std::make_pair(binary_integer, binary_fraction);
    }

    // Извлечение целой и дробной части
    std::string integer_part_str = num_str.substr(is_sign, dot_pos - is_sign);

    std::string frac_partStr = num_str.substr(dot_pos + 1);

    // Преобразование целой части в двоичную
    std::vector<uint32_t> binary_integer = int_part_to_bin(integer_part_str);

    // Преобразование дробной части в двоичную
    std::vector<uint32_t> binary_fraction = frac_to_binary(frac_partStr, frac_bits);

    // Объединение полученных результатов
    return std::make_pair(binary_integer, binary_fraction);
}

//сдвиг влево
FixedPoint FixedPoint::operator<<(int n) const {
    FixedPoint result = *this;
    for (int i = 0; i < n; ++i) {
        result = result * FixedPoint("2.0");
    }
    return result;
}

//сдвиг вправо
FixedPoint FixedPoint::operator>>(int n) const {
    FixedPoint result = *this;
    for (int i = 0; i < n; ++i) {
        result = result / FixedPoint("2.0");
    }
    return result;
}

// Деление с остатком (возвращает пару: частное и остаток)
std::pair<FixedPoint, FixedPoint> FixedPoint::divide_with_remainder(const FixedPoint &other) const {
    if (other.is_zero()) {
        throw std::runtime_error("Division by zero");
    }

    // Получаем целочисленное частное
    FixedPoint quotient = *this / other;
    quotient.set_precision(0); // Отбрасываем дробную часть
    
    // Вычисляем остаток
    FixedPoint remainder = *this - (quotient * other);
    
    // Корректируем знак остатка (должен совпадать с делимым)
    remainder.is_negative = this->is_negative;
    
    return {quotient, remainder};
}

// Побитовый XOR 
FixedPoint FixedPoint::operator^(const FixedPoint &other) const {
    // Приводим числа к одинаковому количеству дробных битов
    int max_frac_bits = std::max(fractional_bits, other.fractional_bits);
    FixedPoint a = *this;
    FixedPoint b = other;
    a.set_precision(max_frac_bits);
    b.set_precision(max_frac_bits);
    
    // Собираем все биты (целые + дробные) в один вектор
    std::vector<uint32_t> bits_a = a.integer;
    bits_a.insert(bits_a.end(), a.fractional.begin(), a.fractional.end());
    
    std::vector<uint32_t> bits_b = b.integer;
    bits_b.insert(bits_b.end(), b.fractional.begin(), b.fractional.end());
    
    // Выполняем XOR
    size_t max_size = std::max(bits_a.size(), bits_b.size());
    std::vector<uint32_t> result_bits(max_size, 0);
    for (size_t i = 0; i < max_size; ++i) {
        uint32_t val_a = (i < bits_a.size()) ? bits_a[i] : 0;
        uint32_t val_b = (i < bits_b.size()) ? bits_b[i] : 0;
        result_bits[i] = val_a ^ val_b;
    }
    
    // Разделяем обратно на целую и дробную части
    FixedPoint result("0.0", max_frac_bits);
    size_t int_size = a.integer.size();
    result.integer.assign(result_bits.begin(), result_bits.begin() + int_size);
    result.fractional.assign(result_bits.begin() + int_size, result_bits.end());
    
    return result;
}


// Определяемый пользователем оператор литерала для создания объектов с фиксированной точкой
FixedPoint operator""_long(long double number) {
    return FixedPoint(std::to_string(number), 64); 

}



