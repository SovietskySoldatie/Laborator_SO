#!/bin/bash

# ./treasure_manager --list Hunt001
./treasure_manager --add Hunt001 < test1.txt
# ./treasure_manager --list Hunt001
# ./treasure_manager --view Hunt001 "Treasure 001"
./treasure_manager --add Hunt001 < test2.txt
# ./treasure_manager --list Hunt001
# ./treasure_manager --view Hunt001 "Treasure003"
# ./treasure_manager --view Hunt001 "Treasure001"
# ./treasure_manager --view Hunt001 "Treasure002"
./treasure_manager --add Hunt001 < test3.txt
# ./treasure_manager --list Hunt001
./treasure_manager --add Hunt002 < test4.txt
# ./treasure_manager --list Hunt002

