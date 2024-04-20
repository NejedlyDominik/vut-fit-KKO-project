#include <utility>
#include <iostream>

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


std::vector<std::uint8_t> compress(const std::vector<std::uint8_t> &data, const bool use_model, HuffmanEncoder &huffman_encoder) {
    std::vector<std::uint8_t> compressed_data(1, COMPRESSED);

    if (use_model) {
        auto preprocessed_data = encode_adj_val_diff(data.begin(), data.end());
        preprocessed_data = encode_rle(preprocessed_data.begin(), preprocessed_data.end(), DEFAULT_MARKER);
        huffman_encoder.initialize_encoding(get_freqs(preprocessed_data.begin(), preprocessed_data.end()), compressed_data);
        huffman_encoder.encode_data(preprocessed_data.begin(), preprocessed_data.end(), compressed_data);
    }
    else {
        huffman_encoder.initialize_encoding(get_freqs(data.begin(), data.end()), compressed_data);
        huffman_encoder.encode_data(data.begin(), data.end(), compressed_data);
    }

    huffman_encoder.finalize_encoding(compressed_data);

    if (compressed_data.size() >= data.size() + 1) {
        std::copy(data.begin(), data.end(), compressed_data.begin() + 1);
        compressed_data.resize(data.size() + 1);
        compressed_data[0] = UNCOMPRESSED;
    }

    return compressed_data;
}


bool decompress(const bool use_model, std::vector<std::uint8_t> &decompressed_data, HuffmanDecoder &huffman_decoder, const std::uint16_t block_decompressed_val_count = 0) {
    if (huffman_decoder.is_source_proccessed()) {
        std::cerr << "Invalid compressed data - unexpected end of the compressed data, expected compression flag" << std::endl;
        return false;
    }

    auto current_data_it = huffman_decoder.get_current_source_it();

    if (*current_data_it == UNCOMPRESSED) {
        if (block_decompressed_val_count == 0) {
            decompressed_data.assign(current_data_it + 1, huffman_decoder.get_source_end_it());
        }
        else {
            decompressed_data.assign(current_data_it + 1, current_data_it + 1 + block_decompressed_val_count);
            huffman_decoder.advance_source(1 + block_decompressed_val_count);
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

    if (use_model) {
        decompressed_data = decode_rle(decompressed_data.begin(), decompressed_data.end(), DEFAULT_MARKER);
        decompressed_data = decode_adj_val_diff(decompressed_data.begin(), decompressed_data.end());
    }

    return true;
}


std::vector<std::uint8_t> compress_statically(const std::vector<std::uint8_t> &data, const bool use_model) {
    auto huffman_encoder = HuffmanEncoder();
    return compress(data, use_model, huffman_encoder);
}


bool decompress_statically(const std::vector<std::uint8_t> &data, const bool use_model, std::vector<std::uint8_t> &decompressed_data) {
    auto huffman_decoder = HuffmanDecoder();
    huffman_decoder.set_source(data.begin(), data.end());
    return decompress(use_model, decompressed_data, huffman_decoder);
}


void transpose_block_emplace(std::vector<std::uint8_t> &block) {
    for (std::uint8_t i = 0; i < BLOCK_SIDE_SIZE; i++) {
        std::uint16_t row_offset = i * BLOCK_SIDE_SIZE;

        for (std::uint8_t j = i + 1; j < BLOCK_SIDE_SIZE; j++) {
            std::swap(block[j + row_offset], block[i + j * BLOCK_SIDE_SIZE]);
        }
    }
}


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
                if (j + serialized_block_offset >= block_val_count) {
                    break;
                }

                serialized_block[j + serialized_block_offset] = deserialized_block[j + deserialized_block_offset];
            }
        }
    }
    else {
        std::uint8_t num_of_orig_width_rows = block_val_count % block_height;

        if (num_of_orig_width_rows == 0) {
            num_of_orig_width_rows = block_height;
        }

        for (std::uint8_t i = 0; i < num_of_orig_width_rows; i++) {
            std::uint16_t deserialized_block_offset = i * BLOCK_SIDE_SIZE;
            std::uint16_t serialized_block_offset = i * block_width;

            for (std::uint8_t j = 0; j < block_width; j++) {
                serialized_block[j + serialized_block_offset] = deserialized_block[j + deserialized_block_offset];
            }
        }

        std::uint8_t shorten_block_width = block_width - 1;

        for (std::uint8_t i = num_of_orig_width_rows; i < block_height; i++) {
            std::uint16_t deserialized_block_offset = i * BLOCK_SIDE_SIZE;
            std::uint16_t serialized_block_offset = i * shorten_block_width;

            for (std::uint8_t j = 0; j < shorten_block_width; j++) {
                serialized_block[j + serialized_block_offset] = deserialized_block[j + deserialized_block_offset];
            }
        }
    }
}


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
                if (j + serialized_block_offset >= block_val_count) {
                    break;
                }

                deserialized_block[j + deserialized_block_offset] = serialized_block[j + serialized_block_offset];
            }
        }
    }
    else {
        std::uint8_t num_of_orig_width_rows = block_val_count % block_height;

        if (num_of_orig_width_rows == 0) {
            num_of_orig_width_rows = block_height;
        }

        for (std::uint8_t i = 0; i < num_of_orig_width_rows; i++) {
            std::uint16_t deserialized_block_offset = i * BLOCK_SIDE_SIZE;
            std::uint16_t serialized_block_offset = i * block_width;

            for (std::uint8_t j = 0; j < block_width; j++) {
                deserialized_block[j + deserialized_block_offset] = serialized_block[j + serialized_block_offset];
            }
        }

        std::uint8_t shorten_block_width = block_width - 1;

        for (std::uint8_t i = num_of_orig_width_rows; i < block_height; i++) {
            std::uint16_t deserialized_block_offset = i * BLOCK_SIDE_SIZE;
            std::uint16_t serialized_block_offset = i * shorten_block_width;

            for (std::uint8_t j = 0; j < shorten_block_width; j++) {
                deserialized_block[j + deserialized_block_offset] = serialized_block[j + serialized_block_offset];
            }
        }
    }
}


