#!/bin/bash

gcc -o pp -std=c99 -pthread pp.c -pg
gcc -o ss -std=c99 -pthread ss.c -pg

./pp
./ss