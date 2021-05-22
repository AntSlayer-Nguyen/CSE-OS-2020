#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main (int argc, char **argv)
{

  if(!fork()) { // child B, forked by A
    printf("Child B id: [%d] and its Parent A id: [%d] \n", getpid(),getppid());
    if(!fork()){ // child E, forked by B
      printf("Child E id: [%d] and its Parent B id: [%d] \n", getpid(),getppid());
      if(!fork()){ //child I, forked by E
          printf("Child I id: [%d] and its Parent E id: [%d] \n", getpid(),getppid());
      }
      else wait(NULL);
    }
    else{
      wait(NULL);
      if(!fork()){ // child F, forked by B
        printf("Child F id: [%d] and its Parent B id: [%d] \n", getpid(),getppid());
      }
      else wait(NULL);
    }
  }
  else {
    wait(NULL);
    if (!fork()){ //child C, forked by A
      printf("Child C id: [%d] and its Parent A id: [%d] \n", getpid(),getppid());
      if(!fork()){ // no child here

      }
      else {
        wait(NULL);
        if(!fork()){ // child G, forked by C
          printf("Child G id: [%d] and its Parent C id: [%d] \n", getpid(),getppid());
        }
        else wait(NULL);
      }
    }
    else {
      wait(NULL);
      if(!fork()){ //child D, forked by A
        printf("Child D id: [%d] and its Parent A id: [%d] \n", getpid(),getppid());
      }
      else wait(NULL);
    }
  }
}
