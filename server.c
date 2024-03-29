
//server.c: Contains all the code necessary to connect to a client and send it text files located in the same directory as its executable. 



#include<string.h>
#include<sys/ioctl.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<stdio.h>
#include<net/if_arp.h>
#include<sys/sendfile.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
   int serverSocket, bindStatus, clientSocket;
   char  fileName[100];
   char  fileSize[256]; 
   char continueSize[1]; // string to determine if the user wants more files 
   struct stat fileStatus;
   struct sockaddr_in clientAddress, serverAddress;
   int filePointer;
   socklen_t clientLength = sizeof(clientAddress);
   serverSocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP); //create socket for server
   ssize_t length;
   off_t offset;
   int remainingData; 
   int sentData = 0;
   int continueSend = 1; 
   if (serverSocket < 0) {
	printf("Client socket is not created. Please try again\n");
        exit(EXIT_FAILURE);
   }
   else {
        printf("Client socket has been created successfully\n");
   }
   
   memset(&serverAddress, 0, sizeof(serverAddress));
   serverAddress.sin_family = AF_INET;
   serverAddress.sin_addr.s_addr = INADDR_ANY;
   serverAddress.sin_port = htons(2500); //intialize data for server
   bindStatus = bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)); //bind the data to the socket
   if (bindStatus == 0) {
	printf("Bound successfully!\n");
   }
   else {
        printf("Bind failure!\n");
        exit(EXIT_FAILURE);
   }
   if ((listen(serverSocket, 5)) == -1) { //listen for up to 5 incoming connections
        printf("Listen failure!\n");
        exit(EXIT_FAILURE);
   }
   clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddress, &clientLength); //accept the connections
   if (clientSocket == -1) {
       printf("Could not accept client socket. Please try again\n");
       exit(EXIT_FAILURE);
   }
   while(continueSend) { //as long as the user wants more files
   if (recv(clientSocket, fileName, sizeof(fileName), 0) != -1) { //recieve the filenames the client wants
       printf("Recieved file name\n");
   }
   else {
       printf("File name not recieved properly. Try again\n");
       exit(EXIT_FAILURE);
   }
     filePointer = open(fileName, O_RDONLY); //try and open the file
    if (filePointer == -1) {
        printf("Error opening file.\n");
        exit(EXIT_FAILURE);
    }
    if (fstat(filePointer, &fileStatus) < 0) { //get the files status
        printf("Error fstating\n"); 
        exit(EXIT_FAILURE);
    }
    sprintf(fileSize, "%d", fileStatus.st_size);
    length = send(clientSocket, fileSize, sizeof(fileSize), 0); //send the files size to the client
    if (length < 0) {
        printf("Error on sending filesize");
        exit(EXIT_FAILURE);
    }
    printf("The server sent a file size of %d\n", fileStatus.st_size);
    offset = 0;
    remainingData = fileStatus.st_size; 
    while (((sentData = sendfile(clientSocket, filePointer, &offset, BUFSIZ)) > 0 ) && (remainingData > 0)) { //while there is still data to be sent, send it to the client
	 fprintf(stdout, "Server sent %d bytes. Offset = %ld, Remaining Data = %d\n", sentData, offset, remainingData);
        remainingData -= sentData; //reduce the offset
         fprintf(stdout, "Server sent %d bytes. Offset = %ld, Remaining Data = %d\n", sentData, offset, remainingData);
    }
    if (recv(clientSocket, continueSize, sizeof(continueSize), 0)  != -1) { //see if user wants to continue
        printf("Recieved continue command\n"); 
        continueSend = atoi(continueSize);
    }
    else {
        printf("Could not recieve continue status. Please try again\n");
        exit(EXIT_FAILURE);
    }
}
    close(clientSocket); //once user is done recieving files, close the connection
    close(serverSocket);
    return 0; 
}
