#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstring>
#include<unistd.h>
#include<pthread.h>
#include<string>
#include<queue>
#include<ctime>
#include<stdlib.h>
#include<fcntl.h>

#define MAX_Clint 32
#define NET_TYPE AF_INET
#define STR_TYPE SOCK_STREAM
#define BUFFSIZE 1024

//lock
pthread_mutex_t Clint_lock;
pthread_mutex_t smess_lock;

//clint queue
struct Clint
{
    int sock_fd;
    struct sockaddr_in c_addr;
    int state;
    socklen_t caddr_len;
    int id;
}clint_list[32];


int id_idx = 0;
//server init
int s_sockfd;
struct sockaddr_in s_addr;


struct message
{
    std::string message;
    int id;
};
std::queue<struct message> message;


int s_init(){
    //init lock  
    pthread_mutex_init(&Clint_lock,NULL);
    pthread_mutex_init(&smess_lock,NULL);
    //init server
    s_sockfd = socket(NET_TYPE,STR_TYPE,0);
    if(s_sockfd == -1){
        close(s_sockfd);
        return -1;
    }
    fcntl(s_sockfd, F_SETFL, O_NONBLOCK);
    s_addr.sin_family = NET_TYPE;
    s_addr.sin_port = htons(8888);
    s_addr.sin_addr.s_addr = INADDR_ANY;
    int reint = bind(s_sockfd,(sockaddr *)&s_addr,sizeof(s_addr));
    if(reint == -1){
        close(s_sockfd);
        return -1;
    }

    //init clintlist
    for(int i = 0;i < MAX_Clint;i++){
        clint_list[i].state = 0;
        bzero(&clint_list[i].c_addr,sizeof(clint_list[i].c_addr));
    }
    return 1;
}



void * s_listen(void *arg){
    printf("listen in\n");
    while (true)
    {
        listen(s_sockfd,SOMAXCONN);
        pthread_mutex_lock(&Clint_lock);
        int cnt = 0;
        for(int i = 0;i < MAX_Clint;i++){
            if(clint_list[i].state == 0){
                char s[] = "connect success!\n";
                clint_list[i].caddr_len = sizeof(clint_list[i].c_addr);
                int newsockfd =  accept(s_sockfd,(sockaddr *)&clint_list[i].c_addr,&(clint_list[i].caddr_len));
                if(newsockfd >= 0)
                {
                    cnt++;
                    clint_list[i].sock_fd = newsockfd;
                    clint_list[i].id = id_idx++;
                    clint_list[i].state = 1;
                    printf("new client fd %d! IP: %s Port: %d\n",clint_list[i].sock_fd,
                     inet_ntoa(clint_list[i].c_addr.sin_addr), ntohs(clint_list[i].c_addr.sin_port));
                    write(clint_list[i].sock_fd,s,sizeof(s));
                    break;
                }
                break;
            }
        }
        pthread_mutex_unlock(&Clint_lock);
        sleep(1);
    }
}

void *save_massage(void *arg){
    printf("save in\n");
    char buff[BUFFSIZE];
    while (true)
    {
        for(int i = 0;i < MAX_Clint;i++){
            if(clint_list[i].state != 0){
                bzero(&buff,sizeof(buff));
                int read_bytes = read(clint_list[i].sock_fd,buff,sizeof(buff));
                if(read_bytes > 0){
                    printf("get new mess from %d:%s\n",i,buff);
                    struct message now;
                    pthread_mutex_lock(&smess_lock);
                    std::string mess = buff;
                    now.message = mess;
                    now.id = clint_list[i].id;
                    message.push(now);
                    pthread_mutex_unlock(&smess_lock);
                    sleep(0.01);
                }
                else if(read_bytes == 0){
                    pthread_mutex_lock(&Clint_lock);
                    clint_list[i].state = 0;
                    printf("clint %d: %s deconnect!\n",i,inet_ntoa(clint_list[i].c_addr.sin_addr));
                    pthread_mutex_unlock(&Clint_lock);
                }
            }
        }

    }
}
void * send_message(void * arg){
    printf("send in\n");
    while (true)
    {
        pthread_mutex_lock(&smess_lock);
        if(!message.empty()){
            struct message now = message.front();
            char buff[BUFFSIZE];
            bzero(&buff,sizeof(buff));
            strcpy(buff,now.message.c_str());
           printf("papre send back message:%s\n",buff);
            pthread_mutex_lock(&Clint_lock);
            for(int i = 0;i < MAX_Clint;i++){
                if(clint_list[i].state == 1){
                    write(clint_list[i].sock_fd,buff,sizeof(buff));
                    printf("send back all success\n");
                }
            }
            pthread_mutex_unlock(&Clint_lock);
            message.pop();
            printf("has message:%ld\n\n",message.size());
        }
        pthread_mutex_unlock(&smess_lock);
    }
    
    
}

int main(){
    while (s_init() == -1)
    {
        s_init();
    }
    printf("init success!\n");
    pthread_t t_lis,t_save,t_send;
    pthread_create(&t_lis,NULL,s_listen,NULL);
    pthread_create(&t_send,NULL,send_message,NULL);
    pthread_create(&t_save,NULL,save_massage,NULL);
    pthread_join(t_lis,NULL);
    pthread_join(t_send,NULL);
    pthread_join(t_save,NULL);
    return 0;
}
