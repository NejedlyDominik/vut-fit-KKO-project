/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 7. 4. 2024
 * 
 * @brief Adjacent value difference model interface
 */


#ifndef MODEL_H
#define MODEL_H


#include <vector>
#include <cstdint>


/**
 * @brief Encode data by adjacent value difference transformation.
 * 
 * @param first Iterator pointing to the first element to be encoded
 * @param first Iterator pointing to the end of the range (one past the last element to be encoded)
 * 
 * @return Encoded data.
 */
std::vector<std::uint8_t> encode_adj_val_diff(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last);

/**
 * @brief Decode data encoded by adjacent value difference transformation.
 * 
 * @param first Iterator pointing to the first element to be decoded
 * @param first Iterator pointing to the end of the range (one past the last element to be decoded)
 * 
 * @return Decoded data.
 */
std::vector<std::uint8_t> decode_adj_val_diff(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last);


#endif
