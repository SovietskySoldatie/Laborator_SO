/*
  Treasure Manager system:
*/

#include "treasure_manager.h"

void print_procedure ( int pipe_id, char *print, int print_size, int hub_id )
{
  // printf ( "\nDEBUG\n" );
  // printf ( "Reached print procedure\n" );
  int bytes_written, act_pointer = 0; // act_pointer used for pointer operations on print
  int len = strlen ( print ); // save number of bytes that have to be written
  print[len] = '\0';

  while ( act_pointer <= len )
    {
      int bytes_should_be_written = -1; // how many bytes should be written
      if ( act_pointer + PRINT_STRING_INCREMENT >= len )
	bytes_should_be_written = strlen ( print + act_pointer ) + 1;
      else
	bytes_should_be_written = PRINT_STRING_INCREMENT;

      bytes_written = write ( pipe_id, print + act_pointer, bytes_should_be_written );
      
      kill ( hub_id, SIGUSR1 );
      act_pointer += bytes_written;
    }

  /*
  while ( ( bytes_written = write ( pipe_id, print + act_pointer, (
						     act_pointer + PRINT_STRING_INCREMENT > print_size // write (INCREMENT) would overflow the BUFFER
						     ) ? strlen ( print + act_pointer ) + 1 : PRINT_STRING_INCREMENT ) // because I would hate complicating with a flag
	    ) > 0 )
    {
      // printf ( "\tWritten %d bytes to pipe %d\n", bytes_written, pipe_id );
      kill ( hub_id, SIGUSR1 );
      // printf ( "\tSent SIGUSR1 signal to %d process\n", hub_id );
      act_pointer += bytes_written;
    }
  */

  /*
  bytes_written = write ( pipe_id, print + act_pointer, (
						     act_pointer + PRINT_STRING_INCREMENT >= print_size // write (INCREMENT) would overflow the BUFFER
							 ) ? strlen ( print + act_pointer ) + 1 : PRINT_STRING_INCREMENT ); // because I would hate complicating with a flag
  // printf ( "\tWritten %d bytes to pipe %d\n", bytes_written, pipe_id );
  kill ( hub_id, SIGUSR1 );
  // printf ( "\tSent SIGUSR1 signal to %d process\n", hub_id );
  act_pointer += bytes_written;
  */
  
  if ( bytes_written < 0 )
    printf ( "Eroare la scriere in pipe\n" );

  // printf ( "Reached print procedure end of function\n" );
  // printf ( "\nEND DEBUG\n" );
}

