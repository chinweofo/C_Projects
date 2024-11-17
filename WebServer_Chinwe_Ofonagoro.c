/*
Author Name: Chinwe Ofonagoro
Program Name: WebServer
Objective: Create a web server
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>

//Global variable and response
char *path = NULL;
const char *message404 = "HTTP/1.0 404 Not Found\r\n";

//Struct to hold server information
struct Server {
    int newSock;  //socket descriptor for the server
    void (*launch)(struct Server *server);  // Function pointer to the server launch function
};

//Function to check if the requested file exists and return its size using stat
long getFileSize(const char *filepath) {
    long fileSize;
    struct stat st;
    if(stat(filepath, &st) == 0){fileSize = st.st_size;}
    else{
        fileSize = -1;
        perror("File Size invalid or empty");
        return -1; //keep going after the error occurs
    }
    return fileSize;
}
/*Function used once a thread is created in the launch() function. 
This takes in the void passed into the pthread parameter, and is converted to an int and called newSocket for each new socket in the while loop from launch(), then frees up its memory (no longer needed)
Path from the command line argument gets written tot he buffer, and the "request" is read only.
If the file exists, then it gets the size of the content in the file, sends the length to the socket and returns the 200 OK message, otherwise it fails and returns the 404 message.*/
void *handleRequest(void *sock_desc) { 
    int newSocket = *(int *)sock_desc; //get the socket descriptor
    free(sock_desc);
    char requestBuffer[30000], filePath[256], fullFilePath[512];
    int httpRequest = read(newSocket, requestBuffer, sizeof(requestBuffer));
    if (httpRequest < 0) {
        close(newSocket); 
        return NULL;
    }
    requestBuffer[httpRequest] = '\0';

    //parses the HTTP request by finding the file path (after the first space and before the second space)
    char *fileStart = strchr(requestBuffer, ' ') + 1;  //skip the first space
    char *fileEnd = strchr(fileStart, ' ');  //find the next space after the file path
    
    if (!fileEnd) {
        write(newSocket, message404, strlen(message404));
        close(newSocket);
        return NULL;
    }
 
    size_t path_len = fileEnd - fileStart;
    snprintf(filePath, sizeof(filePath), "%.*s", (int)path_len, fileStart);  //copy the original file path
    snprintf(fullFilePath, sizeof(fullFilePath), ".%s", filePath);//copies the path with "." so the server knows to read the file name after it

    //check if the file exists and get its size
    long fileSize = getFileSize(fullFilePath); 
    if (fileSize >= 0) {
        char message200[1024];
        snprintf(message200, sizeof(message200), "HTTP/1.0 200 OK\r\nContent-Length: %ld\r\n\r\n", fileSize);

        int filefd = open(fullFilePath, O_RDONLY);
        if (filefd != -1) { 
            write(newSocket, message200, strlen(message200));
            close(filefd);
        } else {//if file didn't open, send 404 messag
            write(newSocket, message404, strlen(message404));
        }
    } else { //if the file doesn't exist, send 404 message
        write(newSocket, message404, strlen(message404));
    }
    close(newSocket);
    return NULL;
}
/*Function that allows the server to run continuously. Uses the information from the server set up in the main function, 
    and creates a thread on each successful connection with the path. Once the thread is created, it utilizes the handleRequest function*/
void launch(struct Server *server) {
    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof(client_addr);
    
    while (1) {
        int *newSocket = malloc(sizeof(int));  //allocate memory for the new socket descriptor
        if ((*newSocket = accept(server->newSock, (struct sockaddr *)&client_addr, &addr_size)) == -1) {
            free(newSocket);
            continue;
        }
        //create a new thread for each incoming connection
        pthread_t tid;
        if (pthread_create(&tid, NULL, handleRequest, newSocket) != 0) {
            close(*newSocket);
            free(newSocket); 
        }
    }
}
// Function to initialize and configure the server
/*Constructor to set up th emain funcitonalities used to create the server. 
Gets the address of the server, creates a socket and binds, listens for connections and will launch the "launch" function*/
struct Server server_const(int domain, int service, int backlog, void (*launch)(struct Server *)) {
    struct Server server;
    int newSock, yes = 1;
    struct addrinfo hints, *servinfo, *p;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    char portString[6];
    snprintf(portString, sizeof(portString), "%d", 8000); 
    if (getaddrinfo(NULL, portString, &hints, &servinfo) != 0){exit(1);}

    //create and bind the socket
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((newSock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){exit(2);} 
        setsockopt(newSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (bind(newSock, p->ai_addr, p->ai_addrlen) == -1) {close(newSock); continue;}
        break;
    }
    freeaddrinfo(servinfo);

    //listen for incoming connections
    if (listen(newSock, backlog) == -1){exit(2);}

    server.newSock = newSock;
    server.launch = launch;
    return server;
}
/*
Takes in the path from the command line, sets up the server constructor, and runs the main launch program to accept and handle the requests given
Fails with usage message if path invalid*/
int main(int argc, char *argv[]) {
    if (argc < 2) {  //fails with usage message if there is no argument
        fprintf(stderr, "usage: %s <path>\n", argv[0]);
        exit(1);
    }
    path = argv[1];
    struct Server server = server_const(AF_INET, SOCK_STREAM, 10, launch);
    server.launch(&server);

    return 0;
}

