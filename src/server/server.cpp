#include "../../includes/webserv.hpp"
#include <fstream>
#include <sstream>

Server::Server()
{
}

Server::~Server()
{
    close(_server_fd);
}

void Server::Init()
{
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to localhost port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) // Listen for only one connection
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept one connection at a time
    client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (client_socket < 0)
    {
        perror("accept");
        return;
    }
    else // I'm serving only one client, NO MORE -_.
        this->Serve_client(client_socket);
}

void Server::Serve_client(int client_socket)
{
    char buffer[4096] = {0};
    Request     *req; // ++;
    Response    *res; 

    while (true)
    {
        
    }
}