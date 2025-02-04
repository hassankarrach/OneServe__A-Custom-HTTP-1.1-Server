#ifndef MAP_HPP
#define MAP_HPP

#include <map>
#include <vector>





namespace http
{

template<class T1 , class T2>

class map
{
private:
    std::map<T1 , T2> data;
public:
    map(){};
    ~map(){};
public:
    int count(T1 str)
    {
        return(data.count(str));
    }
    int find(std::string  str)
    {
        return(!data[str].empty());
    }
    int find(const char * str)
    {
        {
        std::vector<std::string> met = data["methods"]; 
        if(std::count(met.begin(), met.end(), str))
            return 1;
        }
    
        return(!data[str].empty());
    }
    int find(const char *s1 , const char *s2)
    {
        std::vector<std::string> save = data[s1];
        return(std::find(save.begin() , save.end() , s2 ) != save.end());
    }
    void clear()
    {
        data.clear();
    }
    int empty()
    {
        return (data.empty());
    }
    void insert(const char *s1 , const char *s2)
    {
        if(!find(s1))
            data[std::string(s1)].push_back(s2);
    }
    std::vector<std::string> & operator[](std::string str)
    {
        return(data[str]);
    }
    std::string operator[](const char *str)
    {
        return(data[str][0]);
    }
};


}



#endif