
#include "../../includes/server/t_data.hpp"


t_map t_data::operator()(const char *path)
{
    vector< pair<string , t_map > >::iterator it = this->location.begin();

    t_map save; 

    while(it != this->location.end())
    {
        if(!it->first.compare(path))
        {
            save = it->second; 
            break;
        }
        it++;
    }
    if(save.empty())
        std::cout << "is empty" << std::endl;
    return save;
}




vector<string>  t_data::operator[](const char *path)
{
    std::cout << path << std::endl;
    if(server.find(path) != server.end())
    {
        std::cout << "hello world" << std::endl;
       server[path].push_back(""); 
       exit(0);
    }
    
    return this->server[path];
}



string t_data::operator[](int code)
{
    stringstream ss;
    ss << code;
    return(this->error[ss.str()]);
}



int t_data::empty()
{
    return((error.empty() || location.empty() || server.empty()));
}


