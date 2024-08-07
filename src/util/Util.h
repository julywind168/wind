#ifndef WIND_UTIL_H
#define WIND_UTIL_H


#include <string>


std::string readFile(const std::string& filename);

std::string toLowerCase(const std::string& str);

uint32_t nextPowerOf2(uint32_t n);


#endif