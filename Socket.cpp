#include<Socket.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include"errcon.h"
#include<fcntl.h>
Socket::Socket() : fd(-1){
    this->fd = socket(AF_INET,SOCK_STREAM,0);
    errif((fd == -1),"socket cannot create");
}

Socket::Socket(int domain, int type, int protocol) : fd(-1)
{
    this->fd = socket(domain,type,protocol);
    errif((fd == -1),"socket cannot create");
}

void Socket::bind(Inetaddress * addr)
{
    ::bind(this->fd,(sockaddr *)&addr->addr,addr->addr_len);
}

void Socket::listen()
{
    ::listen(this->fd,SOMAXCONN);
}

void Socket::setnonblocking(){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(Inetaddress *addr)
{
    int cli_fd = ::accept(this->fd,(sockaddr *)&addr->addr,(socklen_t *)&addr->addr_len);
    errif(cli_fd == -1,"accept error");
    return cli_fd;
}

int Socket::getFd()
{
    return this->fd;
}
