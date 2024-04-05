#include <algorithm>
#include <vector>
#include <utility>
#include <cstdint>
#include <iostream>


std::vector<uint16_t> HClen(const std::vector<uint64_t> &freq) {
    uint16_t m = freq.size();
    std::vector<uint16_t> hr(2 * m), bitlen(m);
    std::vector<std::pair<uint64_t, uint16_t>> h(m);

    for (uint16_t i = 0; i < m; i++) {
        h[i] = std::make_pair(freq[i], m + i);
    }

    std::make_heap(h.begin(), h.end(), std::greater<>{});

    // Perform Hirschberg-Sieminski algorithm
    while (m > 1) {
        std::pop_heap(h.begin(), h.end(), std::greater<>{});
        auto a = h.back();
        h.pop_back();

        std::pop_heap(h.begin(), h.end(), std::greater<>{});
        auto b = h.back();
        h.pop_back();

        m--;
        hr[a.second] = m;
        hr[b.second] = m;

        h.push_back(std::make_pair(a.first + b.first, m));
        std::push_heap(h.begin(), h.end(), std::greater<>{});
    }

    // Determine code lengths
    for (uint16_t i = 0; i < freq.size(); i++) {
        uint16_t j = hr[freq.size() + i];
        uint16_t l = 1;

        while (j > 1) {
            j = hr[j];
            l++;
        }

        bitlen[i] = l;
    }

    return bitlen;
}





int main(int argc, char *argv[]) {
    std::vector<char> symb = {'a', 'b', 'c', 'd'};
    std::vector<uint64_t> freq = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

    auto a = HClen(freq);

    for (uint16_t &b: a) {
        std::cout << b << " ";
    }
    std::cout << std::endl;

    return 0;
}
