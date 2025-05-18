#!/bin/bash

# gcc -Wall -c treasure_manager.c
# gcc -Wall -c treasure_manager_main.c
# gcc -Wall -o treasure_manager treasure_manager_main.o treasure_manager.o

gcc -Wall -o treasure_manager treasure_manager.c treasure_manager_main.c
gcc -Wall -o calculate_score calculate_score.c
gcc -Wall -o treasure_hub_monitor treasure_hub_monitor.c
gcc -Wall -o treasure_hub treasure_hub_main.c

