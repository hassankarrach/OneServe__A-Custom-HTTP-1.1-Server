#include "../includes/webserv.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include "../includes/http/request.hpp"

int main()
{
	Server server;
	server.Init();

	return 0;
}

/*
	TO-CHECK/DO LATER :
	1 - content length > body length !?
	2 - HANDLE port, being on uri !!!!!!!!
	3 - use the GOD DAMN & refs everywhere ****

	4 - Redirections (301, 302, 303, 307, 308)
 */