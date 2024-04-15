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


std::vector<std::uint8_t> encode_huffman(const std::vector<std::uint8_t> &data) {
    auto huffman_encoder = HuffmanEncoder();
    auto freqs = get_freqs(data);
    std::vector<std::uint8_t> encoded_data;
    huffman_encoder.initialize_encoding(freqs, encoded_data);
    huffman_encoder.encode_data(data, encoded_data);
    huffman_encoder.finalize_encoding(encoded_data);
    return encoded_data;
}


std::vector<std::uint8_t> decode_huffman(const std::vector<std::uint8_t> &data) {
    auto huffman_decoder = HuffmanDecoder();
    huffman_decoder.set_source(data);
    huffman_decoder.initialize_decoding();
    std::vector<std::uint8_t> decoded_data;

    huffman_decoder.decode_data(decoded_data);
    return decoded_data;
}


std::vector<std::uint8_t> compress_statically(const std::vector<std::uint8_t> &data, bool use_model) {

}


std::vector<std::uint8_t> decompress_statically(const std::vector<std::uint8_t> &data, bool use_model) {
    
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

            }
            else {
                auto freqs = get_freqs(input_data);

                if (arg_parser.use_model) {
                    encode_adj_val_diff(input_data);
                }

                output_data = encode_huffman(input_data);
            }
        }
        else {
            auto huffman_decoder = HuffmanDecoder();

            if (arg_parser.adapt_scan) {

            }
            else {
                output_data = decode_huffman(input_data);

                if (arg_parser.use_model) {
                    output_data = decode_rle(output_data, 128);
                    decode_adj_val_diff(output_data);
                }
            }
        }
    }

    if (!write_bin_file(arg_parser.output_file, output_data)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


//encode_adj_val_diff(data);
//auto model_rle_data_en = encode_rle(data, 128);
//auto encoded_data = encode_huffman(model_rle_data_en);
//auto decoded_data = decode_huffman(data);
//auto model_rle_data_de = decode_rle(decoded_data, 128);
//decode_adj_val_diff(model_rle_data_de);
