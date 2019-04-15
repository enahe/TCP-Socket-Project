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
   char continueSize[1];
   int fileSize; 
   int continueSend = 1;
   int convertedSend; 
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
   while(continueSend) {
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
   memset(buffer, 0, sizeof(buffer));
   while ((remainingData > 0) && ((length = read(clientSocket, buffer, BUFSIZ)) > 0)) {
        
	fprintf(recievedFile, "%s", buffer);
        remainingData -= length;
        fprintf(stdout, "Recieved ld bytes, only %d left to go\n", length, remainingData);
       }
       memset(buffer, 0, sizeof(buffer));
       fclose(recievedFile);
       printf("Would you like to recieve more files from the server? Enter 1 for yes, and 0 for no.\n");
       scanf("%s", continueSize); 
       continueSend = atoi(continueSize); 
       if (send(clientSocket, continueSize, sizeof(continueSize),0) != -1) {
       printf("Successfully sent continue response to server.\n");
   }
   else {
       printf("Continue response was not sent to server. Please try again\n"); 
       exit(EXIT_FAILURE);
   }     
}
      close(clientSocket);
      return 0; 
   
  

}

