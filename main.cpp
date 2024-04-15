/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 28. 3. 2024
 * 
 * @brief Program entry point module
 */


#include <cstdlib>

#include "args.h"
#include "io.h"
#include "model.h"
#include "rle.h"
#include "huffman.h"


std::vector<std::uint8_t> compress_statically(const std::vector<std::uint8_t> &data, bool use_model) {
    auto huffman_encoder = HuffmanEncoder();
    auto freqs = get_freqs(data);
    std::vector<std::uint8_t> encoded_data;
    huffman_encoder.initialize_encoding(freqs, encoded_data);
    huffman_encoder.encode_data(data.begin(), data.end(), encoded_data);
    huffman_encoder.finalize_encoding(encoded_data);
    return encoded_data;
}


bool decompress_statically(const std::vector<std::uint8_t> &data, bool use_model, std::vector<std::uint8_t> &decompressed_data) {
    auto huffman_decoder = HuffmanDecoder();
    huffman_decoder.set_source(data.begin(), data.end());

    if (!huffman_decoder.initialize_decoding()) {
        return false;
    }

    std::vector<std::uint8_t> decoded_data;

    if (!huffman_decoder.decode_data(decoded_data)) {
        return false;
    }

    return true;
}


std::vector<std::uint8_t> compress_adaptively(const std::vector<std::uint8_t> &data, bool use_model, std::uint64_t width_value) {

}


bool decompress_adaptively(const std::vector<std::uint8_t> &data, bool use_model, std::uint64_t width_value, std::vector<std::uint8_t> &decompressed_data) {

}


int main(int argc, char *argv[]) {
    ArgParser arg_parser = ArgParser();

    if (!arg_parser.parse_args(argc, argv)) {
        return EXIT_FAILURE;
    }

    if (arg_parser.help) {
        arg_parser.print_usage();
        return EXIT_SUCCESS;
    }

    std::vector<std::uint8_t> input_data;

    if (!read_bin_file(arg_parser.input_file, input_data)) {
        return EXIT_FAILURE;
    }

    std::vector<std::uint8_t> output_data;

    if (!input_data.empty()) {
        if (arg_parser.compress) {
            auto huffman_encoder = HuffmanEncoder();

            if (arg_parser.adapt_scan) {
                output_data = compress_adaptively(input_data, arg_parser.use_model, arg_parser.width_value);
            }
            else {
                output_data = compress_statically(input_data, arg_parser.use_model);
            }
        }
        else {
            auto huffman_decoder = HuffmanDecoder();

            if (arg_parser.adapt_scan) {
                if (!decompress_adaptively(input_data, arg_parser.use_model, arg_parser.width_value, output_data)) {
                    return EXIT_FAILURE;
                }
            }
            else {
                if (!decompress_statically(input_data, arg_parser.use_model, output_data)) {
                    return EXIT_FAILURE;
                }
            }
        }
    }

    if (!write_bin_file(arg_parser.output_file, output_data)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
