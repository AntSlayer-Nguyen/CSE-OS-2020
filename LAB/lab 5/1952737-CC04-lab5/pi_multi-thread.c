#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define  N     20

struct thread_data{
  int thread_id;
  unsigned long int nPoints;
  unsigned long int count_in;
} thread_array[N];

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
unsigned long int counter_in_cir = 0;



void *Monte_Carlo_Technique(void  *thread_arg){
  srand(time(NULL));
  struct thread_data   *data = (struct thread_data*)thread_arg;
  unsigned long int n = data->nPoints;
  double x, y;
  for(int i = 0;i < n;i++){
    x = (double)rand()/(RAND_MAX / 2) - 1;
    y = (double)rand()/(RAND_MAX / 2) - 1;
    if(x * x + y * y <= 1) data->count_in++;
  }
  /* modified section */
  pthread_mutex_lock(&lock);
  counter_in_cir += data->count_in; // critical section
  pthread_mutex_unlock(&lock);
  //remaining section
  pthread_exit(NULL);
}

int main(int argc, char **argv)
{
  if(argc != 2){
      fprintf (stderr , "usage: a.out <integer value>\n" ) ;
      return -1;
  }
  if (atoi( argv[1] ) < 0) {
      fprintf (stderr, "%d must be >= 0\n" , atoi( argv[1]));
      return -1;
  }
  unsigned long int nPoints = atoi(argv[1]);
  
  pthread_t     threads[N];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  time_t start = time(NULL);
  srand(time(NULL));
  for(int i = 0;i < N;i++){
    thread_array[i].thread_id = (int)threads[i];
    thread_array[i].nPoints = nPoints/N;
    thread_array[i].count_in = 0;
    pthread_create(&threads[i],&attr,Monte_Carlo_Technique,(void*)(&thread_array[i]));
    pthread_join(threads[i], NULL);
  }
  
  
  
  double pi = (double)(4 * counter_in_cir)/nPoints;
  printf("%f \n",pi);
  pthread_mutex_destroy(&lock);
  printf("Time taken: %d sec\n", (unsigned int)(time(NULL) - start));
}
