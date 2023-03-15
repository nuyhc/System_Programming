#include <unistd.h>

int main(int argc, char *argv[]){
    if(argc!=3){
        perror("argument error");
        return 1;
    }
    if(symlink(argv[1], argv[2])<0){
        perror("symlink fail");
        return 2;
    }
}