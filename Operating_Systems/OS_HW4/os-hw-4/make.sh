#!/bin/bash

for i in {1..4}; do gcc -c malloc_$i.cpp -o malloc_$i.o; done

zip 213763055_207033259.zip `for i in {1..4}; do echo -n "malloc_$i.cpp "; done` submitters.txt