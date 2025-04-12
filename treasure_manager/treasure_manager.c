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

// function to get file_descriptor for said file ( determined by char **order: 0-root, final-file )

int get_file_descriptor ( uint32_t size, char **strings, int flags )
{
  uint32_t total_strlen = 0; // total strlen for final filepath

  for ( uint32_t i = 0; i < size; i++ )
    total_strlen += ( strlen ( strings[i] ) + 1 ); // to include '/' and '\0' chars

  char *filepath = ( char * ) malloc ( total_strlen * sizeof ( char ) );

  if ( filepath == NULL )
    {
      printf ( "Eroare la creare filepath for file descriptor\n" );
      return -1;
    }
  
  filepath[0] = '\0'; // to ensure strcat works properly

  for ( uint32_t i = 0; i < size; i++ )
    {
      strcat ( filepath, strings[i] );
      if ( i < size - 1 ) // not include '/' for final string ( file itself )
	strcat ( filepath, "/" );
    }
  
  int file_descriptor;

  if ( flags & O_CREAT ) // check to see if O_CREAT was included in flags // needs modes parameter
    file_descriptor = open ( filepath, flags, S_IRUSR | S_IWUSR ); // included read/write permission for owner
  else
    file_descriptor = open ( filepath, flags ); // file descriptor for general treasure file

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

  printf ( "Printing hunt:\n" );
  printf ( "Hunt name: %s\n", hunt_id );
  printf ( "Total file size: %lu\n", ( uint64_t ) statbuf.st_size ); // %lu may not work on 32 bit systems as intended; written for 64 bit systems to not get warning
  printf ( "Last time modified: %s\n", ctime ( &statbuf.st_mtime ) ); // converts time into string format

  // form char ** to be sent as parameter
  // done so if specifications change to use nested folders (inside hunt primary folder)
  // for now, implementation is a bit hardcoded

  char **strings = ( char ** ) malloc ( 3 * sizeof ( char * ) );
  if ( strings == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      return;
    }

  strings[0] = ( char * ) malloc ( ( strlen ( "." ) + 1 ) * sizeof ( char ) );
  strings[1] = ( char * ) malloc ( ( strlen ( hunt_id ) + 1 )* sizeof ( char ) );
  strings[2] = ( char * ) malloc ( ( strlen ( TREASURE_GENERAL_FILENAME ) + 1 )* sizeof ( char ) );

  if ( strings[0] == NULL || strings[1] == NULL || strings[2] == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      return;
    }

  // form strings
  strcpy ( strings[0], "." );
  strcpy ( strings[1], hunt_id );
  strcpy ( strings[2], TREASURE_GENERAL_FILENAME );

  int file_descriptor = get_file_descriptor ( 3, strings, O_RDONLY );

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

  free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings ); 
  close ( file_descriptor );

  if ( bytes_read < 0 )
    {
      printf ( "Eroare la citire in buffer\n" );
      return;
    }
}

void view_treasure ( const char hunt_id[HUNT_ID_SIZE], const char treasure_id[TREASURE_ID_SIZE] )
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

  // form char ** to be sent as parameter
  // done so if specifications change to use nested folders (inside hunt primary folder)
  // for now, implementation is a bit hardcoded

  char **strings = ( char ** ) malloc ( 3 * sizeof ( char * ) );
  if ( strings == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      return;
    }

  strings[0] = ( char * ) malloc ( ( strlen ( "." ) + 1 ) * sizeof ( char ) );
  strings[1] = ( char * ) malloc ( ( strlen ( hunt_id ) + 1 )* sizeof ( char ) );
  strings[2] = ( char * ) malloc ( ( strlen ( TREASURE_GENERAL_FILENAME ) + 1 )* sizeof ( char ) );

  if ( strings[0] == NULL || strings[1] == NULL || strings[2] == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      return;
    }

  // form strings
  strcpy ( strings[0], "." );
  strcpy ( strings[1], hunt_id );
  strcpy ( strings[2], TREASURE_GENERAL_FILENAME );

  int file_descriptor = get_file_descriptor ( 3, strings, O_RDONLY );

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
	    print_treasure ( buffer[i] );
	    return;
	  }
    }

  free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings ); 
  close ( file_descriptor );

  if ( bytes_read < 0 )
    {
      printf ( "Eroare la citire in buffer\n" );
      return;
    }

  printf ( "Treasure %s nu a fost gasit\n", treasure_id );
}

