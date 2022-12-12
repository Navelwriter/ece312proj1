   #include <stdio.h>
   #include <sys/types.h>
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <netdb.h> 
   #include <string.h>

   void error(char *msg)
   {
       perror(msg);
       exit(0);
   }

   int main(int argc, char *argv[])
   {
     int sockfd, portno, n;
     struct sockaddr_in serv_addr;
     struct hostent *server;
     char buffer[256], username[256], message[256];
     if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
     }
     portno = atoi(argv[2]);
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
         error("ERROR opening socket");
     server = gethostbyname(argv[1]);
     if (server == NULL) {
         fprintf(stderr,"ERROR, no such host\n");
         exit(0);
     }
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     bcopy((char *)server->h_addr, 
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
     serv_addr.sin_port = htons(portno);
     if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
         error("ERROR connecting");
     printf("Enter username: ");
     bzero(username,256); 
     fgets(username,255,stdin);
     printf("Please enter the message: ");
     bzero(buffer,256); 
     fgets(buffer,255,stdin);
     int length = strlen(username);
     //make message buffer a combination of username and message in the format "username: message" in the same line
     strcpy(message, username);
     message[length-1] = ':';
     message[length] = ' ';
     strcpy(message+length+1, buffer);
     n = write(sockfd,message,strlen(message));
     if (n < 0) 
          error("ERROR writing to socket");
     bzero(buffer,256);
     n = read(sockfd,buffer,255);
     if (n < 0) 
          error("ERROR reading from socket");
     printf("%s\n",buffer);
     return 0;
   }

