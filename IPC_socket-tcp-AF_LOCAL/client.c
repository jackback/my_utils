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
    int sockfd;
    socklen_t len;
    struct sockaddr_un address;
    int result;
    char ch = 'A';

    //create socket   
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    //name the server socket
    //makeAddr("server_socket", &address, &len);
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, SERVER_NAME);
    address.sun_path[0]=0;
    //len = sizeof(address);
    len =  strlen(SERVER_NAME)  + offsetof(struct sockaddr_un, sun_path);
    //connect to server
    result = connect(sockfd, (struct sockaddr*)&address, len);
    if(result == -1)
    {
        perror("opps:client1");
        exit(1);
    }
    //communicate with server socket
    while(1)
    {
        printf("set send content:");
        scanf("%c",&ch);
        write(sockfd, &ch, 1);
        printf("send to server:%c \n",ch);
        read(sockfd, &ch, 1);
        printf("read from server: %c\n", ch);
    }
    exit(0);
}





















