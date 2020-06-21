#include"stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h> 
#include <math.h>
#include <sys/wait.h>

typedef struct  {
	int factory_number;
	int numMade;
	int numEaten;
	double min_delay_in_ms;
	double avg_delay_in_ms;
	double max_delay_in_ms;
	double total_delay_in_ms;
} stats_fc;

void stats_init(int num_producers);
void stats_cleanup(void);
void stats_record_produced(int factory_number);
void stats_record_consumed(int factory_number, double delay_in_ms);
void stats_display(void);

int numPro=0;
stats_fc *fc=NULL;
_Bool match=true;
sem_t mutex;

void stats_init(int num_producers){
	numPro=num_producers;
	fc=malloc(sizeof(stats_fc)*numPro);
	sem_init (&mutex,0,1);
	for(int i=0;i<numPro;i++){
		fc[i].factory_number=i;
		fc[i].numMade=0;
		fc[i].numEaten=0;
		fc[i].min_delay_in_ms=1000;
		fc[i].avg_delay_in_ms=0;
		fc[i].max_delay_in_ms=0;
		fc[i].total_delay_in_ms=0;
	}
}

void stats_cleanup(void){
	free(fc);
}

void stats_record_produced(int factory_number){
	sem_wait(&mutex);
	fc[factory_number].factory_number= factory_number;
	fc[factory_number].numMade++;
	sem_post(&mutex);
}

void stats_record_consumed(int factory_number, double delay_in_ms){
	fc[factory_number].numEaten++;
	sem_wait(&mutex);
	fc[factory_number].total_delay_in_ms+=delay_in_ms;
	if(delay_in_ms<fc[factory_number].min_delay_in_ms){
		fc[factory_number].min_delay_in_ms=delay_in_ms;
	}
	if(fc[factory_number].max_delay_in_ms<delay_in_ms){
		fc[factory_number].max_delay_in_ms=delay_in_ms;
	}
	fc[factory_number].avg_delay_in_ms=fc[factory_number].total_delay_in_ms/(double)fc[factory_number].numEaten;
	sem_post(&mutex);
}

void stats_display(void){
	printf("Statistics:\n");
	printf("%10s%8s%8s%15s%15s%15s\n","Factory#","#Made","#Eaten","Min Delay[ms]","Avg Delay[ms]","Max Delay[ms]");
	for(int i=0;i<numPro;i++){
		if(fc[i].numMade!=fc[i].numEaten){
			match=false;		
		}		  	printf("%10d%8d%8d%15f%15f%15f\n",fc[i].factory_number=i,fc[i].numMade,fc[i].numEaten,fc[i].min_delay_in_ms,fc[i].avg_delay_in_ms,fc[i].max_delay_in_ms);
	}	
	if(!match){
		printf("ERROR: Mismatch between number made and eaten.\n");
	}
}
