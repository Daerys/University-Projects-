#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>

using ll = long long;
size_t SIZE = 101;

int MOD = 998244353;

std::vector<ll> logCoef(SIZE);
std::vector<ll> expCoef(SIZE);
std::vector<std::vector<ll>> pows(SIZE);

ll mod(ll a) {
    return (a + MOD) % MOD;
}


struct Poly {
    size_t degree;
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

    Poly &operator+=(const Poly &other) {
        size_t size = std::max(degree, other.degree);
        coefficients.resize(size);
        for (int i = 0; i < size; ++i) {
            coefficients[i] = mod((*this)[i] + other[i]);
        }
        degree = size;
        return *this;
    }

    Poly &operator-=(const Poly &other) {
        size_t size = std::max(degree, other.degree);
        coefficients.resize(size);
        for (int i = 0; i < size; ++i) {
            coefficients[i] = mod((*this)[i] - other[i]);
        }
        degree = size;
        return *this;
    }

    Poly &operator*=(const Poly &other) {
        Poly ans = (*this) * other;
        coefficients = ans.coefficients;
        degree = ans.degree;
        return *this;
    }

    Poly &operator/=(const Poly &other) {
        Poly ans = (*this) / other;
        coefficients = ans.coefficients;
        degree = ans.degree;
        return *this;
    }

    Poly operator+(const Poly &other) const {
        size_t size = std::max(degree, other.degree);
        Poly ans(size);
        for (int i = 0; i < size; ++i) {
            ans.coefficients[i] = mod((*this)[i] + other[i]);
        }
        return ans;
    }

    Poly operator-(const Poly &other) const {
        size_t size = std::max(degree, other.degree);
        Poly ans(size);
        for (int i = 0; i < size; ++i) {
            ans.coefficients[i] = mod((*this)[i] - other[i]);
        }
        return ans;
    }

    Poly operator*(const Poly &other) const {
        Poly ans(degree + other.degree - 1);

        for (int i = 0; i < ans.degree; ++i) {
            ll sum = 0;
            for (int j = 0; j < i + 1; ++j) {
                sum = mod(sum + mod((*this)[j] * other[i - j]));
            }
            ans.coefficients[i] = sum;
        }
        return ans;
    }

    Poly operator/(const Poly &other) const {
        int n = 1000;
        Poly ans(n);
        ans.coefficients[0] = mod((*this)[0] / other[0]);
        for (int i = 1; i < n; ++i) {
            ll sum = 0;
            for (int j = 0; j < i; ++j) {
                sum = mod(sum + ans[j] * other[i - j]);
            }
            ans.coefficients[i] = mod(mod((*this)[i] - sum) / other[0]);
        }
        return ans;
    }

};

std::ostream & operator<<(std::ostream & out, const Poly & p) {
    for (auto i : p.coefficients) {
        out << i << " ";
    }
    return out;
}

void initPoly(std::vector<ll> & A, int size) {
    for (int i = 0; i <= size; ++i) {
        std::cin >> A[i];
    }
}

int main() {
    int n, m;
    std::cin >> n >> m;
    std::vector<ll> P(n + 1), Q(m + 1);
    initPoly(P, n);
    initPoly(Q, m);
    Poly A(P), B(Q);
    Poly C = A + B;
    std::cout << C.degree - 1 << '\n' << C << '\n';
    C = A * B;
    std::cout << C.degree - 1 << '\n' << C << '\n';
    C = A / B;
    std::cout << C;
}