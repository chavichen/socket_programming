/* UDPServer.c - An echo UDP server
 * usage: udp_server
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_IP           "127.0.0.1"
#define SERVER_PORT         5555  //Port numbers between 1024 - 49151 are reserved for user server applications
#define BUFFER_SIZE         1024

int main(int argc , char *argv[])
{
    int port;
    int sockfd;
    int recvfd;

    char buffer[BUFFER_SIZE];        //message buffer
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int addr_len = sizeof(client_addr);;
    int data_len;

    /*
     * Create socket for udp
     */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        perror("Create socket failed.");
        exit(-1);
    }

    /*
     * Fill in server information
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;          //IPv4
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    /*
     * Bind socket with the server address
     */
    if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("Bind socket failed.");

        //Close connection
        if(close(sockfd) < 0){
            perror("Close socket failed.");
        }
        exit(-1);
    }

    printf("Server is ready.\n");

    /*
     * Wait for incoming udp datagram
     */
    while(1){
        memset(buffer, 0, BUFFER_SIZE);
        data_len = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr ,&addr_len);
        if(data_len < 0){
            perror("Error in receiving data");
        }
        buffer[data_len] = '\0';
        printf("Received from client(%s):%s\n", inet_ntoa(client_addr.sin_addr), buffer);

        //Reply message to client
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);
    }

    //Close connection
    if(close(sockfd) < 0){
        perror("Close socket failed.");
    }

    return 0;
}
