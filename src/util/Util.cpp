#include "Util.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

std::string readFile(const std::string& filename) {
    std::stringstream buffer;
    std::string line;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Can't open file: " + filename);
    }

    while (std::getline(file, line)) {
        buffer << line << '\n';
    }

    return buffer.str();
}

std::string toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

uint32_t nextPowerOf2(uint32_t n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return (n + 1) == 0 ? 1 : n + 1;
}