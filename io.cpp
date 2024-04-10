/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 6. 4. 2024
 * 
 * @brief Input reading and output writing module
 */


#include <iostream>
#include <cstdio>
#include <sys/stat.h>

#include "io.h"


bool read_bin_file(const std::string &filename, std::vector<std::uint8_t> &buffer) {
    struct stat stats;

    // Use the file statistics to get its size
    if (stat(filename.c_str(), &stats) == -1) {
        std::cerr << "Cannot obtain the size of the input file '" << filename << "' (using stats)" << std::endl;
        return false;
    }

    std::FILE *fd = std::fopen(filename.c_str(), "rb");

    if (fd == NULL) {
        std::cerr << "Cannot open the input file '" << filename << "'" << std::endl;
        return false;
    }

    buffer.resize(stats.st_size);
    
    if (std::fread(buffer.data(), 1, buffer.size(), fd) != buffer.size()) {
        std::cerr << "Cannot read the input file correctly '" << filename << "'" << std::endl;
        std::fclose(fd);
        return false;
    }

    std::fclose(fd);
    return true;
}


bool write_bin_file(const std::string &filename, std::vector<std::uint8_t> &buffer) {
    std::FILE *fd = std::fopen(filename.c_str(), "wb");

    if (fd == NULL) {
        std::cerr << "Cannot open the output file '" << filename << "'" << std::endl;
        return false;
    }

    if (std::fwrite(buffer.data(), 1, buffer.size(), fd) != buffer.size()) {
        std::cerr << "Cannot write to the output file correctly '" << filename << "'" << std::endl;
        std::fclose(fd);
        return false;
    }

    std::fclose(fd);
    return true;
}
