
#ifndef T_DATA_HPP
#define T_DATA_HPP


#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <algorithm>
#include <string>
#include <fstream>
#include <map>
#include <bits/stdc++.h>
#include <string>
#include <vector>
#include "map.hpp"


using namespace std;




typedef http::map<string , vector<string> > t_map;



class t_data
{

public:
    t_data();
    t_map operator()(const char *);
public:
    int empty();
    t_map server;
    t_map error;
    vector<t_map> location;
public:
    std::string operator[](const char *str)
    {
        if(server.find(str))
            return(server[str]);
        return(error[str]);
    }    
};

int strtrim(std::string &str , const char *sharset);
void  throwServerError(bool expr , const char *str);

#endif

