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


#define END_OF_BLOCK 256


std::vector<std::uint64_t> get_freqs(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last);

class HuffmanEncoder {
    private:
        std::vector<std::pair<uint8_t, uint64_t>> codes;
        std::uint8_t encoded_buffer;
        std::uint8_t remaining_buffer_bit_count;
        std::vector<std::vector<uint8_t>> code_bitlen_to_symbols;

        std::vector<std::uint8_t> compute_code_bitlens(const std::vector<std::uint64_t> &freqs);
        void compute_codes(const std::vector<std::uint64_t> &freqs, bool add_end_of_block);
        void clear_buffer();

    public:
        void initialize_encoding(const std::vector<std::uint64_t> &freqs, std::vector<std::uint8_t> &encoded_data, bool add_end_of_block = true);
        void encode_symbol(const std::uint16_t symbol, std::vector<std::uint8_t> &encoded_data);
        void encode_data(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last, std::vector<std::uint8_t> &encoded_data);
        void finalize_encoding(std::vector<std::uint8_t> &encoded_data);
};

class HuffmanDecoder {
    private:
        std::vector<std::uint8_t>::const_iterator current_source_it;
        std::vector<std::uint8_t>::const_iterator source_end_it;
        std::vector<std::uint64_t> first_code;
        std::vector<std::uint8_t> first_symbol;
        std::vector<std::uint16_t> alphabet;
        std::uint8_t encoded_buffer;
        std::uint8_t remaining_buffer_bit_count;

    public:
        void set_source(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last);
        bool initialize_decoding(bool add_end_of_block = true);
        bool decode_symbol(std::uint16_t &symbol);
        bool decode_data_by_end_symbol(std::vector<std::uint8_t> &decoded_data, std::uint16_t end_symbol = END_OF_BLOCK);
        bool decode_data_by_count(std::vector<std::uint8_t> &decoded_data, std::uint64_t count);
        bool is_source_proccessed();
        void advance_source(std::uint64_t num = 1);
        std::vector<std::uint8_t>::const_iterator get_current_source_it();
};


#endif
