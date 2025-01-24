#include "../../includes/http/request_parser.hpp"

bool RequestParser::isDigit(char c)
{
	return c >= '0' && c <= '9';
}
bool RequestParser::isAlpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
bool RequestParser::isHex(char c)
{
	return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
bool RequestParser::isSpace(char c)
{
	return c == ' ' || c == '\t';
}
bool RequestParser::isUnreserved(char c)
{
	return isAlpha(c) || isDigit(c) || c == '-' || c == '.' || c == '_' || c == '~';
}
bool RequestParser::isReserved(char c)
{
	return c == '!' || c == '*' || c == '\'' || c == '(' || c == ')' || c == ';' || c == ':' || c == '@' || c == '&' || c == '=' || c == '+' || c == '$' || c == ',' || c == '/' || c == '?' || c == '#';
}
bool RequestParser::isReservedPath(char c)
{
	return c == '!' || c == '*' || c == '\'' || c == '(' || c == ')' || c == ';' || c == ':' || c == '@' || c == '&' || c == '=' || c == '+' || c == '$' || c == ',' || c == '/';
}
bool RequestParser::isPrintable(char c)
{
	return c >= 32 && c <= 126;
}
bool RequestParser::isSubDelim(char c)
{
	return c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' || c == '*' || c == '+' || c == ',' || c == ';' || c == '=';
}
bool RequestParser::isCtl(char c)
{
	return (c >= 0 && c <= 31) || c == 127;
}
bool RequestParser::isTchar(char c)
{
	return isalpha(c) || isdigit(c) || c == '!' || c == '#' || c == '$' || c == '%' || c == '&' || c == '\'' || c == '*' || c == '+' || c == '-' || c == '.' || c == '^' || c == '_' || c == '`' || c == '|' || c == '~';
}
bool RequestParser::isPunctuation(char c)
{
	return c == '!' || c == '#' || c == '$' || c == '%' || c == '&' ||
		   c == '\'' || c == '*' || c == '+' || c == '-' || c == '.' ||
		   c == '^' || c == '_' || c == '`' || c == '|' || c == '~';
}
int RequestParser::fromHex(char c)
{
	if (isDigit(c))
		return c - '0';
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}
std::string RequestParser::to_lowercase(const std::string &str)
{
	std::string result = str;
	for (size_t i = 0; i < result.length(); ++i)
	{
		result[i] = std::tolower(result[i]);
	}
	return result;
}
