#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>

using Matrix = std::vector<std::set<std::size_t>>;


namespace {

    void initMatrix(Matrix & matrix, int n) {

        for (int i = 2; i <= n; ++i) {
            std::size_t u, v;
            std::cin >> u >> v;
            matrix[u].insert(v);
            matrix[v].insert(u);
        }
    }

    void dumpMatrix(Matrix & matrix, int n) {
        for (int i = 1; i <= n; ++i) {
            std::cout << i << ": [ ";
            for (auto j : matrix[i]) {
                std::cout << j << ' ';
            }
            std::cout << "]" << '\n';
        }
    }

    void dumpCode(std::vector<std::size_t> & code) {
        for (auto i : code) {

            std::cout << i << ' ';
        }
    }

    void
    fillQueue(std::priority_queue<size_t, std::vector<std::size_t>, std::greater<>> & queue, Matrix & matrix,
              size_t n) {
        for (std::size_t i = 1; i <= n; ++i) {
            if (matrix[i].size() == 1) {
                queue.push(i);
            }
        }
    }

    void encode(Matrix & matrix, std::size_t n) {
        std::vector<std::size_t> code;

        std::priority_queue <std::size_t, std::vector<std::size_t>, std::greater<>> queue;

        fillQueue(queue, matrix, n);

        code.reserve(n - 2);
        std::size_t cnt = n;
        while (cnt != 2) {
            --cnt;
            std::size_t nodeToRemove = queue.top();
            std::size_t parent = *matrix[nodeToRemove].begin();
            queue.pop();
            matrix[parent].erase(matrix[parent].find(nodeToRemove));
            if (matrix[parent].size() == 1) {
                queue.push(parent);
            }
            code.push_back(parent);
            matrix[nodeToRemove].clear();
        }
        dumpCode(code);
    }

    void extractCode(std::vector<std::size_t> & code, std::size_t n) {
        for (std::size_t i = 0; i < n - 2; ++i) {
            std::size_t a;
            std::cin >> a;
            code.push_back(a);
        }
    }

    void decode(std::vector<std::size_t> & code, std::size_t n) {
        std::priority_queue <std::size_t, std::vector<std::size_t>, std::greater<>> queue;
        std::size_t vertex = 1;
        bool hasReadQueue = false;
        std::vector<std::size_t> codeCopy(code);
        std::sort(codeCopy.begin(), codeCopy.end());
        std::vector<std::size_t> count(n + 1, 0);
        while (vertex <= n) {
            if (!hasReadQueue) {
                for (auto i: codeCopy) {
                    ++count[i];
                    while (vertex < i) {
                        queue.push(vertex++);
                    }
                    if (vertex == i) {
                        ++vertex;
                    }
                }
                hasReadQueue = true;
                continue;
            }
            queue.push(vertex++);
        }
        for (std::size_t i : code) {
            vertex = queue.top();
            queue.pop();
            std::cout << vertex << ' ' << i  << '\n';
            if (count[i] == 1) {
                queue.push(i);
            }
            --count[i];
        }
        vertex = queue.top();
        queue.pop();
        std::cout << vertex << ' ' << queue.top()  << '\n';
    }
}


int main() {
    std::size_t n;
    std::cin >> n;
//    Matrix matrix;
//    matrix.resize(n + 1);
//    initMatrix(matrix, n);
//    std::vector<std::size_t> code;
//    code.reserve(n + 1);
//    encode(matrix, n);
    std::vector<std::size_t> code;
    code.reserve(n - 2);
    extractCode(code, n);
    decode(code, n);
}