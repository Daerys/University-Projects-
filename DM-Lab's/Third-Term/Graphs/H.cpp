#include <iostream>
#include <vector>
#include <algorithm>

using Matrix = std::vector<std::vector<std::size_t>>;


namespace {

    void initMatrix(Matrix & matrix, std::size_t m) {

        for (std::size_t  i = 0; i < m; ++i) {
            std::size_t u, v;
            std::cin >> u >> v;
            matrix[u][v] = 1;
            matrix[v][u] = 1;
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

    void dumpVector(std::vector<int> & vector) {
        for (std::size_t i = vector.size() - 1; i != 0; --i) {
            std::cout << vector[i] << ' ';
        }
        std::cout << vector[0];
    }

    Matrix mergeVertexes(std::size_t u, std::size_t v, Matrix & matrix) {

        std::size_t m =  matrix.size();
        std::size_t a = std::max(u,v), b = std::min(u,v);

        Matrix newMatrix = matrix;
        newMatrix.erase(newMatrix.begin() + a);
        for (std::size_t i = 0; i < m - 1; ++i) {
            for(std::size_t j = a + 1; j < m; ++j) {
                newMatrix[i][j - 1] = newMatrix[i][j];
            }
        }
        for (std::size_t i = 0; i < a; ++i) {
            if (matrix[i][a] == 0) {
                continue;
            }
            newMatrix[i][b] = newMatrix[b][i] = matrix[i][a];
        }
        for (std::size_t i = a + 1; i < m; ++i) {
            if (matrix[i][a] == 0) {
                continue;
            }
            newMatrix[b][i - 1] = newMatrix[i - 1][b] = matrix[i][a];
        }
        newMatrix[b][b] = 0;
        return newMatrix;
    }

    Matrix splitVertexes(std::size_t u, std::size_t v, Matrix & matrix) {
        Matrix newMatrix;
        newMatrix.resize(matrix.size());
        for (std::size_t i = 0; i < matrix.size(); ++i) {
            newMatrix[i] = matrix[i];
            if (i == u) {
                newMatrix[i][v] = 0;
            }
            if (i == v) {
                newMatrix[i][u] = 0;
            }
        }
        return newMatrix;
    }

    std::vector<int> countPolynomial(Matrix & matrix, std::vector<int> & poly) {
        std::size_t m = matrix.size();
        std::vector<int> polin(poly.size(), 0);
        if (m == 3) {
            if (matrix[1][2] == 0) {
                ++polin[2];
                return polin;
            }
            --polin[1];
            ++polin[2];
            return polin;
        }

        for (std::size_t i = 1; i < m; ++i) {
            std::size_t vert = m;
            for (std::size_t j = 1; j < m; ++j) {
                if (matrix[i][j] == 1) {
                    vert = j;
                    break;
                }
            }
            if (vert == m) {
                continue;
            }
            Matrix merged = mergeVertexes(i, vert, matrix);
            Matrix splitted = splitVertexes(i, vert, matrix);
            std::vector<int> a = countPolynomial(merged, poly);
            std::vector<int> b = countPolynomial(splitted, poly);
            for (std::size_t i = 0; i < poly.size(); i++) {
                polin[i] = b[i] - a[i];
            }
            return polin;
        }
        ++polin[matrix.size() - 1];
        return polin;
    }

}


int main() {
    std::vector<int> poly;
    std::size_t n, m;
    std::cin >> n >> m;

    poly.resize(n + 1, 0);
    Matrix matrix(n + 1, std::vector<std::size_t>(n + 1, 0));
    initMatrix(matrix, m);
    std::cout << n << '\n';
    poly = countPolynomial(matrix, poly);
    dumpVector(poly);
}