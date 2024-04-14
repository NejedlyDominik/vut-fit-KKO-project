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


std::vector<std::uint8_t> encode_huffman(const std::vector<std::uint8_t> &data);
std::vector<std::uint8_t> decode_huffman(const std::vector<std::uint8_t> &data);


#endif
