#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include "../types/resTypes.hpp"
#include "../utils/utils.hpp"
#include <fstream>

#include <sstream>
#include <iomanip>
#include <ctime>

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
	static void Http301(int client_socket);

	static void BadRequest(std::string PageUrl, int client_socket);
	static void NotFound(std::string PageUrl, int client_socket);
	static void MethodNotAllowed(std::string PageUrl);
	static void InternalServerError(std::string PageUrl, int client_socket);
	static void NotImplemented(std::string PageUrl);

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
	bool OpenFile(const std::string &resolvedPath, HttpRequestData &req, int client_socket);
	int Serve(int client_socket, HttpRequestData &req);
	// Post
	int ParseMultiPartFormData(HttpRequestData &req, int client_socket);
	int Post(int client_socket, HttpRequestData &req);
	// Delete
	int Delete(int client_socket, HttpRequestData &req);
};

#endif