// #include <stdio.h>
// #include <netdb.h>
// #include <netinet/in.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include "../constants.h"

// #define SA struct sockaddr
   
// // Function designed for chat between client and server.
// void func(int connfd)
// {
//     char buff[MAX_MSG_SIZE];
//     int n;
//     // infinite loop for chat
//     for (;;) {
//         bzero(buff, MAX_MSG_SIZE);
   
//         // read the message from client and copy it in buffer
//         read(connfd, buff, sizeof(buff));
//         // print buffer which contains the client contents
//         // printf("From client: %s\t To client : ", buff);
//         printf("From client: %s\t", buff);
//         bzero(buff, MAX_MSG_SIZE);
//         // n = 0;
//         // // copy server message in the buffer
//         // while ((buff[n++] = getchar()) != '\n')
//         //     ;
   
//         // and send that buffer to client
//         // write(connfd, buff, sizeof(buff));
   
//         // if msg contains "Exit" then server exit and chat ended.
//         if (strncmp("exit", buff, 4) == 0) {
//             printf("Server Exit...\n");
//             break;
//         }
//     }
// }
   
// // Driver function
// int main()
// {
//     int sockfd, connfd, len;
//     struct sockaddr_in servaddr, cli;
   
//     // socket create and verification
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd == -1) {
//         printf("socket creation failed...\n");
//         exit(0);
//     }
//     else
//         printf("Socket successfully created..\n");
//     bzero(&servaddr, sizeof(servaddr));
   
//     // assign IP, PORT
//     servaddr.sin_family = AF_INET;
//     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     servaddr.sin_port = htons(PORT);
   
//     // Binding newly created socket to given IP and verification
//     if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
//         printf("socket bind failed...\n");
//         exit(0);
//     }
//     else
//         printf("Socket successfully binded..\n");
   
//     // Now server is ready to listen and verification
//     if ((listen(sockfd, 5)) != 0) {
//         printf("Listen failed...\n");
//         exit(0);
//     }
//     else
//         printf("Server listening..\n");
//     len = sizeof(cli);
   
//     // Accept the data packet from client and verification
//     connfd = accept(sockfd, (SA*)&cli, &len);
//     if (connfd < 0) {
//         printf("server accept failed...\n");
//         exit(0);
//     }
//     else
//         printf("server accept the client...\n");
   
//     // Function for chatting between client and server
//     func(connfd);
   
//     // After chatting close the socket
//     close(sockfd);
// }


// C program for the Server Side
 
// inet_addr
#include <arpa/inet.h>
 
// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../constants.h"

// Semaphore variables
sem_t x, y;
pthread_t tid;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;
 
// Reader Function
void* reader(void* param)
{
    // Lock the semaphore
    sem_wait(&x);
    readercount++;
 
    if (readercount == 1)
        sem_wait(&y);
 
    // Unlock the semaphore
    sem_post(&x);
 
    printf("\n%d reader is inside",
           readercount);
 
    sleep(5);
 
    // Lock the semaphore
    sem_wait(&x);
    readercount--;
 
    if (readercount == 0) {
        sem_post(&y);
    }
 
    // Lock the semaphore
    sem_post(&x);
 
    printf("\n%d Reader is leaving",
           readercount + 1);
    pthread_exit(NULL);
}
 
// Writer Function
void* writer(void* param)
{
    printf("\nWriter is trying to enter");
 
    // Lock the semaphore
    sem_wait(&y);
 
    printf("\nWriter has entered");
 
    // Unlock the semaphore
    sem_post(&y);
 
    printf("\nWriter is leaving");
    pthread_exit(NULL);
}
 
// Driver Code
int main()
{
    // Initialize variables
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
 
    socklen_t addr_size;
    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);
 
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
 
    // Bind the socket to the
    // address and port number.
    bind(serverSocket,
         (struct sockaddr*)&serverAddr,
         sizeof(serverAddr));
 
    // Listen on the socket,
    // with 40 max connection
    // requests queued
    if (listen(serverSocket, 50) == 0)
        printf("Listening\n");
    else
        printf("Error\n");
 
    // Array for thread
    pthread_t tid[60];
 
    int i = 0;
 
    while (1) {
        addr_size = sizeof(serverStorage);
 
        // Extract the first
        // connection in the queue
        newSocket = accept(serverSocket,
                           (struct sockaddr*)&serverStorage,
                           &addr_size);
        int choice = 0;
        recv(newSocket,
             &choice, sizeof(choice), 0);
 
        if (choice == 1) {
            // Creater readers thread
            if (pthread_create(&readerthreads[i++], NULL,
                               reader, &newSocket)
                != 0)
 
                // Error in creating thread
                printf("Failed to create thread\n");
        }
        else if (choice == 2) {
            // Create writers thread
            if (pthread_create(&writerthreads[i++], NULL,
                               writer, &newSocket)
                != 0)
 
                // Error in creating thread
                printf("Failed to create thread\n");
        }
 
        if (i >= 50) {
            // Update i
            i = 0;
 
            while (i < 50) {
                // Suspend execution of
                // the calling thread
                // until the target
                // thread terminates
                pthread_join(writerthreads[i++],
                             NULL);
                pthread_join(readerthreads[i++],
                             NULL);
            }
 
            // Update i
            i = 0;
        }
    }
 
    return 0;
}