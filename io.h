/**
 * VUT FIT KKO - Project - Image data compression using Huffman encoding
 *
 * @author Dominik Nejedlý (xnejed09)
 * @date 6. 4. 2024
 * 
 * @brief Input reading and output writing interface
 */


#ifndef IO_H
#define IO_H


#include <vector>
#include <cstdint>


/**
 * Read the contents of the file and store it to the buffer.
 * 
 * @param filename the name of the file to be read from
 * @param buffer buffer to store the file contents
 * 
 * @return True if the file data are successfully read, false otherwise.
 */
bool read_bin_file(char *filename, std::vector<std::uint8_t> &buffer);

/**
 * Write the contents of the buffer to the file.
 * 
 * @param filename the name of the file to be written to
 * @param buffer the buffer whose contents are to be written to the file
 * 
 * @return True in case of successful writing of data to the file, false otherwise.
 */
bool write_bin_file(char *filename, std::vector<std::uint8_t> &buffer);


#endif
