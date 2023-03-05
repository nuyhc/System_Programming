/*
Array I/O example
Handling large size file
*/
#include <stdio.h>
#define ARRAY_SIZE 10

int main(int argc, char *argv){
    // ...
    int i;
    int sample_array[ARRAY_SIZE];
    FILE *stream;

    if((stream=fopen(argv[1], "w"))==NULL){
        perror(argv[1]);
        return 1;
    }

    if(fwrite(sample_array, sizeof(int), ARRAY_SIZE, stream)!=ARRAY_SIZE){
        perror("fwrite error");
        return 2;
    }

    // ...
}