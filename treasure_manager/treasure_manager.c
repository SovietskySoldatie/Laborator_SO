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

  To be later written as a library

  Used macros can be later swapped for more dynamic solutions (especially for strings)
*/

// hunt & path related macros

#define HUNT_ID_SIZE 32 // auxiliary, used later
#define TREASURE_GENERAL_FILENAME "treasure.data"

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

void print_treasure ( TREASURE treasure )
{
  printf ( "\tTreasure ID: %s\n", treasure.id );
  printf ( "\tTreasure user: %s\n", treasure.user_name );
  printf ( "\tTreasure clue: %s\n", treasure.clue_text );
  printf ( "\tTreasure value: %d\n", treasure.value );
  printf ( "\tTreasure coordinates: latitude: %f | longitude: %f\n", treasure.latitude, treasure.longitude );
  printf ( "\n" );
}

// function to form string for filepath ( later to be used in open() function

#define FILE_PATH_INCREMENT HUNT_ID_SIZE+1+TREASURE_ID_SIZE

char *form_filepath ( char const *parent_path, char const *destination_path )
{
  char *filepath;
  uint32_t str_size = strlen ( parent_path ) + 1 ? FILE_PATH_INCREMENT : FILE_PATH_INCREMENT + strlen ( parent_path ); // assigns enough size to filepath string, depending on needs
  filepath = ( char * ) malloc ( str_size * sizeof ( char  ) );
  // tertiary + malloc() replaced if-else below
  /*
  if ( strlen ( parent_path ) + 1 < FILE_PATH_INCREMENT )
    filepath = malloc ( FILE_PATH_INCREMENT * sizeof ( char ) );
  else
    filepath = malloc ( ( FILE_PATH_INCREMENT + strlen ( parent_path ) ) * sizeof ( char ) );
  */

  if ( filepath == NULL ) return NULL;
    
  strcpy ( filepath, parent_path );
  strcat ( filepath, "/" );

  if ( strlen ( filepath ) + strlen ( destination_path ) >= str_size )
    {
      str_size += FILE_PATH_INCREMENT;
      filepath = ( char * ) realloc ( filepath, str_size * sizeof ( char ) );

      if ( filepath == NULL ) return NULL;
    }
  
  strcat ( filepath, destination_path );

  filepath = ( char * ) realloc ( filepath, ( strlen ( filepath ) + 1 ) * sizeof ( char ) );

  return filepath;
}

// list hunt function

void list_hunt ( char const hunt_id[HUNT_ID_SIZE] )
{
  struct stat statbuf;

  if ( stat ( hunt_id, &statbuf ) == -1 ) // to make sure there is sepcified hunt and extract meta-data
    {
    printf ( "Nu a fost gasit hunt ID: %s\n", hunt_id );
    return;
    }

  if ( !S_ISDIR ( statbuf.st_mode ) ) // to make sure hunt is a folder (correct representation in memory)
    {
    printf ( "%s nu este reprezentat in memorie corect\n", hunt_id );
    return;
    }

  printf ( "Printing hunt:\n" );
  printf ( "Hunt name: %s\n", hunt_id );
  printf ( "Total file size: %lu\n", ( uint64_t ) statbuf.st_size ); // %lu may not work on 32 bit systems as intended; written for 64 bit systems to not get warning
  printf ( "Last time modified: %s\n", ctime ( &statbuf.st_mtime ) ); // converts time into string format

  // getting the treasure.data filepath

  char *filepath = form_filepath ( hunt_id, TREASURE_GENERAL_FILENAME );

  if ( filepath == NULL )
    {
      printf ( "Eroare la alocare string for treasure.data filepath\n" );
      return;
    }

  int file_descriptor = open ( filepath, O_RDONLY ); // file descriptor for general treasure file

  if ( file_descriptor == -1 )
    {
      printf ( "Eroare la gasire file_descriptor\n" );
      return;
    }

  free ( filepath );

  // getting and printing treasures

  TREASURE buffer[TREASURE_BUFFER_SIZE]; // buffer == sectiune stil array din treasures from treasure.data
  ssize_t bytes_read;
  uint32_t elements_read;

  while ( ( bytes_read = read ( file_descriptor, buffer, TREASURE_BUFFER_SIZE * sizeof ( TREASURE ) ) ) > 0 )
    {
      if ( bytes_read % elements_read ) // a treasure read was incomplete
	{
	  printf ( "Treasure read was incomplete" );
	  return;
	}
      
      elements_read = bytes_read / sizeof ( TREASURE );
      for ( uint32_t i = 0; i < elements_read; i++ )
	print_treasure ( buffer[i] );
    }

  if ( bytes_read < 0 )
    {
      printf ( "Eroare la citire in buffer" );
      return;
    }
}

int main ( int argc, char **argv )
{
  
  
  return 0;
}
