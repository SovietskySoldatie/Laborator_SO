#!/bin/bash

./treasure_manager --remove_treasure Hunt001 "Treasure 999"
./treasure_manager --remove_treasure Hunt001 "Treasure 001"
./treasure_manager --list Hunt001
./treasure_manager --remove_hunt Hunt002
./treasure_manager --list Hunt002
