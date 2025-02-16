#include "../../includes/http/response.hpp"

void Response::Http413(int client_socket)
{
	Response res;

	res.WithHttpVersion("HTTP/1.1")
		.WithStatus(413)
		.setDefaultHeaders()
		.WithHeader("Content-Type", "text/plain")
		.WithHeader("Connection", "close")
		.WithBody("413 Payload Too Large")
		.Generate()
		.Send(client_socket);
}

void Response::Http201(int client_socket)
{
	// Create the response body
	std::string body = "{\"status\": \"created\", \"filename\": \"filename.txt\"}";

	std::string content_length = NumberToString(body.size());

	// Build the response
	Response res;
	res.WithHttpVersion("HTTP/1.1")
		.WithStatus(201)
		.setDefaultHeaders()
		.WithHeader("Content-Type", "application/json")
		.WithBody(body)
		.Generate()
		.Send(client_socket);
}

static std::string FieldsMapStringify(std::map<std::string, std::string> &Fields)
{
	std::string data;
	data += "{";
	for (std::map<std::string, std::string>::iterator it = Fields.begin(); it != Fields.end(); it++)
	{
		data += "\"" + it->first + "\": \"" + it->second + "\"";
		// Add comma if not last element
		if (it != --Fields.end()) // todo -___-
			data += ", ";
	}
	data += "}";
	return data;
}

