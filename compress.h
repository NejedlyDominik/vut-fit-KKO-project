/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 23. 4. 2024
 * 
 * @brief Compression and decompresion interface
 */


#ifndef COMPRESS_H
#define COMPRESS_H


#include <vector>
#include <cstdint>


/**
 * @brief Compress the data using canonical Huffman encoding with static scanning.
 * 
 * @note The data are scanned horizontally and they are treated as one continuous data block.
 * 
 * @param data The data to be compressed
 * @param use_model Indicates whether the adjacent value difference model and RLE should be used for data preprocessing
 * 
 * @return Statically compressed data.
 */
std::vector<std::uint8_t> compress_statically(const std::vector<std::uint8_t> &data, const bool use_model);

/**
 * @brief Decompress the data compressed using canonical Huffman encoding with static scanning.
 * 
 * @param data The data to be decompressed
 * @param decompressed_data The resulting decompressed data
 * @param use_model Indicates whether the adjacent value difference model and RLE were used for original data preprocessing
 * 
 * @return True in case of successful decompression, false otherwise.
 */
bool decompress_statically(const std::vector<std::uint8_t> &data, std::vector<std::uint8_t> &decompressed_data, const bool use_model);

/**
 * @brief Compress the data using canonical Huffman encoding with adaptive scanning.
 * 
 * @note The data (of 2D image) are decomposed into smaller blocks each of which is compressed independantely using horizontal or vertical scanning depending on the best compression ratio.
 * 
 * @param data The data to be compressed
 * @param width_value The width of data (2D image)
 * @param use_model Indicates whether the adjacent value difference model and RLE should be used for each data block preprocessing
 * 
 * @return Adaptively compressed data.
 */
std::vector<std::uint8_t> compress_adaptively(const std::vector<std::uint8_t> &data, const std::uint64_t width_value, const bool use_model);

/**
 * @brief Decompress the data compressed using canonical Huffman encoding with adaptive scanning.
 * 
 * @param data The data to be decompressed
 * @param decompressed_data The resulting decompressed data
 * @param use_model Indicates whether the adjacent value difference model and RLE were used for each original data block preprocessing
 * 
 * @return True in case of successful decompression, false otherwise.
 */
bool decompress_adaptively(const std::vector<std::uint8_t> &data, std::vector<std::uint8_t> &decompressed_data, const bool use_model);


#endif