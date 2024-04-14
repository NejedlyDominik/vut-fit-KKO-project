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


int main(int argc, char *argv[]) {
    std::vector<std::uint8_t> data;
    read_bin_file("./data/hd01.raw", data);
    encode_adj_val_diff(data);
    auto model_rle_data_en = encode_rle(data, 128);
    auto encoded_data = encode_huffman(model_rle_data_en);
    write_bin_file("test.huff", encoded_data);
    read_bin_file("./test.huff", data);
    auto decoded_data = decode_huffman(data);
    auto model_rle_data_de = decode_rle(decoded_data, 128);
    decode_adj_val_diff(model_rle_data_de);
    write_bin_file("test.raw", model_rle_data_de);
    exit(0);
    ArgParser arg_parser = ArgParser();
    
    if (!arg_parser.parse_args(argc, argv)) {
        return EXIT_FAILURE;
    }

    if (arg_parser.help) {
        arg_parser.print_usage();
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}