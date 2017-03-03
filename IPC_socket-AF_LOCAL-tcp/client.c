#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<sys/un.h>
#include<unistd.h>
#include<stdlib.h>
#include <stddef.h>

#define SERVER_NAME_STREAM "@server_socket_STREAM"    //USE "abstract namespace" local socket
#define CLIENT_NAME_STREAM   "@client_socket_STREAM"      //USE "abstract namespace" local socket

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
    int sockfd;
    if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        
        return -1;
    }

    struct sockaddr_un client_addr;
    socklen_t client_len = 0;
    
    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    client_addr.sun_family = AF_LOCAL;
    strcpy(client_addr.sun_path, CLIENT_NAME_STREAM);
    client_addr.sun_path[0] = 0; //USE "abstract namespace" local socket
    client_len = strlen(CLIENT_NAME_STREAM)  + offsetof(struct sockaddr_un, sun_path);
    
    if (bind(sockfd, (struct sockaddr *)&client_addr, client_len) < 0) {
        perror("socket bind failed");
        return -1;
    }
    
    //name the server socket
    //makeAddr("server_socket", &address, &len);
    struct sockaddr_un server_addr;
    socklen_t server_len = 0;
    
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_LOCAL;
    strcpy(server_addr.sun_path, SERVER_NAME_STREAM);
    server_addr.sun_path[0] = 0;
    server_len =  strlen(SERVER_NAME_STREAM)  + offsetof(struct sockaddr_un, sun_path);

    //connect to server
    if (connect(sockfd, (struct sockaddr*)&server_addr, server_len) < 0) {
        perror("socket connect failed");
        return -1;
    }
    
    //communicate with server socket
    char ch = 'A';
    while(1) {
        printf("input send content(1 byte): \n");
        scanf(" %c", &ch);
        
        printf("send to server(1 byte):%c \n", ch);
        write(sockfd, &ch, 1);
        
        read(sockfd, &ch, 1);
        printf("receive from server(1 byte): %c \n", ch);
    }
    
    exit(0);
}





















