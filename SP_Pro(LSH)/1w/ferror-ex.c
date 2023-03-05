/*
File error check example
*/
#include <stdio.h>

int main(void){
    int ret;
    FILE *fp;

    fp = fopen("test.dat", "r");
    putc('?', fp);
    if(ret=ferror(fp)){
        printf("ferror() return %d\n", ret);
    }

    clearerr(fp);
    printf("ferror() return %d\n", ferror(fp));
    fclose(fp);
    return 0;
}