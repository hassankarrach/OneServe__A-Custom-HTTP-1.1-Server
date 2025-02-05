#include "../../includes/config/Config_Loader.hpp"





ConfigLoader::ConfigLoader() {}

ConfigLoader::~ConfigLoader() {}



std::string ConfigLoader::operator[](const char *str)
{
    if(server.find(str))
        return(server[str]);
    return(error[str]);
}

t_map ConfigLoader::operator()(const char *path)
{
    vector<t_map>::iterator it = location.begin();

    while(it != location.end())
    {
        
        if(it->find("uri" , path))
            return *it;
        it++;
    }
    return(*location.begin());
}



int ConfigLoader::empty()
{
    return((error.empty() || location.empty() || server.empty()));
}






























///  functions  helper

void  throwConfigError(bool expr , const char *str)
{
    if(expr) throw std::runtime_error(str);
}


int strtrim(std::string &str , const char *sharset)
{
    size_t fpos = str.find_first_not_of(sharset);
    size_t lpos = str.find_last_not_of(sharset);
    if(fpos == str.npos )
        fpos = 0;

    str = str.substr(fpos , lpos - fpos + 1);
    return 1;
}
