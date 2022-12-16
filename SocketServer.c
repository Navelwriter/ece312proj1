#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#define BUF_SIZE 256
#define CLADDR_LEN 100
char buffer[BUF_SIZE], username[BUF_SIZE], message[BUF_SIZE], clientAddr[CLADDR_LEN];
int cont = 1;

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void *receiveMessage(void *socket)
{ // the thread function
    int sockfd, ret;
    sockfd = (int)socket;
    while((ret = read(sockfd, buffer, 256)) > 0){ 
        printf("\n");
        buffer[ret] = '\0';
        printf("%s\n", buffer);
    }
    if (ret < 0)
        error("ERROR reading from socket");
    else{
        printf("Closing connection\n");
        cont = 0;
    }


    close(sockfd);
}

// create a thread function to send a message to the client
void *sendMessage(void *socket)
{
    int sockfd, ret;
    sockfd = (int)socket;
    while (1)
    {
        printf("%s: ", username);
        memset(buffer, 0, BUF_SIZE);
        memset(message, 0, BUF_SIZE);
        fgets(buffer, BUF_SIZE, stdin);
        // write the username to the beginning of the message
        strcpy(message, username); // copy the username to the message
        strcat(message, ": ");     // add a colon and a space to the message
        strcat(message, buffer);   // add the message to the message
        message[strlen(message) - 1] = '\0';
        if (buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't')
        {
            write("\nClosing connection...", 23);
            cont = 0;
            break;
        }

        ret = write(sockfd, message, BUF_SIZE);
        if (ret < 0)
            error("ERROR writing to socket");
    }
    close(sockfd);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    pid_t childpid;
    pthread_t send, receive;
    int ret, n;
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    printf("Enter username: ");
    bzero(username, 256);        // clear the buffer
    fgets(username, 255, stdin); // get username from user
    int length = strlen(username);
    // removes the newline character from the username string by replacing it with a null character
    username[length - 1] = '\0';
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    printf("Waiting for a connection...\n");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    bzero(buffer, 256);

    // create a thread to send a message as well as receive a message from the client in a non-blocking way
    // in a loop so that the server can send and receive messages at the same time
    if (newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen))
    {
        inet_ntop(AF_INET, &(cli_addr.sin_addr), clientAddr, CLADDR_LEN); // convert the client address to a string
        if (newsockfd < 0)
            error("ERROR on accept");
        printf("Connection accepted from %s...\n", clientAddr);
        if (ret = pthread_create(&send, NULL, sendMessage, (void *)newsockfd) != 0)
        {
            printf("ERROR: Thread cannot be created: %s\n", strerror(ret));
            error("ERROR on accept");
        }
        if (ret = pthread_create(&receive, NULL, receiveMessage, (void *)newsockfd) != 0)
        {
            printf("ERROR: Thread cannot be created: %s\n", strerror(ret));
            error("ERROR on accept");
        }
    }

    while (cont)
    {
    }

    return 0;
}