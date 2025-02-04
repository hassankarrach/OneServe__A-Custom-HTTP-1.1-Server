#ifndef PARSINGCONFIGFILE_HPP
#define PARSINGCONFIGFILE_HPP

#include "t_data.hpp"




using namespace std;



const string  KEYOFLOCATION[] = {"uri" , "upload"  , "redirect" , "cgi" , "root" , "methods" , "index"};

const string  NAMESOFBLOCKS[] = {"[server]" , "[server.errors]" , "[server.location]"};

const string  KEYOFSERVER[] = {"host"  , "body_size", "port" , "server_name"};

const string  VALUES[] = {"GET" , "POST" , "DELETE"};









class ParsingConfigFile
{

private:
    vector<int (*)(int)> func_ptr;
    vector<t_data >  blocks;
    vector<addrinfo *> save_addr;
    t_data data;
    t_map loc;
    string key;
    string value;
    string filename;
    vector<int> socks;
    int limit;
    int flag;

public:
    ParsingConfigFile();
    t_data operator()(const char *);
    ~ParsingConfigFile();

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


// struct configres;
// {
//     t_map loc;

//     /* data */
// };


extern ParsingConfigFile Config;

#endif