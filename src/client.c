#include <arpa/inet.h>
#include <time.h>
#include <stdint.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include "utils.h"

#define SA struct sockaddr
#define BUFF_SZ 320 

char buffer_inp_client[BUFF_SZ];
short client_id;

void* from_server(void* arg) {
    int sockfd = *(int*)arg;
    char buffer[BUFF_SZ];
    Message msg;
    while (1) {
        bzero(buffer, sizeof(buffer));
        int r = recv(sockfd, (Message*)&msg, sizeof(msg), 0);
        if (r == 0) exit(1);

        printf("\33[2k\r(%d): %s\n", msg.id_sender, msg.input);
        printf("You: %s", buffer_inp_client);
        fflush(stdout);
    }

    return NULL;
}

int start_client(int sockfd) {
    pthread_t thid;
    pthread_create(&thid, NULL, &from_server, &sockfd);
    setNonBlockingInput();
    srand(time(0));
    client_id = rand() % 32767;

    int m;
    for (;;) {
        m = 0;
        printf("\nYou: ");
        fflush(stdout);

        while (1) {
            char c = getchar();
            if (c == '\n' || c == EOF) {
                break;
            } else if (c == 8 || c == 127) { 
                if (m > 0) {
                    printf("\b \b"); 
                    --m;
                }
            } else {
                putchar(c); 
                buffer_inp_client[m++] = c;
            }
        }

        if (m > 0) {
            Message msg;
            construct_message(&msg, buffer_inp_client, client_id);
            send(sockfd, (void*)&msg, sizeof(msg), 0);
            sleep(1);
            bzero(buffer_inp_client, sizeof(buffer_inp_client));
        }
    }
    return 0;
}

