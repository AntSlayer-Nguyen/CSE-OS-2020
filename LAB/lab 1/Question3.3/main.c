#include <stdio.h>
#include <stdlib.h>
#include "readline.h"
#include "factorial.h"

int main(int argc, char *argv[]){
  char str[50];
  fgets(str,sizeof(str),stdin);
  if(read_line(str)==1) printf("%d\n",factorial(atoi(str)));
  else printf("%d\n",-1);
  return 0;
}
