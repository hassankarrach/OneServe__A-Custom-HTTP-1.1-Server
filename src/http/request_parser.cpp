#include "../../includes/http/request_parser.hpp"

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
	this->_res._tmp_file_name = "";
	this->_res._Error_msg = "";
	// ========================================
}

RequestParser::~RequestParser()
{
	if (_tmp_file.is_open())
		_tmp_file.close();
}

int RequestParser::Parse(std::string request)
{
	const char *begin = request.c_str();
	const char *end = begin + request.length();
	const char *current = begin;


	while (current != end)
	{
		switch (this->_res._state)
		{
		case PARSE::STATE_REQUEST_METHOD_START:
		{
			if (!isalpha(*current))
				return (this->_res._Error_msg = "Invalid Character in Method", -1);
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
					return (this->_res._Error_msg = "Invalid Method", -1);
			}
			else if (!isalpha(*current))
				return (this->_res._Error_msg = "Invalid Character in Method", -1);
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
				return (this->_res._Error_msg = "Invalid Character in URI", -1);
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
				return (this->_res._Error_msg = "Invalid Character in URI", -1);
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
				return (this->_res._Error_msg = "Invalid Character in URI [QUERY]", -1);
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
				return (this->_res._Error_msg = "Invalid Character in URI [QUERY]", -1);
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
				return (this->_res._Error_msg = "Invalid Character in URI [FRAGMENT]", -1);
			break;
		}
		case PARSE::STATE_REQUEST_URI_FRAGMENT:
		{
			if (isSpace(*current))
				this->_res._state = PARSE::STATE_REQUEST_SPACE_BEFORE_HTTP_VERSION;
			else if (isAlpha(*current) || isDigit(*current) || isUnreserved(*current) || isSubDelim(*current) || *current == '%' || isReservedPath(*current))
				this->_res._uri.fragment.push_back(*current);
			else
				return (this->_res._Error_msg = "Invalid Character in URI [FRAGMENT]", -1);
			break;
		}
		case PARSE::STATE_REQUEST_SPACE_BEFORE_HTTP_VERSION:
		{
			if (*current == 'H')
				this->_res._state = PARSE::STATE_REQUEST_VERSION_H;
			else
				return (this->_res._Error_msg = "Invalid Character in HTTP Version", -1);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_H:
		{
			if (*current == 'T')
				this->_res._state = PARSE::STATE_REQUEST_VERSION_T1;
			else
				return (this->_res._Error_msg = "Invalid Character in HTTP Version", -1);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_T1:
		{
			if (*current == 'T')
				this->_res._state = PARSE::STATE_REQUEST_VERSION_T2;
			else
				return (this->_res._Error_msg = "Invalid Character in HTTP Version", -1);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_T2:
		{
			if (*current == 'P')
				this->_res._state = PARSE::STATE_REQUEST_VERSION_P;
			else
				return (this->_res._Error_msg = "Invalid Character in HTTP Version", -1);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_P:
		{
			if (*current == '/')
				this->_res._state = PARSE::STATE_REQUEST_VERSION_SLASH;
			else
				return (this->_res._Error_msg = "Invalid Character in HTTP Version", -1);
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
				return (this->_res._Error_msg = "Invalid Character in HTTP Version", -1);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_MAJOR:
		{
			if (*current == '.')
				this->_res._state = PARSE::STATE_REQUEST_VERSION_DOT;
			else
				return (this->_res._Error_msg = "Invalid Character in HTTP Version", -1);
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
				return (this->_res._Error_msg = "Invalid Character in HTTP Version", -1);
			break;
		}
		case PARSE::STATE_REQUEST_VERSION_MINOR:
		{
			if (*current == '\r')
				this->_res._state = PARSE::STATE_REQUEST_LINE_CR;
			else
				return (this->_res._Error_msg = "Invalid end of line [request line]", -1);
			break;
		}
		case PARSE::STATE_REQUEST_LINE_CR:
		{
			if (*current == '\n')
				this->_res._state = PARSE::STATE_REQUEST_LINE_LF;
			else
				return (this->_res._Error_msg = "Invalid end of line [request line]", -1);
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
				return (this->_res._Error_msg = "Invalid Character in Header", -1);
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
				return (this->_res._Error_msg = "Invalid Character in Header", -1);
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
				return (this->_res._Error_msg = "Invalid Character in Header", -1);
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
				return (this->_res._Error_msg = "Invalid Character in Header", -1);
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
				return (this->_res._Error_msg = "Invalid Character in Header", -1);
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
				return (this->_res._Error_msg = "Invalid Character in Header", -1);
			break;
		}
		case PARSE::STATE_HEADER_VALUE:
		{
			if (*current == '\r')
				this->_res._state = PARSE::STATE_HEADER_LINE_CR;
			else if (isPrintable(*current))
				this->_curr_header_value.push_back(*current);
			else
				return (this->_res._Error_msg = "Invalid Character in Header", -1);
			break;
		}
		case PARSE::STATE_HEADER_LINE_CR:
		{
			if (*current == '\n')
				this->_res._state = PARSE::STATE_HEADER_LINE_LF;
			else
				return (this->_res._Error_msg = "Invalid end of line [header line]", -1);
			break;
		}
		case PARSE::STATE_HEADER_LINE_LF:
		{
			if (isAlpha(*current))
				this->_res._state = PARSE::STATE_HEADER_KEY_START;
			else if (*current == '\r')
				this->_res._state = PARSE::STATE_END_OF_HEADERS_CR;
			else
				return (this->_res._Error_msg = "Invalid end of line [header line]", -1);
			// Converte Key to lowercase for case insensitive comparison
			this->_curr_header_key = to_lowercase(this->_curr_header_key);

			this->_res._headers[this->_curr_header_key] = this->_curr_header_value;
			this->_curr_header_key.clear();
			this->_curr_header_value.clear();
			this->_curr_header_key.push_back(*current);
			break;
		}
		case PARSE::STATE_END_OF_HEADERS_CR:
		{
			// Syntax Check
			if (*current == '\n')
				this->_res._state = PARSE::STATE_END_OF_HEADERS_LF;
			else
				return (this->_res._Error_msg = "Invalid end of line [headers]", -1);

			// Gen Checks
			if (this->_res._headers.find("transfer-encoding") != this->_res._headers.end() && this->_res._headers.find("content-length") != this->_res._headers.end()) // only one of them should be there.
				return (this->_res._Error_msg = "Invalid Headers : Both Content-Length & Transfer-Encoding are present", -1);
			if (this->_res._headers.find("host") == this->_res._headers.end())
				return (this->_res._Error_msg = "Invalid Headers : Host is missing", -1);

			// [Content-Length] or [Transfer-Encoding]
			if (this->_res._headers.find("content-length") != this->_res._headers.end())
			{
				this->_res._body_type = PARSE::CONTENT_LENGTH;

				this->_res._body_length = stringToUnsignedLong(this->_res._headers["content-length"]);

				if (this->_res._body_length <= 0 || this->_res._body_length > 10000000) // 10MB
					return (this->_res._Error_msg = "Invalid Content-Length", -1);
			}
			else if (this->_res._headers.find("transfer-encoding") != this->_res._headers.end())
			{
				if (this->_res._headers["transfer-encoding"] == "chunked")
					this->_res._body_type = PARSE::CHUNKED;
				else
					return (this->_res._Error_msg = "Invalid Transfer-Encoding", -1);
			}
			else
				this->_res._body_type = PARSE::NO_BODY;
			// is [multipart/form-data]
			if (this->_res._headers.find("content-type") != this->_res._headers.end())
			{
				if (this->_res._headers["content-type"].find("multipart/form-data") != std::string::npos)
				{
					if (this->_res._headers["content-type"].find("boundary=") != std::string::npos)
					{
						this->_res._boundary = _res._headers["content-type"].substr(this->_res._headers["content-type"].find("boundary=") + 9);
						this->_res._is_multipart = true;
					}
					else
						return (this->_res._Error_msg = "Invalid Content-Type : Missing Boundary", -1);
				}
			}

			// Spef Checks
			if (this->_method_tmp == "GET")
			{
				// Ignore Content-Length if present
				// should i return 1, what if the body is too large ?
				// TWO SOLUTIONS HERE, 1 => Ignore & Drop Connection, 2 => Drain the Socket.
				// TODO =====> I'll go with ignoring the body as NGINX does, and drop the connection right after. (ADD NEW FLAG in _res that decided weither the connection should be closed after the resp)

				return (1);
			}
			else if (this->_method_tmp == "POST")
			{
				// Open tmp file to store the body
				this->_res._tmp_file_name = "www/tmp/" + generateUniqueFileName();
				_tmp_file.open(this->_res._tmp_file_name.c_str(), std::ios::binary | std::ios::trunc);

				if (!_tmp_file.is_open())
					return (this->_res._Error_msg = "Failed to open tmp file", -1);
			}
			else if (this->_method_tmp == "DELETE")
			{
				// Ignore Content-Length if present
				// TODO : Authorization HEADER, Because, why not ?

				return (1);
			}

			break;
		}
		case PARSE::STATE_END_OF_HEADERS_LF:
		{
			// TODO : IF i reach here, it means there is a body should be processed. i open the tmp file anyway to store either one read data, or chunked.
			// TODO :  =============== I WILL MAKE SURE IT ONLY REACHS HERE, IN CASE OF POST REQS. ==================

			// TODO : I MAY ADD A CHECK LATER IF I ALREADY WRITE THE FULL BODY IN THE FILE JUST HERE.
			if (this->_res._body_type == PARSE::CONTENT_LENGTH)
			{
				size_t BytesToWrite = std::min(static_cast<size_t>(end - current), this->_res._body_length);
				_tmp_file.write(current, BytesToWrite);
				this->_res._body_length -= BytesToWrite;
				current += BytesToWrite;
				this->_res._state = PARSE::STATE_BODY_CONTENT_LENGTH;
				if (this->_res._body_length == 0)
					return (_tmp_file.close(), 1);
				continue; // Confusing, but here im avoiding the current++ at the end of the loop, i already incremented it here.
			}
			else if (this->_res._body_type == PARSE::CHUNKED)
			{
				if (isHex(*current))
				{
					this->_res._chunked_size = fromHex(*current);
					this->_res._state = PARSE::STATE_BODY_CHUNKED_SIZE;
				}
				else
					return (this->_res._Error_msg = "Invalid Chunked Size", -1);
			}
			else // NO NEEDS, I GUESS. //todo
				return (1);
			break;
		}
		case PARSE::STATE_BODY_CONTENT_LENGTH:
		{
			size_t BytesToWrite = std::min(static_cast<size_t>(end - current), this->_res._body_length);

			_tmp_file.write(current, BytesToWrite);
			this->_res._body_length -= BytesToWrite;
			current += BytesToWrite;
			if (this->_res._body_length == 0)
				return (_tmp_file.close(), 1);
			continue;
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
				return (this->_res._Error_msg = "Invalid Chunked Size", -1);
			break;
		}
		case PARSE::STATE_BODY_CHUNKED_SIZE_CR:
		{
			if (*current == '\n')
				this->_res._state = PARSE::STATE_BODY_CHUNKED_SIZE_LF;
			else
				return (this->_res._Error_msg = "Invalid Chunked Size [CHUNKED_SIZE Line]", -1);
			break;
		}
		case PARSE::STATE_BODY_CHUNKED_SIZE_LF:
		{
			if (this->_res._chunked_size)
			{
				this->_res._state = PARSE::STATE_BODY_CHUNKED_DATA;
				_tmp_file << *current;
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
				_tmp_file << *current;
				this->_res._chunked_size--;
			}
			else if (*current == '\r')
				this->_res._state = PARSE::STATE_BODY_CHUNKED_DATA_CR;
			else
				return (this->_res._Error_msg = "Invalid Chunked Data", -1);
			break;
		}
		case PARSE::STATE_BODY_CHUNKED_DATA_CR:
		{
			if (*current == '\n')
				this->_res._state = PARSE::STATE_BODY_CHUNKED_DATA_LF;
			else
				return (this->_res._Error_msg = "Invalid end of chunked data", -1);
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
				return (this->_res._Error_msg = "Invalid Chunked Size", -1);
			break;
		}
		case PARSE::STATE_END_OF_CHUNKED_CR:
		{
			if (*current == '\n')
			{
				this->_res._state = PARSE::STATE_END_OF_CHUNKED_LF;
				if (!*(current + 1))
					return (_tmp_file.close(), 1); // Close tmp file, after chunked parsing is done.
			}
			else
				return (this->_res._Error_msg = "Invalid end of chunked data", -1);
			break;
		}
		case PARSE::STATE_END_OF_CHUNKED_LF:
		{
			// it reachs here, because there is a *curr. last char was \n so it should considered as the last chunked data.
			return (-1); // Weird, but yeah. any chars after end of chunks are considered errors.
		}
		default:
			break;
		}
		current++;
	}
	return (0);
}

std::string RequestParser::generateUniqueFileName()
{
	std::ostringstream oss;
	std::time_t now = std::time(NULL);
	struct tm *timeinfo = std::localtime(&now);

	// Buffer to hold the formatted time
	char buffer[20];
	std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);

	// Combine the formatted time with a random number
	oss << "upload_" << buffer << "_" << std::rand() % 10000;
	return oss.str();
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
	return this->_res;
}
