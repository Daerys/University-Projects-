#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>

using ll = long long;
class Poly;


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
    
    void add(ll c) {
        ++degree;
        coefficients.push_back(c);
        }
    
    void trim() {
        auto last_non_zero = std::find_if(coefficients.rbegin(), coefficients.rend(), [](int n) { return n != 0; });
        coefficients.erase(last_non_zero.base(), coefficients.end());
        degree = coefficients.size() - 1;
        }
    
    ll operator[](size_t index) const {
        return index < degree ? coefficients[index] : 0;
        }
    
    Poly operator*(const Poly &other) const {
        Poly ans(degree + other.degree - 1);
        
        for (int i = 0; i < std::max(degree, other.degree) - 1; ++i) {
            ll sum = 0;
            for (int j = 0; j < i + 1; ++j) {
                sum = sum + (*this)[j] * other[i - j];
                }
            ans.coefficients[i] = sum;
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

// Q[0] = 1
ll getSum(const Poly & P, const Poly & Q, size_t n) {
    ll sum = 0;
    for (int j = 0; j < n; ++j) {
        sum = sum + P[j] * Q[n - j];
        }
    return sum;
}

int main() {
    int k;
    std::cin >> k;
    std::vector<ll> A(k);
    std::vector<ll> C(k);
    
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < k; ++j) {
            if (i) {
                std::cin >> C[j];
                continue;
                }
            std::cin >> A[j];
            }
        }
    
    std::vector<ll> cf(k + 1);
    cf[0] = 1;
    for (int i = 1; i < k + 1; ++i) {
        cf[i] = -C[i - 1];
        }
    
    Poly AP = Poly(A);
    Poly Q(cf);
    Poly P = Q * AP;
    P.trim();
    Q.trim();
    std::cout << P.degree << '\n' << P << '\n' << Q.degree << '\n' << Q;
}