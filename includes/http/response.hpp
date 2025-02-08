#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "./cgi_handler.hpp"
#include <string>
#include <map>
#include "../types/resTypes.hpp"
#include "../utils/utils.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

// for access
#include <unistd.h>

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
	// OK
	static void Http200(int client_socket); // OK, with content response
	static void Http201(int client_socket); // OK, for POST request, returns the created resource
	static void Http204(int client_socket); // OK, with no content response, mostly for DELETE request
	// Redirection
	static void Http301(int client_socket, std::string redirection_link);
	// Payload Too Large
	static void Http413(int client_socker);

	static void BadRequest(int client_socket, HttpRequestData &req);
	static void NotFound(int client_socket, HttpRequestData &req);
	static void MethodNotAllowed(int client_socket, HttpRequestData &req);
	static void InternalServerError(int client_socket, HttpRequestData &req);
	static void NotImplemented(int client_socket, HttpRequestData &req);

	// Methods
	Response &WithHttpVersion(std::string version);
	Response &WithStatus(int status);
	Response &setDefaultHeaders();
	Response &WithHeader(std::string key, std::string value);
	Response &WithBody(std::string body);
	Response &Generate(int isChunked = 0);
	int Send(int client_socket);

	void Clear();

	// Get
	bool 		OpenFile(const std::string &resolvedPath, HttpRequestData &req, int client_socket);
	int 		Serve(int client_socket, HttpRequestData &req);
	int 		ServeDirectory(int client_socket, std::string DirPath, HttpRequestData &req);
	int 		ServeFile(int client_socket, std::string resolvedPath, HttpRequestData &req);
	// Post
	int ParseMultiPartFormData(HttpRequestData &req, int client_socket);
	int Post(int client_socket, HttpRequestData &req);
	// Delete
	int Delete(int client_socket, HttpRequestData &req);
};

#endif