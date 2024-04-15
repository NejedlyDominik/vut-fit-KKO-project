/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 7. 4. 2024
 * 
 * @brief Adjacent value difference model module
 */


#include <iterator>

#include "model.h"


std::vector<std::uint8_t> encode_adj_val_diff(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last) {
    std::vector<std::uint8_t> result(std::distance(first, last));
    std::uint8_t prev = 0;

    for (auto &val: result) {
        val = *first - prev;
        prev = *first++;
    }

    return result;
}


std::vector<std::uint8_t> decode_adj_val_diff(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last) {
    std::vector<std::uint8_t> result(std::distance(first, last));
    std::uint8_t prev = 0;

    for (auto &val: result) {
        val = *first + prev;
        prev = *first++;
    }

    return result;
}
