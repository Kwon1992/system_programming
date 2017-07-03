#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for strcmp and memset
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

#define SIZE sizeof(struct sockaddr_in)
#define MAX_LEN 1024

void closesock(int sig);

int sockfd_connect;
int sockfd_listen;

void main(void) {
    char client_message[MAX_LEN];
    char my_message[MAX_LEN];
    struct sockaddr_in server = {AF_INET, htons(1234), INADDR_ANY};
    struct sigaction act;

    act.sa_handler = closesock;
    sigfillset(&(act.sa_mask));
    sigaction(SIGPIPE,&act,NULL);

    if((sockfd_listen = socket(AF_INET,SOCK_STREAM,0))==-1){
        printf("Socket Open Fail\n");
        exit(1);
    }

    if(bind(sockfd_listen, (struct sockaddr*)&server, SIZE)==-1){
        printf("Bind Fail\n");
        exit(1);
    }
    
    if(listen(sockfd_listen,5)==-1){
        printf("Listen Fail\n");
        exit(1);
    }

    printf("Chat Server is opened.... Waiting for Someone\n");

    while(1){
        if((sockfd_connect = accept(sockfd_listen,NULL,NULL))==-1){
            printf("Accept Fail\n");
            continue;
        }

        printf("Someone enter the chat_room!\n"); // 누군가 들어왔음을 알린다. 

        while(1){
            memset(client_message,0,sizeof(client_message));
            memset(my_message,0,sizeof(my_message));
            /* char 배열에 의도하지 않은 문자열이 남을 경우를 
             * 생각해서 memset을 통해서
             * 완전히 비워준다.
             */
              
            if(recv(sockfd_connect, &client_message, MAX_LEN, 0) > 0){
                if(strcmp(client_message, "/quit\n") == 0) break;
                printf("SOMEONE: %s", client_message);
            }
            printf("ME: ");
            fgets(my_message, sizeof(my_message), stdin);
            send(sockfd_connect, &my_message, strlen(my_message), 0);
        }
        printf("sockfd_connect closed\n");
        close(sockfd_connect);
    }
    close(sockfd_listen);
}

void closesock(int sig){
    close(sockfd_connect);
    close(sockfd_listen);
    printf("Connection Lost\n");
    exit(1);
}

