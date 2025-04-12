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

// operations macros

#define OPERATION_STRING_SIZE 32
#define OPERATIONS_NUMBER 5

typedef enum
  {
    ADD_TREASURE = 0, LIST_HUNT, VIEW_TREASURE, REMOVE_TREASURE, REMOVE_HUNT,
    OTHER // to be left as final value no matter what
  } OPERATION;

// hunt & path related macros

#define HUNT_ID_SIZE 32 // auxiliary, used later
#define TREASURE_GENERAL_FILENAME "treasure.data"
#define HUNT_LOG_FILENAME "log.txt"

// treasure struct

// treasure buffer reader macro

#define TREASURE_BUFFER_SIZE 20 // number of treasures saved // => 4080 < 4096 bytes read at once

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

// list hunt function

void list_hunt ( const char hunt_id[HUNT_ID_SIZE] );

// view hunt function

void view_treasure ( const char hunt_id[HUNT_ID_SIZE], const char treasure_id[TREASURE_ID_SIZE] );

/*
  add <hunt_id>
*/

// function to read treasure from stdin

TREASURE *read_treasure ( void );

// function to add treasure to file

void add_treasure ( const char hunt_id[HUNT_ID_SIZE] );

// function to remove treasure from hunt

void remove_reasure (  );

// function to remove hunt

void remove_hunt (  );

