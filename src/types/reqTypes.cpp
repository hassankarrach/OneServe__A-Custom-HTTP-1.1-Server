#include "../../includes/types/reqTypes.hpp"

std::string PARSE::toString(PARSE::state state)
{
	switch (state)
	{
	case PARSE::STATE_REQUEST_METHOD_START:
		return "STATE_REQUEST_METHOD_START";
	case PARSE::STATE_REQUEST_METHOD:
		return "STATE_REQUEST_METHOD";
	case PARSE::STATE_REQUEST_SPACES_BEFORE_URI:
		return "STATE_REQUEST_SPACES_BEFORE_URI";
	case PARSE::STATE_REQUEST_URI:
		return "STATE_REQUEST_URI";
	case PARSE::STATE_REQUEST_URI_QUERY_START:
		return "STATE_REQUEST_URI_QUERY_START";
	case PARSE::STATE_REQUEST_URI_QUERY:
		return "STATE_REQUEST_URI_QUERY";
	case PARSE::STATE_REQUEST_URI_FRAGMENT_START:
		return "STATE_REQUEST_URI_FRAGMENT_START";
	case PARSE::STATE_REQUEST_URI_FRAGMENT:
		return "STATE_REQUEST_URI_FRAGMENT";
	case PARSE::STATE_REQUEST_SPACE_BEFORE_HTTP_VERSION:
		return "STATE_REQUEST_SPACE_BEFORE_HTTP_VERSION";
	case PARSE::STATE_REQUEST_VERSION_H:
		return "STATE_REQUEST_VERSION_H";
	case PARSE::STATE_REQUEST_VERSION_T1:
		return "STATE_REQUEST_VERSION_T1";
	case PARSE::STATE_REQUEST_VERSION_T2:
		return "STATE_REQUEST_VERSION_T2";
	case PARSE::STATE_REQUEST_VERSION_P:
		return "STATE_REQUEST_VERSION_P";
	case PARSE::STATE_REQUEST_VERSION_SLASH:
		return "STATE_REQUEST_VERSION_SLASH";
	case PARSE::STATE_REQUEST_VERSION_MAJOR:
		return "STATE_REQUEST_VERSION_MAJOR";
	case PARSE::STATE_REQUEST_VERSION_DOT:
		return "STATE_REQUEST_VERSION_DOT";
	case PARSE::STATE_REQUEST_VERSION_MINOR:
		return "STATE_REQUEST_VERSION_MINOR";
	case PARSE::STATE_REQUEST_LINE_CR:
		return "STATE_REQUEST_LINE_CR";
	case PARSE::STATE_REQUEST_LINE_LF:
		return "STATE_REQUEST_LINE_LF";
	case PARSE::STATE_HEADER_KEY_START:
		return "STATE_HEADER_KEY_START";
	case PARSE::STATE_HEADER_KEY:
		return "STATE_HEADER_KEY";
	case PARSE::STATE_HEADER_COLON:
		return "STATE_HEADER_COLON";
	case PARSE::STATE_HEADER_SPACE_BEFORE_VALUE:
		return "STATE_HEADER_SPACE_BEFORE_VALUE";
	case PARSE::STATE_HEADER_VALUE_START:
		return "STATE_HEADER_VALUE_START";
	case PARSE::STATE_HEADER_VALUE:
		return "STATE_HEADER_VALUE";
	case PARSE::STATE_HEADER_LINE_CR:
		return "STATE_HEADER_LINE_CR";
	case PARSE::STATE_HEADER_LINE_LF:
		return "STATE_HEADER_LINE_LF";
	case PARSE::STATE_END_OF_HEADERS_CR:
		return "STATE_END_OF_HEADERS_CR";
	case PARSE::STATE_END_OF_HEADERS_LF:
		return "STATE_END_OF_HEADERS_LF";
	case PARSE::STATE_BODY_CONTENT_LENGTH:
		return "STATE_BODY_CONTENT_LENGTH";
	case PARSE::STATE_BODY_CHUNKED_SIZE:
		return "STATE_BODY_CHUNKED_SIZE";
	case PARSE::STATE_BODY_CHUNKED_SIZE_CR:
		return "STATE_BODY_CHUNKED_SIZE_CR";
	case PARSE::STATE_BODY_CHUNKED_SIZE_LF:
		return "STATE_BODY_CHUNKED_SIZE_LF";
	case PARSE::STATE_BODY_CHUNKED_DATA:
		return "STATE_BODY_CHUNKED_DATA";
	case PARSE::STATE_BODY_CHUNKED_DATA_CR:
		return "STATE_BODY_CHUNKED_DATA_CR";
	case PARSE::STATE_BODY_CHUNKED_DATA_LF:
		return "STATE_BODY_CHUNKED_DATA_LF";
	case PARSE::STATE_END_OF_CHUNKED_CR:
		return "STATE_END_OF_CHUNKED_CR";
	case PARSE::STATE_END_OF_CHUNKED_LF:
		return "STATE_END_OF_CHUNKED_LF";
		
	default:
		return "UNKNOWN";
	}
}

