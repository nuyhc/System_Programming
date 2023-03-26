// HW2-2: Fork + IPC
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <fcntl.h>
#define BUFSIZE 10

int main(int argc, char *argv[]){
    int pipe1[2], pipe2[2]; // pipes
    char buf[BUFSIZE]; // buffer
    int fdin, fdout; // input, output
    int n;
    int idx=0;

    // File Open
    if((fdin=open(argv[1], O_RDONLY))<0){
        perror("Open");
        exit(1);
    }

    // Def. 2 pipes
    pipe(pipe1);
    pipe(pipe2);

    if(fork()==0){
        // Child 1
        close(pipe1[1]); // close pipe1 write
        close(pipe2[0]); // close pipe2 read
        while((n=read(pipe1[0], buf, BUFSIZE))!=0){
            while(idx<=BUFSIZE){
                // 소문자 -> 대문자
                if(buf[idx]>='a' && buf[idx]<='z'){ buf[idx] = buf[idx]-32; }
                idx++;
            }
            idx = 0;
            write(pipe2[1], buf, n); // pipe2 write
            memset(buf, 0, BUFSIZE);
        }
        close(pipe1[0]); // close pipe1 read
        close(pipe2[1]); // close pipe2 write

        exit(1);
    }else{
        // Parent
        fdout=creat("output.txt", 0666);
        close(pipe1[0]); // close pipe1 read

        while((n=read(fdin, buf, BUFSIZE))!=0){ write(pipe1[1], buf, n); } // write pipe1 write

        close(pipe1[1]); // close pipe1 write
        close(fdin); // close input file

        if(fork()==0){
            // Child 2
            close(pipe2[1]); // close pipe2 write

            while((n=read(pipe2[0], buf, 10))!=0){
                write(fdout, buf, n);
                memset(buf, 0, BUFSIZE);
            }

            close(pipe2[0]); // close pipe2 read
            close(fdout); // close output file
            exit(1);
        }

        // wait Child Process terminate
        wait(0);
    }
    return 0;
}