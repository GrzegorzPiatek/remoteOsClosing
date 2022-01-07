#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../constants.h"
#include <sys/reboot.h> // close system

#define SA struct sockaddr


void sign_in(int sockfd, char* perm_lvl)
{
    char os_name[MAX_NAME_SIZE],
        msg[MAX_MSG_SIZE] = "new_os ";
    gethostname(os_name, MAX_NAME_SIZE);
    strcat(msg, os_name);
    strcat(msg, " ");
    strcat(msg, perm_lvl);
    printf("MSG: %s\n",msg);
    write(sockfd, msg, sizeof(msg));
}

void close_os(int sockfd){
    char msg[MAX_MSG_SIZE];
    read(sockfd, msg, MAX_MSG_SIZE);
    if (strstr(msg, "close_os")){
        close(sockfd);
        sync();
        reboot(RB_POWER_OFF);
    }
}


int main(int argc, char *argv[])
{
    if(argc < 2){
        fprintf(stderr, "Give minimum permission level (0-9) to close this OS as parameter!\n");
        exit(0);
    }
    else if (!isdigit(argv[1][0]) || strlen(argv[1]) > 1){
        fprintf(stderr, "Give minimum permission level from 0-9 (0 everybody will be able to close this OS)\n");
        exit(0);
    }


    int sockfd;
    struct sockaddr_in servaddr;
    
    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.108");
    servaddr.sin_port = htons(PORT);
   
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
   
    // register this pc by sending msg to server
    sign_in(sockfd, argv[1]);
   
    // w8 for msg to shutdown or restart
    close_os(sockfd);

    // close the socket
    close(sockfd);
}