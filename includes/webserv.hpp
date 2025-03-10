#ifndef WEBSERV_HPP
#define WEBSERV_HPP

// Includes
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include "iostream"
#include <sys/epoll.h>
#include <map>

// Utils
#include "./utils/utils.hpp"

// HTTP
#include "./http/response.hpp"
#include "./http/request.hpp"
#include "../includes/server/server.hpp"
#include "../includes/config/Parse_Config.hpp"
#include "../includes/http/cgi_handler.hpp"
// Sockets Macros
#define PORT 8080
#define BUFFER_SIZE 8192
#define MAX_CONNECTIONS 5
#define MAX_EVENTS 10



#endif