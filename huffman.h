/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 14. 4. 2024
 * 
 * @brief Huffman code encoding and decoding interface
 */


#ifndef HUFFMAN_H
#define HUFFMAN_H


#include <vector>
#include <cstdint>


std::vector<std::uint64_t> get_freqs(const std::vector<std::uint8_t> &data);

class HuffmanEncoder {
    private:
        std::vector<std::pair<uint8_t, uint64_t>> codes;
        std::uint8_t encoded_buffer;
        std::uint8_t remaining_buffer_bit_count;
        std::vector<std::vector<uint8_t>> code_bitlen_to_symbols;

        std::vector<std::uint8_t> compute_code_bitlens(const std::vector<std::uint64_t> &freqs);
        void compute_codes(const std::vector<std::uint64_t> &freqs);
        void clear_buffer();

    public:
        void initialize_encoding(const std::vector<std::uint64_t> &freqs, std::vector<std::uint8_t> &encoded_data);
        void encode_symbol(const std::uint16_t symbol, std::vector<std::uint8_t> &encoded_data);
        void encode_data(const std::vector<std::uint8_t> &data, std::vector<std::uint8_t> &encoded_data);
        void finalize_encoding(std::vector<std::uint8_t> &encoded_data);
};

class HuffmanDecoder {
    private:
        std::vector<std::uint8_t>::const_iterator current_data_it;
        std::vector<std::uint8_t>::const_iterator data_end_it;
        std::vector<std::uint64_t> first_code;
        std::vector<std::uint8_t> first_symbol;
        std::vector<std::uint16_t> alphabet;
        std::uint8_t encoded_buffer;
        std::uint8_t remaining_buffer_bit_count;

    public:
        void set_source(const std::vector<std::uint8_t> &data);
        bool initialize_decoding();
        bool decode_symbol(std::uint16_t &symbol);
        bool decode_data(std::vector<std::uint8_t> &decoded_data);
        bool is_source_proccessed();
};


#endif
