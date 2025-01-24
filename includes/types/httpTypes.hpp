#ifndef HTTP_TYPES_HPP
#define HTTP_TYPES_HPP

#include <string>
#include <map>

namespace Version
{
	enum Type
	{
		HTTP_1_0,
		HTTP_1_1,
		UNKNOWN,
	};

	// Helpers
	std::string toString(Version::Type version);
	Version::Type fromString(std::string version);
}

std::string GetMimeType(const std::string &FilePath);

namespace Method
{
	enum Type
	{
		GET,
		POST,
		DELETE,
		UNKNOWN,
	};

	// Helpers
	std::string toString(Method::Type method);
	Method::Type fromString(std::string method);
}

#endif