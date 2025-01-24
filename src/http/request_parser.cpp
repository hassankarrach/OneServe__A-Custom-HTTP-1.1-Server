#include "../../includes/http/request_parser.hpp"
#include <vector>

int RequestParser::HardClear()
{
	this->_curr_header_key = "";
	this->_curr_header_value = "";
	this->_method_tmp = "";
	this->_version_tmp = "";
	
	this->_res._result = PARSE::RES_INCOMPLETE;
	this->_res._state = PARSE::STATE_REQUEST_METHOD_START;
	this->_res._method = Method::UNKNOWN;
	this->_res._uri.scheme = "";
	this->_res._uri.host = "";
	this->_res._uri.port = "";
	this->_res._uri.path = "";
	this->_res._uri.query = "";
	this->_res._uri.fragment = "";
	this->_res._version = Version::UNKNOWN;
	this->_res._headers = std::map<std::string, std::string>();
	this->_res._body_type = PARSE::NO_BODY;
	this->_res._body = "";
	this->_res._body_length = 0;
	this->_res._chunked_size = 0;
	this->_res._content_type = "";
	this->_res._boundary = "";
	this->_res._Fields = std::map<std::string, std::string>();
	this->_res._is_multipart = false;
	return (1);
}

RequestParser::RequestParser()
{
	// Tmp holders
	this->_curr_header_key = "";
	this->_curr_header_value = "";
	this->_method_tmp = "";
	this->_version_tmp = "";
	// Res
	// ========================================
	this->_res._result = PARSE::RES_INCOMPLETE;
	this->_res._state = PARSE::STATE_REQUEST_METHOD_START;
	this->_res._method = Method::UNKNOWN;
	this->_res._uri.scheme = "";
	this->_res._uri.host = "";
	this->_res._uri.port = "";
	this->_res._uri.path = "";
	this->_res._uri.query = "";
	this->_res._uri.fragment = "";
	this->_res._version = Version::UNKNOWN;
	this->_res._headers = std::map<std::string, std::string>();
	this->_res._body_type = PARSE::NO_BODY;
	this->_res._body = "";
	this->_res._body_length = 0;
	this->_res._chunked_size = 0;
	this->_res._content_type = "";
	this->_res._boundary = "";
	this->_res._Fields = std::map<std::string, std::string>();
	this->_res._is_multipart = false;
	// ========================================
}

RequestParser::~RequestParser()
{
	// std::cout << "Got a " << this->request_method << " request for " << this->uri << " with HTTP version " << this->http_version << std::endl;
	// std::cout << "Ended in state " << PARSE::toString(this->state) << std::endl;

	// std::cout << "==============================================" << std::endl;
	// std::cout << "The headers are: " << std::endl;
	// for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
	// {
	// 	std::cout << it->first << ": " << it->second << std::endl;
	// }
	// std::cout << "Body_type: " << this->body_type << std::endl;
	// if (this->is_multipart)
	// {
	// 	std::cout << "and its boundary is: " << this->boundary << std::endl;
	// }

	// std::cout << "The body is: " << this->body << std::endl;
}

