// HW2-1: Fork + file sharing
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <fcntl.h>
#include <string.h>
#define BUFSIZE 10

int main(int argc, char *argv[]){
    int fd;
    char buf[BUFSIZE];
    int rBytes; // 읽어 온 바이트 수

    // File Open
    if((fd=open(argv[1], O_RDONLY))<0){
        printf("Open: %d ", fd);
        exit(1);
    }

    // Fork
    if(fork()==0){
        // Child 1
        int pid = getpid();
        while((rBytes=read(fd, buf, BUFSIZE))!=0){
            printf("%d : ", pid);
            puts(buf);
            // 메모리 set
            memset(buf, 0, BUFSIZE);
            sleep(1);
        }
        exit(0);
    }else{
        if(fork()==0){
            // Child 2
            int pid = getpid();
            while((rBytes=read(fd, buf, BUFSIZE))!=0){
                printf("%d : ", pid);
                puts(buf);
                // 메모리 set
                memset(buf, 0, BUFSIZE);
                sleep(1);
            }
            exit(1);
        }else{
            // Parent
            int pid = getpid();
            while((rBytes=read(fd, buf, BUFSIZE))!=0){
                printf("%d : ", pid);
                puts(buf);
                // 메모리 set
                memset(buf, 0, BUFSIZE);
                sleep(1);
            }
            // 자식 프로세스 종료 대기
            int status;
            wait(&status);
            exit(2);
        }
    }
    return 0;
}
