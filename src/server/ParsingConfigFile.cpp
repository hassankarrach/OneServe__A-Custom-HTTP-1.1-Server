#include "../../includes/server/ParsingConfigFile.hpp"

string checks = "";

map<int , void (ParsingConfigFile::*)()> funcArray;
map<int , void (ParsingConfigFile::*)()> funcSave;

t_data::t_data() {}

void ParsingConfigFile::test() {}

ParsingConfigFile::ParsingConfigFile(){}

vector<addrinfo *>ParsingConfigFile::getHosts()
{
    return save_addr;
}



t_data ParsingConfigFile::operator()(const char *_str)
{
    string str = _str;
    getKeyValue(str , ':');

    vector<t_data>::iterator it = blocks.begin();
    while(it != blocks.end())
    {
        if(it->server.find("host", key.data()) && it->server.find("port", value.data()))
            return(*it);
        it++;
    }
    throwServerError(true , "Error host is not exist");
    return *it;
}

ParsingConfigFile::~ParsingConfigFile()
{
    func_ptr.clear(); key.clear();
    value.clear(); filename.clear();
    blocks.clear();

};


int ParsingConfigFile::CheckCharacter(char c)
{
    for(unsigned long i = 0;i < func_ptr.size();i++)
        if(func_ptr[i](c)) return 1;
    return (checks.find(c) != checks.npos);
}


void ParsingConfigFile::CheckString(string &str)
{
    for(unsigned long i = 0; i < str.size() ; i++)
        throwServerError(!CheckCharacter(str[i]) , "webserv : Invalid String");
}



void ParsingConfigFile::CheckArrayOfValue(vector<string> &data , int flag)
{
    stringstream  obj(value);
    string word;
    size_t i = 0;
    for(i = 0; obj >> word; i++)
    {
        throwServerError(!flag && !count(&VALUES[0] ,&VALUES[3] , word) , "werbserv : invalid value1" );
        data.push_back(word);
    }
    throwServerError(flag == 1 && i != 1 , "werbserv : invalid value2" );
    throwServerError(flag == 3  && i != 2 , "werbserv : invalid value3" );
}



void ParsingConfigFile::CheckBlockErrors()
{
    throwServerError((key.length() != 3 || key[0] < '3' || key[0] > '5') , "webserv : must btw 300 to 599");
    func_ptr.push_back(isdigit); CheckString(key);

    flag = 1;
    throwServerError(data.error.find(key) , "webserve : dupblicate key error");
    CheckArrayOfValue(data.error[key] , flag);
}

void ParsingConfigFile::CheckBlockLocation()
{
    throwServerError(!count(KEYOFLOCATION , KEYOFLOCATION + 7 , key) ,  "webserv : location this key is not exist");

    flag = (key[0] == 'c') * 2 + (key[0] == 'm') * 0 + !(key[0] == 'm') * 1;
    throwServerError(key[0] != 'c' && loc.find(key) , "webserve : dupblicate key location");
    CheckArrayOfValue(loc[key] , flag);
}


void ParsingConfigFile::CheckInfoServer()
{
    throwServerError(!count(KEYOFSERVER , KEYOFSERVER + 4 , key) ,  "webserv : server this key is not exist");

    flag = (key[0] == 'p' || key[0] == 's') * 2 + !(key[0] == 'p' || key[0] == 's');
    throwServerError(data.server.find(key) , "webserve : dupblicate key server");
    CheckArrayOfValue(data.server[key] , flag);
    
    if(key[0] == 'b')
    {
        value = data.server[key][0];
        checks = "KM"; func_ptr.push_back(isdigit);
        CheckString(value);
        size_t pos = value.find_first_of("KM");
        throwServerError(pos != value.length() - 1 && pos != value.npos  , "Error in body size");
    }
}

void ParsingConfigFile::getKeyValue(string &line , char set)
{
    size_t c;
    c = line.find(set);
    throwServerError(c == line.npos , "webserv : Error line");

    key = line.substr(0 , c);
    value = line.substr(c + 1, line.npos);
    strtrim(key , " \t");
}


void ParsingConfigFile::ft_getaddrinfo()
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
        throwServerError(CodeErr  , gai_strerror(CodeErr));
        save_addr.push_back(res);
    }
}



void ParsingConfigFile::push_back_data()
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



void ParsingConfigFile::push_back_location()
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
    funcArray[8] = &ParsingConfigFile::CheckInfoServer;
    funcArray[17] = &ParsingConfigFile::CheckBlockLocation;
    funcArray[15] = &ParsingConfigFile::CheckBlockErrors;

    funcSave[8] = &ParsingConfigFile::push_back_data;
    funcSave[17] = &ParsingConfigFile::push_back_location;
    funcSave[15] = &ParsingConfigFile::test;
}


void ParsingConfigFile::ParseFile(const char *filename)
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
        throwServerError(!flag  , "webserve : Error");
        
        getKeyValue(line , '=');
        (this->*funcArray[flag])();
        
        key.clear(); value.clear();
        line.clear();func_ptr.clear();
    }
    (this->*funcSave[8])();
}
