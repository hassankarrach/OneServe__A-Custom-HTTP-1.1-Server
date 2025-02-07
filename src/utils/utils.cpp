// Common utility functions
#include "../../includes/utils/utils.hpp"

#include <iostream>
#include <fcntl.h>
#include <sys/socket.h>
#include <cstdlib>
#include "sstream"
#include <ctime>


void setNonBlocking(int sock) {
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "fcntl F_GETFL failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "fcntl F_SETFL failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

unsigned long stringToUnsignedLong(const std::string& str)
{
    char* end;
    unsigned long result = std::strtoul(str.c_str(), &end, 10);
    if (*end != '\0')
        throw std::invalid_argument("Invalid string for conversion to unsigned long"); // catch it later.
    return result;
}

std::string NumberToString(int number)
{
    std::string result;
    std::stringstream ss;
    ss << number;
    ss >> result;
    return result;
}

bool isDirectory(const std::string &path)
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) != 0)
		return false; // Path does not exist or cannot be accessed
	return S_ISDIR(statbuf.st_mode);
}


std::string get_http_date() {
    std::time_t now = std::time(NULL);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
    return std::string(buf);
}