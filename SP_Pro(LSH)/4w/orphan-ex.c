#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void){
    int pid;
    
    if((pid=fork())==0){
        // Child
        printf("Child");
        sleep(1);
    }else{
        // Parent
        printf("Parent");
        exit(0);
    }
}