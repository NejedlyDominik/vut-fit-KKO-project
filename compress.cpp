/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 23. 4. 2024
 * 
 * @brief Compression and decompression module
 */


#include <utility>
#include <iostream>
#include <iterator>

#include "compress.h"
#include "model.h"
#include "rle.h"
#include "huffman.h"


#define COMPRESSED 1
#define UNCOMPRESSED 0

#define HORIZONTAL_SCAN 1
#define VERTICAL_SCAN 0

#define BLOCK_SIDE_SIZE 16
#define BLOCK_SIZE (BLOCK_SIDE_SIZE * BLOCK_SIDE_SIZE)

#define BYTE_BIT_LENGTH 8


/**
 * @brief Compress the data block using canonical Huffman encoding.
 * 
 * @param data The data block to be compressed
 * @param huffman_encoder The canonical Huffman code encoder
 * @param compressed_data The resulting compressed data block
 * @param use_model Indicates whether the adjacent value difference model should be used for data block preprocessing
 * @param use_rle Indicates whether the RLE should be used for data block preprocessing
 */
void compress(
    const std::vector<std::uint8_t> &data, 
    HuffmanEncoder &huffman_encoder, 
    std::vector<std::uint8_t> &compressed_data, 
    const bool use_model, 
    const bool use_rle
) {
    compressed_data.push_back(COMPRESSED);

    if (use_model) {
        auto preprocessed_data = encode_adj_val_diff(data.begin(), data.end());

        if (use_rle) {
            preprocessed_data = encode_rle(preprocessed_data.begin(), preprocessed_data.end(), DEFAULT_MARKER);
        }

        huffman_encoder.initialize_encoding(get_freqs(preprocessed_data.begin(), preprocessed_data.end()), compressed_data);
        huffman_encoder.encode_data(preprocessed_data.begin(), preprocessed_data.end(), compressed_data);
    }
    else {
        if (use_rle) {
            auto preprocessed_data = encode_rle(data.begin(), data.end(), DEFAULT_MARKER);
            huffman_encoder.initialize_encoding(get_freqs(preprocessed_data.begin(), preprocessed_data.end()), compressed_data);
            huffman_encoder.encode_data(preprocessed_data.begin(), preprocessed_data.end(), compressed_data);
        }
        else {
            huffman_encoder.initialize_encoding(get_freqs(data.begin(), data.end()), compressed_data);
            huffman_encoder.encode_data(data.begin(), data.end(), compressed_data);
        }
    }

    huffman_encoder.finalize_encoding(compressed_data);

    // In case it is not possible to achieve compression, keep the data uncompressed
    if (compressed_data.size() >= data.size() + 1) {
        std::copy(data.begin(), data.end(), compressed_data.begin() + 1);
        compressed_data.resize(data.size() + 1);
        compressed_data[0] = UNCOMPRESSED;
    }
}


/**
 * @brief Decompress the data block compressed using canonical Huffman encoding.
 * 
 * @param decompressed_data The resulting decompressed data block
 * @param huffman_decoder The canonical Huffman code decoder
 * @param use_model Indicates whether the adjacent value difference model was used for original data block preprocessing
 * @param use_rle Indicates whether the RLE was used for original data block preprocessing
 * @param block_original_val_count The number of original values in data block (0 by default means that all the remaining data to be decompressed are in the same block)
 *
 * @return True in case of successful decompression, false otherwise.
 */
bool decompress(
    std::vector<std::uint8_t> &decompressed_data, 
    HuffmanDecoder &huffman_decoder, 
    const bool use_model, 
    const bool use_rle, 
    const std::uint16_t block_original_val_count = 0
) {
    if (huffman_decoder.is_source_proccessed()) {
        std::cerr << "Invalid compressed data - unexpected end of the compressed data, expected compression flag" << std::endl;
        return false;
    }

    auto current_data_it = huffman_decoder.get_current_source_it();

    // If the data in the compressed data block are kept uncompressed, use number of original values in data block to determine how many uncompressed symbols to load from source
    if (*current_data_it == UNCOMPRESSED) {
        if (block_original_val_count == 0) {
            decompressed_data.assign(current_data_it + 1, huffman_decoder.get_source_end_it());
        }
        else {
            decompressed_data.assign(current_data_it + 1, current_data_it + 1 + block_original_val_count);
            huffman_decoder.advance_source(1 + block_original_val_count);
        }

        return true;
    }

    huffman_decoder.advance_source(1);

    if (!huffman_decoder.initialize_decoding()) {
        return false;
    }

    decompressed_data.clear();

    if (!huffman_decoder.decode_data_by_end_symbol(decompressed_data)) {
        return false;
    }

    if (use_rle) {
        decompressed_data = decode_rle(decompressed_data.begin(), decompressed_data.end(), DEFAULT_MARKER);
    }

    if (use_model) {
        decompressed_data = decode_adj_val_diff(decompressed_data.begin(), decompressed_data.end());
    }

    return true;
}


