#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>


#define SERVER_NAME_DGRAM "@server_socket_DGRAM"


int main()
{
    //create socket
    int sockfd;
    if ((sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0)) < 0) {
        perror("socket create failed");
        return -1;
    }

#if REUSE_SOCKET
    //设置套接字选项避免地址使用错误  
    int on = 1;
    if((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0){
        printf("setsockopt failed");
        exit(EXIT_FAILURE);
    }
#endif

    struct sockaddr_un server_addr;
    socklen_t server_len = 0;

    //unlink(SERVER_NAME_DGRAM);    //delete the old server socket 
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_LOCAL;
    strcpy(server_addr.sun_path, SERVER_NAME_DGRAM);
    server_addr.sun_path[0] = 0; //USE "abstract namespace" local socket
    //server_len = sizeof(server_addr);
    server_len = strlen(SERVER_NAME_DGRAM)  + offsetof(struct sockaddr_un, sun_path);
    if (bind(sockfd, (struct sockaddr *)&server_addr, server_len) < 0) {
        perror("socket bind failed");
        return -1;
    }

    struct sockaddr_un client_addr;
    socklen_t client_len = sizeof(struct sockaddr_un);  //as addr buffer when input
    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    
    char ch;
    int nread;
    while(1) {
        printf("\n\nserver waiting...\n");

        //read  data from client socket
        nread = recvfrom(sockfd, &ch, 1, 0, (struct sockaddr *)&client_addr, &client_len);
        if(nread == 0) { //client disconnected
            printf("client %d disconnected \n", sockfd);
            sockfd = -1;
        } else {
            printf("read from client %s: %c \n", &client_addr.sun_path[1], ch);
            
            ch ++;
            printf("send to client %s: %c \n", &client_addr.sun_path[1], ch);
            int ret = sendto(sockfd, &ch, 1, 0, (struct sockaddr *)&client_addr, client_len);
            if (ret < 0) {
                printf("server sendto failed %s(%d) \n", strerror(errno), errno);
            }
        }
        usleep(100);//1000 miliseconds = 1 second
    }

    return 0;
}












