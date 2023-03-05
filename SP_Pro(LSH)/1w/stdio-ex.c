/*
Formatted I/O example
*/
#include <stdio.h>

int main(int argc, char *argv){
    FILE *fp;
    char buf[256];
    int num, Nnum;
    char str[30], Nstr[30];

    scanf("%d%s", &num, str);
    if((fp=fopen("test.dat", "w"))==NULL){
        perror("test.dat");
        return 1;
    }

    fprintf(fp, "%d%s\n", num, str);
    if((fp==freopen("test.dat", "r", fp))==NULL){
        perror("test.dat");
        return 1;
    }
    fscanf(fp, "%d%s\n", &Nnum, Nstr);
    printf("%d%s\n", Nnum, Nstr);

    fclose(fp);
    return 0;
}