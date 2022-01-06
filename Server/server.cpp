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

struct os_info{
    char os_name[MAX_OS_NAME_SIZE];
    int permission_lvl;
    int socket;
} os[MAX_OS_NUMBER];

struct message{
    char action[MAX_ACTION_SIZE];
    char name[MAX_ACTION_SIZE];
    int number;
};

// pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void runMsg(message msg, int socketfd){
    printf("-D: runMsg");
    printf("action: %s, name: %s, number: %d \n", msg.action, msg.name, msg.number);
}

// void add_new_os(){

// }

message string2msg(char* str){
    char *token = strtok(str, " ");
    message f_msg;

    strcpy(f_msg.action, token);
    strcpy(f_msg.name, strtok(NULL, " "));
    f_msg.number = atoi(strtok(NULL, " "));
    
    return f_msg;
}

void * socketThread(void *arg){
    char raw_msg[MAX_MSG_SIZE];

    int socketfd = *((int *)arg);
    for(;;){
        if (read(socketfd , raw_msg, MAX_MSG_SIZE) == 0){
            break;
        }
        printf("[%d]: Received: %s", socketfd, raw_msg);
        message msg = string2msg(raw_msg);
        printf("-D: after create msg from string");
        runMsg(msg, socketfd);
    }

    printf("[%d]: Exit socketThread \n", socketfd);
    close(socketfd);
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