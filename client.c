#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
int main () {
   int clientSocket;
   int connected;
   ssize_t length;
   struct sockaddr_in serverAddress;
   char buffer[BUFSIZ];
   char serverIP[25];
   char fileName[100];
   int fileSize; 
   FILE *recievedFile;
   int remainingData = 0;
   struct hostent *hostIP;
   printf("Hello. Please enter the ip address of the server you would like to connect to.\n");
   scanf("%s", serverIP);
   hostIP = gethostbyname(serverIP); 
   clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
   if (clientSocket != -1) {
       printf("Client socket has been created\n");
   }
   else {
       printf("Client socket has not been created. Try again\n"); 
       exit(EXIT_FAILURE);
   }
   memset(&serverAddress, 0, sizeof(serverAddress));
   serverAddress.sin_family = AF_INET;
   serverAddress.sin_port = htons(2500);
   serverAddress.sin_addr= *((struct in_addr*)hostIP->h_addr);
   connected = connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
   if (connected != -1) {
       printf("Connected to server successfully!\n");
   }
   else {
       printf("Did not connect to server. Please try again.\n");
       exit(EXIT_FAILURE); 
   }
   printf("Please enter the file name you would like to retrieve from the server. Please also keep the file name under 99 characters.\n");
   scanf("%s", fileName); 
   if (send(clientSocket, fileName, sizeof(fileName), 0) != -1) {
       printf("Successfully sent filename to server. Awating response.\n");
   }
   else {
       printf("File name was not sent to server. Please try again\n"); 
       exit(EXIT_FAILURE);
   }
   if (recv(clientSocket, buffer, BUFSIZ, 0) != -1) {
       printf("Successfully recieved file size back from server. Beginning transmission\n");
   }
   else {
       printf("Could not recieve size of file from server. Please try again.\n");
       exit(EXIT_FAILURE);
   }
   fileSize = atoi(buffer);
       printf("The client recieved a file size of %d\n", fileSize);
   
   recievedFile = fopen(fileName, "w"); 
   if (recievedFile == NULL) {
       printf("Failed to open the file requested. Please try again\n");
       exit(EXIT_FAILURE);
   }
   remainingData = fileSize; 
   printf("The client recieved a file size of %d\n", remainingData);
   while ((remainingData > 0) && ((length = read(clientSocket, buffer, BUFSIZ)) > 0)) {
	fwrite(buffer, 1, sizeof(buffer), recievedFile);
        remainingData -= length;
        fprintf(stdout, "Recieved %ld bytes, only %d left to go\n", length, remainingData);
       }
      fclose(recievedFile);
      close(clientSocket);
      
      return 0; 
   
  

}

