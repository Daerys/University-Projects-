#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>
#include <numeric>
#include <cmath>

using ll = long long;

size_t SIZE = 10;

namespace {
    class Fraction {
        ll numerator;
        ll denominator;

    public:
        Fraction() {
            numerator = 0;
            denominator = 1;
        };

        Fraction(ll a, ll b) {
            if (a == 0) {
                numerator = 0;
                denominator = 1;
                return;
            }
            ll r = std::gcd(a, b);
            ll sign = (b < 0) ? -1 : 1;
            numerator = sign * a / r;
            denominator = sign * b / r;
        }

        Fraction operator*(ll x) const {
            ll a = std::gcd(x, denominator);
            return {numerator * (x / a), denominator / a};
        }

        Fraction operator/(ll c) const {
            return (*this) * Fraction(1, c);
        }

        Fraction operator+(Fraction other) const {
            ll num = numerator * (other.denominator / std::gcd(denominator, other.denominator)) +
                     other.numerator * (denominator / std::gcd(denominator, other.denominator));
            ll den = denominator / std::gcd(denominator, other.denominator) * other.denominator;
            return {num, den};
        }

        Fraction operator-() const {
            return {-numerator, denominator};
        }

        Fraction operator-(Fraction other) const {
            return (*this) + (-other);
        }

        Fraction operator*(Fraction other) const {
            ll num = numerator * other.numerator;
            ll den = denominator * other.denominator;
            return {num , den };
        }

        Fraction operator/(Fraction other) const {
            return (*this) * Fraction(other.denominator, other.numerator);
        }

        bool operator!=(const Fraction &other) const {
            return !(numerator == other.numerator && denominator == other.denominator);
        }

        friend std::ostream &operator<<(std::ostream &os, const Fraction &f) {
            os << f.numerator << '/' << f.denominator;
//            os << lround((double) f.numerator / (double) f.denominator);
            return os;
        }
    };

    const Fraction ZERO{};
    const Fraction ONE{1, 1};

    struct Poly {
        Poly() = default;

        size_t degree{};
        std::vector<Fraction> coefficients;

        explicit Poly(size_t deg) {
            degree = deg;
            coefficients.resize(deg);
        }

        explicit Poly(std::vector<Fraction> coef) {
            degree = coef.size();
            coefficients = std::move(coef);
        }

        Fraction operator[](size_t index) const {
            return index < degree ? coefficients[index] : ZERO;
        }

        Poly operator[](Poly &z) const {
            size_t d = z.degree - 1;
            Poly ans(SIZE);
            ans.coefficients[0] = (*this)[0];
            for (int i = 1; i * d < SIZE; ++i) {
                // (*this)[i] -> ans.coefficients[i * d];
                ans.coefficients[i * d] = (*this)[i];
            }
            return ans;
        }

        Poly operator+(const Poly other) const {
            size_t size = std::max(degree, other.degree);
            Poly ans(size);
            for (int i = 0; i < size; ++i) {
                ans.coefficients[i] = (*this)[i] + other[i];
            }
            return ans;
        }

        Poly operator-(const Poly &other) const {
            size_t size = std::max(degree, other.degree);
            Poly ans(size);
            for (int i = 0; i < size; ++i) {
                ans.coefficients[i] = (*this)[i] - other[i];
            }
            return ans;
        }

        Poly operator*(const Poly &other) const {
            Poly ans(SIZE + 1);

            for (int i = 0; i < SIZE + 1; ++i) {
                Fraction sum = ZERO;
                for (int j = 0; j < i + 1; ++j) {
                    sum = sum + (*this)[j] * other[i - j];
                }
                ans.coefficients[i] = sum;
            }
            return ans;
        }

        Poly operator*(Fraction c) const {
            Poly ans = *this;
            for (size_t i = 0; i < std::min(ans.coefficients.size(), SIZE + 1); ++i) {
                ans.coefficients[i] = ans[i] * c;
            }
            return ans;
        }

        Poly operator/(const Poly &other) const {
            Poly ans(SIZE);
            ans.coefficients[0] = (*this)[0] / other[0];
            for (int i = 1; i < SIZE; ++i) {
                Fraction sum = ZERO;
                for (int j = 0; j < i; ++j) {
                    sum = sum + ans[j] * other[i - j];
                }
                ans.coefficients[i] = (*this)[i] - sum / other[0];
            }
            return ans;
        }

        Poly operator/(Fraction c) const {
            Poly ans = *this;
            for (size_t i = 0; i < ans.coefficients.size(); ++i) {
                ans.coefficients[i] = ans[i] / c;
            }
            return ans;
        }

        Poly operator/(ll c) const {
            Poly ans = *this;
            for (size_t i = 0; i < ans.coefficients.size(); ++i) {
                ans.coefficients[i] = ans[i] / Fraction(c, 1);
            }
            return ans;
        }

        void operator-=(Poly &other) {
            for (int i = 0; i < coefficients.size(); ++i) {
                coefficients[i] = (*this)[i] - other[i];
            }
        }

        void operator+=(const Poly &other) {
            for (int i = 0; i < coefficients.size(); ++i) {
                coefficients[i] = (*this)[i] + other[i];
            }
        }

        bool operator==(const Poly &other) {
            for (int i = 0; i < std::max(other.coefficients.size(), coefficients.size()); ++i) {
                if (other[i] != (*this)[i]) return false;
            }
            return true;
        }

        [[nodiscard]] Fraction evaluate(Fraction t) const {
            Fraction ans = ZERO;
            for (int i = coefficients.size() - 1; i >= 0; --i) {
                ans = ans * t + (*this)[i];
            }
            return ans;
        }

        friend std::ostream &operator<<(std::ostream &out, const Poly &p) {
            for (int i = 0; i < SIZE; ++i) {
                out << p[i] << " ";
            }
            return out;
        }
    };

    const Poly ONE_POLY(std::vector<Fraction>{ONE});
    const Poly ZERO_POLY(std::vector<Fraction>{ZERO});

}

int main() {
    int r, k;
    std::cin >> r >> k;
    std::vector<Fraction> p(k + 1);
    for (int i = 0; i < k + 1; ++i) {
        int c;
        std::cin >> c;
        p[i] = {c, 1};
    }
    Poly P(p);
    Poly tmp = P;
    Poly C(std::vector<Fraction>{ONE, {-r, 1}});
//    std::cout << tmp.evaluate(Fraction(1, r)) << "\n";
    std::vector<Fraction> coef(k + 1);
    for (int i = 0; i < k + 1; ++i) {
        coef[k - i] = tmp.evaluate(Fraction(1, r));
        if (tmp == ONE_POLY) {
            break;
        }
        tmp = (tmp - Poly(std::vector<Fraction>{coef[k - i]})) / C;
    }
    Poly K(coef);
    ll fact = 1;
    Poly ans = ZERO_POLY;
    Poly n(std::vector<Fraction>{ONE, ONE});
    Poly nm(std::vector<Fraction>{ONE});
    for (int i = 0; i < k + 1; ++i) {
        Poly a = (nm * (coef[i] / fact));
        ans = ans + a;
        nm = nm * n;
        n.coefficients[0] = n.coefficients[0] + ONE;
        fact *= i + 1;
    }
    SIZE = k + 1;
    std::cout << ans;
}