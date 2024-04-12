#include <algorithm>
#include <utility>
#include <iostream>

#include "huffman.h"


#define BYTE_VALUE_COUNT 256
#define FIRST_CODE 0
#define BYTE_BIT_LENGTH 8
#define BlOCK_END_SYMBOL BYTE_VALUE_COUNT


std::vector<std::uint64_t> get_freqs(const std::vector<std::uint8_t> &data) {
    std::vector<std::uint64_t> freqs(BYTE_VALUE_COUNT);

    for (const auto &symbol: data) {
        freqs[symbol]++;
    }

    return freqs;
}


class HuffmanEncoder {
    private:
        std::vector<std::pair<uint8_t, uint64_t>> codes;
        std::uint8_t encoded_byte;
        std::uint8_t remaining_byte_bit_count;
        std::vector<std::vector<uint8_t>> code_bitlen_to_symbols;

        std::vector<std::uint8_t> compute_code_bitlens(const std::vector<std::uint64_t> &freqs) {
            std::uint16_t m = freqs.size();
            std::vector<std::uint16_t> hr(2 * m);
            std::vector<std::pair<std::uint64_t, std::uint16_t>> h(m);

            for (std::uint16_t i = 0; i < m; i++) {
                h[i] = std::make_pair(freqs[i], m + i);
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

            std::vector<std::uint8_t> code_bitlens(freqs.size());

            // Determine code bit lengths
            for (std::uint16_t i = 0; i < freqs.size(); i++) {
                std::uint16_t j = hr[freqs.size() + i];
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
            std::vector<std::uint16_t> used_symbols;
            std::vector<std::uint64_t> used_symbol_freqs;

            for (std::uint16_t i = 0; i < freqs.size(); i++) {
                if (freqs[i] > 0) {
                    used_symbols.push_back(i);
                    used_symbol_freqs.push_back(freqs[i]);
                }
            }

            if (!used_symbols.empty()) {
                // Add special block end symbol (256 = BLOCK_END_SYMBOL) with zero implicit occurence to ensure that its code is one of the longest to be able to recompute it when decoding
                used_symbols.push_back(BlOCK_END_SYMBOL);
                used_symbol_freqs.push_back(0);

                const auto code_bitlens = compute_code_bitlens(used_symbol_freqs);
                std::vector<std::pair<uint8_t, uint16_t>> code_bitlens_and_used_symbols(code_bitlens.size());

                for (std::uint16_t i = 0; i < code_bitlens.size(); i++) {
                    code_bitlens_and_used_symbols[i] = std::make_pair(code_bitlens[i], used_symbols[i]);
                }

                std::sort(code_bitlens_and_used_symbols.begin(), code_bitlens_and_used_symbols.end());

                codes.resize(BYTE_VALUE_COUNT + 1);
                codes[code_bitlens_and_used_symbols.front().second] = std::make_pair(code_bitlens_and_used_symbols.front().first, FIRST_CODE);

                std::uint64_t prev_code = FIRST_CODE;
                std::uint64_t prev_bitlen = code_bitlens_and_used_symbols.front().first;

                for (auto it = code_bitlens_and_used_symbols.begin() + 1, end = code_bitlens_and_used_symbols.end(); it != end; it++) {
                    std::uint64_t code = (prev_code + 1) << (it->first - prev_bitlen);
                    prev_code = code;
                    prev_bitlen = it->first;
                    codes[it->second] = std::make_pair(prev_bitlen, code);
                }

                code_bitlen_to_symbols.resize(code_bitlens_and_used_symbols.back().first);

                // Collect symbols according to their lengths but exclude special data end symbol
                for (auto it = code_bitlens_and_used_symbols.begin(), end = code_bitlens_and_used_symbols.end() - 1; it != end; it++) {
                    code_bitlen_to_symbols[it->first - 1].push_back(it->second);
                }
            }
        }

    public:
        void initialize_encoding(const std::vector<std::uint64_t> &freqs, std::vector<std::uint8_t> &encoded_data) {
            encoded_byte = 0;
            remaining_byte_bit_count = BYTE_BIT_LENGTH;
            compute_codes(freqs);
            encoded_data.push_back(code_bitlen_to_symbols.size() - 1);

            for (const auto &symbols: code_bitlen_to_symbols) {
                encoded_data.push_back(symbols.size());
            }

            for (const auto &symbols: code_bitlen_to_symbols) {
                encoded_data.insert(encoded_data.end(), symbols.begin(), symbols.end());
            }
        }

        void encode_symbol(const std::uint16_t symbol, std::vector<std::uint8_t> &encoded_data) {
            auto code = codes[symbol];
            auto remaining_code_bit_count = code.first;
            auto code_value = code.second;

            if (remaining_code_bit_count >= remaining_byte_bit_count) {
                remaining_code_bit_count -= remaining_byte_bit_count;
                encoded_byte |= (UINT8_MAX >> (BYTE_BIT_LENGTH - remaining_byte_bit_count)) & (code_value >> remaining_code_bit_count);
                encoded_data.push_back(encoded_byte);
                encoded_byte = 0;
                remaining_byte_bit_count = BYTE_BIT_LENGTH;

                while (remaining_code_bit_count >= BYTE_BIT_LENGTH) {
                    remaining_code_bit_count -= BYTE_BIT_LENGTH;
                    encoded_data.push_back(code_value >> remaining_code_bit_count);
                }
            }

            if (remaining_code_bit_count > 0) {
                std::uint8_t mask = UINT8_MAX >> (BYTE_BIT_LENGTH - remaining_byte_bit_count);
                remaining_byte_bit_count -= remaining_code_bit_count;
                encoded_byte |= mask & (code_value << remaining_byte_bit_count);
            }
        }

        void encode_data(const std::vector<std::uint8_t> &data, std::vector<std::uint8_t> &encoded_data) {
            for (const auto &symbol: data) {
                encode_symbol(symbol, encoded_data);
            }
        }

        void finalize_encoding(std::vector<std::uint8_t> &encoded_data) {
            if (remaining_byte_bit_count < BYTE_BIT_LENGTH) {
                encoded_data.push_back(encoded_byte);
                encoded_byte = 0;
                remaining_byte_bit_count = BYTE_BIT_LENGTH;
            }
        }
};


std::vector<std::uint8_t> encode_huffman(const std::vector<std::uint8_t> &data) {
    auto huffman_encoder = HuffmanEncoder();
    auto freqs = get_freqs(data);
    std::vector<std::uint8_t> encoded_data;
    huffman_encoder.initialize_encoding(freqs, encoded_data);
    huffman_encoder.encode_data(data, encoded_data);

    // Encode special data end symbol at the end of encoded data
    huffman_encoder.encode_symbol(BYTE_VALUE_COUNT, encoded_data);
    huffman_encoder.finalize_encoding(encoded_data);

    for (const auto &val: encoded_data) {
        std::cout << unsigned(val) << " ";
    }
    std::cout << std::endl;
    
    return encoded_data;
}


class HuffmanDecoder {
    private:
        std::vector<std::uint64_t> first_code;
        std::vector<std::uint8_t> first_symbol;
        std::vector<std::uint16_t> alphabet;

    public:
        void set_source() {

        }

        bool initialize_decoding() {
            return true;
        }

        bool decode_symbol() {
            return true;
        }

        bool decode_data() {
            return true;
        }

        bool prepare_next_block() {
            return true;
        }
};


std::vector<std::uint8_t> decode_huffman(const std::vector<std::uint8_t> &data) {
    std::uint16_t code_count_number = data.front() + 1;
    std::vector<std::uint64_t> first_code(code_count_number + 1);
    std::vector<std::uint8_t> first_symbol(code_count_number);
    std::vector<std::uint16_t> alphabet;
    std::uint64_t code_value = 0;
    std::uint16_t symbol = 0;

    for (std::uint16_t i = 0; i < code_count_number; i++) {
        first_code[i] = code_value;
        first_symbol[i] = symbol;
        code_value = (code_value + data[i + 1]) << 1;
        symbol += data[i + 1];
    }

    alphabet.insert(alphabet.end(), data.begin() + code_count_number + 1, data.begin() + code_count_number + 1 + symbol);
    // Add special data end symbol to alphabet
    alphabet.push_back(BlOCK_END_SYMBOL);
    // Adapt the anchor code to the special data end symbol
    first_code[code_count_number] = code_value + 2;

    std::vector<std::uint8_t> decoded_data;

    code_value = 0;
//    uint16_t code_len = 0;

    return decoded_data;
}
