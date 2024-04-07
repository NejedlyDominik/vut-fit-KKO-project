#include "rle.h"


#define RLE_TRESHOLD 3

#define MARKER 0
#define COUNT 1
#define SYMBOL 2


std::vector<std::uint8_t> rle_encode(std::vector<std::uint8_t> &data, std::uint8_t marker) {
    std::vector<std::uint8_t> result;

    if (result.empty()) {
        return result;
    }

    std::uint8_t count = 0;
    std::uint8_t prev = data.front();

    for (auto it = data.begin() + 1, end = data.end(); it != end; it++) {
        if (*it == prev && count < UINT8_MAX) {
            count++;
            continue;
        }

        if (count < RLE_TRESHOLD) {
            if (prev != marker) {
                // The count is reduced by one, i.e. 0 represent 1, etc.
                result.insert(result.end(), count + 1, prev);
            }
            else {
                // 2 bytes are enough to represent 1, 2 or 3 marker symbols
                result.insert(result.end(), {marker, count});
            }
        }
        else {
            result.insert(result.end(), {marker, count, prev});
        }
      
        prev = *it;
        count = 0;
    }

    return result;
}


std::vector<std::uint8_t> rle_decode(std::vector<std::uint8_t> &data, std::uint8_t marker) {
    std::uint8_t count, state = 0;
    std::vector<std::uint8_t> result;

    for (const auto &val: data) {
        if (state == MARKER) {
            if (val == marker) {
                state = COUNT;
                continue;
            }

            result.push_back(val);
        }
        else if (state == COUNT) {
            if (val < RLE_TRESHOLD) {
                result.insert(result.end(), val + 1, marker);
                state = MARKER;
                continue;
            }

            count = val;
            state = SYMBOL;
        }
        else {  // symbol
            result.insert(result.end(), count + 1, val);
            state = MARKER;
        }
    }

    return result;
}
