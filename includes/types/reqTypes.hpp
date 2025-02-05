#ifndef REQ_TYPES_HPP
#define REQ_TYPES_HPP

#include "httpTypes.hpp"
#include "../../includes/config/Parse_Config.hpp"

#include <string>

struct Uri
{
	std::string scheme;
	std::string host;
	std::string port;
	std::string path;
	std::string query;
	std::string fragment;
};

namespace PARSE
{
	enum result
	{
		RES_OK,
		RES_INCOMPLETE,
		RES_NEED_MORE,
		RES_ERROR,
	};

	enum body_type
	{
		CHUNKED,
		CONTENT_LENGTH,
		NO_BODY,
	};

	enum MultiPartFormDataState
	{
		STATE_BOUNDARY,
		STATE_CONTENT_DISPOSITION,
		STATE_CONTENT_FIELD_NAME,
		STATE_CONTENT_EMPTY_LINE,
		STATE_CONTENT_FIELD_VALUE,
		STATE_CONTENT_FILE_NAME,
		STATE_CONTENT_TYPE,
		STATE_CONTENT_EMPTY_LINE_AFTER_TYPE,
		STATE_CONTENT_FILE_DATA,
		STATE_EMPTY_LINE_BEFORE_BOUNDARY_END,
		STATE_BOUNDARY_END,
	};

	enum state
	{
		STATE_REQUEST_METHOD_START,
		STATE_REQUEST_METHOD,
		STATE_REQUEST_SPACES_BEFORE_URI,
		STATE_REQUEST_URI,
		STATE_REQUEST_URI_QUERY_START,
		STATE_REQUEST_URI_QUERY,
		STATE_REQUEST_URI_FRAGMENT_START,
		STATE_REQUEST_URI_FRAGMENT,
		STATE_REQUEST_SPACE_BEFORE_HTTP_VERSION,
		STATE_REQUEST_VERSION_H,
		STATE_REQUEST_VERSION_T1,
		STATE_REQUEST_VERSION_T2,
		STATE_REQUEST_VERSION_P,
		STATE_REQUEST_VERSION_SLASH,
		STATE_REQUEST_VERSION_MAJOR,
		STATE_REQUEST_VERSION_DOT,
		STATE_REQUEST_VERSION_MINOR,
		STATE_REQUEST_LINE_CR,
		STATE_REQUEST_LINE_LF,
		STATE_HEADER_KEY_START,
		STATE_HEADER_KEY,
		STATE_HEADER_COLON,
		STATE_HEADER_SPACE_BEFORE_VALUE,
		STATE_HEADER_VALUE_START,
		STATE_HEADER_VALUE,
		STATE_HEADER_LINE_CR,
		STATE_HEADER_LINE_LF,
		STATE_END_OF_HEADERS_CR,
		STATE_END_OF_HEADERS_LF,
		STATE_BODY_CONTENT_LENGTH,
		STATE_BODY_CHUNKED_SIZE,
		STATE_BODY_CHUNKED_SIZE_CR,
		STATE_BODY_CHUNKED_SIZE_LF,
		STATE_BODY_CHUNKED_DATA,
		STATE_BODY_CHUNKED_DATA_CR,
		STATE_BODY_CHUNKED_DATA_LF,
		STATE_END_OF_CHUNKED_CR,
		STATE_END_OF_CHUNKED_LF,
	};

	std::string toString(PARSE::state state);
	std::string toString(PARSE::result result);
};

struct HttpRequestData
{
	// State
	PARSE::result						_result;
	PARSE::state						_state;

	std::string							_Error_msg;

	// Request Method
	Method::Type						_method;
	// Uri
	Uri									_uri;
	// Http Version
	Version::Type						_version;
	// Headers
	std::map<std::string, std::string>	_headers;

	// Body
	PARSE::body_type					_body_type;
	std::string							_body;
	unsigned long						_body_length;
	// Chunked
	unsigned long						_chunked_size;
	// Content type
	std::string							_content_type;

	// Fields (MultiPart reqs)
	std::string							_boundary;
	std::map<std::string, std::string>	_Fields;

	///
	ConfigLoader								_config_res;
	t_map								_location_res;

	// Flags
	int									_is_multipart;
	std::string							_tmp_file_name;
};

// ConfigFileResutl res;

#endif
