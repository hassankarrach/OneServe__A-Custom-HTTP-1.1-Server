#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include "../types/resTypes.hpp"
#include "../utils/utils.hpp"
#include <fstream>

class Response
{
private:
	std::string _version;
	std::string _Status;
	std::map<std::string, std::string> _Headers;
	std::string _Body;

	std::string _Resp;

	// Chunked Serve
	RESPONSE::ChunkedState _ChunkedState;
	std::ifstream _file;
	size_t _fileSize;

public:
	Response();

	// Static Common Responses
	static void BadRequest(std::string PageUrl);
	static void NotFound(std::string PageUrl, int client_socket);
	static void MethodNotAllowed(std::string PageUrl);
	static void InternalServerError(std::string PageUrl);
	static void NotImplemented(std::string PageUrl);
	// POST
	static void Created(int client_socket);


	// Methods
	Response &WithHttpVersion(std::string version);
	Response &WithStatus(int status);
	Response &setDefaultHeaders();
	Response &WithHeader(std::string key, std::string value);
	Response &WithBody(std::string body);
	Response &Generate(int isChunked = 0);
	
	
	void Clear();

	// Serving Strategy
	bool OpenFile(const std::string &resolvedPath, HttpRequestData &req, int client_socket);
	int Serve(int client_socket, HttpRequestData &req);

	int Send(int client_socket);
};

#endif