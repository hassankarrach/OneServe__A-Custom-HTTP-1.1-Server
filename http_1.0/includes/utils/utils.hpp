#ifndef UTILS_H
#define UTILS_H

#include <string>

void setNonBlocking(int sock);
unsigned long stringToUnsignedLong(const std::string& str);

std::string NumberToString(int number);

#endif