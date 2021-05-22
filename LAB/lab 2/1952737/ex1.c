#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MAX_ARR_LENGTH 100

int main (int argc, char **argv)
{
  pid_t       pid;
  char        *buffer;
  size_t      len = 0;
  int         k = 0;
  FILE        *inputFile;
  int         arr[MAX_ARR_LENGTH];
  inputFile = fopen("numbers.txt","r");
  
  while(getline(&buffer, &len, inputFile) != -1) {
    arr[k++] = atoi(buffer);
  }
  fclose(inputFile);
  pid = fork();
  if (pid == 0){  // child process
    int        child_ctr = 0;
    for(int i = 0;i < k;i++) if (arr[i] % 3 == 0) child_ctr++;
    printf("Child: %d \n",child_ctr);
    fflush(stdout);
  }
  else if (pid > 0){ // parent process
      int        parent_ctr = 0;
      for(int i = 0;i < k;i++) if (arr[i] % 2 == 0) parent_ctr++;
      printf("Parent: %d \n",parent_ctr);
      fflush(stdout);
  }
  else{
      perror("fork");
      exit(1);
  }
  return 0;
}
