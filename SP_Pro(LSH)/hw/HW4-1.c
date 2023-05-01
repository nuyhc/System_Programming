#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "linkedlist.h"

void *producer(void *);
void *consumer(void *);

struct LinkedList* plist;
int randomNum; // Random data
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_has_space = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_has_data = PTHREAD_COND_INITIALIZER;

int main(void){
    plist = (struct LinkedList*)malloc(sizeof(struct LinkedList));
    // Init Linked List
    ListInit(plist);

    pthread_t threads[2];
    pthread_create(&threads[0], NULL, producer, NULL);
    pthread_create(&threads[1], NULL, consumer, NULL);

    for (int i=0 ; i<2 ; i++){ pthread_join(threads[i], NULL); }

    freeAllNode(plist);
    return 0;
}

void *producer(void *v){
    for (int i=0 ; i<1000 ; i++){
        if(isFull(plist)){ pthread_cond_wait(&buffer_has_space, &mutex); }
        randomNum = rand()%100 + 1;
        insertItem(plist, randomNum);
        printf("%d: Producer %d\n", i, randomNum);
        pthread_cond_signal(&buffer_has_data);
        pthread_mutex_unlock(&mutex);
        usleep(10);
    }
}

void *consumer(void *v){
    int data;
    for (int i=0 ; i<1000 ; i++){
        pthread_mutex_lock(&mutex);
        if(isEmpty(plist)){ pthread_cond_wait(&buffer_has_data, &mutex); }
        data = getItem(plist);
        printf("%d:                Consumer %d\n", i, data);
        pthread_cond_signal(&buffer_has_space);
        pthread_mutex_unlock(&mutex);
        usleep(10);
    }
}

