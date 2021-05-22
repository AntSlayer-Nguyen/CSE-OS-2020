#include "readline.h"
#include <stdio.h>
#include <string.h>
int read_line(char *str){
int n = strlen(str);
for(int i=0;i<n-1;i++){
if(str[i] > '9' || str[i] < '0') return 0;
}
return 1;
}
