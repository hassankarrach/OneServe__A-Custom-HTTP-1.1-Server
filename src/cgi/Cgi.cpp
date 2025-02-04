#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iostream>

#include "RequestCgi.hpp"
#include "ResponseCgi.hpp"

// Helper function to trim whitespace (C++98 compatible)
static void trim(std::string& str) {
    const char* whitespace = " \t\n\r";
    str.erase(0, str.find_first_not_of(whitespace));
    str.erase(str.find_last_not_of(whitespace) + 1);
}

void handleCGI(RequestCgi& request, ResponseCgi& response) {
    // Check if script is executable
    std::string scriptPath = request.getPathInfo();
    if (access(scriptPath.c_str(), X_OK) == -1) {
        response.setStatus(404);
        return;
    }

    // Setup environment variables
    std::map<std::string, std::string> env;
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["SERVER_PROTOCOL"] = "HTTP/1.1";
    env["REQUEST_METHOD"] = request.getRequestMethod();
    env["SCRIPT_NAME"] = request.getScriptName();
    env["SERVER_PORT"] = request.getServerPort();
    env["PATH_INFO"] = request.getPathInfo();
    env["REDIRECT_STATUS"] = "200";

    if (request.getRequestMethod() == "POST") {
        env["CONTENT_LENGTH"] = request.getContentLength();
        env["CONTENT_TYPE"] = request.getContentType();
    }
    else if (request.getRequestMethod() == "GET") {
        env["QUERY_STRING"] = request.getQueryString();
    }

    // Create pipes for CGI process I/O
    int stdin_pipe[2], stdout_pipe[2];
    if (pipe(stdin_pipe) != 0 || pipe(stdout_pipe) != 0) {
        response.setStatus(500);
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        response.setStatus(500);
        return;
    }
    else if (pid == 0) { // Child process
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);

        dup2(stdin_pipe[0], STDIN_FILENO);
        dup2(stdout_pipe[1], STDOUT_FILENO);

        // Prepare environment variables array for execve
        std::vector<char*> env_ptrs;
        for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it) {
            std::string entry = it->first + "=" + it->second;
            env_ptrs.push_back(strdup(entry.c_str()));
        }
        env_ptrs.push_back(NULL);

        // Prepare arguments: the script path is the only argument
        char* argv[] = { const_cast<char*>(scriptPath.c_str()), NULL };

        execve(scriptPath.c_str(), argv, &env_ptrs[0]);
        _exit(EXIT_FAILURE);
    }
    else { // Parent process
        close(stdin_pipe[0]);
        close(stdout_pipe[1]);

        // For POST requests, write the request body to the CGI process STDIN.
        if (request.getRequestMethod() == "POST") {
            const std::string& body = request.getBody();
            if (!body.empty()) {
                write(stdin_pipe[1], body.c_str(), body.length());
            }
        }
        close(stdin_pipe[1]);

        // Write CGI output directly to a file
        const char* fileName = "cgi_response.txt";
        std::ofstream outFile(fileName, std::ios::binary);
        if (!outFile) {
            response.setStatus(500);
            return;
        }
        char buffer[4096];
        ssize_t bytes_read;
        while ((bytes_read = read(stdout_pipe[0], buffer, sizeof(buffer))) > 0) {
            outFile.write(buffer, bytes_read);
        }
        outFile.close();
        close(stdout_pipe[0]);

        // Wait for child process to finish
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            response.setStatus(502);
            return;
        }

        // Read the entire file content for further processing
        std::ifstream inFile(fileName, std::ios::binary);
        if (!inFile) {
            response.setStatus(500);
            return;
        }
        std::string cgi_output((std::istreambuf_iterator<char>(inFile)),
                                std::istreambuf_iterator<char>());
        inFile.close();

        // Parse headers and body
        size_t header_end = cgi_output.find("\r\n\r\n");
        if (header_end == std::string::npos) {
            response.setBody(cgi_output);
            response.setHeader("Content-Type", "text/plain");
            return;
        }

        std::string headers = cgi_output.substr(0, header_end);
        std::string response_body = cgi_output.substr(header_end + 4);

        std::istringstream header_stream(headers);
        std::string header_line;
        while (std::getline(header_stream, header_line)) {
            size_t colon_pos = header_line.find(':');
            if (colon_pos != std::string::npos) {
                std::string key = header_line.substr(0, colon_pos);
                std::string value = header_line.substr(colon_pos + 1);
                trim(key);
                trim(value);

                if (key == "Status") {
                    size_t space_pos = value.find(' ');
                    if (space_pos != std::string::npos) {
                        response.setStatus(atoi(value.substr(0, space_pos).c_str()));
                    }
                }
                else {
                    response.setHeader(key, value);
                }
            }
        }

        response.setBody(response_body);
    }
}








int main() {
    // Create a test PHP script

    // Create a sample POST request with form data
    RequestCgi request;
    
    // Set basic request information
    request.setRequestMethod("GET");
    request.setScriptName("test.php");
    request.setPathInfo("./test_post.php");
    request.setServerPort("8080");

    
    return 0;
}