#include "../../includes/config/Config_Loader.hpp"


ConfigLoader::ConfigLoader() {}

ConfigLoader::~ConfigLoader() 
{
    server.clear();
    error.clear();
    location.clear();
}



std::string ConfigLoader::operator[](const char *str)
{
    if(server.find(str))
        return(server[str]);
    if(!error.find(str))
        return(string("www/errors/default.html"));

    return(error[str]);
}




t_map findd(string path , vector<t_map> &loc)
{

    vector<t_map>::iterator it = loc.begin();

    while(it != loc.end())
    {
        
        if(it->find("uri" , path.data()))
            return *it;
        it++;
    }
    


    
    if(path[path.size() - 1] == '/')
        path.erase(path.size() - 1 , 1);
    else
        path.erase(path.find_last_of('/')+1 , path.size() - (path.find_last_of('/') + 1));


    return(findd(path , loc));
}

t_map ConfigLoader::operator()(const char *path)
{
    t_map loc = findd(string(path) , location);
    return(loc);
}



int ConfigLoader::empty()
{
    return((error.empty() || location.empty() || server.empty()));
}





























///  functions  helper



int strtrim(std::string &str , const char *sharset)
{
    str = str.substr(0 , str.find_last_of('#'));
    size_t fpos = str.find_first_not_of(sharset);
    size_t lpos = str.find_last_not_of(sharset);
    if(fpos == str.npos )
        fpos = 0;

    str = str.substr(fpos , lpos - fpos + 1);
    return 1;
}
