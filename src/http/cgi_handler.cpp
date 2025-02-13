#include "../../includes/http/cgi_handler.hpp"

// Req CGI
RequestCgi::RequestCgi() : _request_method(""), _script_name(""), _query_string(""),
						   _content_length(""), _content_type(""), _body(""), _cookies(""), _path_info(""), _interpreter("") {}
RequestCgi::RequestCgi(const std::string &request_method,
					   const std::string &script_name,
					   const std::string &query_string,
					   const std::string &content_length,
					   const std::string &content_type,
					   const std::string &body,
                       const std::string &cookies,
					   const std::string &path_info,
					   const std::string &interpreter)
	: _request_method(request_method), _script_name(script_name),
	  _query_string(query_string), _content_length(content_length),
	  _content_type(content_type), _body(body), _cookies(cookies),
      _path_info(path_info), _interpreter(interpreter) {}
RequestCgi::~RequestCgi() {}
// Setters:
void RequestCgi::setRequestMethod(const std::string &request_method) { _request_method = request_method; }
void RequestCgi::setScriptName(const std::string &script_name) { _script_name = script_name; }
void RequestCgi::setQueryString(const std::string &query_string) { _query_string = query_string; }
void RequestCgi::setContentLength(const std::string &content_length) { _content_length = content_length; }
void RequestCgi::setContentType(const std::string &content_type) { _content_type = content_type; }
void RequestCgi::setBody(const std::string &body) { _body = body; }
void RequestCgi::setCookies(const std::string &cookies) { _cookies = cookies; }
void RequestCgi::setPathInfo(const std::string &path_info) { _path_info = path_info; }
void RequestCgi::setInterpreter(const std::string &interpreter) { _interpreter = interpreter; }
// Getters:
const std::string &RequestCgi::getRequestMethod() const { return _request_method; }
const std::string &RequestCgi::getScriptName() const { return _script_name; }
const std::string &RequestCgi::getQueryString() const { return _query_string; }
const std::string &RequestCgi::getContentLength() const { return _content_length; }
const std::string &RequestCgi::getContentType() const { return _content_type; }
const std::string &RequestCgi::getBody() const { return _body; }
const std::string &RequestCgi::getCookies() const { return _cookies; }
const std::string &RequestCgi::getPathInfo() const { return _path_info; }
const std::string &RequestCgi::getInterpreter() const { return _interpreter; }


// Resp CGI
ResponseCgi::ResponseCgi()
    : _status(200), _body(""), _bodyFile("") {}
ResponseCgi::~ResponseCgi() {}
void ResponseCgi::setStatus(int status) {
    _status = status;
}
int ResponseCgi::getStatus() const {
    return _status;
}
void ResponseCgi::setHeader(const std::string &key, const std::string &value) {
    _headers[key] = value;
}
const std::map<std::string, std::string>& ResponseCgi::getHeaders() const {
    return _headers;
}
void ResponseCgi::setBody(const std::string &body) {
    _body = body;
}
const std::string& ResponseCgi::getBody() const {
    return _body;
}
void ResponseCgi::setBodyFile(const std::string &filePath) {
    _bodyFile = filePath;
}
const std::string& ResponseCgi::getBodyFile() const {
    return _bodyFile;
}


// Utility: trim whitespace from both ends of a string
static void trim(std::string &str) {
    const char *whitespace = " \t\n\r";
    str.erase(0, str.find_first_not_of(whitespace));
    str.erase(str.find_last_not_of(whitespace) + 1);
}

