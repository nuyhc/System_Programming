/*
Struct I/O example
*/
#include <stdio.h>
#define LENGTH 10

struct{
    short count;
    char sample;
    long total;
    float numeric[LENGTH];
}object;

int main(int argc, char *argv){
    // ...
    FILE *stream;
    // ...
    if(fwrite(&object, siezof(object), 1, stream)!=1){
        perror("fwrite error");
    }
    // ...
}