int Response::ParseMultiPartFormData(HttpRequestData &req, int client_socket)
{
	// int ChunkSize = 16384;
	std::string field_name;
	std::string field_value;
	std::ofstream curr_file;

	std::string Upload_dir = req._location_res["upload"];
	std::string Root = req._location_res["root"];
	Root += (Root[Root.length() - 1] == '/' ? "" : "/");
	Upload_dir += (Upload_dir[Upload_dir.length() - 1] == '/' ? "" : "/");

	std::string fullDir = Root + Upload_dir;

	// if upload dirr is exist
	if (!isDirectory(fullDir))
		return (req._Error_msg = "Unacessible upload folder", -1);

	// Open tmp file
	std::ifstream MultiPartData((req._tmp_file_name).c_str(), std::ios::binary);
	if (!MultiPartData.is_open())
		return (req._Error_msg = "Could not open tmp file", -1);

	PARSE::MultiPartFormDataState state = PARSE::STATE_BOUNDARY; // Init.

	// read line by line
	std::string line;
	while (std::getline(MultiPartData, line))
	{
		if (line[line.length() - 1] != '\r' && state != PARSE::STATE_CONTENT_FILE_DATA) // exept file data, all lines should end with \r\n
			return (req._Error_msg = "Invalid Line Ending", 0);

		line += "\n";
		switch (state)
		{
		case PARSE::STATE_BOUNDARY:
		{
			if (line == "--" + req._boundary + "\r\n") // fix this.
			{
				state = PARSE::STATE_CONTENT_DISPOSITION;
				break;
			}
			else
				return (req._Error_msg = "Invalid Boundary", 0);
		}
		case PARSE::STATE_CONTENT_DISPOSITION:
		{
			if (line.find("Content-Disposition: form-data;") == std::string::npos)
				return (req._Error_msg = "Invalid Content-Disposition", 0);

			if (line.find("name=") != std::string::npos)
				state = PARSE::STATE_CONTENT_FIELD_NAME;
			else
				return (req._Error_msg = "Invalid Field Name", 0);
		}
		/* fall through */
		case PARSE::STATE_CONTENT_FIELD_NAME:
		{
			field_name = line.substr(line.find("name=") + 6);
			field_name = field_name.substr(0, field_name.find("\""));

			if (line.find("filename=") != std::string::npos)
				state = PARSE::STATE_CONTENT_FILE_NAME;
			else
			{
				state = PARSE::STATE_CONTENT_EMPTY_LINE;
				break;
			}
		}
		/* fall through */
		case PARSE::STATE_CONTENT_FILE_NAME: // Field value is a file
		{
			field_value = line.substr(line.find("filename=") + 10);
			field_value = field_value.substr(0, field_value.find("\""));

			// Check if file name is empty
			if (field_value.empty())
				return (req._Error_msg = "Invalid File Name", 0);

			// If file exists
			std::ifstream curr_file_check((fullDir + field_value).c_str(), std::ios::in);
			if (curr_file_check.is_open())
				return (req._Error_msg = "File already exists", 0);

			// Open file
			curr_file.open((fullDir + field_value).c_str(), std::ios::binary);
			if (!curr_file.is_open())
				return (req._Error_msg = "Could not open file", -1);
			state = PARSE::STATE_CONTENT_TYPE;
			break;
		}
		case PARSE::STATE_CONTENT_EMPTY_LINE:
		{
			if (line == "\r\n")
			{
				state = PARSE::STATE_CONTENT_FIELD_VALUE;
				break;
			}
			else
				return (req._Error_msg = "Invalid Empty Line", 0);
		}
		case PARSE::STATE_CONTENT_FIELD_VALUE:
		{
			field_value = line.substr(0, line.length() - 2); // remove \r\n

			req._Fields[field_name] = field_value;
			state = PARSE::STATE_BOUNDARY;
			break;
		}
		case PARSE::STATE_CONTENT_TYPE:
		{
			if (line.find("Content-Type:") != std::string::npos)
			{
				req._Fields[field_name] = field_value;
				state = PARSE::STATE_CONTENT_EMPTY_LINE_AFTER_TYPE;
				break;
			}
			else
				return (req._Error_msg = "Invalid Content-Type", 0);
		}
		case PARSE::STATE_CONTENT_EMPTY_LINE_AFTER_TYPE:
		{
			if (line == "\r\n")
			{
				state = PARSE::STATE_CONTENT_FILE_DATA;
				break;
			}
			else
				return (req._Error_msg = "Invalid Empty Line After Type", 0);
		}
		case PARSE::STATE_CONTENT_FILE_DATA:
		{
			if (line == "--" + req._boundary + "--\r\n")
			{
				curr_file.close();
				state = PARSE::STATE_BOUNDARY_END;
				break;
			}
			else if (line == "--" + req._boundary + "\r\n")
			{
				curr_file.close();
				state = PARSE::STATE_CONTENT_DISPOSITION;
				break;
			}
			else
			{
				curr_file << line; // remove \r\n
				break;
			}
		}
		case PARSE::STATE_EMPTY_LINE_BEFORE_BOUNDARY_END:
		{
			if (line == "\r\n")
			{
				state = PARSE::STATE_BOUNDARY_END;
				break;
			}
			else
				return (req._Error_msg = "Invalid Empty Line Before Boundary End", 0);
		}
		case PARSE::STATE_BOUNDARY_END:
		{
			if (line == "--\r\n")
			{
				state = PARSE::STATE_BOUNDARY;
				break;
			}
			else
				return (req._Error_msg = "Invalid Boundary End", 0);
		}
		default:
			break;
		}
	}

	std::string data = FieldsMapStringify(req._Fields);
	Response res;
	res.WithHttpVersion("HTTP/1.1")
		.WithStatus(201)
		.setDefaultHeaders()
		.WithHeader("Content-Type", "application/json")
		.WithBody("{\"data\": " + data + "}")
		.Generate()
		.Send(client_socket);
	return (1);
}

int Response::Post(int client_socket, HttpRequestData &req)
{
	std::string Root = req._location_res["root"];
	std::string resolvedPath = Root + req._uri.host; // No index file concat yet.

	// ***CGI***
	std::string ext = (resolvedPath.find_last_of('.') != std::string::npos) ? resolvedPath.substr(resolvedPath.find_last_of('.'))
																			: ""; // [ ".php" || ""]
	if (req._location_res.find("cgi") && !req._location_res[ext.data()].empty())
		return (ServeCGI(client_socket, ext, req));
	// *********

	if (!req._location_res.check("POST"))
	{
		MethodNotAllowed(client_socket, req);
		return (1);
	}

	int res = ParseMultiPartFormData(req, client_socket);

	if (res == 0)
		BadRequest(client_socket, req);
	else if (res < 0)
		InternalServerError(client_socket, req);
	// remove(req._tmp_file_name.c_str());
	return (1);
}
