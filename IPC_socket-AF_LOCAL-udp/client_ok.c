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
    size_t msgLen;
    ssize_t numBytes;
    char resp[BUF_SIZE];

    char *abstract_client;
    char *abstract_server;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        errExit("%s msg...", argv[0]);

    /* Create client socket; bind to unique pathname (based on PID) */
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        errExit("socket");

    abstract_client = "viper_client";
    abstract_server = "viper_server";

    /* Construct address of client and bind it */
    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    strncpy(&claddr.sun_path[1], abstract_client, strlen(abstract_client)); 

    if (bind(sfd, (struct sockaddr *) &claddr, sizeof(sa_family_t) + strlen(abstract_client) + 1) == -1)
        errExit("bind");

    /* Construct address of server */
    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(&svaddr.sun_path[1], abstract_server, strlen(abstract_server));

    /* Send messages to server; echo responses on stdout */
    for (j = 1; j < argc; j++) {
        msgLen = strlen(argv[j]);       /* May be longer than BUF_SIZE */

       /* code FIX */
       printf("Client send %ld bytes to %s \n", (long) msgLen, &(svaddr.sun_path[1]));
       if (sendto(sfd, argv[j], msgLen, 0, (struct sockaddr *) &svaddr, (sizeof(sa_family_t) + strlen(abstract_server) + 1) ) != msgLen) 
              errExit("sendto");

        /* original - non working code - replaced with the code FIX above 
        if (sendto(sfd, argv[j], msgLen, 0, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) != msgLen)
        {
            fatal("sendto");
        } */

        numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
        /* Or equivalently: numBytes = recv(sfd, resp, BUF_SIZE, 0);
                        or: numBytes = read(sfd, resp, BUF_SIZE); */
        if (numBytes == -1)
            errExit("recvfrom");
        printf("Client received %d: %.*s from server \n", j, (int) numBytes, resp);
    }

    remove(claddr.sun_path);            /* Remove client socket pathname */
    exit(EXIT_SUCCESS);
}