std::vector<std::uint8_t> compress_adaptively(const std::vector<std::uint8_t> &data, const bool use_model, const std::uint64_t data_width) {
    const std::uint64_t original_data_size = data.size();
    std::vector<std::uint8_t> compressed_data(16);

    for (std::uint8_t i = 0; i < 8; i++) {
        compressed_data[i] = original_data_size >> i * BYTE_BIT_LENGTH;
        compressed_data[i + 8] = data_width >> i * BYTE_BIT_LENGTH;
    }

    const std::uint64_t data_height = original_data_size / data_width + (original_data_size % data_width != 0 ? 1 : 0);
    std::vector<std::uint8_t> deserialized_block(BLOCK_SIZE);
    std::vector<std::uint8_t> serialized_block;
    std::uint64_t data_horizontal_offset = 0;
    std::uint64_t data_vertical_offset = 0;
    std::uint64_t remaining_decompressed_data_size = original_data_size;
    auto huffman_encoder = HuffmanEncoder();

    while (remaining_decompressed_data_size > 0) {
        std::uint64_t data_block_offset = data_horizontal_offset + data_vertical_offset * data_width;
        std::uint8_t block_width = std::min(static_cast<std::uint64_t>(BLOCK_SIDE_SIZE), data_width - data_horizontal_offset);
        std::uint8_t block_height = std::min(static_cast<std::uint64_t>(BLOCK_SIDE_SIZE), data_height - data_vertical_offset);
        std::uint16_t block_val_count = block_height * block_width;

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

        serialized_block.resize(block_val_count);
        serialize_block(deserialized_block, false, block_val_count, block_width, block_height, serialized_block);
        auto compressed_block_h = compress(serialized_block, use_model, huffman_encoder);

        if (use_model) {
            transpose_block_emplace(deserialized_block);
            serialize_block(deserialized_block, true, block_val_count, block_height, block_width, serialized_block);
            auto compressed_block_v = compress(serialized_block, use_model, huffman_encoder);

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

    return compressed_data;
}


bool decompress_adaptively(const std::vector<std::uint8_t> &data, const bool use_model, std::vector<std::uint8_t> &decompressed_data) {
    if (data.size() < 16) {
        std::cerr << "Invalid compressed data - incomplete size or width of the decompressed data" << std::endl;
        return false;
    }

    std::uint64_t original_data_size = 0;
    std::uint64_t data_width = 0;

    for (std::uint8_t i = 0; i < 8; i++) {
        original_data_size |= static_cast<uint64_t>(data[i]) << i * BYTE_BIT_LENGTH;
        data_width |= static_cast<uint64_t>(data[i + 8]) << i * BYTE_BIT_LENGTH;
    }

    const std::uint64_t data_height = original_data_size / data_width + (original_data_size % data_width != 0 ? 1 : 0);
    decompressed_data.resize(original_data_size);
    std::vector<std::uint8_t> serialized_block;
    std::vector<std::uint8_t> deserialized_block(BLOCK_SIZE);
    std::uint64_t data_horizontal_offset = 0;
    std::uint64_t data_vertical_offset = 0;
    std::uint64_t remaining_decompressed_data_size = original_data_size;
    auto huffman_decoder = HuffmanDecoder();
    huffman_decoder.set_source(data.begin() + 16, data.end());
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
            data_height - data_vertical_offset - (unaligned_data_remainder == 0 || data_height - data_vertical_offset > 16 || data_horizontal_offset < unaligned_data_remainder ? 0 : 1)
        );
        std::uint64_t data_block_offset = data_horizontal_offset + data_vertical_offset * data_width;
        std::uint64_t data_block_end_offset = data_block_offset + block_width + (block_height - 1) * data_width;

        if (!decompress(use_model, serialized_block, huffman_decoder, block_height * block_width - (data_block_end_offset > original_data_size ? data_block_end_offset - original_data_size : 0))) {
            return false;
        }

        if (remaining_decompressed_data_size < serialized_block.size()) {
            std::cerr << "Invalid compressed data - the size of the decompressed data is greater than the size specified in the compressed data header" << std::endl;
            return false;
        }

        deserialize_block(serialized_block, is_transposed, serialized_block.size(), block_width, block_height, deserialized_block);

        if (is_transposed) {
            transpose_block_emplace(deserialized_block);
        }

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
