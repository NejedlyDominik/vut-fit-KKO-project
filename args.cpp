/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 28. 3. 2024
 * 
 * @brief Processing of the command line arguments module
 */


#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <cerrno>
#include <getopt.h>

#include "args.h"


ArgParser::ArgParser() {
    compress = true;
    model = false;
    adapt = false;
    input_file = NULL;
    output_file = NULL;
    width_value = 0;
    help = false;
}


void ArgParser::print_usage() {
    std::cout << "KKO - Project - Image data compression using Huffman encoding" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./huff_codec [-c|-d] [-m] [-a] -i <ifile> -o <ofile> [-w <width_value>] [-h]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -c                  compress the input file (the default application mode)" << std::endl;
    std::cout << "  -d                  decompress the input file" << std::endl;
    std::cout << "  -m                  activate the model and the RLE for preprocessing the input data" << std::endl;
    std::cout << "  -a                  activate the adaptive image scanning mode (by default the sequential scanning" << std::endl;
    std::cout << "                      in the horizontal direction is used without dividing into blocks)" << std::endl;
    std::cout << "  -i <ifile>          the name of the input file (data to compress or decompress depending on the application mode)" << std::endl;
    std::cout << "  -o <ofile>          the name of the output file (the resulting compressed or decompressed data)" << std::endl;
    std::cout << "  -w <width_value>    specify the image width (the width_value is expected to be grater than 0 -- width_value >= 1)," << std::endl;
    std::cout << "                      must be specified in case of the compression application mode (parameter -c)" << std::endl;
    std::cout << "  -h                  print the help to the standard output and exit" << std::endl;
}


bool ArgParser::parse_args(int argc, char *argv[]) {
    int opt;
    char *width_value_arg = NULL;

    while ((opt = getopt(argc, argv, "cdmai:o:w:h")) != -1) {
        switch (opt) {
            case 'c':
                compress = true;
                break;
            case 'd':
                compress = false;
                break;
            case 'm':
                model = true;
                break;
            case 'a':
                adapt = true;
                break;
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'w':
                width_value_arg = optarg;
                break;
            case 'h':
                help = true;
                return true;
        }
    }

    if (input_file == NULL) {
        std::cerr << "Missing input file" << std::endl;
        return false;
    }

    if (output_file == NULL) {
        std::cerr << "Missing output file" << std::endl;
        return false;
    }

    if (compress) {
        if (width_value_arg == NULL) {
            std::cerr << "For compression (paramater -c), the image width (parameter -w) must be set" << std::endl;
            return false;
        }

        char *width_value_end;
        width_value = std::strtoull(width_value_arg, &width_value_end, 0);

        if (*width_value_end != '\0' || width_value < 1) {
            std::cerr << "Invalid value of the image width parameter -w: '" << width_value_arg << "' -- a number greater than 0 is expected (width_value >= 1)"  << std::endl;
            return false;
        }

        if (errno == ERANGE) {
            std::cerr << "Range error of the image width parameter -w: '" << width_value_arg << "' -- the image width is too large" << std::endl;
            return false;
        }
    }

    return true;
}
