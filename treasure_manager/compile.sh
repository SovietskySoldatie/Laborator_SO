#!/bin/bash

gcc -Wall -c treasure_manager.c
gcc -Wall -c main.c
gcc -Wall -o treasure_manager main.o treasure_manager.o
