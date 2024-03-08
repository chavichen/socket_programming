/* UDPClient.c - A UDP client
 * usage: udp_client <server ip> <server port>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFFER_SIZE     1024
#define PORT_MAX       65535   //Port numbers between 1024 - 49151 are reserved for user server applications

void get_current_time_in_ms(char *str_time)
{
   struct timeval tv;
   time_t sec;
   suseconds_t usec;
   struct tm *tm_t;

   gettimeofday(&tv,NULL);
   sec = tv.tv_sec;
   usec = tv.tv_usec;
   tm_t = localtime(&sec);

   if(NULL != tm_t){
     sprintf(str_time,"%02d:%02d:%02d.%03d",
              tm_t->tm_hour,
              tm_t->tm_min,
              tm_t->tm_sec,
              (int)usec/1000);
   }
   return;
}

int is_ipv4(char *ip)
{
    struct sockaddr_in sa;

    if(inet_pton(AF_INET, ip, &(sa.sin_addr)))
        return 1;

    return 0;
}

int main(int argc , char *argv[])
{
    int retry = 0;
    int max_retries = 3;          //the maximum of retries
    int baseDelay = 500;          //ms
    int multiplier = 2;
    int maxInterval = 8000;       //ms
    int waitInterval;
    char time_str[256] = {0};

    int sockfd;
    struct sockaddr_in server_addr;
    int addr_len;
    int ret;
    int byte_received;
    struct timeval timeout;

    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char *server_ip;
    int port;

    /*
     * Check cmdline arguments
     */
    if(argc != 3){
        printf("\nUsage: %s <server ip> <server port>\n", argv[0]);
        printf("like ./udp_client 127.0.0.1 5555\n\n", argv[0]);
        return 1;
    }
    server_ip = argv[1];
    port = atoi(argv[2]);

    if(!is_ipv4(server_ip)){
        printf("\nUsage: %s <server ip> <server port>\nserver ip is invalid IPv4 address.\n", argv[0]);
        return 1;
    }

    if(port <= 0 || port > PORT_MAX){
        printf("\nUsage: %s <server ip> <server port>\nport range is in [1,65535]\n", argv[0]);
        return 1;
    }

    /*
     * Create socket for udp
     */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        perror("Create socket failed.");
        exit(-1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;    //IPv4
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(port);

    timeout.tv_sec = 3;    //the timeout for recvfrom function
    timeout.tv_usec = 0;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    if(ret == -1){
        perror("Set setsockopt failed.\n");
        exit(-1);
    }
    /*
     * Get message from user input
     */
    memset(message, 0, BUFFER_SIZE);
    printf("Please enter the message:\n");
    fgets(message, (BUFFER_SIZE-1), stdin);

    /*
     * Send message to the server with the retry mechanism based on backoff algorithm
     */
    srand(time(NULL));
    for(retry = 0;retry <= max_retries;retry++){
        //Send the message to the server
        ret = sendto(sockfd, (char *)message, strlen(message), 0, (const struct sockaddr *) &server_addr,
sizeof(server_addr));
        if(ret < 0){
            perror("sendto");
            exit(-1);
        }
        get_current_time_in_ms((char *)time_str);
        printf("[%s] Sent message: %s", time_str, message);

        memset(buffer, 0, BUFFER_SIZE);
        byte_received = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr ,&addr_len);
        if(byte_received > 0){
            buffer[byte_received] = '\0';

            get_current_time_in_ms((char *)time_str);
            printf("[%s] Reply from server:%s\n", time_str, buffer);

            //Close connection
            if(close(sockfd) < 0){
                perror("Close socket failed.");
            }
            //Successfully received the echo message from server
            exit(0);
        }
        else if(byte_received == -1 && errno == EAGAIN){
            //continue to retry
        }
        else if(byte_received < 0){
            perror("Error in receiving data");
            break;
        }

        //Retry based on exponential backoff with randomization
        waitInterval = (baseDelay * pow(multiplier, retry)) + (rand() % baseDelay);
        waitInterval = (waitInterval > maxInterval) ?maxInterval:waitInterval;
        //printf("Retry to send message after %d ms\n", waitInterval);
        usleep(waitInterval * 1000);
    }

    //Close connection
    if(close(sockfd) < 0){
        perror("Close socket failed.");
    }

    //Reaching max-retry
    if(retry == (max_retries+1)){
        printf("Reach the maximum of %d retries.\n", max_retries);
        exit(1);
    }

    return 0;
}
