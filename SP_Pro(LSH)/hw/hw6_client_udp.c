#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1024
#define PORT 30000
#define HOSTADDR "172.25.224.1"

int main(int argc, char *argv[]){
    int sd, send_bytes, n, recv_bytes, servaddrsize;
    struct sockaddr_in servaddr;
    char snddata[MAX], rcvdata[MAX];

    printf("create socket ...");
    if((sd=socket(AF_INET, SOCK_DGRAM, 0))<0){
        fprintf(stderr, "can't open socket.\n");
        exit(1);
    }

    bzero((char*)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(HOSTADDR);
    servaddr.sin_port = htons(PORT);

    while(fgets(snddata, MAX, stdin)!=NULL){
        send_bytes = strlen(snddata);
        printf("send data to server ... %s", snddata);
        if((n=sendto(sd, snddata, send_bytes, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)))!=send_bytes){
            fprintf(stderr, "can't send data\n");
            exit(1);
        }
        recv_bytes = 0;
        while(recv_bytes<send_bytes){
            if((n=recvfrom(sd, rcvdata, MAX, 0, (struct sockaddr*)&servaddr, &servaddrsize))<0){
                fprintf(stderr, "can't recv data\n");
                exit(1);
            }
        }
        rcvdata[recv_bytes]=0;
        printf("recv data from server ... %s", rcvdata);
    }
    close(sd);
    return 0;
}