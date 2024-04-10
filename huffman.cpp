#include <algorithm>
#include <utility>
#include <iostream>

#include "huffman.h"


#define BYTE_VAL_COUNT 256
#define FIRST_CODE 0


std::vector<std::uint64_t> get_freqs(const std::vector<std::uint8_t> &data) {
    std::vector<std::uint64_t> freqs(BYTE_VAL_COUNT);

    for (const auto &val: data) {
        freqs[val]++;
    }

    return freqs;
}


std::vector<std::uint8_t> get_huff_code_lens(const std::vector<std::uint64_t> &non_zero_freqs) {
    std::uint16_t m = non_zero_freqs.size();
    std::vector<std::uint16_t> hr(2 * m);
    std::vector<std::pair<std::uint64_t, std::uint16_t>> h(m);

    for (std::uint16_t i = 0; i < m; i++) {
        h[i] = std::make_pair(non_zero_freqs[i], m + i);
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

    std::vector<std::uint8_t> code_lens(non_zero_freqs.size());

    // Determine code lengths
    for (std::uint16_t i = 0; i < non_zero_freqs.size(); i++) {
        std::uint16_t j = hr[non_zero_freqs.size() + i];
        std::uint8_t l = 1;

        while (j > 1) {
            j = hr[j];
            l++;
        }

        code_lens[i] = l;
    }

    return code_lens;
}


std::vector<std::pair<uint8_t, uint64_t>> get_huff_codes(const std::vector<std::uint64_t> &freqs) {
    std::vector<std::uint16_t> used_vals;
    std::vector<std::uint64_t> used_val_freqs;

    for (std::uint16_t i = 0; i < freqs.size(); i++) {
        if (freqs[i] > 0) {
            used_vals.push_back(i);
            used_val_freqs.push_back(freqs[i]);
        }
    }

    std::vector<std::pair<uint8_t, uint64_t>> codes;

    if (used_vals.empty()) {
        return codes;
    }

    const auto code_lens = get_huff_code_lens(used_val_freqs);
    std::vector<std::pair<uint8_t, uint16_t>> code_lens_and_used_vals(code_lens.size());

    for (std::uint16_t i = 0; i < code_lens.size(); i++) {
        code_lens_and_used_vals[i] = std::make_pair(code_lens[i], used_vals[i]);
    }

    std::sort(code_lens_and_used_vals.begin(), code_lens_and_used_vals.end());

    codes.resize(freqs.size());
    codes[code_lens_and_used_vals.front().second] = std::make_pair(code_lens_and_used_vals.front().first, FIRST_CODE);
    std::uint64_t prev_code = FIRST_CODE;
    std::uint64_t prev_len = code_lens_and_used_vals.front().first;

    for (auto it = code_lens_and_used_vals.begin() + 1, end = code_lens_and_used_vals.end(); it != end; it++) {
        std::uint64_t code = (prev_code + 1) << (it->first - prev_len);
        codes[it->second] = std::make_pair(it->first, code);
        prev_code = code;
        prev_len = it->first;
    }

    return codes;
}


std::vector<std::uint8_t> encode_huffman(const std::vector<std::uint8_t> &data) {
    auto freqs = get_freqs(data);
    auto codes = get_huff_codes(freqs);

    for (uint16_t i = 0; i < codes.size(); i++) {
        std::cout << i << " " << unsigned(codes[i].first) << " " << codes[i].second;
        std::cout << std::endl;
    }

    std::vector<std::uint8_t> encoded_data;
    
    return encoded_data;
}


std::vector<std::uint8_t> decode_huffman(const std::vector<std::uint8_t> &data) {
    std::vector<std::uint8_t> decoded_data = data;

    return decoded_data;
}