// =================== Non-Blocking CGI Handler Function with Timeout and File-Based Request Body ===================
void handleCGI(RequestCgi &request, ResponseCgi &response) {
    std::string interpreter = request.getInterpreter();
    std::string scriptFile  = request.getScriptName();

    // Set up the CGI environment variables.
    std::map<std::string, std::string> env;
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["SCRIPT_FILENAME"]   = scriptFile;
    env["SERVER_PROTOCOL"]   = "HTTP/1.1";
    env["REQUEST_METHOD"]    = request.getRequestMethod();
    env["SCRIPT_NAME"]       = request.getScriptName();
    env["PATH_INFO"]         = request.getPathInfo();
    env["HTTP_COOKIE"]       = request.getCookies();
    env["REDIRECT_STATUS"]   = "200"; // For php-cgi compatibility

    if (request.getRequestMethod() == "POST") {
        env["CONTENT_LENGTH"] = request.getContentLength();
        env["CONTENT_TYPE"]   = request.getContentType();
    } else if (request.getRequestMethod() == "GET" && !request.getQueryString().empty()) {
        env["QUERY_STRING"] = request.getQueryString();
    }

    // Create pipes for communication with the CGI process.
    int stdin_pipe[2];
    int stdout_pipe[2];
    if (pipe(stdin_pipe) != 0 || pipe(stdout_pipe) != 0) {
        std::cout << "here" << std::endl;
        response.setStatus(500);
        return;
    }

    // Set the parent's ends of the pipes to non-blocking with FD_CLOEXEC.
    fcntl(stdin_pipe[1], F_SETFL, O_NONBLOCK | FD_CLOEXEC);
    fcntl(stdout_pipe[0], F_SETFL, O_NONBLOCK | FD_CLOEXEC);

    // Fork the process.
    pid_t pid = fork();
    if (pid == -1) {
        std::cout << "here2" << std::endl;
        response.setStatus(500);
        return;
    }
    else if (pid == 0) {  // Child process
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);

        // Redirect child's STDIN and STDOUT.
        dup2(stdin_pipe[0], STDIN_FILENO);
        dup2(stdout_pipe[1], STDOUT_FILENO);

        // Build an environment array for execve.
        std::vector<char*> envp;
        for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it) {
            std::string entry = it->first + "=" + it->second;
            envp.push_back(strdup(entry.c_str()));
        }
        envp.push_back(NULL);

        // Prepare arguments and execute the CGI interpreter.
        char *argv[] = { const_cast<char*>(interpreter.c_str()),
                         const_cast<char*>(scriptFile.c_str()),
                         NULL };
        execve(interpreter.c_str(), argv, &envp[0]);
        _exit(EXIT_FAILURE); // If execve fails.
    }
    else {  // Parent process
        close(stdin_pipe[0]);
        close(stdout_pipe[1]);

        // Instead of a string body, we treat request.getBody() as a file path.
        std::string body_file_path = request.getBody();
        int file_fd = open(body_file_path.c_str(), O_RDONLY);
        if (file_fd < 0) {
            std::cout << "here3" << std::endl;
            response.setStatus(500);
            return;
        }

        // Variables for reading the file contents.
        char file_buf[4096];
        ssize_t bytes_in_buffer = 0;
        ssize_t buffer_offset = 0;
        bool eof_reached = false;

        std::string cgi_output;

        pollfd fds[2];
        fds[0].fd = stdin_pipe[1];
        fds[0].events = POLLOUT;
        fds[1].fd = stdout_pipe[0];
        fds[1].events = POLLIN;

        bool stdin_closed = false;
        bool stdout_closed = false;

        
        const int overall_timeout_ms = 5000; // 5 seconds.
        struct timeval start, now;
        gettimeofday(&start, NULL);

        
        while (!stdin_closed || !stdout_closed) {
            gettimeofday(&now, NULL);
            long elapsed_ms = (now.tv_sec - start.tv_sec) * 1000 +
                              (now.tv_usec - start.tv_usec) / 1000;
            if (elapsed_ms >= overall_timeout_ms) {
                // Overall timeout reached: kill CGI process and return 504.
                kill(pid, SIGKILL);
                waitpid(pid, NULL, 0);
                response.setStatus(504);  // Gateway Timeout.
                close(file_fd);
                return;
            }

            int poll_timeout = overall_timeout_ms - elapsed_ms;
            int ret = poll(fds, 2, poll_timeout);
            if (ret < 0) {
                std::cout << "here4" << std::endl;
                response.setStatus(500);
                close(file_fd);
                return;
            }

            // --- Write request body from file to CGI's STDIN ---
            if (!stdin_closed && (fds[0].revents & POLLOUT)) {
                // If no pending data in our buffer and file not at EOF, try to read.
                if (bytes_in_buffer == 0 && !eof_reached) {
                    bytes_in_buffer = read(file_fd, file_buf, sizeof(file_buf));
                    if (bytes_in_buffer < 0) {
                        if (errno != EAGAIN && errno != EWOULDBLOCK) {
                            std::cout << "here5" << std::endl;
                            response.setStatus(500);
                            close(file_fd);
                            return;
                        } else {
                            bytes_in_buffer = 0;
                        }
                    } else if (bytes_in_buffer == 0) {
                        // End of file reached.
                        eof_reached = true;
                    }
                    buffer_offset = 0;
                }
                // Write any buffered data.
                if (bytes_in_buffer > 0) {
                    ssize_t n = write(stdin_pipe[1],
                                      file_buf + buffer_offset,
                                      bytes_in_buffer - buffer_offset);
                    if (n > 0) {
                        buffer_offset += n;
                        if (buffer_offset >= bytes_in_buffer) {
                            // All data in the buffer was written.
                            bytes_in_buffer = 0;
                            buffer_offset = 0;
                        }
                    } else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                        std::cout << "here6" << std::endl;
                        response.setStatus(500);
                        close(file_fd);
                        return;
                    }
                }
                // If the file is finished and no pending data remains, close the write end.
                if (eof_reached && bytes_in_buffer == 0) {
                    close(stdin_pipe[1]);
                    stdin_closed = true;
                    fds[0].fd = -1;
                    close(file_fd);
                }
            }

            // --- Read CGI output from its STDOUT ---
            if (!stdout_closed && (fds[1].revents & (POLLIN | POLLHUP))) {
                char buffer[4096];
                ssize_t n = read(stdout_pipe[0], buffer, sizeof(buffer));
                if (n > 0) {
                    cgi_output.append(buffer, n);
                } else if (n == 0) {
                    // End-of-file: close the pipe.
                    close(stdout_pipe[0]);
                    stdout_closed = true;
                    fds[1].fd = -1;
                } else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                    std::cout << "here7" << std::endl;
                    response.setStatus(500);
                    return;
                }
            }
        } // End of poll loop.

        // Wait for the CGI process to terminate.
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            response.setStatus(502);
            return;
        }

        // Create a temporary file to store only the CGI body.
        char tmpFileName[] = "www/tmp/cgi_response_XXXXXX";
        int tmp_fd = mkstemp(tmpFileName);
        if (tmp_fd == -1) {
            std::cout << "here8" << std::endl;
            response.setStatus(500);
            return;
        }
        close(tmp_fd);

        // Parse the CGI output to separate headers from the body.
        size_t header_end = cgi_output.find("\r\n\r\n");
        std::string body_content;
        if (header_end == std::string::npos) {
            body_content = cgi_output;
            response.setHeader("Content-Type", "text/plain");
        } else {
            std::string headers = cgi_output.substr(0, header_end);
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
                            response.setStatus(std::atoi(value.substr(0, space_pos).c_str()));
                        }
                    } else {
                        response.setHeader(key, value);
                    }
                }
            }
            // The body starts after the "\r\n\r\n" delimiter.
            body_content = cgi_output.substr(header_end + 4);
        }

        // Overwrite the temporary file so it contains only the body.
        std::ofstream bodyFile(tmpFileName, std::ios::binary | std::ios::trunc);
        if (!bodyFile) {
            std::cout << "here9" << std::endl;
            response.setStatus(500);
            return;
        }
        bodyFile.write(body_content.c_str(), body_content.size());
        bodyFile.close();

        // Associate the temporary file with the response.
        response.setBodyFile(tmpFileName);
    }
}
