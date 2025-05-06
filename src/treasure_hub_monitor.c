/*
  TREASURE HUB
 */

#include "treasure_hub_monitor.h"

int commands_file;

void handle_SIGNAL_COMMAND ( int signal ) // SIGUSR1 used to fork and exec into normal hunt feature
{
  char command_line[MONITOR_COMMAND_MAX_SIZE];
  char aux[MONITOR_COMMAND_MAX_SIZE];
  char ch;

  // used aux to clear '\n' chars in buffer before reading command
  if ( fgets ( aux, MONITOR_COMMAND_MAX_SIZE, commands_file ) == NULL ) // check fgets for errors
    {
      printf ( "Eroare la citire comanda -- monitor level\n" );
      exit ( 1 );
    }
  else // eliminate possible terminating '\n' char
    {
      if ( ch != '\n' ) // insert char taken in while condition
	{
	  command_line[0] = ch;
	  command_line[1] = '\0';
	  strcat ( command_line, aux );
	}
      if ( command_line[strlen ( command_line ) - 1] == '\n' )
	command_line[strlen ( command_line ) - 1] = '\0';
    }
  
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
	strpcy ( args[i], "--" );
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
      free ( args[i] );
      args[i] = NULL;
    }

  pid_t pid_treasure_manager = fork();
		  
  if ( pid_monitor == -1 ) // failure to fork
    {
      printf ( "Eroare la creare treasure manager process -- fork\n" );
      opt = OTHER;
      continue;
    }
  
  if ( pid_treasure_manager == 0 ) // treasure_manager (child) process
    {
      execvp ( TREASURE_MANAGER_LAUNCH, args );
      printf ( "Eroare la create treasure manager process -- exec\n" );
    }

  int status; // may be used for debug
  wait(&status); // wait for treasure manager to finish // to ensure arguments are preserved

  for ( i = 0; args[i] != NULL && i < COMMAND_MAX_NR_ARGS + 1; i++ )
    free ( args[i] );

  free ( args );
}

void handle_SIGNAL_TERMINATE ( int signal ) // SIGUSR2 used to terminate monitor (this) process
{
  goto EXIT_PROCESS;
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
  
  if ( sigaction ( SIGNAL_TERMINATE, &sa1, NULL ) == -1 )
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

 EXIT_PROCESS:
  close ( commands_file );

  return 0;
}
