#include "complex.h"

#include <cmath>

double Complex::real() const
{
    return m_re;
}

double Complex::imag() const
{
    return m_im;
}

double Complex::sumOfSquares() const
{
    return std::pow(m_re, 2.) + std::pow(m_im, 2.);
}

double Complex::abs() const
{
    return std::hypot(m_re, m_im);
}

std::string Complex::str() const
{
    std::string re = std::to_string(real());
    remove_zeros(re);
    std::string im = std::to_string(imag());
    remove_zeros(im);
    return "(" + re + "," + im + ")";
}

std::ostream & operator<<(std::ostream & outstream, const Complex & num)
{
    return outstream << num.str();
}

Complex operator+(const Complex & num1, const Complex & num2)
{
    return Complex(num1.real() + num2.real(), num1.imag() + num2.imag());
}

Complex operator-(const Complex & num1, const Complex & num2)
{
    return Complex(num1.real() - num2.real(), num1.imag() - num2.imag());
}

Complex operator*(const Complex & num1, const Complex & num2)
{
    return Complex(num1.real() * num2.real() - num1.imag() * num2.imag(), num1.real() * num2.imag() + num1.imag() * num2.real());
}

Complex operator/(const Complex & num1, const Complex & num2)
{
    Complex res = num1;
    bool isRational = num2.imag() == 0;
    double div = (isRational ? num2.real() : num2.sumOfSquares());
    res.m_re /= div;
    res.m_im /= div;
    return isRational ? res : res * (~num2);
}

Complex Complex::operator-() const
{
    return Complex(-m_re, -m_im);
}

Complex Complex::operator~() const
{
    return Complex(m_re, -m_im);
}

bool Complex::operator==(const Complex & num) const
{
    return (m_re == num.m_re) && (m_im == num.m_im);
}

bool Complex::operator!=(const Complex & num) const
{
    return !(*this == num);
}

Complex & Complex::operator+=(const Complex & num)
{
    *this = *this + num;
    return *this;
}

Complex & Complex::operator-=(const Complex & num)
{
    *this = *this - num;
    return *this;
}

Complex & Complex::operator*=(const Complex & num)
{
    *this = *this * num;
    return *this;
}

Complex & Complex::operator/=(const Complex & num)
{
    *this = *this / num;
    return *this;
}

void Complex::remove_zeros(std::string & basicString) const
{
    auto it = basicString.find_last_not_of('0');
    auto it_dot = basicString.find('.');
    if (it_dot <= it) {
        basicString.erase(it, basicString.size());
    }
}
