#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>

#define TMPFILE "/tmp/mylog"

static void myexit(void);

int main(void){
    FILE *fp;

    if(atexit(myexit)!=0){
        perror("atexit error");
        exit(1);
    }

    if((fp=fopen(TMPFILE, "a+"))==NULL){
        perror("fopen error");
        exit(2);
    }
    fprintf(fp, "This is temporary log entry of pid %d\n", getpid());
    
    close(fp);
    exit(0);
}

static void myexit(void){
    if(unlink(TMPFILE)){ // 링크를 끊고 파일을 삭제함
        perror("myexit : unlink");
        exit(3);
    }
}