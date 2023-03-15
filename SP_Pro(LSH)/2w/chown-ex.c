#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    int owner_id, group_id, filedes;

    if(argc!=5){
        perror("argument error");
        return 1;
    }
    owner_id = atoi(argv[3]);
    group_id = atoi(argv[4]);
    if(strcmp(argv[1], "chown")==0){
        if(chown(argv[2], owner_id, group_id)){
            perror("chown");
            return 2;
        }
        printf("chown %s to %s, %s\n", argv[2], argv[3], argv[4]);
    }
    else if(strcmp(argv[1], "fchown")==0){
        filedes = open(argv[2], O_RDWR);
        if (fchown(filedes, owner_id, group_id)){
            perror("chown");
            return 3;
        }
        printf("fchown %s to %s, %s\n", argv[2], argv[3], argv[4]);
    }
    else if(strcmp(argv[1], "lchown")==0){
        if(lchown(argv[2], owner_id, group_id)){
            perror("lchown");
            return 4;
        }
        printf("lchown %s to %s, %s\n", argv[2], argv[3], argv[4]);
    }
}