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
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>

typedef struct dirent DIRENT;


// "physics related"
#define SECOND_TO_MICROSECONDS 1000000
#define NUMBER_DIGITS_LIMIT 128

// printing through pipe macros
#define DISK_BUFFER_SIZE 4096 // suma rotunda
#define PRINT_STRING_INCREMENT 1024 // initial 4096, pot aparea probleme pe versiuni Linux <= 2.6

#define CURRENT_FOLDER "."
#define PREVIOUS_FOLDER ".."
#define FULLPATH_MAX_SIZE 1024
#define TREASURE_GENERAL_FILENAME "treasure.data"

// treasure manager launch command
#define TREASURE_MANAGER_LAUNCH "./treasure_manager"

// monitor program launch command
#define MONITOR_PROGRAM_LAUNCH "./treasure_hub_monitor"

#define CALCULATE_SCORE_PROGRAM_LAUNCH "./calculate_score"

// common file between hub_main and monitor to share commands
#define COMMANDS_FILENAME "monitor_commands_file.cmd"

#define COMMAND_MAX_NR_ARGS 6 // does not include NULL terminating element in char **arr // includes treasure manager launch


#define MONITOR_COMMAND_MAX_SIZE 128 // sa ajunga

// commands taken by the treasure_hub program
typedef enum
  {
    START_MONITOR, LIST_HUNTS, LIST_TREASURES, VIEW_TREASURE, STOP_MONITOR, EXIT, CALCULATE_SCORE,
    OTHER // to be left as last
  } MONITOR_COMMAND;

// communication signals:
// SIGUSR1 - normal feature
// SIGUSR2 - stop_monitor
#define SIGNAL_COMMAND SIGUSR1
#define SIGNAL_TERMINATE SIGUSR2

void transform_int_to_string ( int integer, char *string );
