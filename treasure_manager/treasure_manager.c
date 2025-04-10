/*
  Treasure Manager system:
*/

#include "treasure_manager.h"

void print_treasure ( TREASURE treasure )
{
  printf ( "\tTreasure ID: %s\n", treasure.id );
  printf ( "\tTreasure user: %s\n", treasure.user_name );
  printf ( "\tTreasure clue: %s\n", treasure.clue_text );
  printf ( "\tTreasure value: %d\n", treasure.value );
  printf ( "\tTreasure coordinates: latitude: %f | longitude: %f\n", treasure.latitude, treasure.longitude );
  printf ( "\n" );
}

// function to form string for filepath ( later to be used in open() function )

#define FILE_PATH_INCREMENT HUNT_ID_SIZE+1+TREASURE_ID_SIZE

char *form_filepath ( const char *parent_path, const char *destination_path )
{
  char *filepath;
  uint32_t str_size = strlen ( parent_path ) + 1 < FILE_PATH_INCREMENT ? FILE_PATH_INCREMENT : FILE_PATH_INCREMENT + strlen ( parent_path ) + 1; // assigns enough size to filepath string, depending on needs
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

// function to get file_descriptor for said file ( determined by char **order: 0-root, final-file )

int get_file_descriptor ( uint32_t size, const char **strings )
{
  // getting the treasure.data filepath

  char *filepath = NULL;

  for ( uint32_t i = 0; i < size; i++ )
    {
      if ( filepath == NULL ) // base case, technically only for i == 0
	filepath = form_filepath ( "", strings[0] );
      else
	filepath = form_filepath ( filepath, strings[i] );

      if ( filepath == NULL ) // string error case
	{
	  printf ( "Eroare la formare filepath" );
	  return -1;
	}
    }

  /*
    how to modify to set flags separately?
    Parameter similar to registers?
  */

  int file_descriptor = open ( filepath, O_RDONLY ); // file descriptor for general treasure file

  free ( filepath );

  return file_descriptor;
}

// list hunt function

void list_hunt ( const char hunt_id[HUNT_ID_SIZE] )
{
  struct stat statbuf;

  if ( stat ( hunt_id, &statbuf ) == -1 ) // to make sure there is specified hunt and extract meta-data
    {
      printf ( "Nu a fost gasit hunt ID: %s\n", hunt_id );
      return;
    }

  if ( !S_ISDIR ( statbuf.st_mode ) ) // to make sure hunt is a folder (correct representation in memory)
    {
      printf ( "%s nu este reprezentat in memorie corect\n", hunt_id );
      return;
    }

  /*
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    TREBUIE LUAT SIZE DE PE FISIERUL TRESAURE GENERAL, NU DE PE FOLDER
   */

  printf ( "Printing hunt:\n" );
  printf ( "Hunt name: %s\n", hunt_id );
  printf ( "Total file size: %lu\n", ( uint64_t ) statbuf.st_size ); // %lu may not work on 32 bit systems as intended; written for 64 bit systems to not get warning
  printf ( "Last time modified: %s\n", ctime ( &statbuf.st_mtime ) ); // converts time into string format

  int file_descriptor = get_file_descriptor ( 3, { ".", hunt_id, TREASURE_GENERAL_FILENAME } );

  if ( file_descriptor < 0 )
    {
      printf ( "Eroare la creare file_descriptor\n" );
      return;
    }

  // getting and printing treasures

  TREASURE buffer[TREASURE_BUFFER_SIZE]; // buffer == sectiune stil array din treasures from treasure.data
  ssize_t bytes_read;
  uint32_t elements_read;

  while ( ( bytes_read = read ( file_descriptor, buffer, TREASURE_BUFFER_SIZE * sizeof ( TREASURE ) ) ) > 0 )
    {
      if ( bytes_read % elements_read ) // a treasure read was incomplete
	{
	  printf ( "Treasure read was incomplete\n" );
	  return;
	}
      
      elements_read = bytes_read / sizeof ( TREASURE );
      for ( uint32_t i = 0; i < elements_read; i++ )
	print_treasure ( buffer[i] );
    }

  close ( file_descriptor );

  if ( bytes_read < 0 )
    {
      printf ( "Eroare la citire in buffer\n" );
      return;
    }
}

void view_hunt ( const char hunt_id[HUNT_ID_SIZE], const char treasure_id[TREASURE_ID_SIZE] )
{
  struct stat statbuf;

  if ( stat ( hunt_id, &statbuf ) == -1 ) // to make sure there is specified hunt and extract meta-data
    {
      printf ( "Nu a fost gasit hunt ID: %s\n", hunt_id );
      return;
    }

  if ( !S_ISDIR ( statbuf.st_mode ) ) // to make sure hunt is a folder (correct representation in memory)
    {
      printf ( "%s nu este reprezentat in memorie corect\n", hunt_id );
      return;
    }

  int file_descriptor = get_file_descriptor ( 3, { ".", hunt_id, TREASURE_GENERAL_FILENAME } );

  if ( file_descriptor < 0 )
    {
      printf ( "Eroare la creare file_descriptor\n" );
      return;
    }

  // getting treasures and searching

  TREASURE buffer[TREASURE_BUFFER_SIZE]; // buffer == sectiune stil array din treasures from treasure.data
  ssize_t bytes_read;
  uint32_t elements_read;

  while ( ( bytes_read = read ( file_descriptor, buffer, TREASURE_BUFFER_SIZE * sizeof ( TREASURE ) ) ) > 0 )
    {
      if ( bytes_read % elements_read ) // a treasure read was incomplete
	{
	  printf ( "Treasure read was incomplete\n" );
	  return;
	}
      
      elements_read = bytes_read / sizeof ( TREASURE );
      for ( uint32_t i = 0; i < elements_read; i++ )
	if ( strcmp ( buffer[i].id, treasure_id ) == 0 )
	  {
	    print_treaure ( buffer[i] );
	    
	  }
    }

  close ( file_descriptor );

  if ( bytes_read < 0 )
    {
      printf ( "Eroare la citire in buffer\n" );
      return;
    }

  printf ( "Treasure %s nu a fost gasit\n", treasure_id );
}
