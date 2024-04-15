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
    ArgParser arg_parser = ArgParser();
    
    if (!arg_parser.parse_args(argc, argv)) {
        return EXIT_FAILURE;
    }

    if (arg_parser.help) {
        arg_parser.print_usage();
        return EXIT_SUCCESS;
    }

    std::vector<std::uint8_t> input_data;

    if (read_bin_file(arg_parser.input_file, input_data)) {
        return EXIT_FAILURE;
    }

    std::vector<std::uint8_t> output_data;

    if (arg_parser.compress) {
        if (arg_parser.adapt) {

        }
        else {

        }
    }
    else {
        if (arg_parser.adapt) {

        }
        else {
            
        }
    }

    write_bin_file(arg_parser.output_file, output_data);

    return EXIT_SUCCESS;
}


//encode_adj_val_diff(data);
//auto model_rle_data_en = encode_rle(data, 128);
//auto encoded_data = encode_huffman(model_rle_data_en);
//auto decoded_data = decode_huffman(data);
//auto model_rle_data_de = decode_rle(decoded_data, 128);
//decode_adj_val_diff(model_rle_data_de);
