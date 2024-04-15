/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 14. 4. 2024
 * 
 * @brief Huffman code encoding and decoding module
 */


#include <algorithm>
#include <utility>
#include <iostream>

#include "huffman.h"


#define BYTE_VALUE_COUNT 256
#define FIRST_CODE 0
#define BYTE_BIT_LENGTH 8
#define END_OF_BLOCK BYTE_VALUE_COUNT


std::vector<std::uint64_t> get_freqs(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last) {
    std::vector<std::uint64_t> freqs(BYTE_VALUE_COUNT);

    while (first < last) {
        freqs[*first++]++;
    }

    return freqs;
}


std::vector<std::uint8_t> HuffmanEncoder::compute_code_bitlens(const std::vector<std::uint64_t> &freqs) {
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


void HuffmanEncoder::compute_codes(const std::vector<std::uint64_t> &freqs) {
    std::vector<std::uint16_t> used_symbols;
    std::vector<std::uint64_t> used_symbol_freqs;

    for (std::uint16_t i = 0; i < freqs.size(); i++) {
        if (freqs[i] > 0) {
            used_symbols.push_back(i);
            used_symbol_freqs.push_back(freqs[i]);
        }
    }

    if (!used_symbols.empty()) {
        // Add a special end-of-block symbol (256) with zero default occurrence to determine the length of the code so that it can be recomputed during decoding
        used_symbols.push_back(END_OF_BLOCK);
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

        // Collect symbols according to their lengths but exclude the special end-of-block symbol symbol
        for (auto it = code_bitlens_and_used_symbols.begin(), end = code_bitlens_and_used_symbols.end() - 1; it != end; it++) {
            code_bitlen_to_symbols[it->first - 1].push_back(it->second);
        }
    }
}


void HuffmanEncoder::clear_buffer() {
    encoded_buffer = 0;
    remaining_buffer_bit_count = BYTE_BIT_LENGTH;
}


void HuffmanEncoder::initialize_encoding(const std::vector<std::uint64_t> &freqs, std::vector<std::uint8_t> &encoded_data) {
    compute_codes(freqs);
    encoded_data.push_back(code_bitlen_to_symbols.size() - 1);

    for (const auto &symbols: code_bitlen_to_symbols) {
        encoded_data.push_back(symbols.size());
    }

    for (const auto &symbols: code_bitlen_to_symbols) {
        encoded_data.insert(encoded_data.end(), symbols.begin(), symbols.end());
    }

    clear_buffer();
}


void HuffmanEncoder::encode_symbol(const std::uint16_t symbol, std::vector<std::uint8_t> &encoded_data) {
    auto code = codes[symbol];
    auto remaining_code_bit_count = code.first;
    auto code_value = code.second;

    if (remaining_code_bit_count >= remaining_buffer_bit_count) {
        remaining_code_bit_count -= remaining_buffer_bit_count;
        encoded_buffer |= (UINT8_MAX >> (BYTE_BIT_LENGTH - remaining_buffer_bit_count)) & (code_value >> remaining_code_bit_count);
        encoded_data.push_back(encoded_buffer);
        clear_buffer();

        while (remaining_code_bit_count >= BYTE_BIT_LENGTH) {
            remaining_code_bit_count -= BYTE_BIT_LENGTH;
            encoded_data.push_back(code_value >> remaining_code_bit_count);
        }
    }

    if (remaining_code_bit_count > 0) {
        std::uint8_t mask = UINT8_MAX >> (BYTE_BIT_LENGTH - remaining_buffer_bit_count);
        remaining_buffer_bit_count -= remaining_code_bit_count;
        encoded_buffer |= mask & (code_value << remaining_buffer_bit_count);
    }
}


void HuffmanEncoder::encode_data(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last, std::vector<std::uint8_t> &encoded_data) {
    while (first < last) {
        encode_symbol(*first++, encoded_data);
    }
}


void HuffmanEncoder::finalize_encoding(std::vector<std::uint8_t> &encoded_data) {
    // Encode the special end-of-block symbol symbol at the end of encoded data
    encode_symbol(BYTE_VALUE_COUNT, encoded_data);

    if (remaining_buffer_bit_count < BYTE_BIT_LENGTH) {
        encoded_data.push_back(encoded_buffer);
    }
}


void HuffmanDecoder::set_source(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last) {
    current_source_it = first;
    source_end_it = last;
}


bool HuffmanDecoder::initialize_decoding() {
    std::uint16_t code_count_number = *current_source_it++ + 1;

    if (current_source_it + code_count_number > source_end_it) {
        std::cerr << "Invalid number of symbol counts" << std::endl;
        return false;
    }

    std::uint64_t code_value = 0;
    std::uint16_t symbol = 0;

    first_code.resize(code_count_number + 1);
    first_symbol.resize(code_count_number);

    for (std::uint16_t i = 0; i < code_count_number; i++) {
        first_code[i] = code_value;
        first_symbol[i] = symbol;
        code_value = (code_value + *current_source_it) << 1;
        symbol += *current_source_it++;
    }

    if (current_source_it + symbol > source_end_it) {
        std::cerr << "Invalid symbol alphabet" << std::endl;
        return false;
    }

    alphabet.insert(alphabet.end(), current_source_it, current_source_it + symbol);
    current_source_it += symbol;

    // Add the special end-of-block symbol to alphabet
    alphabet.push_back(END_OF_BLOCK);
    // Adapt the anchor code to the special end-of-block symbol
    first_code[code_count_number] = code_value + 2;

    remaining_buffer_bit_count = 0;
    return true;
}


bool HuffmanDecoder::decode_symbol(std::uint16_t &symbol) {
    std::uint64_t code_value = 0;
    std::uint16_t code_len = 0;

    do {
        if (remaining_buffer_bit_count == 0) {
            if (current_source_it == source_end_it) {
                std::cerr << "Cannot decode symbol" << std::endl;
                return false;
            }

            encoded_buffer = *current_source_it++;
            remaining_buffer_bit_count = BYTE_BIT_LENGTH;
        }

        code_len++;
        code_value = (code_value << 1) + (encoded_buffer >> --remaining_buffer_bit_count & 1);
    } while (code_value << 1 >= first_code[code_len]);

    symbol = alphabet[first_symbol[code_len - 1] + code_value - first_code[code_len - 1]];
    return true;
}


bool HuffmanDecoder::decode_data(std::vector<std::uint8_t> &decoded_data) {
    while (current_source_it != source_end_it || remaining_buffer_bit_count > 0) {
        std::uint16_t symbol;

        if (!decode_symbol(symbol)) {
            return false;
        }

        if (symbol == END_OF_BLOCK) {
            break;
        }

        decoded_data.push_back(symbol);
    }

    return true;
}


bool HuffmanDecoder::is_source_proccessed() {
    return current_source_it == source_end_it;
}


void HuffmanDecoder::advance_source() {
    if (current_source_it < source_end_it) {
        current_source_it++;
    }
}


std::vector<std::uint8_t>::const_iterator HuffmanDecoder::get_current_source_it() {
    return current_source_it;
}
