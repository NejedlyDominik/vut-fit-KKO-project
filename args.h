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
        bool compress;              // compression or decompression
        bool model;                 // model and RLE
        bool adapt;                 // adaptie scanning
        char *input_file;
        char *output_file;
        std::uint64_t width_value;  // image width  
        bool help;

        /**
         * Initialize values of instance variables of ArgParser class.
         */
        ArgParser();

        /**
         * Print usage of the program to standard output.
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
