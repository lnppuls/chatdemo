#include<iostream>
#include"Epoll.h"
#include"errcon.h"
#include"Socket.h"

int main(){
    Socket * ser_scoket = new Socket();
    Inetaddress *ser_addr = new Inetaddress(INADDR_ANY,8888);
    ser_scoket->bind(ser_addr);
    ser_scoket->listen();
    Epoll *ep = new Epoll();
    ser_scoket->setnonblocking();
    ep->add_fd(ser_scoket->getFd(),EPOLLIN | EPOLLET);
    while (true)
    {
        
    }
    
}