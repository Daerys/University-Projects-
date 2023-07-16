#include <iostream>
#include <algorithm>
#include <array>


namespace {
    void initLamps(std::array<size_t, 1001> & lamps, std::size_t n) {
        for (std::size_t i = 1; i <= n; ++i) {
            lamps[i] = i;
        }
    }

    void dumpLamps(std::array<size_t, 1001> lamps, std::size_t n) {
        std::cout << 0 << ' ';
        for (auto i = lamps.begin() + 1; i != lamps.begin() + n + 1; ++i) {
            std::cout << *i << ' ';
        }
        std::cout.flush();
    }

    bool lampsComparator(std::size_t i, std::size_t j) {
        std::cout << 1 << ' ' << i << ' ' << j << std::endl;
        std::string answer;
        std::cin >> answer;
        return answer == "YES";
    }
}

int main() {
    std::size_t n;
    std::cin >> n;
    std::array<std::size_t, 1001> lamps{0};

    initLamps(lamps, n);
    std::stable_sort(lamps.begin() + 1, lamps.begin() + n + 1, lampsComparator);

    dumpLamps(lamps, n);
}
