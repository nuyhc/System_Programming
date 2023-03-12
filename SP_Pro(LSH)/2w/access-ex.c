#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    if(argc<2){
        perror("argument error");
        return 1;
    }
    if(access(argv[1], F_OK)==0){
        printf("%s : File Exists\n", argv[1]);
    }
    if(access(argv[1], R_OK)==0){
        printf("%s : Read\n", argv[1]);
    }
    if(access(argv[1], W_OK)==0){
        printf("%s : Write\n", argv[1]);
    }
    if(access(argv[1], X_OK)==0){
        printf("%s : Execute\n", argv[1]);
    }
    else printf("%s : NOT exist\n", argv[1]);
}