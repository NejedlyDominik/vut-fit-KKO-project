/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 8. 4. 2024
 * 
 * @brief Run length encoding (RLE) module
 */


#include "rle.h"


#define RLE_TRESHOLD 3

#define MARKER 0
#define COUNT 1
#define SYMBOL 2


/**
 * Encode the symbol using RLE and append it to the specified code sequence.
 * 
 * @param result the sequence to which the encoded symbol is to be appended
 * @param count the number of repetitions of the symbol
 * @param symbol symbol to be encoded
 * @param marker RLE marker
 */
void encode_and_append_symbol(std::vector<std::uint8_t> &result, uint8_t count, uint8_t symbol, uint8_t marker) {
    if (count < RLE_TRESHOLD) {
        if (symbol != marker) {
            // The count is reduced by one, i.e. 0 represent 1, etc.
            result.insert(result.end(), count + 1, symbol);
        }
        else {
            // 2 bytes are enough to represent 1, 2 or 3 marker symbols
            result.insert(result.end(), {marker, count});
        }
    }
    else {
        result.insert(result.end(), {marker, count, symbol});
    }
}


std::vector<std::uint8_t> encode_rle(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last, std::uint8_t marker) {
    std::vector<std::uint8_t> result;

    if (first == last) {
        return result;
    }

    std::uint8_t count = 0;
    std::uint8_t prev = *first;

    while (++first < last) {
        if (*first == prev && count < UINT8_MAX) {
            count++;
            continue;
        }

        encode_and_append_symbol(result, count, prev, marker);
        prev = *first;
        count = 0;
    }

    encode_and_append_symbol(result, count, prev, marker);
    return result;
}


std::vector<std::uint8_t> decode_rle(std::vector<std::uint8_t>::const_iterator first, std::vector<std::uint8_t>::const_iterator last, std::uint8_t marker) {
    std::uint8_t count, state = MARKER;
    std::vector<std::uint8_t> result;

    for (; first < last; first++) {
        if (state == MARKER) {
            if (*first == marker) {
                state = COUNT;
                continue;
            }

            result.push_back(*first);
        }
        else if (state == COUNT) {
            if (*first < RLE_TRESHOLD) {
                result.insert(result.end(), *first + 1, marker);
                state = MARKER;
                continue;
            }

            count = *first;
            state = SYMBOL;
        }
        else {  // symbol
            result.insert(result.end(), count + 1, *first);
            state = MARKER;
        }
    }

    return result;
}