void compress_statically(const std::vector<std::uint8_t> &data, std::vector<std::uint8_t> &compressed_data, const bool use_model, const bool use_rle) {
    auto huffman_encoder = HuffmanEncoder();
    compress(data, huffman_encoder, compressed_data, use_model, use_rle);
}


bool decompress_statically(
    std::vector<std::uint8_t>::const_iterator first, 
    std::vector<std::uint8_t>::const_iterator last, 
    std::vector<std::uint8_t> &decompressed_data, 
    const bool use_model, 
    const bool use_rle
) {
    auto huffman_decoder = HuffmanDecoder();
    huffman_decoder.set_source(first, last);
    return decompress(decompressed_data, huffman_decoder, use_model, use_rle);
}


/**
 * @brief Transpose the block in place.
 * 
 * @param block The block to be transposed
 */
void transpose_block_in_place(std::vector<std::uint8_t> &block) {
    for (std::uint8_t i = 0; i < BLOCK_SIDE_SIZE; i++) {
        std::uint16_t row_offset = i * BLOCK_SIDE_SIZE;

        for (std::uint8_t j = i + 1; j < BLOCK_SIDE_SIZE; j++) {
            std::swap(block[j + row_offset], block[i + j * BLOCK_SIDE_SIZE]);
        }
    }
}


/**
 * @brief Serialize the data block.
 * 
 * @param deserialized_block The deserialized data (image) block
 * @param is_transposed Indicates whether the deserialized data block is transposed
 * @param block_val_count The number of values (bytes) in the data block
 * @param block_width The deserialized data block width
 * @param block_height The deserialized data block height
 * @param serialized_block The resulting serialized data block
 */
void serialize_block(
    const std::vector<std::uint8_t> &deserialized_block, 
    const bool is_transposed, 
    const std::uint16_t block_val_count, 
    const std::uint8_t block_width, 
    const std::uint8_t block_height, 
    std::vector<std::uint8_t> &serialized_block
) {
    if (!is_transposed) {
        for (std::uint8_t i = 0; i < block_height; i++) {
            std::uint16_t deserialized_block_offset = i * BLOCK_SIDE_SIZE;
            std::uint16_t serialized_block_offset = i * block_width;

            for (std::uint8_t j = 0; j < block_width; j++) {
                // Stop when all the values are processed (for the case when the data does not fill the whole block)
                if (j + serialized_block_offset >= block_val_count) {
                    break;
                }

                serialized_block[j + serialized_block_offset] = deserialized_block[j + deserialized_block_offset];
            }
        }
    }
    else {
        // The number of block unshortened rows (in the case when the data does not fill the whole block the transposed block may have the last few rows 1 value shorter)
        std::uint8_t num_of_orig_width_rows = block_val_count % block_height;

        if (num_of_orig_width_rows == 0) {
            num_of_orig_width_rows = block_height;
        }

        // Serialize unshortened lines
        for (std::uint8_t i = 0; i < num_of_orig_width_rows; i++) {
            std::uint16_t deserialized_block_offset = i * BLOCK_SIDE_SIZE;
            std::uint16_t serialized_block_offset = i * block_width;

            for (std::uint8_t j = 0; j < block_width; j++) {
                serialized_block[j + serialized_block_offset] = deserialized_block[j + deserialized_block_offset];
            }
        }

        std::uint8_t shorten_block_width = block_width - 1;

        // Serialize shortened rows
        for (std::uint8_t i = num_of_orig_width_rows; i < block_height; i++) {
            std::uint16_t deserialized_block_offset = i * BLOCK_SIDE_SIZE;
            std::uint16_t serialized_block_offset = i * shorten_block_width;

            for (std::uint8_t j = 0; j < shorten_block_width; j++) {
                serialized_block[j + serialized_block_offset] = deserialized_block[j + deserialized_block_offset];
            }
        }
    }
}


/**
 * @brief Deserialize the data block.
 * 
 * @param serialized_block The serialized data (image) block
 * @param is_transposed Indicates whether the deserialized data block is transposed
 * @param block_val_count The number of values (bytes) in the data block
 * @param block_width The deserialized data block width
 * @param block_height The deserialized data block height
 * @param deserialized_block The resulting deserialized data block
 */
