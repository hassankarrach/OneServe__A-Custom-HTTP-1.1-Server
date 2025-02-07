    #include "RequestCgi.hpp"

    RequestCgi::RequestCgi() : _request_method(""), _script_name(""), _query_string(""),
          _content_length(""), _content_type(""), _body(""), _path_info("") , _interpreter("") {}

    RequestCgi::RequestCgi(const std::string& request_method,
               const std::string& script_name,
               const std::string& query_string,
               const std::string& content_length,
               const std::string& content_type,
               const std::string& body,
               const std::string& path_info,
               const std::string& interpreter)
        : _request_method(request_method), _script_name(script_name),
          _query_string(query_string), _content_length(content_length),
          _content_type(content_type), _body(body), _path_info(path_info), _interpreter(interpreter) {}

    RequestCgi::~RequestCgi() {}

    // Setters:
    void RequestCgi::setRequestMethod(const std::string &request_method) { _request_method = request_method; }
    void RequestCgi::setScriptName(const std::string &script_name) { _script_name = script_name; }
    void RequestCgi::setQueryString(const std::string &query_string) { _query_string = query_string; }
    void RequestCgi::setContentLength(const std::string &content_length) { _content_length = content_length; }
    void RequestCgi::setContentType(const std::string &content_type) { _content_type = content_type; }
    void RequestCgi::setBody(const std::string &body) { _body = body; }
    void RequestCgi::setPathInfo(const std::string &path_info) { _path_info = path_info; }
    void RequestCgi::setInterpreter(const std::string &interpreter) { _interpreter = interpreter; }

    // Getters:
    const std::string& RequestCgi::getRequestMethod() const { return _request_method; }
    const std::string& RequestCgi::getScriptName() const { return _script_name; }
    const std::string& RequestCgi::getQueryString() const { return _query_string; }
    const std::string& RequestCgi::getContentLength() const { return _content_length; }
    const std::string& RequestCgi::getContentType() const { return _content_type; }
    const std::string& RequestCgi::getBody() const { return _body; }
    const std::string& RequestCgi::getPathInfo() const { return _path_info; }
    const std::string& RequestCgi::getInterpreter() const { return _interpreter; }