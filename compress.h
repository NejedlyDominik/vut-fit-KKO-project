#ifndef COMPRESS_H
#define COMPRESS_H


#include <vector>
#include <cstdint>


std::vector<std::uint8_t> compress_statically(const std::vector<std::uint8_t> &data, bool use_model);
bool decompress_statically(const std::vector<std::uint8_t> &data, bool use_model, std::vector<std::uint8_t> &decompressed_data);


#endif