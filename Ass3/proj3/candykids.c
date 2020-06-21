#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include "bbuff.h"
#include "stats.h"

#define MAX_SIZE 100

_Bool stop_thread = false; // factory threads 
double current_time_in_ms(void);
void * kidThread(void *);
void * factoryThread(void *);

typedef struct  {
    int factory_number;
    double time_stamp_in_ms;
} candy_t;

//get current time
double current_time_in_ms(void)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}

void * factoryThread(void *number){
	int id=(__intptr_t)number;
	while(!stop_thread){

		int waittime=rand()%4;
		printf("\tFactory %d ships candy & waits %ds\n",id,waittime);
		candy_t *candy = malloc(sizeof(candy_t));
		candy->factory_number=id;
		candy->time_stamp_in_ms=current_time_in_ms();
		bbuff_blocking_insert(candy);
		stats_record_produced(id);
		sleep(waittime);
	}
	printf("Candy-factory %d done\n", id);
	pthread_exit (0);
}

void * kidThread(void *arg){
	while(true){
		candy_t *extCandy=(candy_t *)bbuff_blocking_extract();
		double delay_in_ms=current_time_in_ms()-extCandy->time_stamp_in_ms;
		//printf("Factory number of extracted candy is %d. \n",extCandy->factory_number);
		stats_record_consumed(extCandy->factory_number,delay_in_ms);
		free(extCandy);
		sleep(rand()%2);
	}
	pthread_exit (0);
}

int main(int argc, const char * argv[]){
	int numFactories=0;
	int numKids=0;
	int numSeconds=0;

   	// 1.  Extract arguments
	if(argc<4||argc>5){
		printf("There should be exactly three arguments! "); 
        	exit(1); 
	}
	numFactories=atoi(argv[1]);
	numKids=atoi(argv[2]);
	numSeconds=atoi(argv[3]);
	if(argv[1]<=0||argv[2]<=0||argv[3]<=0){
		printf("All arguments must be greater than 0! "); 
        	exit(1); 
	}

   	// 2.  Initialize modules

	pthread_t* kids=malloc(sizeof(pthread_t) * numKids);
	pthread_t* factories=malloc(sizeof(pthread_t) * numFactories);
	bbuff_init();
	stats_init(numFactories);
	srand(time(NULL));

    	// 3.  Launch candy-factory threads
	for(int i=0;i<numFactories;i++){
	//To each thread, pass it its factory number: 0 to (number of factories - 1)
		pthread_create(&factories[i],NULL,factoryThread,(void*)(__intptr_t)i);
	}

    	// 4.  Launch kid threads
	for(int i=0;i<numKids;i++){
		pthread_create(&kids[i],NULL,kidThread,NULL);
	}

 	// 5.  Wait for requested time
	for(int i=0;i<numSeconds;i++){
		sleep(1);
		printf("Time %ds:\n", i);
	}

    	// 6.  Stop candy-factory threads
	stop_thread = true;
	printf("Stopping candy factories...\n");
	for(int i=0;i<numFactories;i++){
		pthread_join(factories[i],NULL);
		//printf("Factory thread %d done\n",i);
	}	

	// 7.  Wait until no more candy
	while(!bbuff_is_empty()){	
		printf("Waiting for all candy to be consumed\n");		
		sleep(1);
	}	

	// 8.  Stop kid threads
	printf("Stopping kids...\n");
	for(int i=0;i<numKids;i++){
		pthread_cancel(kids[i]);
		pthread_join(kids[i],NULL);
	}

	// 9.  Print statistics
	stats_display();

	// 10. Cleanup any allocated memory
	free(kids);
	free(factories);
	//call cleanup functions in statistics module
	stats_cleanup();
	
	return 0;
}

