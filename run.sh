#!/bin/bash

gcc -o qsort -std=c99 -pthread quicksort.c -pg
./qsort