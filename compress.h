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
 * @param decompressed_data The resulting compressed data
 * @param use_model Indicates whether the adjacent value difference model should be used for original data preprocessing
 * @param use_rle Indicates whether the RLE should be used for original data preprocessing
 */
void compress_statically(const std::vector<std::uint8_t> &data, std::vector<std::uint8_t> &compressed_data, const bool use_model, const bool use_rle);

/**
 * @brief Decompress the data compressed using canonical Huffman encoding with static scanning.
 * 
 * @param first Iterator pointing to the first element to be decoded
 * @param last Iterator pointing to the end of the range (one past the last element to be decoded)
 * @param decompressed_data The resulting decompressed data
 * @param use_model Indicates whether the adjacent value difference model was used for original data preprocessing
 * @param use_rle Indicates whether the RLE was used for original data preprocessing
 * 
 * @return True in case of successful decompression, false otherwise.
 */
bool decompress_statically(
    std::vector<std::uint8_t>::const_iterator first, 
    std::vector<std::uint8_t>::const_iterator last, 
    std::vector<std::uint8_t> &decompressed_data, 
    const bool use_model, 
    const bool use_rle
);

/**
 * @brief Compress the data using canonical Huffman encoding with adaptive scanning.
 * 
 * @note The data (of 2D image) are decomposed into smaller blocks each of which is compressed independantely using horizontal or vertical scanning depending on the best compression ratio.
 * 
 * @param data The data to be compressed
 * @param decompressed_data The resulting compressed data
 * @param width_value The width of data (2D image)
 * @param use_model Indicates whether the adjacent value difference model should be used for each data block preprocessing
 * @param use_rle Indicates whether the RLE should be used for each original data block preprocessing
 */
void compress_adaptively(
    const std::vector<std::uint8_t> &data, 
    std::vector<std::uint8_t> &compressed_data, 
    const std::uint64_t width_value, 
    const bool use_model, 
    const bool use_rle
);

/**
 * @brief Decompress the data compressed using canonical Huffman encoding with adaptive scanning.
 * 
 * @param first Iterator pointing to the first element to be decoded
 * @param last Iterator pointing to the end of the range (one past the last element to be decoded)
 * @param decompressed_data The resulting decompressed data
 * @param use_model Indicates whether the adjacent value difference model was used for each original data block preprocessing
 * @param use_rle Indicates whether the RLE was used for each original data block preprocessing
 * 
 * @return True in case of successful decompression, false otherwise.
 */
bool decompress_adaptively(
    std::vector<std::uint8_t>::const_iterator first, 
    std::vector<std::uint8_t>::const_iterator last, 
    std::vector<std::uint8_t> &decompressed_data, 
    const bool use_model, 
    const bool use_rle
);


#endif