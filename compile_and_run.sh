#!/bin/bash

gcc -o pp -std=c99 -pthread qsort_parallel.c -pg
gcc -o ss -std=c99 -pthread qsort_sec.c -pg

./pp
./ss