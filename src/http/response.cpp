#include "../../includes/http/response.hpp"
#include <sys/socket.h>
#include <unistd.h> 
#include <sstream>

Response::Response() : _version(""), _Status(""), _Headers(), _Body(""), _Resp(""), _fileSize(0)
{
	std::cout << "==Response created==" << std::endl;
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
	return *this;
}

bool Response::OpenFile(const std::string &resolvedPath, HttpRequestData &req, int client_socket)
{
	this->_file.open(resolvedPath.c_str(), std::ios::in | std::ios::binary);
	if (!this->_file.is_open())
	{
		(this)->WithHttpVersion(Version::toString(req._version))
			.WithStatus(404)
			.setDefaultHeaders()
			.WithBody("404 Not Found")
			.Generate()
			.Send(client_socket);

		return false;
	}

	this->_file.seekg(0, std::ios::end);
	_fileSize = this->_file.tellg();
	this->_file.seekg(0, std::ios::beg);

	return true;
}

int Response::Serve(int client_socket, HttpRequestData &req)
{
	std::string Root = "www";
	std::string resolvedPath = req._uri.host == "/" ? Root + "/html/index.html" : Root + "/" + req._uri.host;
	const size_t chunk_threshold = 2 * 1024 * 1024; // 2mb
	const size_t buffer_size = 4096;				// 4kb

	if (!_file.is_open())
	{
		if (!OpenFile(resolvedPath, req, client_socket))
			return 1; // Error, file not found

		if (_fileSize < chunk_threshold)
		{
			std::string body;
		try
		{
			body.assign((std::istreambuf_iterator<char>(_file)), std::istreambuf_iterator<char>());
		
		}
		catch(const std::exception& e)
		{
			std::cout << e.what() << std::endl;
			std::cout << "here ------fsf" << std::endl;
			
			// throw(std::logic_error("hello error"));
		
		}

			(this)->WithHttpVersion(Version::toString(req._version))
				.WithStatus(200)
				.setDefaultHeaders()
				.WithHeader("Content-Type", GetMimeType(resolvedPath))
				.WithHeader("Content-Length", NumberToString(body.size()))
				.WithBody(body)
				.Generate()
				.Send(client_socket);

			_file.close();
			return 1; // Done
		}
		else
		{
			// Send headers for chunked transfer encoding
			(this)->WithHttpVersion(Version::toString(req._version))
				.WithStatus(200)
				.setDefaultHeaders()
				.WithHeader("Content-Type", GetMimeType(resolvedPath))
				.WithHeader("Transfer-Encoding", "chunked")
				.Generate(1)
				.Send(client_socket);
			_ChunkedState = RESPONSE::CHUNKED_BODY;
			return 0; // Continue through epoll
		}
	}

	// Read and send the next chunk
	char buffer[buffer_size];
	_file.read(buffer, buffer_size);
	std::string bufferStr(buffer, _file.gcount());

	if (_file.eof() && bufferStr.empty())
	{
		// End of file, send the final chunk
		_ChunkedState = RESPONSE::CHUNKED_END;
		this->Generate(1)
			.Send(client_socket);
		_file.close();
		return 1; // Done
	}
	else
	{
		// Send the current chunk
		(this)->WithBody(bufferStr).Generate(1).Send(client_socket);
		return 0; // Not done, continue through epoll
	}
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
			{
				this->_Resp += it->first + ": " + it->second + "\r\n";
			}
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
	// std::cout << _Resp << std::endl;
	send(client_socket, this->_Resp.c_str(),  this->_Resp.size(), 0); // Send response

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

int Response::HardClear()
{
	_Resp = "";
	_Body = "";
	_Status = "";
	_Headers.clear();
	_fileSize = 0;
	_ChunkedState = RESPONSE::CHUNKED_HEADERS;
	_file.close();
	return 0;
}