int RequestParser::Parse(std::string request)
{
	const char *begin = request.c_str();
	const char *end = begin + request.length();
	const char *current = begin;

	std::ofstream tmp_file("tmp", std::ios::binary | std::ios::trunc); // chunked data & multipart body.
	while (current != end)
	{
		switch (this->_res._state)
		{
		case PARSE::STATE_REQUEST_METHOD_START:
		{
			if (!isalpha(*current))
				return (0);
			else
			{
				this->_method_tmp.push_back(*current);
				this->_res._state = PARSE::STATE_REQUEST_METHOD;
			}
			break;
		}
		case PARSE::STATE_REQUEST_METHOD:
		{
			if (isSpace(*current))
			{
				if (this->_method_tmp == "GET" || this->_method_tmp == "POST" || this->_method_tmp == "DELETE")
					this->_res._state = PARSE::STATE_REQUEST_SPACES_BEFORE_URI;
				else
					return (0);
			}
			else if (!isalpha(*current))
				return (0);
			else
				this->_method_tmp.push_back(*current);
			break;
		}
		case PARSE::STATE_REQUEST_SPACES_BEFORE_URI:
		{
			if (*current == '/' || isAlpha(*current))
			{
				this->_res._uri.host.push_back(*current);
				this->_res._state = PARSE::STATE_REQUEST_URI;
			}
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_URI:
		{
			if (isSpace(*current))
				this->_res._state = PARSE::STATE_REQUEST_SPACE_BEFORE_HTTP_VERSION;
			else if (isAlpha(*current) || isDigit(*current) || isUnreserved(*current) || isSubDelim(*current) || *current == '%' || isReservedPath(*current))
				this->_res._uri.host.push_back(*current);
			else if (*current == '?')
			{
				this->_res._state = PARSE::STATE_REQUEST_URI_QUERY_START;
			}
			else if (*current == '#')
				this->_res._state = PARSE::STATE_REQUEST_URI_FRAGMENT_START;
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_URI_QUERY_START:
		{
			if (isAlpha(*current) || isDigit(*current) || isUnreserved(*current) || isSubDelim(*current) || *current == '%' || isReservedPath(*current))
			{
				this->_res._uri.query.push_back(*current);
				this->_res._state = PARSE::STATE_REQUEST_URI_QUERY;
			}
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_URI_QUERY:
		{
			if (isSpace(*current))
				this->_res._state = PARSE::STATE_REQUEST_SPACE_BEFORE_HTTP_VERSION;
			else if (isAlpha(*current) || isDigit(*current) || isUnreserved(*current) || isSubDelim(*current) || *current == '%' || isReservedPath(*current))
				this->_res._uri.query.push_back(*current);
			else if (*current == '#')
				this->_res._state = PARSE::STATE_REQUEST_URI_FRAGMENT_START;
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_URI_FRAGMENT_START:
		{
			if (isAlpha(*current) || isDigit(*current) || isUnreserved(*current) || isSubDelim(*current) || *current == '%' || isReservedPath(*current))
			{
				this->_res._uri.fragment.push_back(*current);
				this->_res._state = PARSE::STATE_REQUEST_URI_FRAGMENT;
			}
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_URI_FRAGMENT:
		{
			if (isSpace(*current))
				this->_res._state = PARSE::STATE_REQUEST_SPACE_BEFORE_HTTP_VERSION;
			else if (isAlpha(*current) || isDigit(*current) || isUnreserved(*current) || isSubDelim(*current) || *current == '%' || isReservedPath(*current))
				this->_res._uri.fragment.push_back(*current);
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_SPACE_BEFORE_HTTP_VERSION:
		{
			if (*current == 'H')
				this->_res._state = PARSE::STATE_REQUEST_VERSION_H;
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_H:
		{
			if (*current == 'T')
				this->_res._state = PARSE::STATE_REQUEST_VERSION_T1;
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_T1:
		{
			if (*current == 'T')
				this->_res._state = PARSE::STATE_REQUEST_VERSION_T2;
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_T2:
		{
			if (*current == 'P')
				this->_res._state = PARSE::STATE_REQUEST_VERSION_P;
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_P:
		{
			if (*current == '/')
				this->_res._state = PARSE::STATE_REQUEST_VERSION_SLASH;
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_SLASH:
		{
			if (*current == '1')
			{
				this->_version_tmp.push_back(*current);
				this->_res._state = PARSE::STATE_REQUEST_VERSION_MAJOR;
			}
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_MAJOR:
		{
			if (*current == '.')
				this->_res._state = PARSE::STATE_REQUEST_VERSION_DOT;
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_DOT:
		{
			if (*current == '0' || *current == '1')
			{
				this->_version_tmp.push_back('.');
				this->_version_tmp.push_back(*current);
				this->_res._state = PARSE::STATE_REQUEST_VERSION_MINOR;
			}
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_MINOR:
		{
			if (*current == '\r')
				this->_res._state = PARSE::STATE_REQUEST_LINE_CR;
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_LINE_CR:
		{
			if (*current == '\n')
				this->_res._state = PARSE::STATE_REQUEST_LINE_LF;
			else
				return (0);
			break;
		}
		case PARSE::STATE_REQUEST_LINE_LF:
		{
			if (*current == '\r')
				this->_res._state = PARSE::STATE_END_OF_HEADERS_CR;
			else if (isAlpha(*current))
			{
				this->_curr_header_key.push_back(*current);
				this->_res._state = PARSE::STATE_HEADER_KEY_START;
			}
			else
				return (0);
			break;
		}
		case PARSE::STATE_HEADER_KEY_START:
		{
			if (*current == ':')
				this->_res._state = PARSE::STATE_HEADER_COLON;
			else if (isTchar(*current))
			{
				this->_curr_header_key.push_back(*current);
				this->_res._state = PARSE::STATE_HEADER_KEY;
			}
			else
				return (0);
			break;
		}
		case PARSE::STATE_HEADER_KEY:
		{
			if (*current == ':')
				this->_res._state = PARSE::STATE_HEADER_COLON;
			else if (isSpace(*current))
				this->_res._state = PARSE::STATE_HEADER_SPACE_BEFORE_VALUE;
			else if (isTchar(*current))
				this->_curr_header_key.push_back(*current);
			else
				return (0);
			break;
		}
		case PARSE::STATE_HEADER_COLON:
		{
			if (isSpace(*current))
				this->_res._state = PARSE::STATE_HEADER_SPACE_BEFORE_VALUE;
			else if (isPrintable(*current))
			{
				this->_curr_header_value.push_back(*current);
				this->_res._state = PARSE::STATE_HEADER_VALUE_START;
			}
			else
				return (0);
			break;
		}
		case PARSE::STATE_HEADER_SPACE_BEFORE_VALUE:
		{
			if (isPrintable(*current))
			{
				this->_curr_header_value.push_back(*current);
				this->_res._state = PARSE::STATE_HEADER_VALUE;
			}
			else
				return (0);
			break;
		}
		case PARSE::STATE_HEADER_VALUE_START:
		{
			if (isPrintable(*current))
			{
				this->_curr_header_value.push_back(*current);
				this->_res._state = PARSE::STATE_HEADER_VALUE;
			}
			else
				return (0);
			break;
		}
		case PARSE::STATE_HEADER_VALUE:
		{
			if (*current == '\r')
				this->_res._state = PARSE::STATE_HEADER_LINE_CR;
			else if (isPrintable(*current))
				this->_curr_header_value.push_back(*current);
			else
				return (0);
			break;
		}
		case PARSE::STATE_HEADER_LINE_CR:
		{
			if (*current == '\n')
				this->_res._state = PARSE::STATE_HEADER_LINE_LF;
			else
				return (0);
			break;
		}
		case PARSE::STATE_HEADER_LINE_LF:
		{
			if (isAlpha(*current))
				this->_res._state = PARSE::STATE_HEADER_KEY_START;
			else if (*current == '\r')
				this->_res._state = PARSE::STATE_END_OF_HEADERS_CR;
			else
				return (0);

			// Converte Key to lowercase for case insensitive comparison
			this->_curr_header_key = to_lowercase(this->_curr_header_key);

			if (this->_curr_header_key == "transfer-encoding" && this->_curr_header_value == "chunked")
				this->_res._body_type = PARSE::CHUNKED;
			else if (this->_curr_header_key == "content-length")
			{
				this->_res._body_type = PARSE::CONTENT_LENGTH;
				this->_res._body_length = stringToUnsignedLong(this->_curr_header_value);
			}
			else if (this->_curr_header_key == "content-type") // check boundary
			{
				if (this->_curr_header_value.find("multipart/form-data") != std::string::npos)
				{
					if (this->_curr_header_value.find("boundary=") != std::string::npos)
					{
						this->_res._boundary = this->_curr_header_value.substr(this->_curr_header_value.find("boundary=") + 9);
						this->_res._is_multipart = true;
					}
					else
						return (0);
				}
			}
			this->_res._headers[this->_curr_header_key] = this->_curr_header_value;
			this->_curr_header_key.clear();
			this->_curr_header_value.clear();
			this->_curr_header_key.push_back(*current);
			break;
		}
		case PARSE::STATE_END_OF_HEADERS_CR:
		{
			if (this->_res._headers.find("transfer-encoding") != this->_res._headers.end() && this->_res._headers.find("content-length") != this->_res._headers.end()) // only one of them should be there.
				return (0);

			if (this->_res._headers.find("host") == this->_res._headers.end())
				return (0);
			if (*current == '\n')
				this->_res._state = PARSE::STATE_END_OF_HEADERS_LF;
			else
				return (0);

			if (!*(current + 1))
				return (1);
			break;
		}
		case PARSE::STATE_END_OF_HEADERS_LF:
		{
			if (this->_res._body_type == PARSE::CONTENT_LENGTH && !this->_res._body_length) // No body
				return (0);

			if (this->_res._body_type == PARSE::CONTENT_LENGTH && this->_method_tmp != "GET")
			{
				this->_res._body.push_back(*current);
				this->_res._state = PARSE::STATE_BODY_CONTENT_LENGTH;
			}
			else if (this->_res._body_type == PARSE::CHUNKED && this->_method_tmp != "GET")
			{
				if (isHex(*current))
				{
					this->_res._chunked_size = fromHex(*current);
					this->_res._state = PARSE::STATE_BODY_CHUNKED_SIZE;
				}
				else
					return (0);
			}
			else
				return (1);
			break;
		}
		case PARSE::STATE_BODY_CONTENT_LENGTH:
		{
			if (this->_res._body.length() < this->_res._body_length)
				this->_res._body.push_back(*current);
			else
				return (0);

			if (this->_res._body.length() == this->_res._body_length)
				return (1);
			break;
		}
		case PARSE::STATE_BODY_CHUNKED_SIZE:
		{
			if (*current == '\r')
				this->_res._state = PARSE::STATE_BODY_CHUNKED_SIZE_CR;
			else if (isHex(*current))
			{
				this->_res._chunked_size = this->_res._chunked_size * 16 + fromHex(*current);
			}
			else
				return (0);
			break;
		}
		case PARSE::STATE_BODY_CHUNKED_SIZE_CR:
		{
			if (*current == '\n')
				this->_res._state = PARSE::STATE_BODY_CHUNKED_SIZE_LF;
			else
				return (0);
			break;
		}
		case PARSE::STATE_BODY_CHUNKED_SIZE_LF:
		{
			if (this->_res._chunked_size)
			{
				this->_res._state = PARSE::STATE_BODY_CHUNKED_DATA;
				this->_res._body.push_back(*current);
				this->_res._chunked_size--;
			}
			else
				this->_res._state = PARSE::STATE_END_OF_CHUNKED_CR;
			break;
		}
		case PARSE::STATE_BODY_CHUNKED_DATA:
		{
			if (this->_res._chunked_size)
			{
				this->_res._body.push_back(*current);
				this->_res._chunked_size--;
			}
			else if (*current == '\r')
				this->_res._state = PARSE::STATE_BODY_CHUNKED_DATA_CR;
			else
				return (0);
			break;
		}
		case PARSE::STATE_BODY_CHUNKED_DATA_CR:
		{
			if (*current == '\n')
				this->_res._state = PARSE::STATE_BODY_CHUNKED_DATA_LF;
			else
				return (0);
			break;
		}
		case PARSE::STATE_BODY_CHUNKED_DATA_LF:
		{
			if (isHex(*current))
			{
				this->_res._chunked_size = fromHex(*current);
				this->_res._state = PARSE::STATE_BODY_CHUNKED_SIZE;
			}
			else if (*current == '\r')
				this->_res._state = PARSE::STATE_END_OF_CHUNKED_CR;
			else
				return (0);
			break;
		}
		case PARSE::STATE_END_OF_CHUNKED_CR:
		{
			if (*current == '\n')
			{
				this->_res._state = PARSE::STATE_END_OF_CHUNKED_LF;
				if (!*(current + 1))
					return (1);
			}
			else
				return (0);
			break;
		}
		case PARSE::STATE_END_OF_CHUNKED_LF:
		{
			// it reachs here, because there is a *curr. last char was \n so it should considered as the last chunked data.
			return (0); // Weird, but yeah. any chars after end of chunks are considered errors.
		}
		default:
			break;
		}
		current++;
	}

	tmp_file.close();
	return (1);
}

int RequestParser::ParseMultiPartFormData()
{
	// Field/Value pairs
	std::string field_name;
	std::string field_value;
	std::ofstream file;
	this->_res._boundary = "------------------------74678048f707cd13";

	// Open tmp file
	std::ifstream tmp_file("tmp", std::ios::binary);
	if (!tmp_file.is_open())
	{
		std::cerr << "Error: Could not open tmp file." << std::endl;
		return (0);
	}

	PARSE::MultiPartFormDataState state = PARSE::STATE_BOUNDARY; // Init.

	// read line by line
	std::string line;
	while (std::getline(tmp_file, line))
	{
		if (line[line.length() - 1] != '\r' && state != PARSE::STATE_CONTENT_FILE_DATA) // exept file data, all lines should end with \r\n
			return (0);

		line += "\n";

		switch (state)
		{
		case PARSE::STATE_BOUNDARY:
		{
			std::string boundary = "--" + this->_res._boundary + "\r\n";

			if (line == boundary)
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
			file.open(field_value.c_str(), std::ios::binary);

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

			this->_res._Fields[field_name] = field_value;

			state = PARSE::STATE_BOUNDARY;
			break;
		}
		case PARSE::STATE_CONTENT_TYPE:
		{
			if (line.find("Content-Type:") != std::string::npos)
			{
				this->_res._Fields[field_name] = field_value;
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
			if (line == "--" + this->_res._boundary + "--\r\n")
			{
				state = PARSE::STATE_BOUNDARY_END;
				break;
			}
			else
			{
				file << line;
				break;
			}
			break;
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
	return (1);
}

HttpRequestData RequestParser::getResult()
{
	if (this->_method_tmp == "GET")
		this->_res._method = Method::GET;
	else if (this->_method_tmp == "POST")
		this->_res._method = Method::POST;
	else
		this->_res._method = Method::DELETE;

	if (this->_version_tmp == "1.0")
		this->_res._version = Version::HTTP_1_0;
	else
		this->_res._version = Version::HTTP_1_1;

	// Refactor URI HERE

	return (this->_res);
}