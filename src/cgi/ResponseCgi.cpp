#include "ResponseCgi.hpp"

ResponseCgi::ResponseCgi()
    : _status(0),
      _body(""),
      _headers() {}

ResponseCgi::~ResponseCgi() {}

//Setters:
void ResponseCgi::setStatus(int status) {
    _status = status;
}

void ResponseCgi::setBody(const std::string& body) {
    _body = body;
}

void ResponseCgi::setHeader(const std::string& key, const std::string& value) {
    _headers[key] = value;
}

// Getters:
int ResponseCgi::getStatus() const {
    return _status;
}

const std::string& ResponseCgi::getBody() const {
    return _body;
}

const std::map<std::string, std::string>& ResponseCgi::getHeaders() const {
    return _headers;
}

