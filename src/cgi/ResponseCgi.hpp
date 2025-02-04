#ifndef RESPONSECGI_HPP
#define RESPONSECGI_HPP

#include <string>
#include <map>

class ResponseCgi {
private:
    int _status;
    std::string _body;
    std::map<std::string, std::string> _headers;

public:
    // Constructors:
    ResponseCgi();
    
    ~ResponseCgi();

    // Setters:
    void setStatus(int status);
    void setBody(const std::string& body);
    void setHeader(const std::string& key, const std::string& value);

    // Getters:
    int getStatus() const;
    const std::string& getBody() const;
    const std::map<std::string, std::string>& getHeaders() const;
};


#endif