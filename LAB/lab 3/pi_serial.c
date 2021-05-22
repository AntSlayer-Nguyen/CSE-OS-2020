#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

double Monte_Carlo_Technique(unsigned long int nPoints){
  unsigned long int counter_in_circle = 0;
  srand(time(NULL));
  double x, y;
  for(int i = 0;i < nPoints;i++){
    x = (double)rand()/(RAND_MAX / 2) - 1;
    y = (double)rand()/(RAND_MAX / 2) - 1;
    if(x * x + y * y <= 1) counter_in_circle++;
  }
  double pi = (double)(4 * counter_in_circle)/nPoints;
  return pi;
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
  double pi = Monte_Carlo_Technique(nPoints);
  printf("%f \n",pi);

}
