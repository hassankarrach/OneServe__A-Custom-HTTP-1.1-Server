#ifndef PARSE_CONFIG_HPP
#define PARSE_CONFIG_HPP

#include "Config_Loader.hpp"

using namespace std;

const string  KEYOFLOCATION[] = {"uri" , "upload"  , "redirect" , "cgi" , "root" , "methods" , "index"};

const string  NAMESOFBLOCKS[] = {"[server]" , "[server.errors]" , "[server.location]"};

const string  KEYOFSERVER[] = {"host"  , "body_size", "port" , "server_name"};

const string  VALUES[] = {"GET" , "POST" , "DELETE"};




class Parse_Config
{

private:
    vector<int (*)(int)> func_ptr;
    vector<ConfigLoader >  blocks;
    vector<addrinfo *> save_addr;
    vector<int> socks;
    ConfigLoader data;
    t_map loc;
    string key;
    string value;
    string filename;
    int limit;
    int flag;

public:
    Parse_Config();
    ~Parse_Config();

public:
    ConfigLoader operator()(const char *);

public:
    vector<addrinfo *>getHosts();

public:
    void getKeyValue(string &line , char set);
    int CheckCharacter(char c);
    void CheckString(string &str);
    void ParseFile(const char *_file);
    void CheckBlockLocation();
    void CheckBlockErrors();
    void CheckInfoServer();
    void push_back_data();
    void push_back_location();
    void CheckArrayOfValue(vector<string> &data ,int flag);
    void ft_getaddrinfo();
    void test();

};

extern Parse_Config Config;

#endif