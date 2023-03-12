#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#define BUFFER_SIZE 100

int main(int argc, char *argv[]){
    char buf[BUFFER_SIZE];
    int read_size = 0;

    if(argc!=2){
        perror("argument error");
        return 1;
    }
    if((read_size=readlink(argv[1], buf, BUFFER_SIZE))<0){
        perror("readlink");
        return 2;
    }

    buf[read_size]="\0";
    printf("%s\n", buf);
}