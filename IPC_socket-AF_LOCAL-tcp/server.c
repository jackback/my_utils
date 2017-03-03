#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<sys/un.h>
#include<unistd.h>
#include<stdlib.h>
#include <stddef.h>
  
#define SERVER_NAME_STREAM "@server_socket_STREAM"

/*
 * Create a UNIX-domain socket address in the Linux "abstract namespace". 
 * 
 * The socket code doesn't require null termination on the filename, but 
 * we do it anyway so string functions work. 
 */

int makeAddr(const char* name, struct sockaddr_un* pAddr, socklen_t* pSockLen)
{
    int nameLen = strlen(name);
    if (nameLen >= (int) sizeof(pAddr->sun_path) -1)  /* too long? */
        return -1;
    pAddr->sun_path[0] = '\0';  /* abstract namespace */
    strcpy(pAddr->sun_path+1, name);
    pAddr->sun_family = AF_LOCAL;
    *pSockLen = 1 + nameLen + offsetof(struct sockaddr_un, sun_path);
    return 0;
}

int main()
{
    //create socket
    int listen_sockfd;
    if ((listen_sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        perror("socket create failed");
        return -1;
    }

#if REUSE_SOCKET
    //设置套接字选项避免地址使用错误  
    int on = 1;
    if((setsockopt(listen_sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
        printf("setsockopt failed");
        exit(EXIT_FAILURE);
    }
#endif

    struct sockaddr_un server_addr;
    socklen_t server_len = 0;
    
    //unlink(SERVER_NAME_DGRAM);    //delete the old server socket 
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_LOCAL;
    strcpy(server_addr.sun_path, SERVER_NAME_STREAM);
    server_addr.sun_path[0] = 0; //USE "abstract namespace" local socket
    server_len = strlen(SERVER_NAME_STREAM)  + offsetof(struct sockaddr_un, sun_path);
    //makeAddr("server_socket", &server_addr, &server_len);
    if (bind(listen_sockfd, (struct sockaddr *)&server_addr, server_len) < 0) {
        perror("socket bind failed");
        return -1;
    }

    //listen the server
    if (listen(listen_sockfd, 5) < 0) {
        perror("socket create failed");
        return -1;
    }

    int client_sockfd = -1;
    struct sockaddr_un client_addr;
    socklen_t client_len = sizeof(struct sockaddr_un);
    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    
    char ch;
    int nread;
    while(1) {
        printf("\n\nserver waiting...\n");
        //accept client connect
        if(client_sockfd == -1) {
            if ((client_sockfd = accept(listen_sockfd, (struct sockaddr*)&client_addr, &client_len)) < 0) {
                continue;
            }
        }

        //read  data from client socket
        nread = read(client_sockfd, &ch, 1);
        if(nread == 0) { //client disconnected
            printf("client %s disconnected \n", &client_addr.sun_path[1]);
            client_sockfd = -1;
        } else {
            printf("read from client %s: %c \n", &client_addr.sun_path[1], ch);
            
            ch ++;
            printf("send to client %s: %c \n", &client_addr.sun_path[1], ch);
            write(client_sockfd, &ch, 1);
        }
        usleep(100);//1000 miliseconds = 1 second
    }

    return 0;
}












