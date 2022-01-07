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

int os_counter = 0, user_counter = 0;

struct os_info{
    char name[MAX_NAME_SIZE];
    int permission_lvl;
    int socketfd;
} os[MAX_OS_NUMBER];

struct user_info{
    char name[MAX_NAME_SIZE];
    int permission_lvl;
    int socketfd;
} user[MAX_USER_NUMBER];

struct message{
    char action[MAX_ACTION_SIZE];
    char name[MAX_ACTION_SIZE];
    int number;
};

pthread_mutex_t guard = PTHREAD_MUTEX_INITIALIZER;

int findOsIndex(char *os_name){
    for(int i=0; i<os_counter; i++ ){
        if( strcmp( os[i].name, os_name)  == 0 ){
            return i;
        }
    }
    return -1;
}

int findUserIndex(char *username){
    for(int i=0; i<user_counter; i++ ){
        if( strcmp( user[i].name, username)  == 0 ){
            return i;
        }
    }
    return -1;
}

void add_new_os(char *os_name, int permission_lvl, int socketfd){
    if(int os_index = findOsIndex(os_name) >= 0){
        os[os_index].permission_lvl = permission_lvl;
        os[os_index].socketfd = socketfd;
        printf("<log> New OS[%d]: %s perm_lvl: %d\n", os_index, os_name, permission_lvl);

    }
    else{
        strcpy(os[os_counter].name, os_name);
        os[os_counter].permission_lvl = permission_lvl;
        os[os_counter].socketfd = socketfd;
        os_counter++;
        printf("<log> New OS[%d]: %s perm_lvl: %d\n", os_index, os_name, permission_lvl);
    }
}

void add_new_user(char *username, int permission_lvl, int socketfd){
    if(int user_index = findUserIndex(username) >= 0){
        user[user_index].socketfd = socketfd;
        printf("<log> Update User[%d]: %s perm_lvl: %d\n", user_index, username, permission_lvl);

    }
    else{
        strcpy(user[user_counter].name, username);
        user[user_counter].permission_lvl = permission_lvl;
        user[user_counter].socketfd = socketfd;
        user_counter++;
        printf("<log> New user: [%d]: %s perm_lvl: %d\n", user_index, username, permission_lvl);
    }
}

int close_os(int os_index){
printf("try to close os with socket: [%d]\n", os[os_index].socketfd);
    if(os[os_index].socketfd){
        write(os[os_index].socketfd, "close_os now 0", sizeof("close_os now 0"));
        printf("<log> Sended close signal to [%s]\n", os[os_index].name);
        os[os_index].socketfd = 0;
        return 1;
    }
    printf("<log> No connection with [%s]", os[os_index].name);
    return 0; //no connection with OS probably closed
}

int runMsg(message msg, int socketfd){
    printf("running msg: %s, %s, %d", msg.action, msg.name, msg.number);
    if(!strcmp(msg.action, "new_os")){    
        add_new_os(msg.name, msg.number, socketfd);
        return 1;
    }
    else if(!strcmp(msg.action, "new_user")){
        add_new_user(msg.name, msg.number, socketfd);
        return 1;
    }
    else if(!strcmp(msg.action, "close_os")){
        close_os(findOsIndex(msg.name));
        return 1;
    }
    return 0;
}

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
        printf("[%d]: Received: %s\n", socketfd, raw_msg);
        message msg = string2msg(raw_msg);
        pthread_mutex_lock(&guard);
            runMsg(msg, socketfd);
        pthread_mutex_unlock(&guard);
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
        printf("<log> Socket creation failed...\n");
        exit(0);
    }
    else{
        printf("<log> Socket successfully created..\n");
    }

    bzero(&serverAddr, sizeof(serverAddr));
   
    // assign IP, PORT
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);
   
    // Binding newly created socket to given IP and verification
    if ((bind(serverSocket, (SA*)&serverAddr, sizeof(serverAddr))) != 0) {
        printf("<log> Socket bind failed...\n");
        exit(0);
    }
    else
        printf("<log> Socket successfully binded..\n");
   
    // Now server is ready to listen and verification
    if ((listen(serverSocket, 5)) != 0) {
        printf("<log> Listen failed...\n");
        exit(0);
    }
    else
        printf("<log> Server listening..\n");

    pthread_t tid[60];
    int i = 0;
    while(1){
        //Accept call creates a new socket for the incoming connection
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

        //for each client request creates a thread and assign the client request to it to process
        //so the main thread can entertain next request
        if( pthread_create(&tid[i++], NULL, socketThread, &newSocket) != 0 )
            printf("<log> Failed to create thread\n");

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

    close(serverSocket);
}