#include "mem.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>



#define ARRAY_SIZE 5

void * proc(void *args) {
	int i;
	int index = 0;
	long choice = (long)args;
	char * mem[ARRAY_SIZE];
	for (i = 0; i < ARRAY_SIZE; i++) {
		if (rand() % 2) {
			/* Allocate memory */
			unsigned int size = 1 << ((rand() % 4) + 4);
			mem[index] = mem_alloc(size,choice);
			
			if (mem[index] != NULL) {
				index++;
			}
		}else{
			// Free memory
			if (index == 0) {
				continue;
			}
			unsigned char j = rand() % index;
			mem_free(mem[j]);
			mem[j] = mem[index - 1];
			index--;
		}
	}
}

int main(int argc, char* argv[]) {
	
	if(argc != 2){
      fprintf (stderr , "usage: a.out <integer value>\n" ) ;
      return -1;
    }    
	if(atoi(argv[1]) > 3 || atoi(argv[1]) < 1) {
		fprintf (stderr , "Wrong choice!!\n" ) ;
        return -1;
	}
	srand(time(NULL));
	long arg = atoi(argv[1]);
	switch(arg){
		case 1: ;
		    printf("------------------------------------------------------------------------- \n"); 
		    printf("-----------------------------First-fit alloc----------------------------- \n");
			printf("------------------------------------------------------------------------- \n"); 
			break;
		case 2: ;
		    printf("------------------------------------------------------------------------- \n"); 
		    printf("-----------------------------Best-fit alloc------------------------------ \n");
			printf("------------------------------------------------------------------------- \n"); 
			break;
		case 3: ;
		    printf("------------------------------------------------------------------------- \n"); 
		    printf("-----------------------------Worst-fit alloc----------------------------- \n");
			printf("------------------------------------------------------------------------- \n"); 
			break;
	}
	// Allocate 1KB memory
	mem_init(1 << 10);
	
	pthread_t p_0, p_1;
	pthread_create(&p_0, NULL, proc, (void*)arg);
	pthread_create(&p_1, NULL, proc, (void*)arg);
	pthread_join(p_0, NULL);
	pthread_join(p_1, NULL);
	
	mem_finish();
	printf("---------------------------------END------------------------------------- \n");
}

