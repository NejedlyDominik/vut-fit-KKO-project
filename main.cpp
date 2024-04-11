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
#include "huffman.h"


int main(int argc, char *argv[]) {
    std::vector<std::uint8_t> data{'a','a','a','a','a','a','a','a','a','a','b','c','c','c','c','c','c','c','c','c','c','c','c','c','c','c','d','d','d','d','d','d','d'};
    //read_bin_file("./data/hd01.raw", data);
    auto encoded_data = encode_huffman(data);
    write_bin_file("test.out", encoded_data);
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