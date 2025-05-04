#!/bin/bash

gcc -Wall -c treasure_manager.c
gcc -Wall -c treasure_manager_main.c
gcc -Wall -o treasure_manager treasure_manager_main.o treasure_manager.o
