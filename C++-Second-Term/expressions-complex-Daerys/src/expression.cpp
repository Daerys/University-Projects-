#include <expression.h>

template <char m_oper, class Operation>
std::ostream & AbstractBinary<m_oper, Operation>::dump(std::ostream & out) const
{
    out << "(";
    m_left->dump(out);
    out << " " << m_oper << " ";
    m_right->dump(out);
    return out << ")";
}

template <char m_oper, class Operation>
Expression * AbstractBinary<m_oper, Operation>::clone() const
{
    return new AbstractBinary(*this);
}

template <char m_oper, class Operation>
Complex AbstractBinary<m_oper, Operation>::eval(const std::map<std::string, Complex> & values) const
{
    return operation(m_left->eval(values), m_right->eval(values));
}

template <char m_oper, class Operation>
std::ostream & AbstractUnary<m_oper, Operation>::dump(std::ostream & out) const
{
    out << "(" << m_oper;
    m_value->dump(out);
    return out << ")";
}

template <char m_oper, class Operation>
Complex AbstractUnary<m_oper, Operation>::eval(const std::map<std::string, Complex> & values) const
{
    return operation(m_value->eval(values));
}

template <char m_oper, class Operation>
Expression * AbstractUnary<m_oper, Operation>::clone() const
{
    return new AbstractUnary(*this);
}

std::ostream & Const::dump(std::ostream & out) const
{
    return out << m_value;
}

Complex Const::eval(const std::map<std::string, Complex> &) const
{
    return m_value;
}

Expression * Const::clone() const
{
    return new Const(*this);
}

Complex Variable::eval(const std::map<std::string, Complex> & values) const
{
    return values.at(m_param);
}

Expression * Variable::clone() const
{
    return new Variable(*this);
}

std::ostream & Variable::dump(std::ostream & out) const
{
    return out << m_param;
}

Add operator+(const Expression & left, const Expression & right)
{
    return Add(left, right);
}

Subtract operator-(const Expression & left, const Expression & right)
{
    return Subtract(left, right);
}

Divide operator/(const Expression & left, const Expression & right)
{
    return Divide(left, right);
}

Multiply operator*(const Expression & left, const Expression & right)
{
    return Multiply(left, right);
}

Negate operator-(const Expression & value)
{
    return Negate(value);
}

Conjugate operator~(const Expression & value)
{
    return Conjugate(value);
}
