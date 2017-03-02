#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<sys/un.h>
#include<unistd.h>
#include<stdlib.h>
#include <stddef.h>
  
#define SERVER_NAME "@server_socket"

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
    pAddr->sun_family = AF_UNIX;
    *pSockLen = 1 + nameLen + offsetof(struct sockaddr_un, sun_path);
    return 0;
}

int main()
{
    int server_sockfd, client_sockfd;
    socklen_t server_len, client_len;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    char ch;
    int nread;

    //delete the old server socket
    //unlink("server_socket");
    //create socket
    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

#if REUSE_SOCKET
    //设置套接字选项避免地址使用错误  
    int on = 1;
    if((setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0){
        printf("setsockopt failed");
        exit(EXIT_FAILURE);
    }
#endif

    //name the socket
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SERVER_NAME);
    server_addr.sun_path[0]=0;
    //server_len = sizeof(server_addr);
    server_len = strlen(SERVER_NAME)  + offsetof(struct sockaddr_un, sun_path);
    //makeAddr("server_socket", &server_addr, &server_len);
    bind(server_sockfd, (struct sockaddr *)&server_addr, server_len);

    //listen the server
    listen(server_sockfd, 5);
    client_sockfd = -1;
    client_len = sizeof(client_addr);
    while(1){
        printf("\n\nserver waiting...\n");
        //accept client connect
        if(client_sockfd == -1){
            client_sockfd = accept(server_sockfd,(struct sockaddr*)&client_addr, &client_len);
        }

        //read  data from client socket
        nread = read(client_sockfd, &ch, 1);
        if(nread == 0){//client disconnected
            printf("client %d disconnected\n",client_sockfd);
            client_sockfd = -1;
        }
        else{
            printf("read from client %d: %c\n",client_sockfd,ch);
            ch ++;
            printf("send to client %d: %c\n",client_sockfd,ch);
            write(client_sockfd, &ch, 1);
        }
        usleep(100);//1000 miliseconds = 1 second
    }

    return 0;
}












