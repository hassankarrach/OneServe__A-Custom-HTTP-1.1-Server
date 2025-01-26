
#include "../../includes/server/ParsingConfigFile.hpp"

string checks = "";

t_data::t_data() {}

void ParsingConfigFile::test() {}

void split(string &s , vector<string > &data)
{
    stringstream  obj(s);
    string word;
    while (obj >> word)
        data.push_back(word);
}

















t_data ParsingConfigFile::operator()(const char *host)
{

    vector<pair<string , t_data> >::iterator it = blocks.begin();
    t_data save;
    while(it != blocks.end())
    {
        if(!it->first.compare(host))
        {
            save =  it->second;
            break;
        }
        it++;
    }
    return save;
}



ParsingConfigFile::ParsingConfigFile()
{
    res = NULL;

}

ParsingConfigFile::~ParsingConfigFile()
{
    func_ptr.clear(); key.clear();
    value.clear(); filename.clear();
    blocks.clear();

};


int strtrim(string &str , const char *sharset)
{
    size_t fpos = str.find_first_not_of(sharset);
    size_t lpos = str.find_last_not_of(sharset);
    if(fpos == str.npos)
        fpos = 0;
    str = str.substr(fpos , lpos - fpos + 1);
    return 0;
}

int ParsingConfigFile::CheckCharacter(char c)
{

    for(unsigned long i = 0;i < func_ptr.size();i++)
    {
        if(func_ptr[i](c))
            return 1;
    }
    return (checks.find(c) != checks.npos);
}


void ParsingConfigFile::CheckString(string &str)
{
    for(unsigned long i = 0; i < str.size() ; i++)
    {
        if(!CheckCharacter(str[i]))
        {
            std::cout << str << std::endl;
            throw(std::logic_error("webserv : Invalid String"));
        }
    }
}



void ParsingConfigFile::CheckArrayOfValue(vector<string> &data , int first , int size)
{
    if(!limit && data.size() >  1)
        throw(std::range_error("werbserv : invalid arguments"));

    for(size_t i = 0; i < data.size(); i++)
    {
        if(!flag && find(&VALUES[first] , &VALUES[first + size] , data[i]) == &VALUES[first + size])
            throw(std::range_error("werbserv : invalid value"));
        else
            CheckString(data[i]);
    }
}



void ParsingConfigFile::CheckBlockErrors()
{
    if(key.length() != 3 || key[0] < '3' || key[0] > '5')
        throw(std::range_error("webserv : must btw 300 to 599"));

    func_ptr.push_back(isdigit); CheckString(key);
    func_ptr[0] = isalnum; checks = "./_" ; CheckString(value);
    this->data.error[key] = value;
    
}


void ParsingConfigFile::CheckBlockLocation()
{
    if(find(KEYOFLOCATION , KEYOFLOCATION + 5 , key) == (KEYOFLOCATION + 5))
        throw(std::runtime_error("webserv : location this key is not exist"));

    split(value , pair_location.second[key]);
}


void ParsingConfigFile::CheckInfoServer()
{
    if(find(KEYOFSERVER , KEYOFSERVER + 4 , key) == (KEYOFSERVER + 4))
        throw(std::runtime_error("webserv : server this key is not exist"));
    split(value , data.server[key]);
}

void ParsingConfigFile::getKeyValue(string &line)
{
    size_t c = line.find("="); line.at(c);

    key = line.substr(0 , c);
    value = line.substr(c + 1, line.npos);
    strtrim(key , " \t");strtrim(value , " \t");
    checks = "_"; func_ptr.push_back(isalnum);
    CheckString(key);
    checks = " _/."; 
    CheckString(value);
}



void ParsingConfigFile::Checkblock(t_data &info)
{
    limit = 0; flag = 1;
    func_ptr.push_back(isalnum); checks = "/." ;
    CheckArrayOfValue(info.location.begin()->second["uri"] , 0 , 0);

    limit = 1;
    CheckArrayOfValue(info.location.begin()->second["methods"] , 2 , 3);
}


vector<addrinfo *>ParsingConfigFile::getHosts()
{
    return save_addr;
}




void ParsingConfigFile::push_back_data()
{
    push_back_location();
    if(data.error.empty() && data.server.empty() && data.location.empty())
        return;
    t_data _data  = data;
    save_block.second = _data;
    save_block.first = save_block.second.server["host"][0].data();
    save_block.first += ":";
    save_block.first += save_block.second.server["port"][0].data();
    
    pair<string , t_data>  save = save_block;
    res = NULL;
    int CodeErr =  getaddrinfo(data.server["host"][0].data(), data.server["port"][0].data(), NULL ,&res);

    if(CodeErr < 0)
        throw(std::logic_error(gai_strerror(CodeErr)));
    
    save_addr.push_back(res);
    blocks.push_back(save);

    data.server.clear(); data.error.clear(); data.location.clear();
}



void ParsingConfigFile::push_back_location()
{
    if(pair_location.second.empty())
        return;

    pair<string , map<string , vector<string> > > save;
    pair_location.first = pair_location.second["uri"][0].data();
    save = pair_location;
    data.location.push_back(save);
    Checkblock(data);

    pair_location.first.clear(); pair_location.second.clear();
}



void ParsingConfigFile::ParseFile(const char *filename)
{
    string line;
    int flag = 0;
    std::ifstream file(filename , ios::out);

    map<int , void (ParsingConfigFile::*)()> funcArray;
    funcArray[8] = &ParsingConfigFile::CheckInfoServer;
    funcArray[17] = &ParsingConfigFile::CheckBlockLocation;
    funcArray[15] = &ParsingConfigFile::CheckBlockErrors;

    map<int , void (ParsingConfigFile::*)()> funcSave;
    funcSave[8] = &ParsingConfigFile::push_back_data;
    funcSave[17] = &ParsingConfigFile::push_back_location;
    funcSave[15] = &ParsingConfigFile::test;


    while(std::getline(file , line) && !strtrim(line , " \t"))
    {
        if(line.empty()) continue;
        
        if((find(NAMESOFBLOCKS, NAMESOFBLOCKS + 3, line) != (NAMESOFBLOCKS + 3)))
        {
            flag = (int)line.size();
            (this->*funcSave[flag])();
            continue;
        }

        getKeyValue(line);
        (this->*funcArray[flag])();

        key.clear(); value.clear(); line.clear();
        checks.clear(); func_ptr.clear();
    }
    (this->*funcSave[8])();

}

