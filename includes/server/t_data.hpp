
#ifndef T_DATA
#define T_DATA


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


typedef map<string , vector<string> > t_map;



class t_data
{

public:
    t_data();
    t_map operator()(const char *);
    vector<string> operator[](const char *);
    string operator[](int code);
public:
    int empty();
   t_map server;
    map<string , string > error;
    vector< pair<string ,t_map > > location;

};



#endif