// reallocation ifs do not have such a big impact on performance, at most only one of their bodies gets executed // implemented like this because I am too lazy to rewrite it // say no to vibe coding or anythin similar
void print_treasure ( TREASURE treasure, char **print, int *print_size )
{
  char string[PRINT_STRING_INCREMENT];
  string[0] = '\0';
  
  sprintf ( string, "__________________________________________________\n" );
  if ( *print_size <= strlen ( *print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      *print_size += PRINT_STRING_INCREMENT;
      *print = realloc ( *print, *print_size * sizeof ( char ) );

      if ( *print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  // return 1;
	}
    }
  strcat ( *print, string );
  
  sprintf ( string, "\tTreasure ID: %s\n", treasure.id );
  if ( *print_size <= strlen ( *print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      *print_size += PRINT_STRING_INCREMENT;
      *print = realloc ( *print, *print_size * sizeof ( char ) );

      if ( *print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  // return 1;
	}
    }
  strcat ( *print, string );
  
  sprintf ( string, "\tTreasure user: %s\n", treasure.user_name );
  if ( *print_size <= strlen ( *print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      *print_size += PRINT_STRING_INCREMENT;
      *print = realloc ( *print, *print_size * sizeof ( char ) );

      if ( *print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  // return 1;
	}
    }
  strcat ( *print, string );
  
  sprintf ( string, "\tTreasure clue: %s\n", treasure.clue_text );
  if ( *print_size <= strlen ( *print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      *print_size += PRINT_STRING_INCREMENT;
      *print = realloc ( *print, *print_size * sizeof ( char ) );

      if ( *print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  // return 1;
	}
    }
  strcat ( *print, string );
  
  sprintf ( string, "\tTreasure value: %d\n", treasure.value );
  if ( *print_size <= strlen ( *print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      *print_size += PRINT_STRING_INCREMENT;
      *print = realloc ( *print, *print_size * sizeof ( char ) );

      if ( *print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  // return 1;
	}
    }
  strcat ( *print, string );
  
  sprintf ( string, "\tTreasure coordinates: latitude: %f | longitude: %f\n", treasure.latitude, treasure.longitude );
  if ( *print_size <= strlen ( *print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      *print_size += PRINT_STRING_INCREMENT;
      *print = realloc ( *print, *print_size * sizeof ( char ) );

      if ( *print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  // return 1;
	}
    }
  strcat ( *print, string );
  
  sprintf ( string, "\n" );
  if ( *print_size <= strlen ( *print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      *print_size += PRINT_STRING_INCREMENT;
      *print = realloc ( *print, *print_size * sizeof ( char ) );

      if ( *print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  // return 1;
	}
    }
  strcat ( *print, string );
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

int get_log_file_descriptor ( const char *hunt_id, int create_file_flag ) // special case of get_file_descriptor()
{
  // form char ** to be sent as parameter for log file

  char **strings = ( char ** ) malloc ( 3 * sizeof ( char * ) );
  if ( strings == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      return 1;
    }

  strings[0] = ( char * ) malloc ( ( strlen ( "." ) + 1 ) * sizeof ( char ) );
  strings[1] = ( char * ) malloc ( ( strlen ( hunt_id ) + 1 ) * sizeof ( char ) );
  strings[2] = ( char * ) malloc ( ( strlen ( HUNT_LOG_FILENAME ) + 1 ) * sizeof ( char ) );

  if ( strings[0] == NULL || strings[1] == NULL || strings[2] == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      return 1;
    }

  // form strings
  strcpy ( strings[0], "." );
  strcpy ( strings[1], hunt_id );
  strcpy ( strings[2], HUNT_LOG_FILENAME );

  int log_file_descriptor;
  
  if ( create_file_flag )
    log_file_descriptor = get_file_descriptor ( 3, strings, O_CREAT | O_WRONLY | O_APPEND );
  else
    log_file_descriptor = get_file_descriptor ( 3, strings, O_WRONLY | O_APPEND );

  // create symlink to log file
  
  if ( log_file_descriptor >= 0 && create_file_flag ) // only done if log file created
    {
      char hard_filepath[strlen ( "./" ) + HUNT_ID_SIZE + strlen ( HUNT_LOG_FILENAME ) + 3];
      char sym_filepath[strlen ( SYM_LOG_FILENAME_START ) + HUNT_ID_SIZE + 1]; // string to create filepath for symlink to log hunt

      // form actual filepath
      
      strcpy ( hard_filepath, "./" );
      strcat ( hard_filepath, hunt_id );
      strcat ( hard_filepath, "/" );
      strcat ( hard_filepath, HUNT_LOG_FILENAME );

      // form sym filepath
      
      strcpy ( sym_filepath, SYM_LOG_FILENAME_START );
      strcat ( sym_filepath, hunt_id );

      if ( symlink ( hard_filepath, sym_filepath ) != 0 ) // create symlink // check for errors
	{
	  printf ( "Eroare la creare symbolic link to log file\n" );
	}
    }

  free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings );

  return log_file_descriptor;
}

// list hunt function

int list_hunt ( const char hunt_id[HUNT_ID_SIZE], int pipe_id, int hub_id )
{
  struct stat statbuf;

  int print_size = PRINT_STRING_INCREMENT; // used to remember print buffer maximum size // for no memory leaks
  char *print = ( char * ) malloc ( print_size * sizeof ( char ) ); // buffer for pipe
  char string[PRINT_STRING_INCREMENT]; // string concatenated repeatedly to print buffer

  if ( print == NULL )
    {
      printf ( "Eroare la alocare print string\n" );
      return 1;
    }

  print[0] = '\0';
  string[0] = '\0';

  if ( stat ( hunt_id, &statbuf ) == -1 ) // to make sure there is specified hunt and extract meta-data
    {
      sprintf ( string, "Nu a fost gasit hunt ID: %s\n", hunt_id );
      if ( print_size <= strlen ( print ) + strlen ( string ) + 1 ) // to include '\0'
	{
	  print_size += PRINT_STRING_INCREMENT;
	  print = realloc ( print, print_size * sizeof ( char ) );
	  
	  if ( print == NULL )
	    {
	      printf ( "Eroare la realocare print string\n" );
	      return 1;
	    }
	}
      strcat ( print, string );

      print_procedure ( pipe_id, print, print_size, hub_id );

      free ( print );
      
      return 0;
    }

  if ( !S_ISDIR ( statbuf.st_mode ) ) // to make sure hunt is a folder (correct representation in memory)
    {
      printf ( "%s nu este reprezentat in memorie corect\n", hunt_id );
      return 1;
    }

  // form char ** to be sent as parameter
  // done so if specifications change to use nested folders (inside hunt primary folder)
  // for now, implementation is a bit hardcoded

  char **strings = ( char ** ) malloc ( 3 * sizeof ( char * ) );
  if ( strings == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      free ( print );
      return 1;
    }

  strings[0] = ( char * ) malloc ( ( strlen ( "." ) + 1 ) * sizeof ( char ) );
  strings[1] = ( char * ) malloc ( ( strlen ( hunt_id ) + 1 ) * sizeof ( char ) );
  strings[2] = ( char * ) malloc ( ( strlen ( TREASURE_GENERAL_FILENAME ) + 1 ) * sizeof ( char ) );

  if ( strings[0] == NULL || strings[1] == NULL || strings[2] == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );

      free ( print );
      
      if ( strings[0] != NULL )
	free ( strings[0] );
      if ( strings[1] != NULL )
	free ( strings[1] );
      if ( strings[2] != NULL )
	free ( strings[2] );
      free ( strings );
      
      return 1;
    }

  // form strings
  strcpy ( strings[0], "." );
  strcpy ( strings[1], hunt_id );
  strcpy ( strings[2], TREASURE_GENERAL_FILENAME );

  // to read file meta-data
  char filepath[strlen ( hunt_id ) + strlen ( TREASURE_GENERAL_FILENAME ) + 5];
  strcpy ( filepath, "./" );
  strcat ( filepath, hunt_id );
  strcat ( filepath, "/" );
  strcat ( filepath, TREASURE_GENERAL_FILENAME );

  if ( stat ( filepath, &statbuf ) == -1 ) // to make sure there is treasure.data and extract meta-data
    {
      printf ( "Nu a fost gasit treasure.data\n" );
      free ( print );
      return 0;
    }

  sprintf ( string, "Printing hunt:\n" );
  if ( print_size <= strlen ( print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      print_size += PRINT_STRING_INCREMENT;
      print = realloc ( print, print_size * sizeof ( char ) );

      if ( print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  return 1;
	}
    }
  strcat ( print, string );
  
  sprintf ( string, "Hunt name: %s\n", hunt_id );
  if ( print_size <= strlen ( print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      print_size += PRINT_STRING_INCREMENT;
      print = realloc ( print, print_size * sizeof ( char ) );

      if ( print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  return 1;
	}
    }
  strcat ( print, string );
  
  sprintf ( string, "Total data file size: %lu bytes\n", ( uint64_t ) statbuf.st_size ); // %lu may not work on 32 bit systems as intended; written for 64 bit systems to not get warning
  if ( print_size <= strlen ( print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      print_size += PRINT_STRING_INCREMENT;
      print = realloc ( print, print_size * sizeof ( char ) );

      if ( print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  return 1;
	}
    }
  strcat ( print, string );
  
  sprintf ( string, "Last time modified: %s\n", ctime ( &statbuf.st_mtime ) ); // converts time into string format
  if ( print_size <= strlen ( print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      print_size += PRINT_STRING_INCREMENT;
      print = realloc ( print, print_size * sizeof ( char ) );

      if ( print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  return 1;
	}
    }
  strcat ( print, string );
  

  int file_descriptor = get_file_descriptor ( 3, strings, O_RDONLY );

  if ( file_descriptor < 0 )
    {
      printf ( "Eroare la creare file_descriptor\n" );
      free ( print );
      free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings );
      return 1;
    }

  // printf ( "Got file descriptor\n" );

  // getting and printing treasures

  TREASURE buffer[TREASURE_BUFFER_SIZE]; // buffer == sectiune stil array din treasures from treasure.data
  ssize_t bytes_read;
  uint32_t elements_read;

  while ( ( bytes_read = read ( file_descriptor, buffer, TREASURE_BUFFER_SIZE * sizeof ( TREASURE ) ) ) > 0 )
    {
      // liniile comentate au rol de debug
      
      // printf ( "Entered while ( reading bytes ) loop\n" );
      // printf ( "Read %ld bytes\n", bytes_read );

      
      // problema aici la aritmetica floats
      // solved
      
      if ( bytes_read % sizeof ( TREASURE ) ) // a treasure read was incomplete
	{
	  printf ( "Treasure read was incomplete\n" );
	  free ( print );
	  free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings );
	  close ( file_descriptor );
	  return 1;
	}
      
      // printf ( "Skipped incomplete treasure read\n" );
      
      elements_read = bytes_read / sizeof ( TREASURE );
      for ( uint32_t i = 0; i < elements_read; i++ )
	print_treasure ( buffer[i], &print, &print_size );
    }
  free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings ); 
  close ( file_descriptor );

  if ( bytes_read < 0 )
    {
      printf ( "Eroare la citire in buffer\n" );
      return 1;
    }

  print_procedure ( pipe_id, print, print_size, hub_id );

  free ( print );

  return 0;
}

int view_treasure ( const char hunt_id[HUNT_ID_SIZE], const char treasure_id[TREASURE_ID_SIZE], int pipe_id, int hub_id )
{
  struct stat statbuf;

  int print_size = PRINT_STRING_INCREMENT; // used to remember print buffer maximum size // for no memory leaks
  char *print = ( char * ) malloc ( print_size * sizeof ( char ) ); // buffer for pipe
  char string[PRINT_STRING_INCREMENT]; // string concatenated repeatedly to print buffer

  if ( print == NULL )
    {
      printf ( "Eroare la alocare print string\n" );
      return 1;
    }

  print[0] = '\0';
  string[0] = '\0';

  if ( stat ( hunt_id, &statbuf ) == -1 ) // to make sure there is specified hunt and extract meta-data
    {
      sprintf ( string, "Nu a fost gasit hunt ID: %s\n", hunt_id );
      
      if ( print_size <= strlen ( print ) + strlen ( string ) + 1 ) // to include '\0'
	{
	  print_size += PRINT_STRING_INCREMENT;
	  print = realloc ( print, print_size * sizeof ( char ) );
	  
	  if ( print == NULL )
	    {
	      printf ( "Eroare la realocare print string\n" );
	      return 1;
	    }
	}
      strcat ( print, string );

      print_procedure ( pipe_id, print, print_size, hub_id );

      free ( print );
      
      return 0;
    }

  if ( !S_ISDIR ( statbuf.st_mode ) ) // to make sure hunt is a folder (correct representation in memory)
    {
      printf ( "%s nu este reprezentat in memorie corect\n", hunt_id );
      free ( print );
      return 1;
    }

  // form char ** to be sent as parameter
  // done so if specifications change to use nested folders (inside hunt primary folder)
  // for now, implementation is a bit hardcoded

  char **strings = ( char ** ) malloc ( 3 * sizeof ( char * ) );
  if ( strings == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      free ( print );
      return 1;
    }

  strings[0] = ( char * ) malloc ( ( strlen ( "." ) + 1 ) * sizeof ( char ) );
  strings[1] = ( char * ) malloc ( ( strlen ( hunt_id ) + 1 ) * sizeof ( char ) );
  strings[2] = ( char * ) malloc ( ( strlen ( TREASURE_GENERAL_FILENAME ) + 1 ) * sizeof ( char ) );

  if ( strings[0] == NULL || strings[1] == NULL || strings[2] == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );

      free ( print );
      
      if ( strings[0] != NULL )
	free ( strings[0] );
      if ( strings[1] != NULL )
	free ( strings[1] );
      if ( strings[2] != NULL )
	free ( strings[2] );
      free ( strings );
      
      return 1;
    }

  // form strings
  strcpy ( strings[0], "." );
  strcpy ( strings[1], hunt_id );
  strcpy ( strings[2], TREASURE_GENERAL_FILENAME );

  int file_descriptor = get_file_descriptor ( 3, strings, O_RDONLY );

  if ( file_descriptor < 0 )
    {
      printf ( "Eroare la creare file_descriptor\n" );
      free ( print );
      free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings );
      return 1;
    }

  // getting treasures and searching

  TREASURE buffer[TREASURE_BUFFER_SIZE]; // buffer == sectiune stil array din treasures from treasure.data
  ssize_t bytes_read;
  uint32_t elements_read;

  while ( ( bytes_read = read ( file_descriptor, buffer, TREASURE_BUFFER_SIZE * sizeof ( TREASURE ) ) ) > 0 )
    {
      // liniile comentate au rol de debug
      
      // printf ( "Entered while ( reading bytes ) loop\n" );
      // printf ( "Read %ld bytes\n", bytes_read );

      
      // problema aici la aritmetica floats
      // solved
      
      if ( bytes_read % sizeof ( TREASURE ) ) // a treasure read was incomplete
	{
	  printf ( "Treasure read was incomplete\n" );
	  free ( print );
	  free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings );
	  close ( file_descriptor );
	  return 1;
	}
      
      // printf ( "Skipped incomplete treasure read\n" );
      
      elements_read = bytes_read / sizeof ( TREASURE );
      for ( uint32_t i = 0; i < elements_read; i++ )
	if ( strcmp ( buffer[i].id, treasure_id ) == 0 )
	  {
	    print_treasure ( buffer[i], &print, &print_size );

	    print_procedure ( pipe_id, print, print_size, hub_id );

	    free ( print );
	    free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings );
	    close ( file_descriptor );
	    return 0;
	  }
    }
  
  free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings ); 
  close ( file_descriptor );

  if ( bytes_read < 0 )
    {
      printf ( "Eroare la citire in buffer\n" );
      free ( print );
      return 1;
    }

  sprintf ( string, "Nu a fost gasit treasure ID: %s\n", treasure_id );
  
  if ( print_size <= strlen ( print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      print_size += PRINT_STRING_INCREMENT;
      print = realloc ( print, print_size * sizeof ( char ) );

      if ( print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  return 1;
	}
    }
  strcat ( print, string );

  print_procedure ( pipe_id, print, print_size, hub_id );

  free ( print );

  return 0;
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
  char aux[TREASURE_ID_SIZE];

  printf ( "Please insert treasure ID | MAX %u charachters:\n", TREASURE_ID_SIZE - 1 );

  while ( ( ch = fgetc ( stdin ) ) == '\n' ); // clean any '\n' from buffer

  // used aux to clear '\n' chars in buffer before reading treasure ID
  if ( fgets ( aux, TREASURE_ID_SIZE, stdin ) == NULL ) // check fgets for errors
    {
      printf ( "Eroare la citire treasure ID\n" );
      free ( treasure );
      return NULL;
    }
  else // eliminate possible terminating '\n' char
    {
      if ( ch != '\n' ) // insert char taken in while condition
	{
	  treasure->id[0] = ch;
	  treasure->id[1] = '\0';
	  strcat ( treasure->id, aux );
	}
      if ( treasure->id[strlen ( treasure->id ) - 1] == '\n' )
	treasure->id[strlen ( treasure->id ) - 1] = '\0';
    }

  printf ( "Please insert treasure User Name | MAX %u charachters:\n", USER_NAME_SIZE - 1 );
  
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

  printf ( "Please insert treasure Clue Text | MAX %u charachters:\n", CLUE_TEXT_SIZE - 1 );
  
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

int add_treasure ( const char hunt_id[HUNT_ID_SIZE] )
{
  struct stat statbuf;

  if ( stat ( hunt_id, &statbuf ) == -1 ) // if there is NOT already a hunt with hunt_id
    {
      if ( mkdir ( hunt_id, S_IRUSR | S_IWUSR | S_IXUSR ) != 0 ) // create hunt directory // full owner permissions
	{
	  printf ( "Could not create hunt ID: %s\n", hunt_id );
	  return 1;
	}

      // create ( and immediately close ) log file

      int log_file_descriptor = get_log_file_descriptor ( hunt_id, CREATE_FILE );

      if ( log_file_descriptor < 0 )
	{
	  printf ( "Eroare la creare log file for hunt %s\n", hunt_id );
	  return 1;
	}
      
      close ( log_file_descriptor );
      
      printf ( "Created hunt ID: %s\n", hunt_id );
    }

  TREASURE *treasure = read_treasure ();

  if ( treasure == NULL )
    {
      printf ( "Eroare la citire treasure\n" );
      return 1;
    }

  // form char ** to be sent as parameter
  // done so if specifications change to use nested folders (inside hunt primary folder)
  // for now, implementation is a bit hardcoded

  char **strings = ( char ** ) malloc ( 3 * sizeof ( char * ) );
  if ( strings == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      return 1;
    }

  strings[0] = ( char * ) malloc ( ( strlen ( "." ) + 1 ) * sizeof ( char ) );
  strings[1] = ( char * ) malloc ( ( strlen ( hunt_id ) + 1 ) * sizeof ( char ) );
  strings[2] = ( char * ) malloc ( ( strlen ( TREASURE_GENERAL_FILENAME ) + 1 ) * sizeof ( char ) );

  if ( strings[0] == NULL || strings[1] == NULL || strings[2] == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      return 1;
    }

  // form strings
  strcpy ( strings[0], "." );
  strcpy ( strings[1], hunt_id );
  strcpy ( strings[2], TREASURE_GENERAL_FILENAME );

  int file_descriptor = get_file_descriptor ( 3, strings, O_RDWR | O_CREAT | O_APPEND );

  free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings );

  if ( file_descriptor < 0 )
    {
      printf ( "Eroare la creare file_descriptor\n" );
      free ( treasure );
      return 1;
    }

  // check if treasure was already added to file

  TREASURE buffer[TREASURE_BUFFER_SIZE]; // buffer == sectiune stil array din treasures from treasure.data
  ssize_t bytes_read;
  uint32_t elements_read;

  lseek ( file_descriptor, 0, SEEK_SET );

  while ( ( bytes_read = read ( file_descriptor, buffer, TREASURE_BUFFER_SIZE * sizeof ( TREASURE ) ) ) > 0 )
    {
      if ( bytes_read % sizeof ( TREASURE ) ) // a treasure read was incomplete
	{
	  printf ( "Treasure read was incomplete\n" );
	  close ( file_descriptor );
	  free ( treasure );
	  return 1;
	}
      
      elements_read = bytes_read / sizeof ( TREASURE );
      for ( uint32_t i = 0; i < elements_read; i++ )
	if ( strcmp ( buffer[i].id, treasure->id ) == 0 )
	  {
	    printf ( "Treasure %s already was added to the file\n", treasure->id );
	    // print_treasure ( buffer[i] ); // linie folosita pentru debug
	    free ( treasure );
	    close ( file_descriptor );
	    return 0; // still considered succesful
	  }
    }

  if ( bytes_read < 0 )
    {
      printf ( "Eroare la citire in buffer\n" );
      free ( treasure );
      close ( file_descriptor );
      return 1;
    }
  
  ssize_t bytes_written = write ( file_descriptor, treasure, sizeof ( TREASURE ) );

  close ( file_descriptor );

  if ( bytes_written == -1 || bytes_written != sizeof ( TREASURE ) ) // write error 
    {
      printf ( "Error when writing to file\n" );
      printf ( "Bytes written: %ld\n", bytes_written );
      free ( treasure );
      return 1;
    }

  free ( treasure );

  printf ( "\nTreasure added succesfully\n" );

  return 0;
}

// function to remove treasure from hunt

int remove_treasure ( const char hunt_id[HUNT_ID_SIZE], const char treasure_id[TREASURE_ID_SIZE] )
{
  struct stat statbuf;

  if ( stat ( hunt_id, &statbuf ) == -1 ) // to make sure there is specified hunt and extract meta-data
    {
      printf ( "Nu a fost gasit hunt ID: %s\n", hunt_id );
      return 0;
    }

  if ( !S_ISDIR ( statbuf.st_mode ) ) // to make sure hunt is a folder (correct representation in memory)
    {
      printf ( "%s nu este reprezentat in memorie corect\n", hunt_id );
      return 1;
    }

  // form char ** to be sent as parameter
  // done so if specifications change to use nested folders (inside hunt primary folder)
  // for now, implementation is a bit hardcoded

  char **strings = ( char ** ) malloc ( 3 * sizeof ( char * ) );
  if ( strings == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      return 1;
    }

  strings[0] = ( char * ) malloc ( ( strlen ( "." ) + 1 ) * sizeof ( char ) );
  strings[1] = ( char * ) malloc ( ( strlen ( hunt_id ) + 1 ) * sizeof ( char ) );
  strings[2] = ( char * ) malloc ( ( strlen ( TREASURE_GENERAL_FILENAME ) + 1 ) * sizeof ( char ) );

  if ( strings[0] == NULL || strings[1] == NULL || strings[2] == NULL )
    {
      printf ( "Eroare la creare parametru strings pentru filepath\n" );
      
      if ( strings[0] != NULL )
	free ( strings[0] );
      if ( strings[1] != NULL )
	free ( strings[1] );
      if ( strings[2] != NULL )
	free ( strings[2] );
      free ( strings );
      
      return 1;
    }

  // form strings
  strcpy ( strings[0], "." );
  strcpy ( strings[1], hunt_id );
  strcpy ( strings[2], TREASURE_GENERAL_FILENAME );

  int file_descriptor = get_file_descriptor ( 3, strings, O_RDWR );

  if ( file_descriptor < 0 )
    {
      printf ( "Eroare la creare file_descriptor\n" );
      free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings );
      return 1;
    }

  free ( strings[0] ); free ( strings[1] ); free ( strings[2] ); free ( strings );

  // getting treasures and searching

  TREASURE *clone = NULL; // array to remember all treasures in file
  uint32_t size_max = TREASURE_BUFFER_SIZE, size_act = 0, treasure_found_flag = 0;

  clone = ( TREASURE * ) malloc ( size_max * sizeof ( TREASURE ) );
  if ( clone == NULL )
    {
      printf ( "Eroare la alocare clone array\n" );
      close ( file_descriptor );

      return 1;
    }

  TREASURE buffer[TREASURE_BUFFER_SIZE]; // buffer == sectiune stil array din treasures from treasure.data
  ssize_t bytes_read, bytes_written;
  uint32_t elements_read;

  while ( ( bytes_read = read ( file_descriptor, buffer, TREASURE_BUFFER_SIZE * sizeof ( TREASURE ) ) ) > 0 )
    {
      // liniile comentate au rol de debug
      
      // printf ( "Entered while ( reading bytes ) loop\n" );
      // printf ( "Read %ld bytes\n", bytes_read );
      
      if ( bytes_read % sizeof ( TREASURE ) ) // a treasure read was incomplete
	{
	  printf ( "Treasure read was incomplete\n" );
	  free ( clone );
	  close ( file_descriptor );
	  return 1;
	}
      
      // printf ( "Skipped incomplete treasure read\n" );
      
      elements_read = bytes_read / sizeof ( TREASURE );
      for ( uint32_t i = 0; i < elements_read; i++ )
	{
	  if ( strcmp ( buffer[i].id, treasure_id ) == 0 ) // treasure found => skip array element
	    {
	      treasure_found_flag = 1;
	    }
	  else // add normal array element
	    {
	      clone[size_act++] = buffer[i];
	    }

	  if ( size_act == size_max ) // reallocate clone size when needed
	    {
	      size_max += TREASURE_BUFFER_SIZE;

	      clone = ( TREASURE * ) realloc ( clone, size_max * sizeof ( TREASURE ) );
	      if ( clone == NULL )
		{
		  printf ( "Eroare la alocare clone array\n" );
		  close ( file_descriptor );
		  
		  return 1;
		}
	    }
	}
    }

  if ( bytes_read < 0 )
    {
      printf ( "Eroare la citire in buffer\n" );
      close ( file_descriptor );
      
      return 1;
    }

  if ( !treasure_found_flag ) // leave file as is
    printf ( "Nu a fost gasit treasure ID: %s\n", treasure_id );
  else // overwrite file to eliminate treasure found
    {
      // done so to not lose time moving cursor in file
      
      lseek ( file_descriptor, 0, SEEK_SET ); // set file cursor to beginning
      
      ftruncate ( file_descriptor, 0 ); // empty file content

      // to not use more declarations
      
      size_max = size_act;
      size_act = 0;
      
      while ( size_act < size_max ) // rewrite file
	{
	  uint32_t elements_to_write = TREASURE_BUFFER_SIZE;
      
	  if ( size_max - size_act < TREASURE_BUFFER_SIZE )
	    elements_to_write = size_max - size_act;

	  bytes_written = write ( file_descriptor, &clone[size_act], elements_to_write * sizeof ( TREASURE ) );

	  if ( bytes_written != elements_to_write * sizeof ( TREASURE ) ) // treasure write was incomplete
	    {
	      printf ( "Eroare la rescriere in fisier\n" );
	      close ( file_descriptor );
	      free ( clone );

	      return 1;
	    }

	  size_act += elements_to_write;
	}

      printf ( "Treasure was removed succesfully\n" );
    }
  
  close ( file_descriptor );
  free ( clone );
  
  return 0;
}

// function to remove hunt

int remove_hunt ( const char hunt_id[HUNT_ID_SIZE] )
{
  struct stat stat_buf;
  
  if ( stat ( hunt_id, &stat_buf ) != 0 )
    {
      printf ( "Nu a fost gasit hunt ID: %s\n", hunt_id );
      return 1;
    }
  if ( !S_ISDIR ( stat_buf.st_mode ) )
    {
      printf ( "Eroare la reprezentarea in memorie a hunt ID: %s\n", hunt_id );
      return 1;
    }
  
  // hard-coded implementation, will be changed with specifications when multiple files are used

  char filepath[strlen ( hunt_id ) + strlen ( TREASURE_GENERAL_FILENAME ) + 4];
  // + 4 to include '.', '/' and '\0'
  // TREASURE_GENERAL_FILENAME has more chars than HUNT_LOG_FILENAME

  // start with log file symbolic link

  char sym_filepath[strlen ( SYM_LOG_FILENAME_START ) + HUNT_ID_SIZE + 1]; // string to create filepath for symlink to log hunt
  
  strcpy ( sym_filepath, SYM_LOG_FILENAME_START );
  strcat ( sym_filepath, hunt_id );

  if ( unlink ( sym_filepath ) != 0 ) // check for errors
    {
      printf ( "Eroare la stergere log file link\n" );
      return 1;
    }

  // repeat for log file proper
  // prefer to lose log file rather than data file

  strcpy ( filepath, "." );

  strcat ( filepath, "/" );
  strcat ( filepath, hunt_id );
  
  strcat ( filepath, "/" );
  strcat ( filepath, HUNT_LOG_FILENAME );
  
  if ( unlink ( filepath ) != 0 ) // check for errors
    {
      printf ( "Eroare la stergere log file\n" );
      return 1;
    }

  // repeat for data file

  strcpy ( filepath, "." );

  strcat ( filepath, "/" );
  strcat ( filepath, hunt_id );
  
  strcat ( filepath, "/" );
  strcat ( filepath, TREASURE_GENERAL_FILENAME );
  
  if ( unlink ( filepath ) != 0 ) // check for errors
    {
      printf ( "Eroare la stergere data file\n" );
      return 1;
    }

  // apply to folder

  strcpy ( filepath, "." );

  strcat ( filepath, "/" );
  strcat ( filepath, hunt_id );

  if ( rmdir ( filepath ) != 0 ) // check for errors
    {
      printf ( "Eroare la stergere folder\n" );
      return 1;
    }
  
  return 0;
}

int list_all_hunts ( int pipe_id, int hub_id )
{
  int flag_no_hunt_found = 1;
  
  DIR *dir = opendir ( CURRENT_FOLDER ); // DIR struct already exists :D
  DIRENT *entry;
  
  if ( dir == NULL )
    {
      printf ( "Eroare la deschiderea directorului curent\n" );
      return 1;
    }

  int print_size = PRINT_STRING_INCREMENT; // used to remember print buffer maximum size // for no memory leaks
  char *print = ( char * ) malloc ( print_size * sizeof ( char ) ); // buffer for pipe
  char string[PRINT_STRING_INCREMENT]; // string concatenated repeatedly to print buffer

  if ( print == NULL )
    {
      printf ( "Eroare la alocare print string\n" );
      return 1;
    }

  print[0] = '\0';
  string[0] = '\0';

  sprintf ( string, "Printing all hunts:\n" );
  
  if ( print_size <= strlen ( print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      print_size += PRINT_STRING_INCREMENT;
      print = realloc ( print, print_size * sizeof ( char ) );

      if ( print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  return 1;
	}
    }
  strcat ( print, string );
  
  while ( ( entry = readdir ( dir ) ) != NULL )
    {
      if ( strcmp ( entry->d_name, CURRENT_FOLDER ) == 0 || strcmp ( entry->d_name, PREVIOUS_FOLDER ) == 0 ) // avoid "." and ".." folders
	continue;

      // evaluate current entry if is folder or not
      char fullpath[FULLPATH_MAX_SIZE];

      strcpy ( fullpath, CURRENT_FOLDER );
      strcat ( fullpath, "/" );
      strcat ( fullpath, entry->d_name );

      struct stat statbuf;
      
      if ( stat ( fullpath, &statbuf ) != 0 )
	{
	  printf ( "Eroare la stat intrare in folder curent\n" );
	  free ( print );
	  return 1;
	}
      
      if ( S_ISDIR ( statbuf.st_mode ) )
	{
	  flag_no_hunt_found = 0;
	  sprintf( string, "\tHUNT: %s | ", entry->d_name );

	  if ( print_size <= strlen ( print ) + strlen ( string ) + 1 ) // to include '\0'
	    {
	      print_size += PRINT_STRING_INCREMENT;
	      print = realloc ( print, print_size * sizeof ( char ) );
	      
	      if ( print == NULL )
		{
		  printf ( "Eroare la realocare print string\n" );
		  return 1;
		}
	    }
	  strcat ( print, string );
	  
	  strcat ( fullpath, "/" );
	  strcat ( fullpath, TREASURE_GENERAL_FILENAME );

	  if ( stat ( fullpath, &statbuf ) != 0 )
	    {
	      printf ( "\n\tEroare la deschidere treasure data file in Hunt\n" );
	      free ( print );
	      return 1;
	    }

	  sprintf ( string, "Number of treasures: %lu\n", ( uint64_t ) statbuf.st_size / sizeof ( TREASURE ) );

	  if ( print_size <= strlen ( print ) + strlen ( string ) + 1 ) // to include '\0'
	    {
	      print_size += PRINT_STRING_INCREMENT;
	      print = realloc ( print, print_size * sizeof ( char ) );
	      
	      if ( print == NULL )
		{
		  printf ( "Eroare la realocare print string\n" );
		  return 1;
		}
	    }
	  strcat ( print, string );
	}
    }

  if ( flag_no_hunt_found )
    {
      sprintf ( string, "\tNo hunts found\n" );
      if ( print_size <= strlen ( print ) + strlen ( string ) + 1 ) // to include '\0'
	{
	  print_size += PRINT_STRING_INCREMENT;
	  print = realloc ( print, print_size * sizeof ( char ) );
	  
	  if ( print == NULL )
	    {
	      printf ( "Eroare la realocare print string\n" );
	      return 1;
	    }
	}
      strcat ( print, string );
    }
  
  sprintf ( string, "\n" );
  if ( print_size <= strlen ( print ) + strlen ( string ) + 1 ) // to include '\0'
    {
      print_size += PRINT_STRING_INCREMENT;
      print = realloc ( print, print_size * sizeof ( char ) );

      if ( print == NULL )
	{
	  printf ( "Eroare la realocare print string\n" );
	  return 1;
	}
    }
  strcat ( print, string );

  print_procedure ( pipe_id, print, print_size, hub_id );

  free ( print );
  
  if ( closedir ( dir ) == -1 )
    {
      printf ( "Eroare la închiderea directorului" );
      return 1;
    }
  
  return 0;
}
