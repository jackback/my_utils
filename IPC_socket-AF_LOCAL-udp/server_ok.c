#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>


#define BUF_SIZE 256

void errExit(const char *errMsg, ...) {
    va_list args;       //定义一个va_list类型的变量，用来储存单个参数  
    va_start(args, errMsg); //使args指向可变参数的第一个参数  
    vprintf(errMsg, args);  //必须用vprintf等带V的  
    va_end(args);       //结束可变参数的获取 

    printf("\n");
    exit(1);
}


int main(int argc, char *argv[])
{
    struct sockaddr_un svaddr, claddr;
    int sfd, j;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];

    char *abstract_server;

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);       /* Create server socket */
    if (sfd == -1)
        errExit("socket");

    /* Construct well-known address and bind server socket to it */
    abstract_server = "viper_server";

    if (remove(abstract_server) == -1 && errno != ENOENT)
        errExit("remove-%s", abstract_server);

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(&svaddr.sun_path[1], abstract_server, strlen(abstract_server)); //svaddr.sun_path[0]=0

    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(sa_family_t) + strlen(abstract_server) + 1) == -1)
        errExit("bind");

    /* Receive messages, convert to uppercase, and return to client */

    for (;;) {
        len = sizeof(struct sockaddr_un);  //addr buffer length
        numBytes = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *) &claddr, &len);
        if (numBytes == -1)
            errExit("recvfrom");

        printf("Server received %ld bytes from %s \n", (long) numBytes, &(claddr.sun_path[1]));

        for (j = 0; j < numBytes; j++)
            buf[j] = toupper((unsigned char) buf[j]);

        if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *) &claddr, len) != numBytes)
            errExit("sendto");
        else
            printf("Server send %ld bytes to %s \n", (long) numBytes, &(claddr.sun_path[1]));
     }
 }


