#include <iostream>
#include <string>
#include <vector>
#include <deque>

using Matrix = std::vector<std::vector<int>>;

namespace {
    void initMatrix(Matrix & matrix, int n) {
        for (int i = 2; i <= n; ++i) {
            std::string edges;
            std::cin >> edges;

            for (int j = 0; j < i - 1; ++j) {
                if (edges[j] == '1') {
                    matrix[i][j + 1] = 1;
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

    bool contains(Matrix & matrix, int vertexU, int vertexV) {
        return matrix[vertexU][vertexV] == 1;
    }

    void swap(std::deque<int> & queue, int a, int b) {
        while (b - a >= 0) {
            int temp = queue[a];
            queue[a] = queue[b];
            queue[b] = temp;
            ++a;
            --b;
        }
    }


    void findHamiltonCycle(Matrix & matrix, int n) {
        int cur = 0;
        std::deque<int> queue;

        for (int i = 1; i <= n; ++i) {
            queue.push_back(i);
        }

        for (int i = 0; i < n * (n - 1); ++i) {
            if (!contains(matrix, queue[0], queue[1])) {
                cur = 0;
                int j = 2;
                while ((!contains(matrix, queue[0], queue[j])
                        || !contains(matrix, queue[1], queue[j + 1])) && j < n) {
                    ++j;
                }
                if (j >= n) {
                    j = 2;
                    while (!contains(matrix, queue[0], queue[j])) {
                        ++j;
                    }
                }
                swap(queue, 1, j);
            }
            ++cur;
            if (cur > n) {
                break;
            }
            queue.push_back(queue.front());
            queue.pop_front();
        }

        for (int i = 0; i < n; ++i) {
            std::cout << queue[i] << " ";
        }
        std::cout << '\n';
    }
}


int main() {
    int n;
    std::cin >> n;
    Matrix matrix(n + 1, std::vector<int>(n + 1, 0));
    initMatrix(matrix, n);
    findHamiltonCycle(matrix, n);
}
