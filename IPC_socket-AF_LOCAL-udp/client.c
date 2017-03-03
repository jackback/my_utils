#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<sys/un.h>
#include<unistd.h>
#include<stdlib.h>
#include <stddef.h>
#include <errno.h>

#define SERVER_NAME_DGRAM "@server_socket_DGRAM"     //USE "abstract namespace" local socket
#define CLIENT_NAME_DGRAM   "@client_socket_DGRAM"       //USE "abstract namespace" local socket


int main()
{
    //create socket
    int sockfd;
    if ((sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    struct sockaddr_un client_addr;
    socklen_t client_len = 0;
    
    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    client_addr.sun_family = AF_LOCAL;
    strcpy(client_addr.sun_path, CLIENT_NAME_DGRAM);
    client_addr.sun_path[0] = 0; //USE "abstract namespace" local socket
    client_len = strlen(CLIENT_NAME_DGRAM)  + offsetof(struct sockaddr_un, sun_path);
    
    if (bind(sockfd, (struct sockaddr *)&client_addr, client_len) < 0) {
        perror("socket bind failed");
        return -1;
    }

    //name the server socket
    struct sockaddr_un server_addr;
    socklen_t server_len = 0;
    
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_LOCAL;
    strcpy(server_addr.sun_path, SERVER_NAME_DGRAM);
    server_addr.sun_path[0] = 0;
    server_len = strlen(SERVER_NAME_DGRAM)  + offsetof(struct sockaddr_un, sun_path);

    //communicate with server socket
    char ch = 'A';
    while(1) {
        printf("input send content(1 byte): \n");
        scanf(" %c", &ch);
        
        printf("send to server(1 byte):%c \n", ch);
        int ret = sendto(sockfd, &ch, 1, 0, (struct sockaddr *)&server_addr, server_len);
        if (ret < 0) {
            printf("client sendto failed %s(%d) \n", strerror(errno), errno);
        }
        
        ret = recvfrom(sockfd, &ch, 1, 0, (struct sockaddr *)&server_addr, &server_len);
        if (ret < 0) {
            printf("client recvfrom failed %s(%d) \n", strerror(errno), errno);
        }
        
        printf("receive from server(1 byte): %c \n", ch);
    }
    
    exit(0);
}





















