/*
  TREASURE HUB
 */

#include "treasure_hub_monitor.h"

int commands_file;

void handle_SIGNAL_COMMAND ( int signal ) // SIGUSR1 used to fork and exec into normal hunt feature
{
  char command_line[MONITOR_COMMAND_MAX_SIZE];

  ssize_t bytes_read;
  bytes_read = read ( commands_file, command_line, ( MONITOR_COMMAND_MAX_SIZE - 1 ) * sizeof ( char ) );

  if ( bytes_read < 0 )
    {
      printf ( "Eroare la citire comanda -- monitor level\n" );
      exit ( 1 );
    }

  command_line[bytes_read + 1] = '\0'; // just to be sure

  if ( command_line[strlen ( command_line ) - 1] == '\n' )
    command_line[strlen ( command_line ) - 1] = '\0';
  
  char *token;
  char sep[] = " \n"; // space and \n

  char **args = ( char ** ) malloc ( ( COMMAND_MAX_NR_ARGS + 1 ) * sizeof ( char * ) );
  if ( args == NULL )
    {
      printf ( "Eroare la alocare char **args -- monitor level\n" );
      exit ( 1 );
    }

  int i;
  
  for ( i = 0; i < COMMAND_MAX_NR_ARGS + 1; i++ )
    {
      args[i] = ( char * ) malloc ( ( MONITOR_COMMAND_MAX_SIZE >> 1 ) * sizeof ( char ) ); // >> 2 ar fi la limita, din nou presupunem ca exista destula memorie
      if ( args[i] == NULL )
	{
	  printf ( "Eroare la alocare char *args -- monitor level\n" );

	  for ( int j = 0; j < i; j++ )
	    free ( args[j] );
	  free ( args );
	  
	  exit ( 1 );
	}

      if ( i == 1 ) // operation/feature selection argument
	strcpy ( args[i], "--" );
      else
	strcpy ( args[i], "" );
    }

  i = 0;
  strcat ( args[i], TREASURE_MANAGER_LAUNCH );
  token = strtok ( command_line, sep );

  do
    {
      strcat ( args[++i], token );
      token = strtok ( NULL, sep );
    } while ( token != NULL );

  for ( i = i + 1; i < COMMAND_MAX_NR_ARGS + 1; i++ )
    {
      if ( args[i] != NULL )
	free ( args[i] );
      args[i] = NULL;
    }

  /*
  printf ( "\n\tDebug monitor args[]:\n" );

  // debug reasons
  for ( i = 0; i < COMMAND_MAX_NR_ARGS + 1; i++ )
    {
      if ( args[i] != NULL )
	printf ( "\t\t%s\n", args[i] );
      else
	printf ( "\t\t%s\n", "NULL" );
    }
  printf ( "\n" );
  */

  if ( args[COMMAND_MAX_NR_ARGS] != NULL && i == COMMAND_MAX_NR_ARGS ) // hardcoded because IDK why it doesn't work
    free ( args[COMMAND_MAX_NR_ARGS] );
  args[COMMAND_MAX_NR_ARGS] = NULL;
  

  pid_t pid_treasure_manager = fork();
		  
  if ( pid_treasure_manager == -1 ) // failure to fork
    {
      printf ( "Eroare la creare treasure manager process -- fork\n" );
      exit ( 1 );
    }
  
  if ( pid_treasure_manager == 0 ) // treasure_manager (child) process
    {
      execvp ( args[0], args );
      printf ( "Eroare la create treasure manager process -- exec -- %d\n", errno );
    }

  int status; // may be used for debug
  waitpid ( pid_treasure_manager, &status, 0 ); // wait for treasure manager to finish // to ensure arguments are preserved

  for ( i = 0; args[i] != NULL && i < COMMAND_MAX_NR_ARGS + 1; i++ )
    free ( args[i] );

  free ( args );
}

void handle_SIGNAL_TERMINATE ( int signal ) // SIGUSR2 used to terminate monitor (this) process
{
  close ( commands_file );
  exit ( 0 );
}

int main ( void )
{
  // assign signal handling functions
  
  struct sigaction sa1;
  sa1.sa_handler = &handle_SIGNAL_COMMAND;
  sigemptyset ( &sa1.sa_mask );
  sa1.sa_flags = SA_RESTART | SA_NOCLDSTOP;  // SA_RESTART evita întreruperea apelurilor blocante
  
  if ( sigaction ( SIGNAL_COMMAND, &sa1, NULL ) == -1 )
    {
      printf ( "Eroare la alocare sigaction SIGNAL_COMMAND -- monitor level\n" );
      close ( commands_file );
      exit ( 1 ) ;
    }

  struct sigaction sa2;
  sa2.sa_handler = &handle_SIGNAL_TERMINATE;
  sigemptyset ( &sa2.sa_mask );
  sa2.sa_flags = SA_RESTART | SA_NOCLDSTOP;  // SA_RESTART evita întreruperea apelurilor blocante
  
  if ( sigaction ( SIGNAL_TERMINATE, &sa2, NULL ) == -1 )
    {
      printf ( "Eroare la alocare sigaction SIGNAL_TERMINATE -- monitor level\n" );
      close ( commands_file );
      exit ( 1 ) ;
    }

  // open commands file
  
  commands_file = open ( COMMANDS_FILENAME, O_RDONLY );
  
  if ( commands_file == -1 )
    {
      printf ( "Eroare la creare fisier comenzi -- hub-level\n" );
      
      exit ( 1 );
    }

  while ( 1 ); // idle state of process

  return 0;
}
