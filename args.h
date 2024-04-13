/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 28. 3. 2024
 *
 * @brief Processing of the command line arguments interface
 */


#ifndef ARGS_H
#define ARGS_H


#include <cstdint>


/**
 * @class Parser of the command line arguments
 */
class ArgParser {
    public:
        bool compress = true;           // compression or decompression
        bool model = false;             // model and RLE
        bool adapt = false;             // adaptie scanning
        char *input_file = NULL;
        char *output_file = NULL;
        std::uint64_t width_value = 0;  // image width  
        bool help = false;

        /**
         * Print usage of the program to the standard output.
         */
        void print_usage();

        /**
         * Parse command line arguments.
         * 
         * @param argc The number of command line arguments
         * @param argv Values of command line arguments
         * 
         * @return True in case of successul parse, false otherwise (in case of ivalid usage of command line arguments).
         */
        bool parse_args(int argc, char *argv[]);
};


#endif
