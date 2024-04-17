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
 * Find optimal marker.
 * 
 * @param first iterator pointing to the first element of the given data
 * @param last iterator pointing to the end of the range (one behind the last element of the given data)
 * 
 * @return Optimal marker for given data.
 */
std::uint8_t get_optimal_marker(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last);

/**
 * Encode data using RLE.
 * 
 * @param first iterator pointing to the first element to be encoded
 * @param last iterator pointing to the end of the range (one behind the last element to be encoded)
 * @param marker RLE marker
 * 
 * @return Encoded data.
 */
std::vector<std::uint8_t> encode_rle(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last, std::uint8_t marker = DEFAULT_MARKER);

/**
 * Decode data encoded using RLE.
 * 
 * @param first iterator pointing to the first element to be decoded
 * @param last iterator pointing to the end of the range (one behind the last element to be decoded)
 * @param marker RLE marker
 * 
 * @return Decoded data.
 */
std::vector<std::uint8_t> decode_rle(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last, std::uint8_t marker = DEFAULT_MARKER);


#endif
