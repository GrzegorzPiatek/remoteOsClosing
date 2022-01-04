#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "../constants.h"
#include <unistd.h> // for close
#include <pthread.h>

#define SA struct sockaddr

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


// Function designed for chat between client and server.
void func(int connfd)
{
    char buff[MAX_MSG_SIZE];
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX_MSG_SIZE);
   
        // read the message from client and copy it in buffer
        read(connfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        // printf("From client: %s\t To client : ", buff);
        printf("From client: %s\t", buff);
        bzero(buff, MAX_MSG_SIZE);
        // n = 0;
        // // copy server message in the buffer
        // while ((buff[n++] = getchar()) != '\n')
        //     ;
   
        // and send that buffer to client
        // write(connfd, buff, sizeof(buff));
   
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}

void * socketThread(void *arg)
{   
    char buff[MAX_MSG_SIZE];

    int newSocket = *((int *)arg);
    for(;;){
        if (read(newSocket , buff, MAX_MSG_SIZE) == 0){
            break;
        }
        printf("[%d]: From client: %s\t", newSocket buff);
    }

    printf("Exit socketThread \n");
    close(newSocket);
    pthread_exit(NULL);
}

// Driver function
int main()
{
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size = sizeof(serverStorage);


    // socket create and verification
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else{
        printf("Socket successfully created..\n");
    }

    bzero(&serverAddr, sizeof(serverAddr));
   
    // assign IP, PORT
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);
   
    // Binding newly created socket to given IP and verification
    if ((bind(serverSocket, (SA*)&serverAddr, sizeof(serverAddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    // Now server is ready to listen and verification
    if ((listen(serverSocket, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");

    pthread_t tid[60];
    int i = 0;
    while(1){
        //Accept call creates a new socket for the incoming connection
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

        //for each client request creates a thread and assign the client request to it to process
        //so the main thread can entertain next request
        if( pthread_create(&tid[i++], NULL, socketThread, &newSocket) != 0 )
            printf("Failed to create thread\n");

        if( i >= 50)
        {
            i = 0;
            while(i < 50)
            {
            pthread_join(tid[i++],NULL);
            }
            i = 0;
        }
    }

    // After chatting close the socket
    close(serverSocket);
}