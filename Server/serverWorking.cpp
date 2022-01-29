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
#include <sstream>
#include <string>


#define SA struct sockaddr

int os_counter = 0, user_counter = 0;

struct os_info{
    std::string name;
    int permission_lvl;
    int socketfd;
} os[MAX_OS_NUMBER];

struct user_info{
    std::string name;
    int permission_lvl;
    int socketfd;
} user[MAX_USER_NUMBER];

struct message{
    std::string action;
    std::string name;
    int number;
};

pthread_mutex_t guard = PTHREAD_MUTEX_INITIALIZER;


void sendError(int socketfd, const char* msg){
    char buff[MAX_MSG_SIZE];
    strcpy(buff, "error ");
    strcat(buff, msg);
    strcat(buff, "\n");
    printf("<log> buffer send: %s\n",buff);
    write(socketfd, buff, sizeof(buff));
}

void sendSuccess(int socketfd, const char* msg){
    char buff[MAX_MSG_SIZE];
    strcpy(buff, "success ");
    strcat(buff, msg);
    strcat(buff, "\n");
    printf("<log> buffer send: %s\n",buff);
    write(socketfd, buff, sizeof(buff));
}

int findOsIndex(std::string os_name){
    for(int i=0; i<os_counter; i++ ){
        if( os[i].name == os_name ){
            return i;
        }
    }
    return -1;
}

int findUserIndex(std::string username){
    for(int i=0; i<user_counter; i++ ){
        if( user[i].name == username ){
            return i;
        }
    }
    return -1;
}

int findUserIndexBySocket(int socketfd){
    for(int i=0; i<user_counter; i++ ){
        if(user[i].socketfd == socketfd){
            return i;
        }
    }
    return -1;
}


void addNewOS(std::string os_name, int permission_lvl, int socketfd){
    int os_index = findOsIndex(os_name);
    if(os_index >= 0){
        os[os_index].permission_lvl = permission_lvl;
        os[os_index].socketfd = socketfd;
        printf("<log> Update OS[%d]: %s perm_lvl: %d\n", os_index, os_name.c_str(), permission_lvl);

    }
    else{
        os[os_counter].name = os_name;
        os[os_counter].permission_lvl = permission_lvl;
        os[os_counter].socketfd = socketfd;
        os_counter++;
        printf("<log> New OS[%d]: %s perm_lvl: %d\n", os_counter, os_name.c_str(), permission_lvl);
    }
}

void addRootUser(){
    user[0].name = "root";
    user[0].permission_lvl = 9;
    user_counter++;
}

void addNewUser(std::string username, int permission_lvl, int socketfd){
    int user_index;
    if( user[findUserIndexBySocket(socketfd)].permission_lvl < 9){
        sendError(socketfd, "permission_denied");
    }
    else{
        if((user_index = findUserIndex(username)) >= 0){
            user[user_index].socketfd = socketfd;
            printf("<log> Update User[%d]: %s perm_lvl: %d\n", user_index, username.c_str(), permission_lvl);
        }
        else{
            user[user_counter].name = username;
            user[user_counter].permission_lvl = permission_lvl;
            user[user_counter].socketfd = socketfd;
            user_counter++;
            printf("<log> New user: [%d]: %s perm_lvl: %d\n", user_index, username.c_str(), permission_lvl);
        }
        sendSuccess(socketfd, "added ");
    }
}

void loginUser(std::string username, int socketfd){
    int user_index;
    if((user_index = findUserIndex(username)) >= 0){
        user[user_index].socketfd = socketfd;
        printf("<log> Login User[%d]: %s perm_lvl: %d\n", user_index, username.c_str(), user[user_index].permission_lvl);
        sendSuccess(socketfd, "correct_login ");
    }
    else{
        sendError(socketfd, "no_user ");
    }
}

int closeOs(int os_index, int user_index){
    printf("<log> Try to close os with socket: [%d]\n", os[os_index].socketfd);
    if(user[user_index].permission_lvl >= os[os_index].permission_lvl){
        if(os[os_index].socketfd){
            write(os[os_index].socketfd, "close_os now 0\n", sizeof("close_os now 0\n"));
            printf("    <log> Send close signal to [%s]\n", os[os_index].name.c_str());
            os[os_index].socketfd = 0;
            sendSuccess(user[user_index].socketfd, "signal_sended ");
            return 1;
        }
        else{
            sendError(user[user_index].socketfd, "already_closed ");
            printf("     <log> No connection with [%s]\n", os[os_index].name.c_str());
        }
    }
    else{
        sendError(user[user_index].socketfd, "permission_denied ");
    }

    return 0; //no connection with OS probably closed
}


void sendOsNames(int socketfd){
    std::string os_msgs[os_counter];
    int active_os_counter = 0;
    for(int i=0; i<os_counter; i++ ){
        if(os[i].socketfd > 0){
            os_msgs[active_os_counter] = "active_os " + os[i].name + " " + std::to_string(os[i].permission_lvl) + "\n";
            active_os_counter++;
        }
    }
    std::string info_msg = "active_os_number " + std::to_string(active_os_counter) + "\n";
    printf("<log> Start sending %d os name to [%s]\n", active_os_counter, user[findUserIndexBySocket(socketfd)].name.c_str());
    write(socketfd, info_msg.c_str(), info_msg.length());
    for(int i = 0; i < active_os_counter; i++){
        write(socketfd, os_msgs[i].c_str(), os_msgs[i].length());
        printf("    <log> Send %s\n", os_msgs[i].c_str());
    }
}


int runMsg(message msg, int socketfd){
    printf("<log> running msg: %s, %s, %d\n", msg.action.c_str(), msg.name.c_str(), msg.number);
    if(msg.action == "login"){
        loginUser(msg.name, socketfd);
        return 1;
    }
    else if(msg.action == "new_os"){    
        addNewOS(msg.name, msg.number, socketfd);
        return 1;
    }
    else if(msg.action == "new_user"){
        addNewUser(msg.name, msg.number, socketfd);
        return 1;
    }
    else if(msg.action == "close_os"){
        closeOs(findOsIndex(msg.name), findUserIndexBySocket(socketfd));
        return 1;
    }
    else if(msg.action == "get_active_os"){
        sendOsNames(socketfd);
    }
    else{
        sendError(socketfd, "msg");
        return 1;
    }
    return 0;
}

message string2msg(char* str){
    message f_msg;
    std::stringstream ss(str);
    std::string word;
    try{
        if(ss >> word){
            f_msg.action = word;
        }
        if(ss >> word){
            f_msg.name = word;
        }
        if(ss >> word){
            f_msg.number = std::stoi(word);
        }
    }
    catch (const std::exception& e) {
        std::string erro("error");
        f_msg.action = erro;
    }
    return f_msg;
}

void * socketThread(void *arg){

    int socketfd = *((int *)arg);
    for(;;){
        printf("[%d]: Try read\n", socketfd);
        char raw_msg[MAX_MSG_SIZE] {};
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

    addRootUser();

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