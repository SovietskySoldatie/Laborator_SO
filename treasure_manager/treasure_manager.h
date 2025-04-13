#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
  Treasure Manager system:

  Used macros can be later swapped for more dynamic solutions (especially for strings)
*/

// operations and command line macros

#define OPERATION_STRING_SIZE 32
#define OPERATIONS_NUMBER 5

#define COMMAND_LINE_SIZE 128 // worst-case, enough place for expansion of specifications

typedef enum
  {
    ADD_TREASURE = 0, LIST_HUNT, VIEW_TREASURE, REMOVE_TREASURE, REMOVE_HUNT,
    OTHER // to be left as final value no matter what
  } OPERATION;

// hunt & path related macros

#define HUNT_ID_SIZE 32
#define HUNT_LOG_FILENAME "logs.txt"
#define TREASURE_GENERAL_FILENAME "treasure.data"

// treasure struct

// treasure buffer reader macro

#define TREASURE_BUFFER_SIZE 20 // number of treasures saved in buffer // => 4080 < 4096 bytes read at once

// treasure macros

#define TREASURE_ID_SIZE 32
#define USER_NAME_SIZE 32
#define CLUE_TEXT_SIZE 128

typedef struct // total size == 204 bytes
{
  int32_t value;
  float latitude, longitude;
  char id[TREASURE_ID_SIZE]; // id as string due to example
  char user_name[USER_NAME_SIZE], clue_text[CLUE_TEXT_SIZE];
} TREASURE;

void print_treasure ( TREASURE treasure );

// function to form string for filepath ( later to be used in open() function )

#define FILE_PATH_INCREMENT HUNT_ID_SIZE+1+TREASURE_ID_SIZE

// function to get file_descriptor for said file ( determined by char **order: 0-root, final-file )

int get_file_descriptor ( uint32_t size, char **strings, int flags );

// function used to get log_file_descriptor, specific for main()

#define CREATE_FILE ~0 // => ~CREATE_FILE == 0

int get_log_file_descriptor ( const char *hunt_id, int create_file_flag );

// list hunt function

int list_hunt ( const char hunt_id[HUNT_ID_SIZE] );

// view hunt function

int view_treasure ( const char hunt_id[HUNT_ID_SIZE], const char treasure_id[TREASURE_ID_SIZE] );

/*
  add <hunt_id>
*/

// function to read treasure from stdin

TREASURE *read_treasure ( void );

// function to add treasure to file

int add_treasure ( const char hunt_id[HUNT_ID_SIZE] );

// function to remove treasure from hunt

int remove_treasure ( const char hunt_id[HUNT_ID_SIZE], const char treasure_id[TREASURE_ID_SIZE] );

// function to remove hunt

int remove_hunt ( const char hunt_id[HUNT_ID_SIZE] );
