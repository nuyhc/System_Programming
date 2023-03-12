#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    FILE *fpo; // file pointer
    int fdo; // file descriptor

    if (argc!=2){
        perror(argv[0]);
        return 1;
    }
    if((fdo=open(argv[1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR))==-1){
        perror(argv[1]);
        return 1;
    }
    if((fpo=fdopen(fdo, "r+"))==NULL){
        perror("fdopen");
        return 2;
    }
    fprintf(fpo, "Hello, world!\n");
    fclose(fpo);
}