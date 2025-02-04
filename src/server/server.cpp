
#include "../../includes/server/server.hpp"
using namespace std;

Server::Server()
{
    flag = 0;
    epfd = epoll_create(1);
}

void Server::CreatServer(vector<addrinfo *> hosts)
{
    int option = 1;
    for (unsigned long i = 0; i < hosts.size(); i++)
    {
        int fd = socket(hosts[i]->ai_family, hosts[i]->ai_socktype, hosts[i]->ai_protocol);

        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); //

        // int flags = fcntl(fd, F_GETFL, 0);
        // fcntl(fd, F_SETFL, flags | O_NONBLOCK);

        bind(fd, hosts[i]->ai_addr, hosts[i]->ai_addrlen);

        if (errno < 0)
            throw(std::logic_error(strerror(errno)));

        listen(fd, 1000);

        sockfds.push_back(fd);
        epoll_event event;

        event.events = EPOLLIN;
        event.data.fd = fd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
        // ADD_Events(fd, EPOLLIN ,EPOLL_CTL_ADD);
    }
}

bool Server::find(int fd)
{
    vector<int>::iterator it = std::find(sockfds.begin(), sockfds.end(), fd);
    return (it != sockfds.end());
}

Server::~Server()
{
    close(epfd);
};

void Server::ADD_Events(int _fd, EPOLL_EVENTS ev, int op)
{

    epoll_event event;

    event.events = ev;
    event.data.fd = _fd;
    epoll_ctl(epfd, op, _fd, &event);
}

void Server::ForEachEvents(epoll_event *events, int n_events)
{
    char buffer[4096];
    int fd;

    for (int i = 0; i < n_events; i++)
    {
        fd = events[i].data.fd;
        if (this->find(fd))
        {
            // Accept connection code remains the same
            std::cout << "\n\n============================ block connection ============================\n\n"
                      << std::endl;
            fd = accept(fd, NULL, NULL);
            serv[fd] = new my_class(fd);
            epoll_event event;
            event.events = EPOLLIN;
            event.data.fd = fd;
            epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
        }
        else if (events[i].events & EPOLLIN)
        {


            // begin 
            std::cout << "\n\n-------------------------- block request --------------------------\n\n"
                      << std::endl;
            ssize_t len = recv(fd, buffer, sizeof(buffer), MSG_DONTWAIT);

            if (len < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    continue;
                
                std::cout << "Receive error: " << strerror(errno) << std::endl;
                close(fd);
                serv.erase(fd);
                continue;
            }

            if (len == 0)
            {
                std::cout << "Connection closed by client" << std::endl;
                close(fd);
                serv.erase(fd);
                continue;
            }

            if (static_cast<size_t>(len) < sizeof(buffer))
                buffer[len] = '\0';

            int reqParser_res = serv[fd]->req.Parse(std::string(buffer, len));

            if (reqParser_res == 1)
            {
                std::cout << "Request parsing completed." << std::endl;
                serv[fd]->resData = serv[fd]->req.getResult();
                ADD_Events(fd, EPOLLOUT, EPOLL_CTL_MOD);
            }
            else if (reqParser_res == 0)
            {
                std::cout << "Request parsing not completed. Needs More." << std::endl;
            }
            else
            {
                // std::cout << "Bad Request: " << serv[fd]->req.getResult()._Error_msg << std::endl;
                Response::BadRequest(fd);
                close(fd);
                // serv.erase(fd);
            }
            // end
        }
        else if (events[i].events & EPOLLOUT)
        {
            cout << "\n\n+++++++++++++++++++++++++ block respond +++++++++++++++++++++++++\n\n"
                 << std::endl;
            Method::Type method = serv[fd]->resData._method;
            switch (method)
            {
            case Method::GET:
            {
                if (serv[fd]->res.Serve(fd, serv[fd]->resData))
                {
                    ADD_Events(fd, EPOLLIN, EPOLL_CTL_MOD);
                    delete serv[fd];
                    serv[fd] = new my_class(fd);
                }
                else
                {
                }
                break;
            }
            case Method::POST:
            {
                if (serv[fd]->res.Post(fd, serv[fd]->resData))
                {
                    ADD_Events(fd, EPOLLIN, EPOLL_CTL_MOD);
                    delete serv[fd];
                    serv[fd] = new my_class(fd);
                }
                else
                {
                    std::cout << "Post failed" << std::endl;
                    exit(1);
                }
                break;
            }
            case Method::DELETE:
            {
                if (serv[fd]->res.Delete(fd, serv[fd]->resData))
                {
                    ADD_Events(fd, EPOLLIN, EPOLL_CTL_MOD);
                    delete serv[fd];
                    serv[fd] = new my_class(fd);
                }
                break;
            }
            default:
                break;
            }
        }
    }
}

void Server::CreatMultiplexing()
{
    epoll_event events[sockfds.size()];
    std::map<int, my_class *>::iterator it;
    int n_events;
    it = serv.begin();

    while (true)
    {
        n_events = epoll_wait(epfd, events, sockfds.size(), 0);
        ForEachEvents(events, n_events);

        it = (it != serv.end() && it->second->check()) ? it++ : serv.begin();
    }
}
