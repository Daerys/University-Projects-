#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using Matrix = std::vector<std::vector<int>>;

Matrix matrix;
namespace {

    void initPath(std::vector<std::size_t> & vert, std::size_t n) {
        for (std::size_t i = 0 ; i < n; ++i) {
            vert[i] = i + 1;
        }
    }

    void initMatrix(Matrix & matrix, int n) {
        for (std::size_t i = 0; i <= n; ++i) {
            matrix[i].resize(n + 1, 0);
        }

        for (int i = 2; i <= n; ++i) {
            std::string edges;
            std::cin >> edges;

            for (int j = 0; j < i - 1; ++j) {
                if (edges[j] == '1') {
                    matrix[i][j + 1] = 1;
                } else {
                    matrix[j + 1][i] = 1;
                }
            }
        }
    }

    void dumpMatrix(Matrix & matrix, int n) {
        for (int i = 1; i <= n; ++i) {
            std::cout << i << ": [";
            for (int j = 1; j <= n; ++j) {
                if (matrix[i][j] == 1) {
                    std::cout << " " <<  j << " ";
                }
            }
            std::cout << "]" << '\n';
        }
    }

    void dumpVertexes(std::vector<std::size_t> & vertexes) {
        for (auto i : vertexes) {
            std::cout << i << ' ';
        }
    }

    bool comp(std::size_t u, std::size_t v) {
        return matrix[u][v] == 1;
    }

    void extractCycleOuttaPath(std::vector<std::size_t> & cycle, std::vector<std::size_t> & path) {
        for (std::size_t i = path.size() - 1; i >= 2; --i) {
            if (matrix[path[i]][path[0]] == 1) {
                cycle.insert(cycle.begin(), path.begin(), path.begin() + i + 1);
                path.erase(path.begin(), path.begin() + i + 1);
                break;
            }
        }
    }

    bool allIn(std::size_t vertex, std::vector<std::size_t> & cycle) {
        for (auto i : cycle) {
            if (matrix[i][vertex] != 1) {
                return false;
            }
        }
        return true;
    }

    std::size_t findFirstOut(std::size_t vertex, std::vector<std::size_t> & cycle) {
        for (std::size_t i = 0; i < cycle.size(); i++) {
            if (matrix[vertex][cycle[i]] == 1) {
                return i;
            }
        }
        return 0;
    }

    void createCycle(std::vector<std::size_t> & path, std::vector<std::size_t> & cycle) {
        std::vector<std::size_t> subPath;
        while (!path.empty()) {
            if (allIn(path[0], cycle)) {
                subPath.push_back(path.front());
                path.erase(path.begin());
                continue;
            }
            std::size_t index = findFirstOut(path[0], cycle);
            subPath.push_back(path.front());
            path.erase(path.begin());
            cycle.insert(cycle.begin() + index, subPath.begin(), subPath.end());
            subPath.clear();
        }
    }
}


int main() {
    int n;
    std::cin >> n;
    matrix.resize(n + 1);
    initMatrix(matrix, n);

    std::vector<std::size_t> hamiltonPath(n);
    initPath(hamiltonPath, n);

    std::stable_sort(hamiltonPath.begin(), hamiltonPath.end(), comp);

    std::vector<std::size_t> hamiltonCycle;

    extractCycleOuttaPath(hamiltonCycle, hamiltonPath);

    createCycle(hamiltonPath, hamiltonCycle);

    dumpVertexes(hamiltonCycle);
}
