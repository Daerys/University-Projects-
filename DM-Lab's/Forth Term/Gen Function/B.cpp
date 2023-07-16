#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>

using ll = long long;
class Poly;

size_t SIZE = 101;

int MOD = 998244353;
std::vector<ll> logCoef(SIZE);

std::vector<ll> expCoef(SIZE);

ll mod(ll a) {
    return (a + MOD) % MOD;
}


ll getInv(ll a, ll deg = MOD - 2) {
    ll res = 1;
    a = a % MOD;

    while (deg) {
        if (deg & 1) {
            res = (res * a) % MOD;
        }

        deg >>= 1;
        a = (a * a) % MOD;
    }

    return res;
}

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

        for (int i = 0; i < std::min(ans.coefficients.size(), SIZE); ++i) {
            ll sum = 0;
            for (int j = 0; j < i + 1; ++j) {
                sum = mod(sum + mod((*this)[j] * other[i - j]));
            }
            ans.coefficients[i] = sum;
        }
        return ans;
    }

    Poly operator*(ll c) const {
        Poly ans = *this;
        for (size_t i = 0; i < std::min(ans.coefficients.size(), SIZE); ++i) {
            ans.coefficients[i] = mod(ans[i] * c);
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

    Poly operator/(ll c) const {
        Poly ans = *this;
        ll inv = getInv(c);
        for (size_t i = 0; i < ans.coefficients.size(); ++i) {
            ans.coefficients[i] = mod(ans[i] * inv);
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


void initLog() {
    int sign = 1;
    for (ll i = 1; i < SIZE; ++i) {
        logCoef[i] = mod(sign * getInv(i));
        sign *= -1;
    }
}

void initExp() {
    expCoef[0] = 1;
    for (ll i = 1; i < SIZE; ++i) {
        expCoef[i] = mod(expCoef[i - 1] * getInv(i));
    }
}

Poly initPoly(int n) {
    std::vector<ll> coef(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> coef[i];
    }
    return Poly(coef);
}


Poly exp(Poly & poly, ll m) {
    Poly ans(poly.degree);
    std::vector<Poly> pows(2);
    pows[0] = Poly(std::vector<ll>{1});
    pows[1] = poly;
    ans.coefficients[0] = 1;
    for (int i = 1; i < m; ++i) {
        ans = ans + (pows[i % 2] * expCoef[i]);
        pows[(i + 1) % 2] = pows[i % 2] * poly;
    }
    return ans;
}

Poly ln(Poly & poly, ll m) {
    Poly ans(poly.degree);
    std::vector<Poly> pows(2);
    pows[0] = {poly * poly};
    pows[1] = {poly};
    for (ll i = 1; i < m; ++i) {
        ans = ans + (pows[i % 2] * logCoef[i]);
        pows[i % 2] = pows[(i + 1) % 2] * poly;
    }
    return ans;
}

Poly sqrt(Poly &L, ll m) {
    Poly P = ln(L, m) / 2;
    return exp(P, m);
}

int main() {
    initExp();
    initLog();

    int n, m;
    std::cin >> n >> m;
    Poly P = initPoly(n + 1);
    Poly E = exp(P, m);
    Poly L = ln(P, m);


    Poly S = sqrt(P, m);
    for (int i = 0; i < m; ++i) {
        std::cout << S[i] << " ";
    }
    std::cout << '\n';
    for (int i = 0; i < m; ++i) {
        std::cout << E[i] << " ";
    }
    std::cout << '\n';
    for (int i = 0; i < m; ++i) {
        std::cout << L[i] << " ";
    }
    std::cout << '\n';
}
