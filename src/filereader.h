//
// Created by troll on 3/22/2025.
//

#ifndef FILEREADER_H
#define FILEREADER_H

#include <fstream>
#include <filesystem>

inline std::string read_file(const std::filesystem::__cxx11::path &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file");
    size_t file_size = file.tellg();
    std::string buffer;
    buffer.reserve(file_size);
    file.seekg(0);
    size_t counter = 0;
    while (counter++ != file_size)
        buffer += static_cast<char>(file.get());
    file.close();
    return buffer;
}

#endif //FILEREADER_H
