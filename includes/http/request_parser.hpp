#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <string>
#include <iostream>
#include <fstream>
#include "cstdlib"
#include "../utils/utils.hpp"
#include "../types/reqTypes.hpp"

#include <sstream>
#include <iomanip>
#include <ctime>

class RequestParser
{
private:
	// Res
	HttpRequestData	_res;
	
	// Tmp holders
	std::ofstream	_tmp_file;
	std::string		_tmp_file_name;
	std::string		_method_tmp;
	std::string		_version_tmp;
	// Headers
	std::string		_curr_header_key;
	std::string		_curr_header_value;


protected:
	static bool 	isSpace(char c);
	static bool 	isDigit(char c);
	static bool 	isAlpha(char c);
	static bool 	isHex(char c);
	// Uri
	static bool 	isUnreserved(char c);
	static bool 	isReserved(char c);
	static bool 	isReservedPath(char c);
	static bool 	isSubDelim(char c);
	static bool 	isCtl(char c);
	static bool 	isTchar(char c);
	static bool 	isPunctuation(char c);
	static int		fromHex(char c);
	bool			isPrintable(char c);
	std::string 	to_lowercase(const std::string& str);

public:
	RequestParser();
	~RequestParser();

	std::string		generateUniqueFileName();
	
	int				Parse(std::string request);
	int				ParseMultiPartFormData();
	HttpRequestData getResult();
};

#endif