TREASURE *read_treasure ( void )
{
  TREASURE *treasure = ( TREASURE * ) malloc ( sizeof ( TREASURE ) );

  if ( treasure == NULL )
    {
      printf ( "Eroare la alocare pointer treasure\n" );
      return NULL;
    }
  
  char ch; // auxiliary char

  printf ( "Please insert treasure ID:\n" );

  while ( ( ch = fgetc ( stdin ) ) == '\n' ); // clean any '\n' from buffer

  if ( fgets ( treasure->id, TREASURE_ID_SIZE, stdin ) == NULL ) // check fgets for errors
    {
      printf ( "Eroare la citire treasure ID\n" );
      free ( treasure );
      return NULL;
    }
  else // eliminate possible terminating '\n' char
    {
      if ( treasure->id[strlen ( treasure->id ) - 1] == '\n' )
	treasure->id[strlen ( treasure->id ) - 1] = '\0';
    }

  printf ( "Please insert treasure User Name:\n" );
  
  if ( fgets ( treasure->user_name, USER_NAME_SIZE, stdin ) == NULL ) // check fgets for errors
    {
      printf ( "Eroare la citire treasure User Name\n" );
      free ( treasure );
      return NULL;
    }
  else // eliminate possible terminating '\n' char
    {
      if ( treasure->user_name[strlen ( treasure->user_name ) - 1] == '\n' )
	treasure->user_name[strlen ( treasure->user_name ) - 1] = '\0';
    }

  printf ( "Please insert treasure Clue Text:\n" );
  
  if ( fgets ( treasure->clue_text, CLUE_TEXT_SIZE, stdin ) == NULL ) // check fgets for errors
    {
      printf ( "Eroare la citire treasure Clue Text\n" );
      free ( treasure );
      return NULL;
    }
  else // eliminate possible terminating '\n' char
    {
      if ( treasure->clue_text[strlen ( treasure->clue_text ) - 1] == '\n' )
	treasure->clue_text[strlen ( treasure->clue_text ) - 1] = '\0';
    }

  printf ( "Please insert treasure Value: " );
  scanf ( "%d", &treasure->value );

  int flag_incorrect_floats;
  
  do
    {
      printf ( "Please insert treasure Coordinates:\n" );
      printf ( "Latitude: " );
      scanf ( "%f", &treasure->latitude );
      printf ( "Longitude: " );
      scanf ( "%f", &treasure->longitude );

      flag_incorrect_floats = 0;

      if ( !( -90 <= treasure->latitude && treasure->latitude <= 90 )
	   ||
	   !( -180 <= treasure->longitude && treasure->longitude <= 180 ) )
	{
	  printf ( "Latitude must be within -90 and 90 degrees; Longitude must be within -180 and 180 degrees\n" );
	  flag_incorrect_floats = 1;
	}
    } while ( flag_incorrect_floats );

  return treasure;
}

void add_treasure ( const char hunt_id[HUNT_ID_SIZE] )
{
  struct stat statbuf;

  if ( stat ( hunt_id, &statbuf ) == -1 ) // if there is NOT already a hunt with hunt_id
    {
      if ( mkdir ( hunt_id, S_IRUSR | S_IWUSR | S_IXUSR ) != 0 ) // create hunt directory // full owner permissions
	{
	  printf ( "Could not create hunt ID: %s\n", hunt_id );
	  return;
	}
      
      printf ( "Created hunt ID: %s\n", hunt_id );
    }

  TREASURE *treasure = read_treasure ();

  if ( treasure == NULL )
    {
      printf ( "Eroare la citire treasure\n" );
      return;
    }

  // form char ** to be sent as parameter
  // done so if specifications change to use nested folders (inside hunt primary folder)
  // for now, implementation is a bit hardcoded

  char **strings = ( char ** ) malloc ( 3 * sizeof ( char * ) );
  if ( strings == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      return;
    }

  strings[0] = ( char * ) malloc ( ( strlen ( "." ) + 1 ) * sizeof ( char ) );
  strings[1] = ( char * ) malloc ( ( strlen ( hunt_id ) + 1 )* sizeof ( char ) );
  strings[2] = ( char * ) malloc ( ( strlen ( TREASURE_GENERAL_FILENAME ) + 1 )* sizeof ( char ) );

  if ( strings[0] == NULL || strings[1] == NULL || strings[2] == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      return;
    }

  // form strings
  strcpy ( strings[0], "." );
  strcpy ( strings[1], hunt_id );
  strcpy ( strings[2], TREASURE_GENERAL_FILENAME );

  int file_descriptor = get_file_descriptor ( 3, strings, O_WRONLY | O_CREAT | O_APPEND );

  free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings );

  if ( file_descriptor < 0 )
    {
      printf ( "Eroare la creare file_descriptor\n" );
      free ( treasure );
      return;
    }

  int32_t bytes_written = write ( file_descriptor, treasure, sizeof ( TREASURE ) );

  close ( file_descriptor );

  if ( bytes_written == -1 || bytes_written != sizeof ( TREASURE ) ) // write error 
    {
      printf ( "Error when writing to file\n" );
      printf ( "Bytes written: %d\n", bytes_written );
      free ( treasure );
      return;
    }

  free ( treasure );

  printf ( "Treasure added succesfully\n" );
}