void deserialize_block(
    const std::vector<std::uint8_t> &serialized_block, 
    const bool is_transposed, 
    const std::uint16_t block_val_count, 
    const std::uint8_t block_width, 
    const std::uint8_t block_height, 
    std::vector<std::uint8_t> &deserialized_block
) {
    if (!is_transposed) {
        for (std::uint8_t i = 0; i < block_height; i++) {
            std::uint16_t deserialized_block_offset = i * BLOCK_SIDE_SIZE;
            std::uint16_t serialized_block_offset = i * block_width;

            for (std::uint8_t j = 0; j < block_width; j++) {
                // Stop when all the values are processed (for the case when the data does not fill the whole block)
                if (j + serialized_block_offset >= block_val_count) {
                    break;
                }

                deserialized_block[j + deserialized_block_offset] = serialized_block[j + serialized_block_offset];
            }
        }
    }
    else {
        // The number of block unshortened rows (in the case when the data does not fill the whole block the transposed block may have the last few rows 1 value shorter)
        std::uint8_t num_of_orig_width_rows = block_val_count % block_height;

        if (num_of_orig_width_rows == 0) {
            num_of_orig_width_rows = block_height;
        }

        // Deserialize unshortened lines
        for (std::uint8_t i = 0; i < num_of_orig_width_rows; i++) {
            std::uint16_t deserialized_block_offset = i * BLOCK_SIDE_SIZE;
            std::uint16_t serialized_block_offset = i * block_width;

            for (std::uint8_t j = 0; j < block_width; j++) {
                deserialized_block[j + deserialized_block_offset] = serialized_block[j + serialized_block_offset];
            }
        }

        std::uint8_t shorten_block_width = block_width - 1;

        // deserialize shortened lines
        for (std::uint8_t i = num_of_orig_width_rows; i < block_height; i++) {
            std::uint16_t deserialized_block_offset = i * BLOCK_SIDE_SIZE;
            std::uint16_t serialized_block_offset = i * shorten_block_width;

            for (std::uint8_t j = 0; j < shorten_block_width; j++) {
                deserialized_block[j + deserialized_block_offset] = serialized_block[j + serialized_block_offset];
            }
        }
    }
}


void compress_adaptively(
    const std::vector<std::uint8_t> &data, 
    std::vector<std::uint8_t> &compressed_data, 
    const std::uint64_t data_width, 
    const bool use_model, 
    const bool use_rle
) {
    const std::uint64_t original_data_size = data.size();
    compressed_data.resize(16);

    // Store the original data size and its width to the beginning of the compressed data
    for (std::uint8_t i = 0; i < 8; i++) {
        compressed_data[i] = original_data_size >> i * BYTE_BIT_LENGTH;
        compressed_data[i + 8] = data_width >> i * BYTE_BIT_LENGTH;
    }

    const std::uint64_t data_height = original_data_size / data_width + (original_data_size % data_width != 0 ? 1 : 0);
    std::vector<std::uint8_t> deserialized_block(BLOCK_SIZE);
    std::vector<std::uint8_t> serialized_block, compressed_block_h, compressed_block_v;
    std::uint64_t data_horizontal_offset = 0;
    std::uint64_t data_vertical_offset = 0;
    std::uint64_t remaining_decompressed_data_size = original_data_size;
    auto huffman_encoder = HuffmanEncoder();

    while (remaining_decompressed_data_size > 0) {
        std::uint64_t data_block_offset = data_horizontal_offset + data_vertical_offset * data_width;
        std::uint8_t block_width = std::min(static_cast<std::uint64_t>(BLOCK_SIDE_SIZE), data_width - data_horizontal_offset);
        std::uint8_t block_height = std::min(static_cast<std::uint64_t>(BLOCK_SIDE_SIZE), data_height - data_vertical_offset);
        std::uint16_t block_val_count = block_height * block_width;

        // Extract deserialized data block from the original data
        for (std::uint8_t i = 0; i < block_height; i++) {
            std::uint16_t block_offset = i * BLOCK_SIDE_SIZE;
            std::uint64_t data_offset = i * data_width + data_block_offset;

            for (std::uint8_t j = 0; j < block_width; j++) {
                if (j + data_offset >= original_data_size) {
                    block_val_count += j - block_width;
                    break;
                }

                deserialized_block[j + block_offset] = data[j + data_offset];
            }
        }

        data_horizontal_offset += BLOCK_SIDE_SIZE;

        if (data_horizontal_offset >= data_width) {
            data_horizontal_offset = 0;
            data_vertical_offset += BLOCK_SIDE_SIZE;
        }

        // Serialize the extracted deseriaized data block and compress it
        serialized_block.resize(block_val_count);
        serialize_block(deserialized_block, false, block_val_count, block_width, block_height, serialized_block);
        compressed_block_h.clear();
        compress(serialized_block, huffman_encoder, compressed_block_h, use_model, use_rle);

        if (use_model || use_rle) {
            transpose_block_in_place(deserialized_block);
            serialize_block(deserialized_block, true, block_val_count, block_height, block_width, serialized_block);
            compressed_block_v.clear();
            compress(serialized_block, huffman_encoder, compressed_block_v, use_model, use_rle);

            if (compressed_block_h.size() > compressed_block_v.size()) {
                compressed_data.push_back(VERTICAL_SCAN);
                compressed_data.insert(compressed_data.end(), compressed_block_v.begin(), compressed_block_v.end());
            }
            else {
                compressed_data.push_back(HORIZONTAL_SCAN);
                compressed_data.insert(compressed_data.end(), compressed_block_h.begin(), compressed_block_h.end());
            }
        }
        else {
            compressed_data.push_back(HORIZONTAL_SCAN);
            compressed_data.insert(compressed_data.end(), compressed_block_h.begin(), compressed_block_h.end());
        }

        remaining_decompressed_data_size -= block_val_count;
    }
}


