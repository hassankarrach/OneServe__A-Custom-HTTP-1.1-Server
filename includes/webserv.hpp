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

// Utils
#include "./utils/utils.hpp"

// HTTP
#include "./http/response.hpp"
#include "./http/request.hpp"

// Sockets Macros
#define PORT 8080
#define BUFFER_SIZE 8192
#define MAX_CONNECTIONS 5
#define MAX_EVENTS 10


class Server {
	private :
		int _server_fd;

	public :
		Server();
		~Server();

		void Init();
		void Serve_client(int client_socket);
};


#endif