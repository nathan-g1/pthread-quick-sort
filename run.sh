#!/bin/bash

gcc -o quicksort -std=c99 -pthread quicksort.c -pg
./quicksort