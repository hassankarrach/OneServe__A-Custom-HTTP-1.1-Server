#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "./request_parser.hpp"

class Request : public RequestParser {

	public :
		Request();
		~Request();
};

#endif