#ifndef RLE_H
#define RLE_H


#include <vector>
#include <cstdint>


std::vector<std::uint8_t> rle_encode(std::vector<std::uint8_t> &data, std::uint8_t marker);
std::vector<std::uint8_t> rle_decode(std::vector<std::uint8_t> &data, std::uint8_t marker);


#endif
