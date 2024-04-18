#ifndef COMPRESS_H
#define COMPRESS_H


#include <vector>
#include <cstdint>


std::vector<std::uint8_t> compress_statically(const std::vector<std::uint8_t> &data, const bool use_model);
bool decompress_statically(const std::vector<std::uint8_t> &data, const bool use_model, std::vector<std::uint8_t> &decompressed_data);
std::vector<std::uint8_t> compress_adaptively(const std::vector<std::uint8_t> &data, const bool use_model, const std::uint64_t width_value);
//bool decompress_adaptively(const std::vector<std::uint8_t> &data, const bool use_model, const std::uint64_t width_value, std::vector<std::uint8_t> &decompressed_data);


#endif