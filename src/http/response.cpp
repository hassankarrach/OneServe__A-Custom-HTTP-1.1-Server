#include "../../includes/http/response.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>

Response::Response() : _version(""), _Status(""), _Headers(), _Body(""), _Resp(""), _fileSize(0)
{
	_ChunkedState = RESPONSE::CHUNKED_HEADERS;
}

Response &Response::WithHttpVersion(std::string version)
{
	(void)version;
	this->_version = "HTTP/1.1";
	return *this;
}

Response &Response::WithStatus(int status)
{
	this->_Status = std::string(NumberToString(status).c_str()) + " " + RESPONSE::toString(static_cast<RESPONSE::ResponseCode>(status));
	return *this;
}

Response &Response::setDefaultHeaders()
{
	_Headers["Server"] = "Webserv";
	// _Headers["Connection"] = "close";
	return *this;
}

Response &Response::WithHeader(std::string key, std::string value)
{
	_Headers[key] = value;
	return *this;
}

Response &Response::WithBody(std::string body)
{
	_Body = body;
	(*this).WithHeader("content-length", NumberToString(body.size()));
	return *this;
}

void Response::Http200(int client_socket)
{
	Response res;
	res.WithHttpVersion("HTTP/1.1")
		.WithStatus(200)
		.setDefaultHeaders()
		.Generate()
		.Send(client_socket);
}
void Response::BadRequest(std::string PageUrl, int client_socket)
{
	std::ifstream DefFile;
	std::ifstream file(PageUrl.c_str());

	std::string body;
	
	if (!file.is_open())
	{
		DefFile.open("www/errors/400.html");
		body.assign((std::istreambuf_iterator<char>(DefFile)), std::istreambuf_iterator<char>());
	}
	else
		body.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	Response res;
	res.WithHttpVersion("HTTP/1.1")
		.WithStatus(400)
		.setDefaultHeaders()
		.WithHeader("Content-Type", "text/html")
		.WithBody(body)
		.Generate()
		.Send(client_socket);
}
void Response::NotFound(std::string PageUrl, int client_socket)
{
	std::ifstream DefFile;
	std::ifstream file(PageUrl.c_str());

	std::string body;
	
	if (!file.is_open())
	{
		DefFile.open("www/errors/404.html");
		body.assign((std::istreambuf_iterator<char>(DefFile)), std::istreambuf_iterator<char>());
	}
	else
		body.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	Response res;
	res.WithHttpVersion("HTTP/1.1")
		.WithStatus(404)
		.setDefaultHeaders()
		.WithHeader("Content-Type", "text/html")
		.WithBody(body)
		.Generate()
		.Send(client_socket);
}
void Response::MethodNotAllowed(std::string PageUrl)
{
	std::ifstream DefFile;
	std::ifstream file(PageUrl.c_str());

	std::string body;
	
	if (!file.is_open())
	{
		DefFile.open("www/errors/405.html");
		body.assign((std::istreambuf_iterator<char>(DefFile)), std::istreambuf_iterator<char>());
	}
	else
		body.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	Response res;
	res.WithHttpVersion("HTTP/1.1")
		.WithStatus(405)
		.setDefaultHeaders()
		.WithHeader("Content-Type", "text/html")
		.WithBody(body)
		.Generate()
		.Send(1);
}
void Response::InternalServerError(std::string PageUrl, int client_socket)
{
	std::ifstream DefFile;
	std::ifstream file(PageUrl.c_str());

	std::string body;
	
	if (!file.is_open())
	{
		DefFile.open("www/errors/500.html");
		body.assign((std::istreambuf_iterator<char>(DefFile)), std::istreambuf_iterator<char>());
	}
	else
		body.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	Response res;
	res.WithHttpVersion("HTTP/1.1")
		.WithStatus(500)
		.setDefaultHeaders()
		.WithHeader("Content-Type", "text/html")
		.WithBody(body)
		.Generate()
		.Send(client_socket);
}
void Response::NotImplemented(std::string PageUrl)
{
	std::ifstream DefFile;
	std::ifstream file(PageUrl.c_str());

	std::string body;
	
	if (!file.is_open())
	{
		DefFile.open("www/errors/501.html");
		body.assign((std::istreambuf_iterator<char>(DefFile)), std::istreambuf_iterator<char>());
	}
	else
		body.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	Response res;
	res.WithHttpVersion("HTTP/1.1")
		.WithStatus(501)
		.setDefaultHeaders()
		.WithHeader("Content-Type", "text/html")
		.WithBody(body)
		.Generate()
		.Send(1);
}

Response &Response::Generate(int isChunked)
{
	if (isChunked)
	{
		switch (this->_ChunkedState)
		{
		case RESPONSE::CHUNKED_HEADERS:
		{
			this->_Resp = _version + " " + _Status + "\r\n";
			for (std::map<std::string, std::string>::iterator it = _Headers.begin(); it != _Headers.end(); it++)
				this->_Resp += it->first + ": " + it->second + "\r\n";
			this->_Resp += "\r\n";
			break;
		}
		case RESPONSE::CHUNKED_BODY:
		{
			std::stringstream hexStream;
			hexStream << std::hex << this->_Body.size();
			std::string hexSize = hexStream.str();
			this->_Resp = hexSize + "\r\n";
			this->_Resp += this->_Body + "\r\n";
			break;
		}
		case RESPONSE::CHUNKED_END:
		{
			this->_Resp = "0\r\n\r\n";
			break;
		}
		default:
			break;
		}
	}
	else
	{
		this->_Resp = _version + " " + _Status + "\r\n";
		for (std::map<std::string, std::string>::iterator it = _Headers.begin(); it != _Headers.end(); it++)
		{
			this->_Resp += it->first + ": " + it->second + "\r\n";
		}
		this->_Resp += "\r\n";
		this->_Resp += _Body;
	}
	return *this;
}

int Response::Send(int client_socket)
{
	// std::cout << "Response: " << _Resp << std::endl;
	send(client_socket, this->_Resp.c_str(), this->_Resp.size(), MSG_NOSIGNAL); // Send response
	Clear();
	return 0;
}

void Response::Clear()
{
	_Resp = "";
	_Body = "";
	_Status = "";
	_Headers.clear();
}
