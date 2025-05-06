/*
  TREASURE HUB header
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

// monitor program launch command
#define MONITOR_PROGRAM_LAUNCH "./treasure_hub_monitor.exe"

// common file between hub_main and monitor to share commands
#define COMMANDS_FILENAME "monitor_commands_file.cmd"


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

// Declarații funcții

int start_monitor();
int stop_monitor();
int send_command(command_t *cmd);
int handle_sigchld(int sig);
int print_prompt();
int parse_user_input(char *input);
int check_monitor_status();
