#include "factorial.h"
#include <stdio.h>
int factorial (const int aNumber){
if(aNumber <= 0) return 1;
else return aNumber * factorial(aNumber - 1);
}
