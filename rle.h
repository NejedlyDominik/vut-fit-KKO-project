/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 8. 4. 2024
 * 
 * @brief Run length encoding (RLE) interface
 */


#ifndef RLE_H
#define RLE_H


#include <vector>
#include <cstdint>


#define DEFAULT_MARKER 128


/**
 * @brief Find optimal marker for the data specified by parameters.
 * 
 * @param first Iterator pointing to the first element of the given data
 * @param last Iterator pointing to the end of the range (one past the last element of the given data)
 * 
 * @return Optimal marker for given data.
 */
std::uint8_t get_optimal_marker(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last);

/**
 * @brief Encode data using RLE.
 * 
 * @param first Iterator pointing to the first element to be encoded
 * @param last Iterator pointing to the end of the range (one past the last element to be encoded)
 * @param marker RLE marker
 * 
 * @return Encoded data.
 */
std::vector<std::uint8_t> encode_rle(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last, std::uint8_t marker = DEFAULT_MARKER);

/**
 * @brief Decode data encoded using RLE.
 * 
 * @param first Iterator pointing to the first element to be decoded
 * @param last Iterator pointing to the end of the range (one past the last element to be decoded)
 * @param marker RLE marker
 * 
 * @return Decoded data.
 */
std::vector<std::uint8_t> decode_rle(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last, std::uint8_t marker = DEFAULT_MARKER);


#endif
