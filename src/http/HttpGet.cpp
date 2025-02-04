#include "../../includes/http/response.hpp"

void Response::Http301(int client_socket)
{
	Response res;
	res.WithHttpVersion("HTTP/1.1")
		.WithStatus(301)
		.setDefaultHeaders()
		.WithHeader("Location", "/?redirected=true")
		.WithHeader("connection", "keep-alive")
		.Generate()
		.Send(client_socket);
}

bool Response::OpenFile(const std::string &resolvedPath, HttpRequestData &req, int client_socket)
{
	(void)client_socket;
	(void)req;

	this->_file.open(resolvedPath.c_str(), std::ios::in | std::ios::binary);
	if (!this->_file.is_open())
	{
		// File not found
		NotFound(client_socket, req);
		return false;
	}
	this->_file.seekg(0, std::ios::end);
	_fileSize = this->_file.tellg();
	this->_file.seekg(0, std::ios::beg);
	return true;
}

static bool isDirectory(const std::string &path)
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) != 0)
		return false; // Path does not exist or cannot be accessed
	return S_ISDIR(statbuf.st_mode);
}

int Response::Serve(int client_socket, HttpRequestData &req)
{
	std::string Root = "www";
	std::string resolvedPath = req._uri.host[req._uri.host.size() - 1] == '/' ? Root + req._uri.host + "index.html"
																			  : Root + req._uri.host;

	// /IMAGE.PHP
	//resCGI =  hanglecgi(HttpRequestData, resCGI);
	// res.CGI.getStatus() = 200 / 404 - 500


	const size_t chunk_threshold = 2 * 1024 * 1024; // 2mb
	const size_t buffer_size = 4096;				// 4kb

	if (req._uri.host == "/redirect_me_plz")
	{
		Http301(client_socket);
		return 1;
	}

	// check if file is a directory using opendir
	if (isDirectory(resolvedPath))
	{
		// check if the directory has an index.html file
		if (access((resolvedPath + "/index.html").c_str(), F_OK) == 0)
			resolvedPath += "/index.html";
		else
			return ServeDirectory(client_socket, resolvedPath);
	}

	if (!_file.is_open())
	{
		if (!OpenFile(resolvedPath, req, client_socket))
			return 1; // Error, file not found

		if (_fileSize < chunk_threshold)
			return ServeFile(client_socket, resolvedPath, req);
		else
		{
			(this)->WithHttpVersion(Version::toString(req._version)).WithStatus(200).setDefaultHeaders().WithHeader("Content-Type", GetMimeType(resolvedPath)).WithHeader("Transfer-Encoding", "chunked").Generate(1).Send(client_socket);
			_ChunkedState = RESPONSE::CHUNKED_BODY;
			return 0;
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

int Response::ServeFile(int client_socket, std::string resolvedPath, HttpRequestData &req)
{
	std::string body;

	body.assign((std::istreambuf_iterator<char>(_file)), std::istreambuf_iterator<char>()); // TODO May fail here, in case of file not found or other errors.

	(this)->WithHttpVersion(Version::toString(req._version)).WithStatus(200).setDefaultHeaders().WithHeader("Content-Type", GetMimeType(resolvedPath)).WithBody(body).Generate().Send(client_socket);

	_file.close();
	return 1;
}

int Response::ServeDirectory(int client_socket, std::string DirPath)
{
	DIR *dir;
	struct dirent *ent;
	std::ostringstream html;

	dir = opendir(DirPath.c_str());

	if (dir == NULL)
	{
		InternalServerError(client_socket);
		return 1;
	}

	html << "<html><head><title>Index of " << DirPath << "</title></head><body><h1>Index of " << DirPath << "</h1><hr><pre>";

	while ((ent = readdir(dir)) != NULL)
	{
		std::string name = ent->d_name;
		if (name == "." || name == "..")
			continue; // Skip current and parent directory links
		if (isDirectory(DirPath + "/" + name))
			name += "/"; // Append a slash to indicate a directory

		// Generate relative path safely
		std::string relativePath = DirPath;
		if (relativePath.compare(0, 4, "www/") == 0) // Ensure "www/" prefix exists
			relativePath = relativePath.substr(4);

		html << "<a href=\"" << relativePath + "/" + name << "\">" << name << "</a><br>";
	}

	html << "</pre><hr></body></html>";
	closedir(dir);

	(this)->WithHttpVersion("HTTP/1.1").WithStatus(200).setDefaultHeaders().WithHeader("Content-Type", "text/html").WithBody(html.str()).Generate().Send(client_socket);

	return 1;
}