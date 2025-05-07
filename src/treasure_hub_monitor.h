/*
  TREASURE HUB header
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

// "physics related"
#define SECOND_TO_MICROSECONDS 1000000

#define CURRENT_FOLDER "."

// treasure manager launch command
#define TREASURE_MANAGER_LAUNCH "./treasure_manager"

// monitor program launch command
#define MONITOR_PROGRAM_LAUNCH "./treasure_hub_monitor"

// common file between hub_main and monitor to share commands
#define COMMANDS_FILENAME "monitor_commands_file.cmd"

#define COMMAND_MAX_NR_ARGS 4 // does not include NULL terminating element in char **arr // includes treasure manager launch


#define MONITOR_COMMAND_MAX_SIZE 128 // sa ajunga

// commands taken by the treasure_hub program
typedef enum
  {
    START_MONITOR, LIST_HUNTS, LIST_TREASURES, VIEW_TREASURE, STOP_MONITOR, EXIT,
    OTHER // to be left as last
  } MONITOR_COMMAND;

// communication signals:
// SIGUSR1 - normal feature
// SIGUSR2 - stop_monitor
#define SIGNAL_COMMAND SIGUSR1
#define SIGNAL_TERMINATE SIGUSR2
