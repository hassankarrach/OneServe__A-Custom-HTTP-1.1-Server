
#ifndef CONFIG_LOADER_HPP
#define CONFIG_LOADER_HPP


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


using namespace std;




namespace http
{

template<class T1 , class T2>
// create new class like  std::map     for  overriding functions  
class map
{

private:
    std::map<T1 , T2> data;

public:
    map(){};
    ~map(){};

public:
    int count(T1 str);
    int find(std::string  str);
    int find(const char * str);
    int find(const char *s1 , const char *s2);
    int empty();

public:
    void clear();
    void insert(const char *s1 , const char *s2);

public:
    std::vector<std::string> & operator[](std::string str);
    std::string operator[](const char *str);
};


}




typedef http::map<string , vector<string> > t_map;


// this class for loading data server 
class ConfigLoader
{

public:
    t_map server;
    t_map error;
    vector<t_map> location;

public:
    ConfigLoader();
    ~ConfigLoader();

public:
    t_map operator()(const char *);
    std::string operator[](const char *str);

public:
    int empty();    
};

int strtrim(std::string &str , const char *sharset);
void  throwConfigError(bool expr , const char *str);

#include "Config_Loader.tpp"


#endif

