
#ifndef PARSINGCONFIGFILE_HPP
#define PARSINGCONFIGFILE_HPP

#include "t_data.hpp"




using namespace std;



const string  KEYOFLOCATION[] = {"uri" , "cgi" , "root" , "methods" , "index"};

const string  NAMESOFBLOCKS[] = {"[server]" , "[server.errors]" , "[server.location]"};

const string  KEYOFSERVER[] = {"host"  , "body_size", "port" , "server_name"};



const string  VALUES[] = {"GET" , "POST" , "DELETE"};









class ParsingConfigFile
{

private:
    vector<int (*)(int)> func_ptr;
    vector<pair<string , t_data> >  blocks;
    pair<string , t_data>  save_block;
    t_data data;
    pair<string ,t_map > pair_location;
    string key;
    string value;
    string filename;
    addrinfo *res;
    vector<addrinfo *> save_addr;
    int limit;
    int flag;
    string path;
public:

    ParsingConfigFile();
    t_data operator()(const char *);
    ~ParsingConfigFile();

public:
    void getKeyValue(string &line);
    int CheckCharacter(char c);
    void CheckString(string &str);
    void ParseFile(const char *_file);
    void CheckBlockLocation();
    void CheckBlockErrors();
    void CheckInfoServer();
    void push_back_data();
    void push_back_location();
    void CheckArrayOfValue(vector<string> &data , int first , int size);
    void Checkblock(t_data &info);
    void test();
    vector<addrinfo *>getHosts();

};


void split(string &s , vector<string > &data);

#endif