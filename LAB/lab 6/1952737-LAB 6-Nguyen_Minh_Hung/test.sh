#!/bin/bash

gcc mem.c main.c -lpthread -o mem
./mem 1   # first-fit
./mem 2   # best-fit
./mem 3   # worst-fit

rm -f *.o mem # clean mem
	
