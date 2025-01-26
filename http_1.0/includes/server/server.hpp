
#ifndef SERVER_HPP
#define SERVER_HPP

#include "../http/request.hpp"
#include "../http/response.hpp"


#include <iostream>
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include <csignal>
#include <cstdlib> 
#include <vector>
#include <sstream>
#include <bits/stdc++.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <algorithm>
#include <cerrno>
// #include "../CONFIG_FILE/ParsingConfigFile.hpp"



using namespace std;




class my_class
{
public:
    my_class() {};
    ~my_class(){};
    
public:
    int fd;
    Request req;
    HttpRequestData resData;
    Response res;
    
    
    int execute()
    {
        return(res.Serve(fd , resData));
    };
    
};





class Server
{

private:
    vector<int> sockfds;
    int epfd;

public:
    Server();
    ~Server();

public:
    map<int , my_class*> serv;
    void CreatServer(vector<addrinfo *> hosts);
    int find(int fd);

public:
    void ADD_Events(int _fd , EPOLL_EVENTS ev , int op );
    void CreatMultiplexing();
    void ForEachEvents(epoll_event *events , int n_events );
};


#endif