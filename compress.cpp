#include <utility>

#include "compress.h"
#include "model.h"
#include "rle.h"
#include "huffman.h"


#define COMPRESSED 1
#define UNCOMPRESSED 0

#define HORIZONTAL_SCAN 1
#define VERTICAL_SCAN 0

#define BLOCK_SIDE_SIZE 16


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


bool decompress(const std::vector<std::uint8_t> &data, const bool use_model, std::vector<std::uint8_t> &decompressed_data, HuffmanDecoder &huffman_decoder) {
    if (data.front() == UNCOMPRESSED) {
        decompressed_data.assign(data.begin() + 1, data.end());
        return true;
    }

    huffman_decoder.set_source(data.begin() + 1, data.end());

    if (!huffman_decoder.initialize_decoding()) {
        return false;
    }

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
    return decompress(data, use_model, decompressed_data, huffman_decoder);
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


std::vector<std::uint8_t> compress_adaptively(const std::vector<std::uint8_t> &data, const bool use_model, const std::uint64_t width_value) {
    const std::uint64_t data_size = data.size();
    std::vector<std::uint8_t> compressed_data;

    for (std::uint8_t i = 0; i < 64; i += 8) {
        compressed_data.push_back(data_size >> i);
    }

    for (std::uint8_t i = 0; i < 64; i += 8) {
        compressed_data.push_back(width_value >> i);
    }

    const std::uint64_t height_value = data_size / width_value + (data_size % width_value != 0 ? 1 : 0);
    std::vector<std::uint8_t> deserialized_block(BLOCK_SIDE_SIZE * BLOCK_SIDE_SIZE);
    std::vector<std::uint8_t> serialized_block(BLOCK_SIDE_SIZE * BLOCK_SIDE_SIZE);
    std::uint64_t data_horizontal_offset = 0;
    std::uint64_t data_vertical_offset = 0;
    std::uint64_t processed_val_count = 0;

    while (processed_val_count != data_size) {
        std::uint16_t block_val_count = 0;
        std::uint64_t data_offset = data_horizontal_offset + data_vertical_offset * width_value;
        std::uint8_t block_width = std::min(static_cast<std::uint64_t>(BLOCK_SIDE_SIZE), width_value - data_horizontal_offset);
        std::uint8_t block_height = std::min(static_cast<std::uint64_t>(BLOCK_SIDE_SIZE), height_value - data_vertical_offset);

        for (std::uint8_t i = 0; i < block_height; i++) {
            std::uint16_t block_offset = i * BLOCK_SIDE_SIZE;
            data_offset += i * width_value;

            for (std::uint8_t j = 0; j < block_width; j++) {
                if (j + data_offset >= data_size) {
                    block_val_count = j;
                    break;
                }

                deserialized_block[j + block_offset] = data[j + data_offset];
            }
        }

        block_val_count += (block_height - 1) * block_width;
        data_horizontal_offset += BLOCK_SIDE_SIZE;

        if (data_horizontal_offset >= width_value) {
            data_horizontal_offset = 0;
            data_vertical_offset += BLOCK_SIDE_SIZE;
        }

        if (use_model) {
            
        }

        processed_val_count += block_val_count;
    }

    return compressed_data;
}


//bool decompress_adaptively(const std::vector<std::uint8_t> &data, const bool use_model, const std::uint64_t width_value, std::vector<std::uint8_t> &decompressed_data) {
//
//}
