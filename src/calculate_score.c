#include "treasure_manager.h"

#define ARRAY_INCREMENT 32

typedef struct
{
  int score;
  char user_name[USER_NAME_SIZE];
} USER_SCORE;

void print_procedure ( int pipe_id, char *print, int print_size, int hub_id )
{
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
  
  if ( bytes_written < 0 )
    printf ( "Eroare la scriere in pipe\n" );
}

int check_for_pos ( TREASURE elem, USER_SCORE *array, int size )
{
  for ( int i = 0; i < size; i++ )
    if ( strcmp ( elem.user_name, array[i].user_name ) == 0 )
      return i;

  return size;
}

int main ( int argc, char **args )
{
  // printf ( "DEBUG Enters calculate_score for %s\n", args[1] );
  
  int pipe_id = atoi ( args[2] ), hub_id = atoi ( args[3] ); // get pipe and hub for print

  int file_descriptor = open ( args[1], O_RDONLY ); // get treasure.data file desccriptor

  int array_size_act = 0, array_size_max = ARRAY_INCREMENT;

  USER_SCORE *array_score = ( USER_SCORE *  ) malloc ( array_size_max * sizeof ( USER_SCORE ) );
  if ( array_score == NULL )
    {
      printf ( "Eroare la alocare array score\n" );
      // free ( print );
      return 1;
    }

  TREASURE buffer[TREASURE_BUFFER_SIZE]; // buffer == sectiune stil array din treasures from treasure.data
  ssize_t bytes_read;
  uint32_t elements_read;

  while ( ( bytes_read = read ( file_descriptor, buffer, TREASURE_BUFFER_SIZE * sizeof ( TREASURE ) ) ) > 0 )
    {
      
      if ( bytes_read % sizeof ( TREASURE ) ) // a treasure read was incomplete
	{
	  printf ( "Treasure read was incomplete\n" );
	  // free ( print );
	  close ( file_descriptor );
	  return 1;
	}
      
      elements_read = bytes_read / sizeof ( TREASURE );
      for ( uint32_t i = 0; i < elements_read; i++ )
	{
	  int index = check_for_pos ( buffer[i], array_score, array_size_act ); // find position in array // that should be occupied

	  if ( index == array_size_act )
	    {
	      if ( array_size_act == array_size_max )
		{
		  array_size_max += ARRAY_INCREMENT;
		  array_score = realloc ( array_score, array_size_max * sizeof ( USER_SCORE ) );
		  if ( array_score == NULL )
		    {
		      printf ( "Eroare la realocare array score\n" );
		      // free ( print );
		      return 1;
		    }
		}
	      array_score[index].score = buffer[i].value;
	      strcpy ( array_score[index].user_name, buffer[i].user_name );
	      array_size_act++;
	    }
	  else
	    array_score[index].score += buffer[i].value;
	}
    }
  close ( file_descriptor );

  if ( bytes_read < 0 )
    {
      printf ( "Eroare la citire in buffer\n" );
      return 1;
    }

  /*
   for ( int i = 0; i < array_size_act; i++ )
    {
      printf ( "DEBUG\t\tUser : %s | Score : %d\n", array_score[i].user_name, array_score[i].score );
    }
  */
  
   // printf ( "DEBUG Finished calculating scores for %s\n", args[1] );
  
  
  // the printing bullshit
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

  for ( int i = 0; i < array_size_act; i++ )
    {
      sprintf ( string, "\t\tUser : %s | Score : %d\n", array_score[i].user_name, array_score[i].score );

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

  free ( array_score );

  print_procedure ( pipe_id, print, print_size, hub_id );

  free ( print );
  
  return 0;
}
