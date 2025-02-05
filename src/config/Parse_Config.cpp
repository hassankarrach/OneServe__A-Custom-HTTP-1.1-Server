#include "../../includes/config/Parse_Config.hpp"

string checks = "";

map<int , void (Parse_Config::*)()> funcArray;
map<int , void (Parse_Config::*)()> funcSave;


void Parse_Config::test() {}

Parse_Config::Parse_Config(){}

vector<addrinfo *>Parse_Config::getHosts()
{
    return save_addr;
}



ConfigLoader Parse_Config::operator()(const char *_str)
{
    string str = _str;
    getKeyValue(str , ':');

    vector<ConfigLoader>::iterator it = blocks.begin();
    while(it != blocks.end())
    {
        if(it->server.find("host", key.data()) && it->server.find("port", value.data()))
            return(*it);
        it++;
    }
    throwConfigError(true , "Error host is not exist");
    return *it;
}

Parse_Config::~Parse_Config()
{
    func_ptr.clear(); key.clear();
    value.clear(); filename.clear();
    blocks.clear();

};


int Parse_Config::CheckCharacter(char c)
{
    for(unsigned long i = 0;i < func_ptr.size();i++)
        if(func_ptr[i](c)) return 1;
    return (checks.find(c) != checks.npos);
}


void Parse_Config::CheckString(string &str)
{
    for(unsigned long i = 0; i < str.size() ; i++)
        throwConfigError(!CheckCharacter(str[i]) , "webserv : Invalid String");
}



void Parse_Config::CheckArrayOfValue(vector<string> &data , int flag)
{
    stringstream  obj(value);
    string word;
    size_t i = 0;
    for(i = 0; obj >> word; i++)
    {
        throwConfigError(!flag && !count(&VALUES[0] ,&VALUES[3] , word) , "werbserv : invalid value1" );
        data.push_back(word);
    }
    throwConfigError(flag == 1 && i != 1 , "werbserv : invalid value2" );
    throwConfigError(flag == 3  && i != 2 , "werbserv : invalid value3" );
}



void Parse_Config::CheckBlockErrors()
{
    throwConfigError((key.length() != 3 || key[0] < '3' || key[0] > '5') , "webserv : must btw 300 to 599");
    func_ptr.push_back(isdigit); CheckString(key);

    flag = 1;
    throwConfigError(data.error.find(key) , "webserve : dupblicate key error");
    CheckArrayOfValue(data.error[key] , flag);
}

void Parse_Config::CheckBlockLocation()
{
    throwConfigError(!count(KEYOFLOCATION , KEYOFLOCATION + 7 , key) ,  "webserv : location this key is not exist");

    flag = (key[0] == 'c') * 2 + (key[0] == 'm') * 0 + !(key[0] == 'm') * 1;
    throwConfigError(key[0] != 'c' && loc.find(key) , "webserve : dupblicate key location");
    CheckArrayOfValue(loc[key] , flag);
}


void Parse_Config::CheckInfoServer()
{
    throwConfigError(!count(KEYOFSERVER , KEYOFSERVER + 4 , key) ,  "webserv : server this key is not exist");

    flag = (key[0] == 'p' || key[0] == 's') * 2 + !(key[0] == 'p' || key[0] == 's');
    throwConfigError(data.server.find(key) , "webserve : dupblicate key server");
    CheckArrayOfValue(data.server[key] , flag);
    
    if(key[0] == 'b')
    {
        value = data.server[key][0];
        checks = "KM"; func_ptr.push_back(isdigit);
        CheckString(value);
        size_t pos = value.find_first_of("KM");
        throwConfigError(pos != value.length() - 1 && pos != value.npos  , "Error in body size");
    }
}

void Parse_Config::getKeyValue(string &line , char set)
{
    size_t c;
    c = line.find(set);
    throwConfigError(c == line.npos , "webserv : Error line");

    key = line.substr(0 , c);
    value = line.substr(c + 1, line.npos);
    strtrim(key , " \t");
}


void Parse_Config::ft_getaddrinfo()
{
    vector<string> it;
    const char *host;
    addrinfo *res;
    int CodeErr; 

    it = data.server[string("port")];
    host = data.server["host"].data();

    for(unsigned long i = 0; i < it.size() ; i++)
    {
        res = NULL;
        CodeErr =  getaddrinfo(host, it[i].data(), NULL ,&res);
        throwConfigError(CodeErr  , gai_strerror(CodeErr));
        save_addr.push_back(res);
    }
}



void Parse_Config::push_back_data()
{
    push_back_location();
    if(data.error.empty() && data.server.empty() && data.location.empty())
        return;

    { // add default data server
    data.error.insert("404" , "www/errors/400.html"); // TODO:
    data.server.insert("host" , "127.0.0.1");
    data.server.insert("port" , "8080");
    data.server.insert("body_size" , "4096K");
    data.server.insert("server_name" , "localhost");
    }

    ft_getaddrinfo();
    blocks.push_back(data);
    data.server.clear(); data.error.clear(); data.location.clear();
}



void Parse_Config::push_back_location()
{
    if(loc.empty())
        return;

    { // add default data  location
    loc.insert("root" , "/usr/share/webserve/");
    
    }

    data.location.push_back(loc);
    loc.clear(); loc.clear();
}


void _fill()
{
    funcArray[8] = &Parse_Config::CheckInfoServer;
    funcArray[17] = &Parse_Config::CheckBlockLocation;
    funcArray[15] = &Parse_Config::CheckBlockErrors;

    funcSave[8] = &Parse_Config::push_back_data;
    funcSave[17] = &Parse_Config::push_back_location;
    funcSave[15] = &Parse_Config::test;
}


void Parse_Config::ParseFile(const char *filename)
{
    string line;
    int flag;
    std::ifstream file(filename , ios::out);
    
    flag = 0;
    _fill();
    for(int i = 0; std::getline(file , line) && strtrim(line , " \t") ; i++)
    {
        if(line.empty()) continue;
        if(count(NAMESOFBLOCKS, NAMESOFBLOCKS + 3, line))
        {
            flag = (int)line.size();
            (this->*funcSave[flag])();
            continue;
        }
        throwConfigError(!flag  , "webserve : Error");
        
        getKeyValue(line , '=');
        (this->*funcArray[flag])();
        
        key.clear(); value.clear();
        line.clear();func_ptr.clear();
    }
    (this->*funcSave[8])();
}
