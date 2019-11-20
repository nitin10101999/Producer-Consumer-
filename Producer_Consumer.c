#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h>
#define BUFF_SIZE 4
pthread_t *producers;
pthread_t *consumers;
sem_t empty,fill,mutex;
int *BUFFER,NUM_OF_PRODUCER,NUM_OF_CONSUMER,BUFF_POS=-1;

int add_item(pthread_t self){
	int i = 0;
	int item = 1 + rand()%50;
	while(!pthread_equal(*(producers+i),self) && i < NUM_OF_PRODUCER){
		i++;
	}
	printf("Producer %d added item %d \n",i+1,item);
	return item;
}
void* producer(void *param){

	while(true){
		int p = add_item(pthread_self());
		sem_wait(&empty);
		sem_wait(&mutex);
		BUFF_POS = BUFF_POS +1;		
		*(BUFFER + BUFF_POS) = p; 
		sem_post(&mutex);
		sem_post(&fill);
		sleep(1 + rand()%RAND_MAX);
	}
	
	return NULL;
}
void remove_item(int item,pthread_t self){
	int i = 0;
	while(!pthread_equal(*(consumers+i),self) && i < NUM_OF_CONSUMER){
		i++;
	}

	printf("\nBuffer:");
	for(int j=0;j<=BUFF_POS;++j)
		printf("%d ",*(BUFFER+j));
	printf("\nConsumer %d removed item %d \nCurrent buffer pos: %d\n",i+1,item,BUFF_POS);
	BUFF_POS = BUFF_POS -1;
	
}

void* consumer(void *param){
	int c;
	while(true){
		sem_wait(&fill);
		sem_wait(&mutex);
		c = *(BUFFER+BUFF_POS);
		remove_item(c,pthread_self());
		sem_post(&mutex);
		sem_post(&empty);
		sleep(1+rand()%RAND_MAX);
	}

	return NULL;
}

int main(int argc, char *argv[]){
	int sleep_time = atoi(argv[1]);
	NUM_OF_PRODUCER = atoi(argv[2]);
	NUM_OF_CONSUMER = atoi(argv[3]);
	BUFFER = (int*) malloc(BUFF_SIZE*sizeof(int));
	producers = (pthread_t*) malloc(NUM_OF_PRODUCER*sizeof(pthread_t));
	consumers = (pthread_t*) malloc(NUM_OF_CONSUMER*sizeof(pthread_t));
	sem_init(&empty,0,BUFF_SIZE);
	sem_init(&fill,0,0);
	sem_init(&mutex,0,1);
	int i,error;
	//srand(time(NULL));
	for(i=0;i<NUM_OF_PRODUCER;i++){
		error = pthread_create(producers+i,NULL,&producer,NULL);
		if(error == 0)
			printf("producer %d created\n",i+1);
		else
			printf("Error  in creating producer %d: %s\n",i+1,strerror(error));
	}

	for(i=0;i<NUM_OF_CONSUMER;i++){
		error = pthread_create(consumers+i,NULL,&consumer,NULL);
		if(error == 0)
			printf("consumer %d created\n",i+1);
		else
			printf("Error in creating consumer %d: %s\n",i+1,strerror(error));
	}
	sleep(sleep_time);
	return 0;
}

