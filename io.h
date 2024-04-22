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
#include <string>
#include <cstdint>


/**
 * @brief Read the contents of the file and store it to the buffer.
 * 
 * @param filename The name of the file to be read from
 * @param buffer Buffer to store the file contents
 * 
 * @return True if the file data are successfully read, false otherwise.
 */
bool read_bin_file(const std::string &filename, std::vector<std::uint8_t> &buffer);

/**
 * @brief Write the contents of the buffer to the file.
 * 
 * @param filename The name of the file to be written to
 * @param buffer The buffer whose contents are to be written to the file
 * 
 * @return True in case of successful writing of data to the file, false otherwise.
 */
bool write_bin_file(const std::string &filename, std::vector<std::uint8_t> &buffer);


#endif
