#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>
#include <numeric>
#include <cmath>

using ll = long long;

size_t SIZE = 10;

namespace {
    struct Poly {
        Poly() = default;

        size_t degree{};
        std::vector<ll> coefficients;

        explicit Poly(int deg) {
            degree = deg;
            coefficients.resize(deg);
        }

        explicit Poly(std::vector<ll> coef) {
            degree = coef.size();
            coefficients = std::move(coef);
        }

        ll operator[](size_t index) const {
            return index < degree ? coefficients[index] : 0;
        }

        Poly operator*(const Poly &other) const {
            Poly ans(degree + other.degree - 1);

            for (int i = 0; i < std::min(ans.degree, SIZE + 1); ++i) {
                for (int j = 0; j < i + 1; ++j) {
                    ans.coefficients[i] += (*this)[j] * other[i - j];
                }
            }
            return ans;
        }

        [[nodiscard]] ll evaluate(ll t) const {
            ll ans = 0;
            for (int i = coefficients.size() - 1; i >= 0; --i) {
                ans = ans * t + (*this)[i];
            }
            return ans;
        }

        void trim() {
            auto last_non_zero = std::find_if(coefficients.rbegin(), coefficients.rend(), [](int n) { return n != 0; }) - 1;
            coefficients.erase(last_non_zero.base(), coefficients.end());
            degree = coefficients.size() - 1;
        }

        friend std::ostream &operator<<(std::ostream &out, const Poly &p) {
            for (int i = 0; i < p.degree; ++i) {
                out << p[i] << " ";
            }
            return out;
        }
    };

    Poly blessRNG(Poly & a, Poly & b) {
        Poly ans(a.degree + b.degree - 1);

        for (int i = 0; i < ans.degree; ++i) {
            for (int j = 0; j < i + 1; ++j) {
                ans.coefficients[i] += a[j] * b[i - j];
            }
        }
        return ans;
    }

    const Poly ONE_POLY(std::vector<ll>{1});
    const Poly ZERO_POLY(std::vector<ll>{0});

    Poly pow(Poly &base, ll exponent) {
        if (!exponent) { return ONE_POLY; }
        Poly result(std::vector<ll>({1}));
        Poly temp_base = base;

        while (exponent) {
            if (exponent & 1) {
                result = blessRNG(result, temp_base);
            }
            temp_base = blessRNG(temp_base, temp_base);
            exponent >>= 1;
        }

        return result;
    }

}

int main() {
    int r, k;
    std::cin >> r >> k;
    SIZE = k;
    ++k;
    Poly Q = pow(*new Poly(std::vector<ll>{1, -r}), k);
    std::vector<ll> d(k);
    for (int i = 0; i < k; ++i) {
        std::cin >> d[i];
    }
    Poly D(d);
    ll rk = 1;
    std::vector<ll> c (k, 0);
    for (int i = 0; i < k; ++i) {
        c[i] = D.evaluate(i) * rk;
        rk *= r;
    }
    Poly C(c);
    Poly P = Q * C;
    P.trim();
    std::cout << P.degree - 1 << '\n' << P << '\n' << Q.degree - 1 << '\n' << Q;
//
//    Poly AP = Poly(A);
//    Poly Q(cf);
//    Poly P = Q * AP;
//    P.trim();
//    Q.trim();
//    std::cout << P.degree << '\n' << P << '\n' << Q.degree << '\n' << Q;
}