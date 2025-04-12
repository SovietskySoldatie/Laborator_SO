#!/bin/bash

./treasure_manager --add Hunt001 < test.txt
./treasure_manager --list Hunt001
./treasure_manager --view Hunt001 Treasure001
./treasure_manager --list Hunt002
./treasure_manager --view Hunt002 Treasure001
./treasure_manager --view Hunt001 Treasure002
