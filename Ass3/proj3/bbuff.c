#include"bbuff.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h> 

void* buffer[BUFFER_SIZE];
sem_t mutex;
sem_t rmd;//remainder size
sem_t current;//current size
int index;
int num;//number of candies


void bbuff_init(void);
void bbuff_blocking_insert(void* item);
void* bbuff_blocking_extract(void);
_Bool bbuff_is_empty(void);
void bbuff_cleanup(void);

void bbuff_init(void){
	index=0;
	num=0;
	for(int i=0;i<BUFFER_SIZE;i++)
		buffer[i]=NULL;
	sem_init(&mutex,0,1);
	sem_init(&current,0,0);
	sem_init(&rmd,0,BUFFER_SIZE);
}

void bbuff_blocking_insert(void* item){
	sem_wait(&rmd);
	sem_wait(&mutex);
	buffer[index]=item;
	num++;
	index++;	
	sem_post(&mutex);
	sem_post(&current);
}

void* bbuff_blocking_extract(void){
	sem_wait(&current);
	sem_wait(&mutex);
	index--;
	num--;
	void  *temp=buffer[index];
	buffer[index]=NULL;
	sem_post(&mutex);
	sem_post(&rmd);
	return temp;
}

_Bool bbuff_is_empty(void){
	return num==0;
}