bool decompress_adaptively(
    std::vector<std::uint8_t>::const_iterator first, 
    std::vector<std::uint8_t>::const_iterator last, 
    std::vector<std::uint8_t> &decompressed_data, 
    const bool use_model, 
    const bool use_rle
) {
    if (std::distance(first, last) < 16) {
        std::cerr << "Invalid compressed data - incomplete size or width of the decompressed data" << std::endl;
        return false;
    }

    std::uint64_t original_data_size = 0;
    std::uint64_t data_width = 0;

    // Get the original data size and its width
    for (std::uint8_t i = 0; i < 8; i++) {
        original_data_size |= static_cast<uint64_t>(*first) << i * BYTE_BIT_LENGTH;
        data_width |= static_cast<uint64_t>(*(first++ + 8)) << i * BYTE_BIT_LENGTH;
    }

    const std::uint64_t data_height = original_data_size / data_width + (original_data_size % data_width != 0 ? 1 : 0);
    decompressed_data.resize(original_data_size);
    std::vector<std::uint8_t> serialized_block;
    std::vector<std::uint8_t> deserialized_block(BLOCK_SIZE);
    std::uint64_t data_horizontal_offset = 0;
    std::uint64_t data_vertical_offset = 0;
    std::uint64_t remaining_decompressed_data_size = original_data_size;
    auto huffman_decoder = HuffmanDecoder();
    huffman_decoder.set_source(first + 8, last);
    const std::uint64_t unaligned_data_remainder = original_data_size % data_width;

    while (remaining_decompressed_data_size > 0) {
        if (huffman_decoder.is_source_proccessed()) {
            std::cerr << "Invalid compressed data - the size of the decompressed data is lower than the size specified in the compressed data header" << std::endl;
            return false;
        }

        bool is_transposed = *huffman_decoder.get_current_source_it() == VERTICAL_SCAN;
        huffman_decoder.advance_source();

        std::uint8_t block_width = std::min(static_cast<std::uint64_t>(BLOCK_SIDE_SIZE), data_width - data_horizontal_offset);
        std::uint8_t block_height = std::min(
            static_cast<std::uint64_t>(BLOCK_SIDE_SIZE), 
            data_height - data_vertical_offset 
                - (unaligned_data_remainder == 0 || data_height - data_vertical_offset > BLOCK_SIDE_SIZE || data_horizontal_offset < unaligned_data_remainder ? 0 : 1)
        );
        std::uint64_t data_block_offset = data_horizontal_offset + data_vertical_offset * data_width;
        std::uint64_t data_block_end_offset = data_block_offset + block_width + (block_height - 1) * data_width;

        // Decompress the serialized data block and deserialize it
        if (!decompress(
            serialized_block, 
            huffman_decoder, 
            use_model, 
            use_rle, 
            block_height * block_width - (data_block_end_offset > original_data_size ? data_block_end_offset - original_data_size : 0)
        )) {
            return false;
        }

        if (remaining_decompressed_data_size < serialized_block.size()) {
            std::cerr << "Invalid compressed data - the size of the decompressed data is greater than the size specified in the compressed data header" << std::endl;
            return false;
        }

        deserialize_block(serialized_block, is_transposed, serialized_block.size(), block_width, block_height, deserialized_block);

        if (is_transposed) {
            transpose_block_in_place(deserialized_block);
        }

        // Put the deserialized data block to its original position in the original data
        for (std::uint8_t i = 0; i < block_height; i++) {
            std::uint16_t block_offset = i * BLOCK_SIDE_SIZE;
            std::uint64_t data_offset = i * data_width + data_block_offset;

            for (std::uint8_t j = 0; j < block_width; j++) {
                if (j + data_offset >= original_data_size) {
                    break;
                }

                decompressed_data[j + data_offset] = deserialized_block[j + block_offset];
            }
        }

        remaining_decompressed_data_size -= serialized_block.size();
        data_horizontal_offset += BLOCK_SIDE_SIZE;

        if (data_horizontal_offset >= data_width) {
            data_horizontal_offset = 0;
            data_vertical_offset += BLOCK_SIDE_SIZE;
        }
    }

    return true;
}
