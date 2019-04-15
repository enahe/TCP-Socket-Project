
//client.c: contains all the code necessary to transfer files from client to server. Creates a socket for the client, connects it to the server, and asks the server for files to transfer. 



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
   ssize_t length; //length of file to be transfered. 
   struct sockaddr_in serverAddress;
   char buffer[BUFSIZ];
   char serverIP[25]; // string of server's ip address
   char fileName[100]; // string of filename to copy
   char continueSize[1]; // string asking user if they want to continue
   int fileSize; 
   int continueSend = 1;
   FILE *recievedFile;
   int remainingData = 0;
   struct hostent *hostIP;
   printf("Hello. Please enter the ip address of the server you would like to connect to.\n");
   scanf("%s", serverIP);
   hostIP = gethostbyname(serverIP); //get the ip address of the server 
   clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //create client socket
   if (clientSocket != -1) {
       printf("Client socket has been created\n");
   }
   else {
       printf("Client socket has not been created. Try again\n"); 
       exit(EXIT_FAILURE);
   }
   memset(&serverAddress, 0, sizeof(serverAddress)); //0 out serverAddress 
   serverAddress.sin_family = AF_INET;
   serverAddress.sin_port = htons(2500);
   serverAddress.sin_addr= *((struct in_addr*)hostIP->h_addr); //create the server's address
   connected = connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)); //connect the client to the server
   if (connected != -1) {
       printf("Connected to server successfully!\n");
   }
   else {
       printf("Did not connect to server. Please try again.\n");
       exit(EXIT_FAILURE); 
   }
   while(continueSend) { //continue to ask for more files, as long as the user wants more
   printf("Please enter the file name you would like to retrieve from the server. Please also keep the file name under 99 characters.\n");
   scanf("%s", fileName); 
   if (send(clientSocket, fileName, sizeof(fileName), 0) != -1) { //send the filename that you would like to copy to server
       printf("Successfully sent filename to server. Awating response.\n");
   }
   else {
       printf("File name was not sent to server. Please try again\n"); 
       exit(EXIT_FAILURE);
   }
   if (recv(clientSocket, buffer, BUFSIZ, 0) != -1) { //get size of file from server
       printf("Successfully recieved file size back from server. Beginning transmission\n");
   }
   else {
       printf("Could not recieve size of file from server. Please try again.\n");
       exit(EXIT_FAILURE);
   }
   fileSize = atoi(buffer);
       printf("The client recieved a file size of %d\n", fileSize);
   
   recievedFile = fopen(fileName, "w"); //open an empty file on the client with the same name as the one on the server
   if (recievedFile == NULL) {
       printf("Failed to open the file requested. Please try again\n");
       exit(EXIT_FAILURE);
   }
   remainingData = fileSize; 
   printf("The client recieved a file size of %d\n", remainingData);
   memset(buffer, 0, sizeof(buffer));
   while ((remainingData > 0) && ((length = read(clientSocket, buffer, BUFSIZ)) > 0)) { //while the server continues to send more data
        
	fprintf(recievedFile, "%s", buffer); //copy the text from the buffer of the server to the text file on this machine
        remainingData -= length; //reduce the amount of data left to recieve
        fprintf(stdout, "Recieved ld bytes, only %d left to go\n", length, remainingData);
       }
       memset(buffer, 0, sizeof(buffer)); 
       fclose(recievedFile); // close the file
       printf("Would you like to recieve more files from the server? Enter 1 for yes, and 0 for no.\n");
       scanf("%s", continueSize); //ask for more files
       continueSend = atoi(continueSize); 
       if (send(clientSocket, continueSize, sizeof(continueSize),0) != -1) { //send clients response to server
       printf("Successfully sent continue response to server.\n");
   }
   else {
       printf("Continue response was not sent to server. Please try again\n"); 
       exit(EXIT_FAILURE);
   }     
}
      close(clientSocket); //once the user is done, close the socket
      return 0; 
   
  

}

