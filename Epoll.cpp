#include "Epoll.h"
#include"errcon.h"
#include<unistd.h>
#include<cstring>
#define MAXEVENTS 1000


Epoll::Epoll() :ep_fd(-1),events(nullptr)
{
    ep_fd = epoll_create1(0);
    errif(ep_fd == -1,"epoll create fail");
    events = new epoll_event[MAXEVENTS];
}

Epoll::~Epoll()
{
    if(ep_fd != -1){
        close(ep_fd);
        ep_fd = -1;
    }
    delete [] events;
}

void Epoll::add_fd(int fd, uint32_t op)
{
    struct epoll_event ev;
    bzero(&ev,sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    if(epoll_ctl(ep_fd,EPOLL_CTL_ADD,fd,&ev)==-1,"epoll add erroe");
}

std::vector<epoll_event> Epoll::poll(int timeout)
{
    std::vector<epoll_event> activeEvents;
    int nfds = epoll_wait(ep_fd,events,MAXEVENTS,timeout);
    errif(nfds == -1,"epoll wait error");
    for(int i = 0;i < nfds;i++){
        activeEvents.push_back(events[i]);
    }
    return activeEvents;
}
