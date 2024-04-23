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
#include "compress.h"


/**
 * @brief Based on command line arguments compress or decompress the input file in the specified mode or print help to the standard output.
 * 
 * @param argc The number of command line arguments
 * @param argv Values of command line arguments
 * 
 * @return 0 in case of successful compression or decompression, 1 otherwise.
 */
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
            if (arg_parser.adapt_scan) {
                output_data = compress_adaptively(input_data, arg_parser.width_value, arg_parser.use_model);
            }
            else {
                output_data = compress_statically(input_data, arg_parser.use_model);
            }
        }
        else {
            if (arg_parser.adapt_scan) {
                if (!decompress_adaptively(input_data, output_data, arg_parser.use_model)) {
                    return EXIT_FAILURE;
                }
            }
            else {
                if (!decompress_statically(input_data, output_data, arg_parser.use_model)) {
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
