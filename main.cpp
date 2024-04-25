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

#ifdef STATS
#include <chrono>
#include "huffman.h"
#include <cmath>
#include <iostream>
#endif


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

#ifdef STATS
    const auto start = std::chrono::high_resolution_clock::now();
#endif

    bool use_rle = arg_parser.use_model;

    if (!input_data.empty()) {
        if (arg_parser.compress) {
            if (arg_parser.adapt_scan) {
                output_data = compress_adaptively(input_data, arg_parser.width_value, arg_parser.use_model, use_rle);
            }
            else {
                output_data = compress_statically(input_data, arg_parser.use_model, use_rle);
            }
        }
        else {
            if (arg_parser.adapt_scan) {
                if (!decompress_adaptively(input_data, output_data, arg_parser.use_model, use_rle)) {
                    return EXIT_FAILURE;
                }
            }
            else {
                if (!decompress_statically(input_data, output_data, arg_parser.use_model, use_rle)) {
                    return EXIT_FAILURE;
                }
            }
        }
    }

#ifdef STATS
    const auto end = std::chrono::high_resolution_clock::now();
#endif

    if (!write_bin_file(arg_parser.output_file, output_data)) {
        return EXIT_FAILURE;
    }

#ifdef STATS
    const std::chrono::duration<double> diff{end - start};
    std::vector<std::uint64_t> freqs;
    std::uint64_t original_data_size;

    if (arg_parser.compress) {
        freqs = get_freqs(input_data.begin(), input_data.end());
        original_data_size = input_data.size();
    }
    else {
        freqs = get_freqs(output_data.begin(), output_data.end());
        original_data_size = output_data.size();
    }

    double entrophy = 0;

    for (const auto &val: freqs) {
        if (val != 0) {
            entrophy += (static_cast<double>(val) / original_data_size) * std::log2(static_cast<double>(original_data_size) / val);
        }
    }

    if (arg_parser.compress) {
        std::cout << "Bits per symbol: " << (output_data.size() * 8.0) / original_data_size << std::endl;
        std::cout << "Compression time (s): " << diff.count() << std::endl;
        std::cout << "Original data entrophy: " << entrophy << std::endl;
    }
    else {
        std::cout << "Bits per symbol: " << (input_data.size() * 8.0) / original_data_size << std::endl;
        std::cout << "Dempression time (s): " << diff.count() << std::endl;
        std::cout << "Original data entrophy: " << entrophy << std::endl;
    }
#endif

    return EXIT_SUCCESS;
}
