/*
* SocketsClient.c
* Created by Noah Lee and Clark Ren
* This is the client side of the chat program, 
* it will connect to the server and allow the user to send and receive messages
*/


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>

int contin = 1;
char buffer[256], username[256], message[256];
int sockfd, portno, n;
int length;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void * receiveMessage(void * socket){
    int ret;
    while((ret = read(sockfd, buffer, 256)) > 0){
        n = read(sockfd, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");
        printf("\n%s\n", buffer);
    }
}

void * sendMessage(void * socket){
    while(1){
        printf("%s: ", username);
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        if (buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't')
        {
            write("\nClosing connection...", 23);
            contin = 0;
            break;
        }
        // make message buffer a combination of username and message in the format "username: message" in the same line
        strcpy(message, username);
        message[length - 1] = ':';
        message[length] = ' ';
        strcpy(message + length + 1, buffer);
        n = write(sockfd, message, strlen(message)); // send the message to the server
        if (n < 0)
            error("ERROR writing to socket");
        bzero(buffer, 256);
    }
}

int main(int argc, char *argv[])
{
    
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]); // get the port number
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // create a socket
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]); // get the host name
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length); // copy the host name to the server address
    serv_addr.sin_port = htons(portno);

    int newsockfd;
    if (newsockfd = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))){
        error("ERROR connecting");
    }
    printf("Connection accepted from %d...\n", serv_addr);
    printf("Enter username: ");
    bzero(username, 256); // clear the buffer
    fgets(username, 255, stdin); // get username from user
    length = strlen(username);
    username[length - 1] = NULL;
    // Loop until the user enters an empty message
    pthread_t rThread, sThread;
    
    int rec, send;
    if (rec = pthread_create(&rThread, NULL, receiveMessage, (void *) newsockfd)) { // create a thread to receive messages from the server
        printf("ERROR: Return Code from pthread_create() is %d\n", rec);
        error("ERROR creating thread");
    }
    if (send = pthread_create(&sThread, NULL, sendMessage, (void *) newsockfd)) { // create a thread to send messages to the server
        printf("ERROR: Return Code from pthread_create() is %d\n", send);
        error("ERROR creating thread");
    }
    while (contin){        
    }
    return 0;
}