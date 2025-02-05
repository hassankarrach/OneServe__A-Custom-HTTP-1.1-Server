


#include "../../includes/config/Config_Loader.hpp"



namespace http 
{  

template<class T1 , class T2>
int map<T1 , T2>::count(T1 str)
{
        return(data.count(str));
}

template<class T1 , class T2>
int map<T1 , T2>::find(std::string  str)
{
    return(!data[str].empty());
}


template<class T1 , class T2>
int map<T1 , T2>::find(const char * str)
{
    {
    std::vector<std::string> met = data["methods"]; 
    if(std::count(met.begin(), met.end(), str))
        return 1;
    }

    return(!data[str].empty());
}


template<class T1 , class T2>
int map<T1 , T2>::find(const char *s1 , const char *s2)
{
    std::vector<std::string> save = data[s1];
    return(std::find(save.begin() , save.end() , s2 ) != save.end());
}


template<class T1 , class T2>
void map<T1 , T2>::clear()
{
    data.clear();
}


template<class T1 , class T2>
int map<T1 , T2>::empty()
{
    return (data.empty());
}


template<class T1 , class T2>
void map<T1 , T2>::insert(const char *s1 , const char *s2)
{
    if(!find(s1))
        data[std::string(s1)].push_back(s2);
}


template<class T1 , class T2>
std::vector<std::string> & map<T1 , T2>::operator[](std::string str)
{
    return(data[str]);
}

template<class T1 , class T2>
std::string  map<T1 , T2>::operator[](const char *str)
{
    return(data[str][0]);
}


}
