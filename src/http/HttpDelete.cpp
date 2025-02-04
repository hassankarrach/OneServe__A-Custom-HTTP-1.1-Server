#include "../../includes/http/response.hpp"

void Response::Http204(int client_socket)
{
	Response res;
	res.WithHttpVersion("HTTP/1.1")
		.WithStatus(204)
		.setDefaultHeaders()
		.Generate()
		.Send(client_socket);
}

int Response::Delete(int client_socket, HttpRequestData &req)
{
	std::string File = "www/uploads/" + req._uri.host;

	// Check if the file exists in cpp
	if (access(File.c_str(), F_OK) == -1)
	{
		NotFound(client_socket, req);
		return 1;
	}

	// Delete the file
	int status = remove(File.c_str());
	if (status != 0)
	{
		InternalServerError(client_socket);
		return 1;
	}
	else
		Http204(client_socket);
	return 1;
}