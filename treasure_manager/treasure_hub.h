/*
	TREASURE HUB
*/

typedef enum {
    CMD_NONE = 0,
    CMD_START_MONITOR,
    CMD_LIST_HUNTS,
    CMD_LIST_TREASURES,
    CMD_VIEW_TREASURE,
    CMD_STOP_MONITOR
} command_type_t;

// Structura pentru comanda transmisă monitorului
typedef struct {
    command_type_t cmd;
    char hunt_id[256];
    char treasure_id[256];
} command_t;

// Definim semnalele folosite
#define SIGNAL_COMMAND   SIGUSR1
#define SIGNAL_TERMINATE SIGUSR2

// Fișierul prin care treasure_hub comunică cu monitorul
#define COMMAND_FILE "monitor_command.txt"

// Timpul de întârziere pentru închiderea monitorului (microsecunde)
#define MONITOR_DELAY_US 500000

// Variabilă externă pentru PID-ul monitorului
extern pid_t monitor_pid;
extern int monitor_running;

// Declarații funcții

int start_monitor();
int stop_monitor();
int send_command(command_t *cmd);
int handle_sigchld(int sig);
int print_prompt();
int parse_user_input(char *input);
int check_monitor_status();