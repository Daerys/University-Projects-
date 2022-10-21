#pragma once

#include "complex.h"

#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <string>

struct Expression
{

    virtual Complex eval(std::map<std::string, Complex> const & values = {}) const = 0;

    virtual Expression * clone() const = 0;

    friend std::ostream & operator<<(std::ostream & out, const Expression & expression)
    {
        return expression.dump(out);
    };

    virtual std::ostream & dump(std::ostream & out) const = 0;

    virtual ~Expression(){};
};

template <char m_oper, class Operation>
class AbstractBinary : public Expression
{
private:
    std::ostream & dump(std::ostream & out) const override;
    Operation operation;
    std::shared_ptr<Expression> m_left;
    std::shared_ptr<Expression> m_right;

public:
    AbstractBinary(const Expression & left, const Expression & right)
        : m_left(left.clone())
        , m_right(right.clone())
    {
    }

    Expression * clone() const override;

    Complex eval(std::map<std::string, Complex> const & values = {}) const override;
};

template <char m_oper, class Operation>
class AbstractUnary : public Expression
{
private:
    std::ostream & dump(std::ostream & out) const override;
    Operation operation;
    std::shared_ptr<Expression> m_value;

public:
    AbstractUnary(const Expression & value)
        : m_value(value.clone())
    {
    }

    Complex eval(std::map<std::string, Complex> const & values = {}) const override;

    Expression * clone() const override;
};

struct Conj
{
    Complex operator()(const Complex & number) const
    {
        return ~number;
    }
};
class Const : public Expression
{
private:
    std::ostream & dump(std::ostream & out) const override;
    Complex m_value;

public:
    Const(const Complex & value)
        : m_value(value)
    {
    }

    Complex eval(std::map<std::string, Complex> const & /*values*/ = {}) const override;

    Expression * clone() const override;
};

class Variable : public Expression
{
private:
    std::ostream & dump(std::ostream & out) const override;
    std::string m_param;

public:
    Variable(const std::string_view str)
        : m_param(str)
    {
    }

    Complex eval(std::map<std::string, Complex> const & values = {}) const override;

    Expression * clone() const override;
};

using Add = AbstractBinary<'+', std::plus<Complex>>;
using Subtract = AbstractBinary<'-', std::minus<Complex>>;
using Multiply = AbstractBinary<'*', std::multiplies<Complex>>;
using Divide = AbstractBinary<'/', std::divides<Complex>>;

using Negate = AbstractUnary<'-', std::negate<Complex>>;
using Conjugate = AbstractUnary<'~', Conj>;

Add operator+(const Expression & left, const Expression & right);
Subtract operator-(const Expression & left, const Expression & right);
Divide operator/(const Expression & left, const Expression & right);
Multiply operator*(const Expression & left, const Expression & right);
Negate operator-(const Expression & value);
Conjugate operator~(const Expression & value);