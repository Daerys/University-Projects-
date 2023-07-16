#include <iostream>
#include <vector>
#include <set>

using Matrix = std::vector<std::set<std::size_t>>;


namespace {

    void initMatrix(Matrix & matrix, std::size_t m) {

        for (std::size_t  i = 0; i < m; ++i) {
            std::size_t u, v;
            std::cin >> u >> v;
            matrix[u].insert(v);
            matrix[v].insert(u);
        }
    }

    void dumpMatrix(Matrix & matrix, std::size_t  n) {
        for (int i = 1; i <= n; ++i) {
            std::cout << i << ": [ ";
            for (auto j : matrix[i]) {
                std::cout << j << ' ';
            }
            std::cout << "]" << '\n';
        }
    }

    void dumpVector(std::vector<std::size_t> & vector) {
        for (std::size_t i = 1; i < vector.size(); ++i) {
            std::cout << vector[i] << '\n';
        }
    }

    void setColors(std::vector<std::size_t> & colors, Matrix & matrix, std::size_t vertex, std::size_t k) {
        std::vector<bool> ban(k + 1, false);
        for (auto i : matrix[vertex]) {
            ban[colors[i]] = true;
        }
        for (std::size_t i = 1; i <= k; ++i) {
            if (!ban[i]) {
                colors[vertex] = i;
                break;
            }
        }
        ban.clear();
        for (auto i : matrix[vertex]) {
            if (colors[i] == 0) {
                setColors(colors, matrix, i, k);
            }
        }
    }

}


int main() {
    Matrix matrix;
    std::size_t n, m, k = 0;
    std::vector<std::size_t> colors;

    std::cin >> n >> m;
    colors.resize(n + 1, 0);
    matrix.resize(n + 1);
    initMatrix(matrix, m);
    for (auto & i : matrix) {
        k = std::max(k, i.size());
    }
    if (k % 2 == 0) {
        ++k;
    }
    std::cout << k << '\n';
    setColors(colors, matrix, 1, k);
    dumpVector(colors);
}
