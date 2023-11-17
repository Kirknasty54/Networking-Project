#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <error.h>
#include <string.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


#define ERROR -1
//max num of clients the server can handle at one time
#define MAX_CLIENTS 2
//size of buffer to pass to send and recieve
#define MAX_DATA 1024

int main(int argc, char **argv){
    struct sockaddr_in server;
    struct sockaddr_in client;
    //sock serves as server socket we are goind to use
    int sock;
    //serves as new client socket
    int new;
    //length of the socket address structure
    int sockaddr_len = sizeof(struct sockaddr_in);
    //length of recieved data
    int data_len;
    //buffer for storing data
    char data[MAX_DATA];

    //creates a new socket using socket system call, checks for errors and if one presents itself, prints error message by way of perror and then exits the program
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR){
        perror("server socket: ");
        exit(-1);
    }

    //configures server adresss structure and specifies address family is IPv4
    //sets port number from cmd line arguement, allows connections from any IP address and sets padding in the address structures to 0
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(&server.sin_zero, 8);

    //binds the server socket to a specific address and port using bind,
    //checks for erros and exits if one presents
    if((bind(sock, (struct sockaddr *)&server, sockaddr_len)) == ERROR){
        perror("bind: ");
        exit(-1);
    }

    //sets the server socket to listening state thus allowing it to accept incoming connections, also checks for errors and exits program if one occurs
    if((listen(sock, MAX_CLIENTS)) == ERROR){
        perror("listen");
        exit(-1);
    }

    //continously accepts new client connections by way of accept()
    //extracts first connection request on queue of pending connections and creates a new socket descriptor( to communicate with the client
    while(1){
        if((new = accept(sock, (struct sockaddr *)&client, &sockaddr_len)) == ERROR){
            perror("accept");
            exit(-1);
        }

        //signals new client connected and its info
        printf("New client connected from port num %d and IP %s\n", ntohs(client.sin_port), inet_ntoa(client.sin_addr));

        data_len = 1;
        //server recives data from client via recv()
        //if received data is Done, tell user, print message,. break out of loop and close connection
        //otherwise send data back to client via send()
        while(data_len){
            data_len = recv(new, data, MAX_DATA, 0);

            if(data_len){
                if(strncmp(data, "Done", 4)== 0){break;}
                send(new, data, data_len, 0);
                data[data_len] = '\0';
                printf("Sent msg: %s", data);
            }
        }

        //user has disconnected and closes client socket
        printf("Client disconnected\n");
        close(new);
    }
}