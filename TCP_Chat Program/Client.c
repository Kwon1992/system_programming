#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for memset
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

#define SIZE sizeof(struct sockaddr_in)
#define MAX_LEN 1024

void main(void) {
    int sockfd;
    char send_msg[MAX_LEN], recv_msg[MAX_LEN];

    struct sockaddr_in server = {AF_INET, htons(1234)};
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("Socket Open Fail\n");
        exit(1);
    }

    if((connect(sockfd,(struct sockaddr*)&server, SIZE)) == -1){
        printf("Connet Fail\n");
        exit(1);
    }

    printf("Type '/quit' if you want to disconnect!\n");     

    while(1) {
        memset(send_msg,0,sizeof(send_msg));
        memset(recv_msg,0,sizeof(recv_msg));
        printf("ME: ");
        fgets(send_msg,sizeof(send_msg),stdin);

        if(strcmp(send_msg, "/quit\n") == 0) {
            break;
        }

        if(send(sockfd, send_msg, strlen(send_msg), 0) < 0){
            printf("Send Fail\n");
            exit(1);
        } 
        if(recv(sockfd, recv_msg, MAX_LEN, 0) < 0) {
            printf("Receive Fail\n");
            exit(1);
        } else {
            printf("SOMEONE: %s", recv_msg);
        }
    }
}

