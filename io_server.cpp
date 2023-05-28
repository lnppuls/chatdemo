#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstring>
#include<unistd.h>
#include<sys/epoll.h>
#include<fcntl.h>

#define MAX_EVENTS 64

struct epoll_event events[MAX_EVENTS],ev;

int setnonblocking(int fd){
    int old_flag = fcntl(fd,F_GETFL);
    int new_flag = old_flag | O_NONBLOCK;
    fcntl(fd,F_SETFL,new_flag);
    return old_flag;
}


void handl(int fd,int i){
    char buff[1024];
    while(true){   
        bzero(buff,sizeof(buff));
        int read_ret = read(fd,buff,sizeof(buff));
        if(read_ret > 0){
            printf("from:%d,%s\n",fd,buff);
            write(fd,buff,sizeof(buff));
        }
        else if(read_ret == 0){
            close(events[i].data.fd);   //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
        else{
            break;
        }
    }
    
}

int main(){
    int se_sockfd = socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in se_addr;
    se_addr.sin_addr.s_addr = INADDR_ANY;
    se_addr.sin_family = AF_INET;
    se_addr.sin_port = htons(8888);
    bind(se_sockfd,(sockaddr *)&se_addr,sizeof(se_addr));
    listen(se_sockfd,SOMAXCONN);
    int epfd = epoll_create1(0); //参数表示监听事件的大小

    ev.events = EPOLLIN; //使用ET模式
    ev.data.fd = se_sockfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,se_sockfd,&ev);
    while (true)
    {
        int fdnum = epoll_wait(epfd,events,MAX_EVENTS,-1);
        for(int i = 0;i < fdnum;i++){
            struct sockaddr_in clint_addr;
            bzero(&clint_addr,sizeof(clint_addr));
            socklen_t cli_len = (socklen_t)sizeof(clint_addr);
            if(events[i].data.fd == se_sockfd){   //发生事件的fd是服务器socket fd，表示有新客户端连接
                int cli_fd = accept(se_sockfd,(sockaddr *)&clint_addr,&cli_len);
                printf("new client fd %d! IP: %s Port: %d\n",cli_fd, inet_ntoa(clint_addr.sin_addr), ntohs(clint_addr.sin_port));
                ev.data.fd = cli_fd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(cli_fd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, cli_fd, &ev);
            }
            else if(events[i].events & EPOLLIN){  // 发生事件的是客户端
                handl(events[i].data.fd,i);
            }
        }
    }
    

}