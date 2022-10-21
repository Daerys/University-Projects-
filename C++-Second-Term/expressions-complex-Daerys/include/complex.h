#pragma once

#include <string>

struct Complex
{
    Complex(const double real = 0., const double imag = 0.)
        : m_re(real)
        , m_im(imag)
    {
    }

    double real() const;
    double imag() const;
    double sumOfSquares() const;
    double abs() const;
    std::string str() const;

    friend Complex operator+(const Complex & num1, const Complex & num2);
    friend Complex operator-(const Complex & num1, const Complex & num2);
    friend Complex operator*(const Complex & num1, const Complex & num2);
    friend Complex operator/(const Complex & num1, const Complex & num2);
    Complex operator-() const;
    Complex operator~() const;

    Complex & operator/=(const Complex & num);
    Complex & operator*=(const Complex & num);
    Complex & operator+=(const Complex & num);
    Complex & operator-=(const Complex & num);

    bool operator==(const Complex & num) const;
    bool operator!=(const Complex & num) const;

    friend std::ostream & operator<<(std::ostream &, const Complex &);

private:
    double m_re, m_im;
    void remove_zeros(std::string & basicString) const;
};
