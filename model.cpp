/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 7. 4. 2024
 * 
 * @brief Adjacent value difference model module
 */


#include "model.h"


void adj_val_diff_encode(std::vector<std::uint8_t> &data) {
    std::uint8_t diff, prev = 0;

    for (auto &val: data) {
        diff = val - prev;
        prev = val;
        val = diff;
    } 
}


void adj_val_diff_decode(std::vector<std::uint8_t> &data) {
    std::uint8_t prev = 0;

    for (auto &val: data) {
        val += prev;
        prev = val;
    }
}
