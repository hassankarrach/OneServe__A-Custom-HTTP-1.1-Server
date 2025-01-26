#include "../../includes/types/httpTypes.hpp"

std::string Method::toString(Method::Type method)
{
	switch (method)
	{
	case Method::GET:
		return "GET";
	case Method::POST:
		return "POST";
	case Method::DELETE:
		return "DELETE";
	default:
		return "UNKNOWN";
	}
}

Method::Type Method::fromString(std::string method)
{
	if (method == "GET")
		return Method::GET;
	else if (method == "POST")
		return Method::POST;
	else if (method == "DELETE")
		return Method::DELETE;
	else
		return Method::UNKNOWN;
}

std::string Version::toString(Version::Type version)
{
	switch (version)
	{
	case Version::HTTP_1_0:
		return "HTTP/1.0";
	case Version::HTTP_1_1:
		return "HTTP/1.1";
	default:
		return "UNKNOWN";
	}
}

Version::Type Version::fromString(std::string version)
{
	if (version == "HTTP/1.0")
		return Version::HTTP_1_0;
	else if (version == "HTTP/1.1")
		return Version::HTTP_1_1;
	else
		return Version::UNKNOWN;
}

std::string GetMimeType(const std::string &FilePath)
{
	size_t dotPos = FilePath.find_last_of('.');
	if (dotPos == std::string::npos)
		return "application/octet-stream";

	std::string extension = FilePath.substr(dotPos);

	std::map<std::string, std::string> MimeTypes;
	// Common mime types
	MimeTypes[".html"] = "text/html";
	MimeTypes[".css"] = "text/css";
	MimeTypes[".js"] = "text/javascript";
	MimeTypes[".jpg"] = "image/jpeg";
	MimeTypes[".jpeg"] = "image/jpeg";
	MimeTypes[".png"] = "image/png";
	MimeTypes[".gif"] = "image/gif";
	MimeTypes[".pdf"] = "application/pdf";
	MimeTypes[".doc"] = "application/msword";
	MimeTypes[".zip"] = "application/zip";
	MimeTypes[".mp3"] = "audio/mpeg";
	MimeTypes[".wav"] = "audio/wav";
	MimeTypes[".mp4"] = "video/mp4";
	MimeTypes[".avi"] = "video/x-msvideo";
	MimeTypes[".json"] = "application/json";
	MimeTypes[".xml"] = "application/xml";
	MimeTypes[".svg"] = "image/svg+xml";
	MimeTypes[".ico"] = "image/x-icon";
	MimeTypes[".ttf"] = "font/ttf";
	MimeTypes[".otf"] = "font/otf";
	MimeTypes[".woff"] = "font/woff";

	if (MimeTypes.find(extension) != MimeTypes.end())
		return MimeTypes[extension];
	else
		return "application/octet-stream";
}