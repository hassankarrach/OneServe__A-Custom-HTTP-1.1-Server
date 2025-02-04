#ifndef REQUESTCGI_HPP
#define REQUESTCGI_HPP

#include <string>
class RequestCgi {

private:
    std::string _request_method;
    std::string _script_name;
    std::string _query_string;
    std::string _content_length;
    std::string _content_type;
    std::string _body;
    std::string _path_info;
    std::string _path_translated;
    std::string _server_port;

public:
    // Constructors:
    RequestCgi();

    
    RequestCgi(const std::string& request_method,
               const std::string& script_name,
               const std::string& query_string,
               const std::string& content_length,
               const std::string& content_type,
               const std::string& body,
               const std::string& path_info,
               const std::string& path_translated,
               const std::string& server_port);


    ~RequestCgi();

    // Setters:
    void setRequestMethod(const std::string& request_method);
    void setScriptName(const std::string& script_name);
    void setQueryString(const std::string& query_string);
    void setContentLength(const std::string& content_length);
    void setContentType(const std::string& content_type);
    void setBody(const std::string& body);
    void setPathInfo(const std::string& path_info);
    void setPathTranslated(const std::string& path_translated);
    void setServerPort(const std::string& server_port);

    // Getters:
    const std::string& getRequestMethod() const;
    const std::string& getScriptName() const;
    const std::string& getQueryString() const;
    const std::string& getContentLength() const;
    const std::string& getContentType() const;
    const std::string& getBody() const;
    const std::string& getPathInfo() const;
    const std::string& getPathTranslated() const;
    const std::string& getServerPort() const;


};



#endif // REQUESTCGI_HPP