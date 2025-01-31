#include "../../includes/http/response.hpp"

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

int Response::ParseMultiPartFormData(HttpRequestData &req)
{
	// TODO : Handle File close.
	// Field/Value pairs
	std::string							field_name;
	std::string							field_value;
	std::ofstream						curr_file;

	std::map<std::string, std::string>	Fields;

	std::string							UPLOAD_DIR = "./www/uploads/";
	// std::string							TMP_DIR = "./www/tmp/";

	// Open tmp file
	std::ifstream MultiPartData((req._tmp_file_name).c_str(), std::ios::binary);
	if (!MultiPartData.is_open())
	{
		std::cout << "Error: Could not open tmp file." << std::endl;
		return (0);
	}

	PARSE::MultiPartFormDataState state = PARSE::STATE_BOUNDARY; // Init.

	// read line by line
	std::string line;
	while (std::getline(MultiPartData, line))
	{
		if (line[line.length() - 1] != '\r' && state != PARSE::STATE_CONTENT_FILE_DATA) // exept file data, all lines should end with \r\n
			return (0);		

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
				return (0);
		}
		case PARSE::STATE_CONTENT_DISPOSITION:
		{
			if (line.find("Content-Disposition: form-data;") == std::string::npos)
				return (0);

			if (line.find("name=") != std::string::npos)
				state = PARSE::STATE_CONTENT_FIELD_NAME;
			else
				return (0);
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

			// Open file
			curr_file.open((UPLOAD_DIR + field_value).c_str(), std::ios::binary);
			if (!curr_file.is_open())
				return (0);
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
				return (0);
		}
		case PARSE::STATE_CONTENT_FIELD_VALUE:
		{
			field_value = line.substr(0, line.length() - 2); // remove \r\n

			Fields[field_name] = field_value;
			state = PARSE::STATE_BOUNDARY;
			break;
		}
		case PARSE::STATE_CONTENT_TYPE:
		{
			if (line.find("Content-Type:") != std::string::npos)
			{
				Fields[field_name] = field_value;
				state = PARSE::STATE_CONTENT_EMPTY_LINE_AFTER_TYPE;
				break;
			}
			else
				return (0);
		}
		case PARSE::STATE_CONTENT_EMPTY_LINE_AFTER_TYPE:
		{
			if (line == "\r\n")
			{
				state = PARSE::STATE_CONTENT_FILE_DATA;
				break;
			}
			else
				return (0);
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
			std::cout << line << std::endl;
			if (line == "\r\n")
			{
				state = PARSE::STATE_BOUNDARY_END;
				break;
			}
			else
				return (0);
		}
		case PARSE::STATE_BOUNDARY_END:
		{
			if (line == "--\r\n")
			{
				state = PARSE::STATE_BOUNDARY;
				break;
			}
			else
				return (0);
		}
		default:
			break;
		}
	}

	// Close file
	MultiPartData.close();
	remove(req._tmp_file_name.c_str());
	return (1);
}

int Response::Post(int client_socket, HttpRequestData &req)
{
	if (ParseMultiPartFormData(req))
	{
		Http201(client_socket);
		return (1);
	}
	else
	{
		InternalServerError("www/html/errors/500.html"); // For now.
		return (0);
	}
	return (1);
}
