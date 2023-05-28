#include"Inetaddress.h"
#include<unistd.h>
#include<cstring>
Inetaddress::Inetaddress() : addr_len(sizeof(addr))
{
    bzero(&addr,sizeof(addr));
}

Inetaddress::Inetaddress(const char *ip, uint16_t port) : addr_len(sizeof(addr))
{
    bzero(&addr,sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr_len = sizeof(addr);
}

Inetaddress::~Inetaddress()
{
}
