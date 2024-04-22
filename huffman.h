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


/**
 * @brief Get the frequency of occurrences of each symbol in the data specified by parameters.
 * 
 * @param first Iterator pointing to the first element of the given data
 * @param last Iterator pointing to the end of the range (one past the last element of the given data)
 *  
 * @return Frequencies of occurrences of all symbols.
 */
std::vector<std::uint64_t> get_freqs(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last);

/**
 * @class Canonical Huffman code encoder
 */
class HuffmanEncoder {
    private:
        std::vector<std::pair<uint8_t, uint64_t>> codes;            // Huffman code with its length for each symbol
        std::uint8_t encoded_buffer;                                // The buffer storing the last 8 encoded bits
        std::uint8_t remaining_buffer_bit_count;                    // The number of remaining available bits in encoded buffer
        std::vector<std::vector<uint8_t>> code_bitlen_to_symbols;   // Symbols sorted by the lengths of their markers
        bool is_added_end_of_block;                                 // Specifies whether a code for the special end-of-block symbol is added

        /**
         * @brief Compute the bit lengths of the canonical Huffman codes according to frequencies of occurences of symbols.
         * 
         * @param freqs Frequencies of occurences of symbols
         *  
         * @return Bit lengths of Huffman codes for individual symbols.
         */
        std::vector<std::uint8_t> compute_code_bitlens(const std::vector<std::uint64_t> &freqs);

        /**
         * @brief Compute the canonical Huffman codes of individual symbols.
         * 
         * @param freqs Frequencies of occurences of symbols
         */
        void compute_codes(const std::vector<std::uint64_t> &freqs);

        /**
         * @brief Clear the buffer storing the last 8 encoded bits data and reset its number of remaining available bits.
         */
        void clear_buffer();

    public:
        /**
         * @brief Compute the canonical Huffman codebook according to frequencies of occurences of individual symbols and store it to the encoded data.
         * 
         * @param freqs Frequencies of occurences of symbols
         * @param encoded_data Buffer for storing encoded data
         * @param add_end_of_block Specifies whether a code for the special end-of-block symbol should be added (true by default)
         */
        void initialize_encoding(const std::vector<std::uint64_t> &freqs, std::vector<std::uint8_t> &encoded_data, bool add_end_of_block = true);

        /**
         * @brief Encode the symbol using canonical Huffman encoding.
         * 
         * @param symbol The symbol to be encoded
         * @param encoded_data Buffer for storing encoded data
         */
        void encode_symbol(const std::uint16_t symbol, std::vector<std::uint8_t> &encoded_data);

        /**
         * @brief Encode data using canonical Huffman encoding.
         * 
         * @param first Iterator pointing to the first element to be encoded
         * @param last Iterator pointing to the end of the range (one past the last element to be encoded)
         * @param encoded_data Buffer for storing encoded data
         */
        void encode_data(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last, std::vector<std::uint8_t> &encoded_data);

        /**
         * @brief Encode the end-of-block symbol if is added and add the buffer storing the last 8 encoded bits to the end of the encoded data if the buffer is not empty.
         * 
         * @param encoded_data Buffer for storing encoded data
         */
        void finalize_encoding(std::vector<std::uint8_t> &encoded_data);
};

/**
 * @class Canonical Huffman code decoder
 */
class HuffmanDecoder {
    private:
        std::vector<std::uint8_t>::const_iterator current_source_it;    // Iterator pointing to the current symbol to be decoded
        std::vector<std::uint8_t>::const_iterator source_end_it;        // Iterator one past the last element to be decoded
        std::vector<std::uint64_t> first_code;                          // Values of the first codes of individual bit lengths specified by first_code_index + 1
        std::vector<std::uint8_t> first_symbol;                         // Indexes of the first symbols in symbol alphabet with code bit lengths first_symbol_index + 1
        std::vector<std::uint16_t> alphabet;                            // Symbol alphabet
        std::uint8_t encoded_buffer;                                    // The buffer storing current 8 bits for decoding
        std::uint8_t remaining_buffer_bit_count;                        // The number of remaining bits for decoding in encoded buffer

    public:
        /**
         * @brief Set the source encoded data to decode.
         * 
         * @param first Iterator pointing to the first element to be decoded
         * @param last Iterator pointing to the end of the range (one past the last element to be decoded)
         */
        void set_source(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last);

        /**
         * @brief Prepare the first codes and the first symbols  according to codebook in encoded data.
         * 
         * @param add_end_of_block Specifies whether a code for the special end-of-block symbol should be added (true by default)
         * 
         * @return True in case of successful initialization, False otherwise.
         */
        bool initialize_decoding(bool add_end_of_block = true);

        /**
         * @brief Decode the current source encoded symbol using canonical Huffman encoding.
         * 
         * @param symbol The resulting decoded symbol
         * 
         * @return True if the symbol is successufully decoded, False otherwise.
         */
        bool decode_symbol(std::uint16_t &symbol);

        /**
         * @brief Decode current source encoded data until the first occurence of the specified end symbol.
         * 
         * @note Decode the entire encoded source data if it does not contain end symbol.
         * 
         * @param decoded_data Buffer for storing decoded data
         * @param end_symbol The last symbol to be decoded (end-of-block symbol by default) that is not added to decoded data buffer
         * 
         * @return True in case of successul decoding, False otherwise.
         */
        bool decode_data_by_end_symbol(std::vector<std::uint8_t> &decoded_data, std::uint16_t end_symbol = END_OF_BLOCK);

        /**
         * @brief Decode the specified number of encoded symbols of the current source.
         * 
         * @note Decode the entire encoded source data if the specified number of symbols to be decoded is larger than the count of remaining encoded symbols in the source.
         * 
         * @param decoded_data Buffer for storing decoded data
         * @param count The number of symbols to be decoded
         * 
         * @return True in case of successul decoding, False otherwise.
         */
        bool decode_data_by_count(std::vector<std::uint8_t> &decoded_data, std::uint64_t count);

        /**
         * @brief Check if the source is processed.
         * 
         * @return True if the source is processed, False otherwise.
         */
        bool is_source_proccessed();

        /**
         * @brief Increment the source iterator by the value specified by the parameter.
         * 
         * @note If the value is greater than the number of remaining elements, source iterator is set to be equal to the source end iterator.
         * 
         * @param num The non-negative number of by which the source to be incremented (1 by default)
         */
        void advance_source(std::uint64_t num = 1);

        /**
         * @brief Get the current source iterator.
         * 
         * @return The current source iterator.
         */
        std::vector<std::uint8_t>::const_iterator get_current_source_it();

        /**
         * @brief Get the source end iterator.
         * 
         * @return The source end iterator.
         */
        std::vector<std::uint8_t>::const_iterator get_source_end_it();
};


#endif
