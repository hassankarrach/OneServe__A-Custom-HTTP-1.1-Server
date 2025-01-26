
#include "../../includes/server/server.hpp"

using namespace std;


string Responde()
{

    string data;
    ifstream file("/home/abquaoub/Desktop/webserv/OneServe__A-Custom-HTTP-1.1-Server/src/index.html");
    std::string ss;
    getline(file , ss , '\0');
    std::ostringstream tt;

    tt << ss.size();

    data = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: html\r\nContent-Length: " ;
    data += tt.str();
    data += "\r\n\r\n";
    data += ss;
    return data;
}





Server::Server()
{
    epfd = epoll_create(1);

}


void Server::CreatServer(vector<addrinfo *> hosts)
{
    int option = 1;
    for(unsigned long i = 0; i < hosts.size(); i++)
    {
        int fd = socket(hosts[i]->ai_family , hosts[i]->ai_socktype , hosts[i]->ai_protocol);
        
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); // 
        
        bind(fd , hosts[i]->ai_addr , hosts[i]->ai_addrlen);
    
        if(errno < 0)
            throw(std::logic_error(strerror(errno)));
        
        listen(fd , 1000);

        sockfds.push_back(fd);
        epoll_event event;

        event.events =  EPOLLIN   ;
        event.data.fd = fd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
        // ADD_Events(fd, EPOLLIN ,EPOLL_CTL_ADD);
    }
}



int Server::find(int fd)
{
    vector<int>::iterator it = std::find(sockfds.begin() , sockfds.end() , fd );
    return *it;
}


Server::~Server()
{
    close(epfd);
};



void Server::ADD_Events(int _fd , EPOLL_EVENTS ev , int op )
{

    epoll_event event;

    event.events =  ev ;
    event.data.fd = _fd;
    epoll_ctl(epfd, op, _fd, &event);

}




void Server::ForEachEvents(epoll_event *events , int n_events )
{

    char buffer[4096];
    for(int i = 0; i < n_events ;i++)
    {
        int fd;
        if(events[i].data.fd == this->sockfds[0])
        {
            std::cout << "\n\n\n============================ block connection ============================\n\n\n" << std::endl;
            fd = accept(events[i].data.fd , NULL , NULL);
            serv[fd] = new my_class();

            ADD_Events(fd , EPOLLIN , EPOLL_CTL_ADD);
            // ADD_Events(events[i].data.fd , EPOLLHUP , EPOLL_CTL_MOD);
        }
        else if(events[i].events & EPOLLIN)
        {
            cout << "\n\n\n\n--------------------------block request --------------------------\n\n\n\n\n" << std::endl ; 
            
            
            int len = recv(events[i].data.fd , &buffer , sizeof(buffer) , 0);
            buffer[len] = 0;
            std::cout <<  "+++++++++++++++++++++++++++++++" << events[i].data.fd   << "\n" <<  buffer << std::endl;

            serv[events[i].data.fd]->req.Parse(buffer);
            serv[events[i].data.fd]->resData = serv[events[i].data.fd]->req.getResult();
            
            ADD_Events(events[i].data.fd , EPOLLOUT ,  EPOLL_CTL_MOD );
        }
        else if(events[i].events & EPOLLOUT)
        {
            fd = events[i].data.fd;
            cout << "\n\n\n\n--------------------------block respond --------------------------\n\n\n\n\n" << std::endl ; 
        
            
            if(serv[fd]->res.Serve(fd , serv[fd]->resData))
            {
                // ADD_Events(fd , EPOLLIN ,  EPOLL_CTL_MOD);
                std::cout << "hello" << std::endl;
                // serv[fd] = new my_class();
                close(fd);
            }
            
            

        }

        

    }


}
    





void Server::CreatMultiplexing()
{
    std::cout << sockfds.size() << std::endl;
    epoll_event events[sockfds.size()];
    int n_events;
    while(true)
    {
        n_events = epoll_wait(epfd, events, sockfds.size(), 0);
        ForEachEvents(events , n_events);
    }

}
