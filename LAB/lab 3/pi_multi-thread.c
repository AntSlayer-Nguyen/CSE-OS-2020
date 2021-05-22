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


void *Monte_Carlo_Technique(void  *thread_arg){
  unsigned long int counter_in_circle = 0;
  srand(time(NULL));
  struct thread_data   *data = (struct thread_data*)thread_arg;
  unsigned long int n = data->nPoints;
  double x, y;
  for(int i = 0;i < n;i++){
    x = (double)rand()/(RAND_MAX / 2) - 1;
    y = (double)rand()/(RAND_MAX / 2) - 1;
    if(x * x + y * y <= 1) data->count_in++;
  }
  pthread_exit(NULL);
}

int main(int argc, char **argv)
{
  srand(time(NULL));
  if(argc != 2){
      fprintf (stderr , "usage: a.out <integer value>\n" ) ;
      return -1;
  }
  if (atoi( argv[1] ) < 0) {
      fprintf (stderr, "%d must be >= 0\n" , atoi( argv[1]));
      return -1;
  }
  unsigned long int nPoints = atoi(argv[1]);
  unsigned long int counter_in_cir = 0;
  pthread_t     threads[N];
  pthread_attr_t attr[N];
  for(int i = 0;i < N;i++){
    pthread_attr_init(&attr[i]);
    thread_array[i].thread_id = (int)threads[i];
    thread_array[i].nPoints = nPoints/N;
    thread_array[i].count_in = 0;
    pthread_create(&threads[i],&attr[i],Monte_Carlo_Technique,(void*)(&thread_array[i]));
  }
  if(nPoints % N != 0){
    unsigned int remain = nPoints % N;
    for(int i = 0;i < remain;i++){
        double x,y;
        x = (double)rand()/(RAND_MAX / 2) - 1;
        y = (double)rand()/(RAND_MAX / 2) - 1;
        if(x * x + y * y <= 1) counter_in_cir++;
    }
  }
  for(int i=0; i<N; i++)
    {
      pthread_join(threads[i],NULL);
      counter_in_cir += thread_array[i].count_in;
    }
  double pi = (double)(4 * counter_in_cir)/nPoints;
  printf("%f \n",pi);
  pthread_exit(NULL);
}
