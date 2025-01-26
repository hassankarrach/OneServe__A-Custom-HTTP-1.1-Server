#ifndef RES_TYPES_HPP
#define RES_TYPES_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "httpTypes.hpp"
#include "reqTypes.hpp"
#include <map>

namespace RESPONSE
{
	// Common http status codes
	enum ResponseCode
	{
		OK = 200,
		Created = 201,
		NoContent = 204,
		BadRequest = 400,
		Unauthorized = 401,
		Forbidden = 403,
		NotFound = 404,
		InternalServerError = 500,
		NotImplemented = 501,
		ServiceUnavailable = 503
	};

	enum ChunkedState
	{
		CHUNKED_HEADERS,
		CHUNKED_BODY,
		CHUNKED_END
	};

	// Helpers
	std::string toString(RESPONSE::ResponseCode responseCode);
};

#endif