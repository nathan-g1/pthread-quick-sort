#!/bin/bash

gcc -o qss -std=c99 -pthread qss.c -pg
gcc -o qsp -std=c99 -pthread qsp.c -pg
./qsp
./qss