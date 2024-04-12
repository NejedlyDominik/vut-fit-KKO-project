#include <algorithm>
#include <utility>
#include <iostream>

#include "huffman.h"


#define BYTE_VAL_COUNT 256
#define FIRST_CODE 0
#define BYTE_BITLEN 8


std::vector<std::uint64_t> get_freqs(const std::vector<std::uint8_t> &data) {
    std::vector<std::uint64_t> freqs(BYTE_VAL_COUNT);

    for (const auto &val: data) {
        freqs[val]++;
    }

    return freqs;
}


class HuffmanEncoder {
    private:
        std::vector<std::pair<uint8_t, uint64_t>> codes;
        std::uint8_t encoded_byte;
        std::uint8_t remaining_byte_bit_count;
        std::vector<std::vector<uint8_t>> code_bitlens_to_values;

        std::vector<std::uint8_t> compute_code_bitlens(const std::vector<std::uint64_t> &non_zero_freqs) {
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

            std::vector<std::uint8_t> code_bitlens(non_zero_freqs.size());

            // Determine code bit lengths
            for (std::uint16_t i = 0; i < non_zero_freqs.size(); i++) {
                std::uint16_t j = hr[non_zero_freqs.size() + i];
                std::uint8_t l = 1;

                while (j > 1) {
                    j = hr[j];
                    l++;
                }

                code_bitlens[i] = l;
            }

            return code_bitlens;
        }

        void compute_codes(const std::vector<std::uint64_t> &freqs) {
            std::vector<std::uint16_t> used_vals;
            std::vector<std::uint64_t> used_val_freqs;

            for (std::uint16_t i = 0; i < freqs.size(); i++) {
                if (freqs[i] > 0) {
                    used_vals.push_back(i);
                    used_val_freqs.push_back(freqs[i]);
                }
            }

            if (!used_vals.empty()) {
                const auto code_bitlens = compute_code_bitlens(used_val_freqs);
                std::vector<std::pair<uint8_t, uint16_t>> code_bitlens_and_used_vals(code_bitlens.size());

                for (std::uint16_t i = 0; i < code_bitlens.size(); i++) {
                    code_bitlens_and_used_vals[i] = std::make_pair(code_bitlens[i], used_vals[i]);
                }

                std::sort(code_bitlens_and_used_vals.begin(), code_bitlens_and_used_vals.end());

                codes.resize(freqs.size());
                codes[code_bitlens_and_used_vals.front().second] = std::make_pair(code_bitlens_and_used_vals.front().first, FIRST_CODE);
                std::uint64_t prev_code = FIRST_CODE;
                std::uint64_t prev_bitlen = code_bitlens_and_used_vals.front().first;
                code_bitlens_to_values.resize(code_bitlens_and_used_vals.back().first);

                //  Exclude special data end value (256 = BYTE_VAL_COUNT) from bit length to values buffer
                if (code_bitlens_and_used_vals.front().second < BYTE_VAL_COUNT) {
                    code_bitlens_to_values[code_bitlens_and_used_vals.front().first - 1].push_back(code_bitlens_and_used_vals.front().second);
                }

                for (auto it = code_bitlens_and_used_vals.begin() + 1, end = code_bitlens_and_used_vals.end(); it != end; it++) {
                    std::uint64_t code = (prev_code + 1) << (it->first - prev_bitlen);
                    prev_code = code;
                    prev_bitlen = it->first;
                    codes[it->second] = std::make_pair(prev_bitlen, code);

                    if (it->second < BYTE_VAL_COUNT) {
                        code_bitlens_to_values[prev_bitlen - 1].push_back(it->second);
                    }
                }
            }
        }

    public:
        HuffmanEncoder(const std::vector<std::uint64_t> &freqs) {
            encoded_byte = 0;
            remaining_byte_bit_count = BYTE_BITLEN;
            compute_codes(freqs);
        }

        void initialize_encoding(std::vector<std::uint8_t> &encoded_data) {
            encoded_data.push_back(code_bitlens_to_values.size());

            for (const auto &vals: code_bitlens_to_values) {
                encoded_data.push_back(vals.size());
            }

            for (const auto &vals: code_bitlens_to_values) {
                encoded_data.insert(encoded_data.end(), vals.begin(), vals.end());
            }
        }

        void encode_value(const std::uint16_t val, std::vector<std::uint8_t> &encoded_data) {
            auto code = codes[val];
            auto remaining_code_bit_count = code.first;
            auto code_val = code.second;

            if (remaining_code_bit_count >= remaining_byte_bit_count) {
                remaining_code_bit_count -= remaining_byte_bit_count;
                encoded_byte |= (UINT8_MAX >> (BYTE_BITLEN - remaining_byte_bit_count)) & (code_val >> remaining_code_bit_count);
                encoded_data.push_back(encoded_byte);
                encoded_byte = 0;
                remaining_byte_bit_count = BYTE_BITLEN;

                while (remaining_code_bit_count >= BYTE_BITLEN) {
                    remaining_code_bit_count -= BYTE_BITLEN;
                    encoded_data.push_back(code_val >> remaining_code_bit_count);
                }
            }

            if (remaining_code_bit_count > 0) {
                std::uint8_t mask = UINT8_MAX >> (BYTE_BITLEN - remaining_byte_bit_count);
                remaining_byte_bit_count -= remaining_code_bit_count;
                encoded_byte |= mask & (code_val << remaining_byte_bit_count);
            }
        }

        void finalize_encoding(std::vector<std::uint8_t> &encoded_data) {
            if (remaining_byte_bit_count < BYTE_BITLEN) {
                encoded_data.push_back(encoded_byte);
                encoded_byte = 0;
                remaining_byte_bit_count = BYTE_BITLEN;
            }
        }
};


std::vector<std::uint8_t> encode_huffman(const std::vector<std::uint8_t> &data, const bool add_data_end_val) {
    auto freqs = get_freqs(data);

    if (add_data_end_val) {
        // Add special data end value (256 = BYTE_VAL_COUNT) with one implicit occurence
        freqs.push_back(1);
    }

    auto huffman_encoder = HuffmanEncoder(freqs);
    std::vector<std::uint8_t> encoded_data;
    huffman_encoder.initialize_encoding(encoded_data);

    for (const auto &val: data) {
        huffman_encoder.encode_value(val, encoded_data);
    }

    if (add_data_end_val) {
        // Encode special data end value at the end of encoded data
        huffman_encoder.encode_value(BYTE_VAL_COUNT, encoded_data);
    }

    huffman_encoder.finalize_encoding(encoded_data);

    for (const auto &val: encoded_data) {
        std::cout << unsigned(val) << " ";
    }
    std::cout << std::endl;
    
    return encoded_data;
}


std::vector<std::uint8_t> decode_huffman(const std::vector<std::uint8_t> &data) {
    std::vector<std::uint8_t> decoded_data = data;

    return decoded_data;
}
