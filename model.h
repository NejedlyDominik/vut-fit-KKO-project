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
 * Encode data by adjacent value difference transformation.
 * 
 * @param data the data to be encoded (contains the result after encoding)
 */
void encode_adj_val_diff(std::vector<std::uint8_t> &data);

/**
 * Decode data encoded by adjacent value difference transformation.
 * 
 * @param data the data to be decoded (contains the result after decoding)
 */
void decode_adj_val_diff(std::vector<std::uint8_t> &data);


#endif
