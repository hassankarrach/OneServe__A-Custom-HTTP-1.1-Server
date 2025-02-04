#include "../../includes/server/t_data.hpp"



void  throwServerError(bool expr , const char *str)
{
    if(expr)
        throw std::runtime_error(str);
}



t_map t_data::operator()(const char *path)
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



int t_data::empty()
{
    return((error.empty() || location.empty() || server.empty()));
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
