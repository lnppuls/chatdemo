#pragma once
#include<arpa/inet.h>

class Inetaddress
{
public:
    struct sockaddr_in addr;
    socklen_t addr_len;
    Inetaddress();
    Inetaddress(const char *ip,uint16_t port);
    ~Inetaddress();
};
