#pragma once 
#include"Inetaddress.h"

class Inetaddress;

class Socket
{
private:
    int fd;
public:
    Socket();
    Socket(int domain,int type,int protocol);
    ~Socket();

    void bind(Inetaddress *);
    void listen();
    void setnonblocking();

    int accept(Inetaddress *);

    int getFd();
};