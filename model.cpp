/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 7. 4. 2024
 * 
 * @brief Adjacent value difference model module
 */


#include "model.h"


std::vector<std::uint8_t> encode_adj_val_diff(const std::vector<std::uint8_t> &data) {
    std::vector<std::uint8_t> result(data.size());
    std::uint8_t diff, prev = 0;

    for (std::uint64_t i = 0; i < data.size(); i++) {
        diff = result[i] - prev;
        prev = result[i];
        result[i] = diff;
    }

    return result;
}


std::vector<std::uint8_t> decode_adj_val_diff(const std::vector<std::uint8_t> &data) {
    std::vector<std::uint8_t> result(data.size());
    std::uint8_t prev = 0;

    for (std::uint64_t i = 0; i < data.size(); i++) {
        result[i] += prev;
        prev = result[i];
    }

    return result;
}
