#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    struct stat statbuf;
    if(argc!=2){
        perror("argument error");
        return 1;
    }
    if(lstat(argv[1], &statbuf)<0){
        perror("lstat");
        return 2;
    }
    if(S_ISREG(statbuf.st_mode)){
        if(chmod(argv[1], (statbuf.st_mode & ~S_IXGRP))<0){
            perror("chmod");
            return 3;
        }
    }
    else printf("%s is not regular file\n", argv[1]);
}