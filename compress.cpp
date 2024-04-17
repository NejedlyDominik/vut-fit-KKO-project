#include "compress.h"
#include "model.h"
#include "rle.h"
#include "huffman.h"

#define COMPRESSED 1
#define UNCOMPRESSED 0


std::vector<std::uint8_t> compress_statically(const std::vector<std::uint8_t> &data, bool use_model) {
    auto huffman_encoder = HuffmanEncoder();
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


bool decompress_statically(const std::vector<std::uint8_t> &data, bool use_model, std::vector<std::uint8_t> &decompressed_data) {
    if (data.front() == UNCOMPRESSED) {
        decompressed_data.assign(data.begin() + 1, data.end());
        return true;
    }

    auto huffman_decoder = HuffmanDecoder();
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


//std::vector<std::uint8_t> compress_adaptively(const std::vector<std::uint8_t> &data, bool use_model, std::uint64_t width_value) {
//
//}


//bool decompress_adaptively(const std::vector<std::uint8_t> &data, bool use_model, std::uint64_t width_value, std::vector<std::uint8_t> &decompressed_data) {
//
//}
