/*
  TREASURE HUB
*/

// program to create and send signal(s) to the monitor process
// this program validates commands for the monitor and sends them to intermediary file

#include "treasure_hub_monitor.h"

// flag to remember if monitor is active or not
int flag_monitor_active; // not exactly elegant, couldn't find a better way to make it work

MONITOR_COMMAND menu ( char *command_line )
{
  MONITOR_COMMAND ret = OTHER;

  printf ( "\n__________________________________________\n" );
  printf ( "Optiuni Treasure Hub:\n" );
  printf ( "start_monitor\n" );
  printf ( "list_hunts\n" );
  printf ( "list_treasures\n" );
  printf ( "view_treasure\n" );
  printf ( "stop_monitor\n" );
  printf ( "exit\n" );

  char aux[MONITOR_COMMAND_MAX_SIZE];
  char ch;

  char *token;
  char sep[] = " \n"; // space and \n

  while ( ret == OTHER )
    {
      printf ( "\nInserati optiunea (no spaces after):\n" );

      while ( ( ch = fgetc ( stdin ) ) == '\n' ); // clean any '\n' from buffer

      // used aux to clear '\n' chars in buffer before reading command
      if ( fgets ( aux, MONITOR_COMMAND_MAX_SIZE, stdin ) == NULL ) // check fgets for errors
	{
	  printf ( "Comanda invalida\n" );
	  ret = OTHER;
	  continue;
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

      strcpy ( aux, command_line );
      token = strtok ( aux, sep );

      if ( strcmp ( command_line, "start_monitor" ) == 0 )
	{
	  ret = START_MONITOR;
	}
      else if ( strcmp ( command_line, "stop_monitor" ) == 0 )
	{
	  ret = STOP_MONITOR;
	}
      else if ( strcmp ( command_line, "exit" ) == 0 )
	{
	  ret = EXIT;
	}
      else if ( strcmp ( token, "list_hunts" ) == 0 )
	{
	  ret = LIST_HUNTS;
	}
      else if ( strcmp ( token, "list_treasures" ) == 0 )
	{
	  ret = LIST_TREASURES;
	}
      else if ( strcmp ( token, "view_treasure" ) == 0 )
	{
	  ret = VIEW_TREASURE;
	}
      else
	{
	  ret = OTHER;
	}
    }

  printf ( "\n__________________________________________\n" );
  
  return ret;
}

void handle_SIGCHLD ( int signal ) // requires child to send SIGCHLD on termination
{
  flag_monitor_active = 0;
}

int main ( void )
{
  int commands_file = open ( COMMANDS_FILENAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR ); // always reset the file to empty-file

  if ( commands_file == -1 )
    {
      printf ( "Eroare la creare fisier comenzi -- hub-level\n" );
      
      exit ( 1 );
    }
  
  char command_line[MONITOR_COMMAND_MAX_SIZE];
  MONITOR_COMMAND opt;

  pid_t pid_monitor = -1;
  flag_monitor_active = 0;

  struct sigaction sa;
  sa.sa_handler = &handle_SIGCHLD;
  sigemptyset ( &sa.sa_mask );
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;  // SA_RESTART evita întreruperea apelurilor blocante
  
  if ( sigaction ( SIGCHLD, &sa, NULL ) == -1 )
    {
      printf ( "Eroare la alocare sigaction -- hub level\n" );
      close ( commands_file );
      exit ( 1 ) ;
    }
  
  while ( ( opt = menu ( command_line ) ) != OTHER ) // should go on forever (the condition)
    {
      if ( !flag_monitor_active ) pid_monitor = -1; // in case stop_monitor is "asynchronous" // does not stop immediately
      
      char *token;
      char sep[] = " \n"; // space and \n
      char terminating_line_char[] = "\n";
      char copy_command_line[MONITOR_COMMAND_MAX_SIZE]; // just in case command_line content is somehow altered after using strtok

      strcpy ( copy_command_line, command_line );
      
      // first the monitor process related options // as in influencing the process itself
      switch ( opt )
	{
	case START_MONITOR:
	  
	  if ( strcmp ( command_line, "start_monitor" ) == 0 )
	    {
	      if ( pid_monitor > 0 ) // monitor already exists
		{
		  printf ( "ERROR :: Monitor already active\n" );
		  opt = OTHER;
		  continue;
		}
	      else
		{
		  pid_monitor = fork();
		  
		  if ( pid_monitor == -1 )
		    {
		      printf ( "Eroare la creare monitor process -- fork\n" );
		      opt = OTHER;
		      continue;
		    }
		  
		  if ( pid_monitor == 0 ) // monitor ( child ) process
		    {
		      execlp ( MONITOR_PROGRAM_LAUNCH, MONITOR_PROGRAM_LAUNCH, NULL );

		      printf ( "Eroare la create monitor process -- exec -- %d\n", errno );
		    }
		  else // hub ( parent ) process
		    {
		      flag_monitor_active = 1;
		      printf ( "Monitor started as process %d\n", pid_monitor ); // for debug
		      // printf ( "Monitor started\n" ); // for final draft
		    }
		}
	    }
	  else
	    {
	      printf ( "Comanda invalida\n" );
	      opt = OTHER;
	      continue;
	    }
	  
	  break;
	  
	case STOP_MONITOR:
	  
	  if ( strcmp ( command_line, "stop_monitor" ) == 0 )
	    {
	      if ( pid_monitor < 0 ) // monitor doesn't exist
		{
		  printf ( "ERROR :: Monitor not active\n" );
		  opt = OTHER;
		  continue;
		}
	      else
		{
		  kill ( pid_monitor, SIGNAL_TERMINATE );
		  // pid_monitor = -1;
		}
	    }
	  else
	    {
	      printf ( "Comanda invalida\n" );
	      opt = OTHER;
	      continue;
	    }
	  
	  break;
	  
	case EXIT:
	  
	  if ( strcmp ( command_line, "exit" ) == 0 )
	    {
	      if ( !flag_monitor_active && pid_monitor == -1 ) // monitor inactive
		{
		  opt = OTHER;
		  goto EXIT_LOOP; // because a simple break would (probably) relate only to the switch-case
		}
	      else
		{
		  opt = OTHER;
		  printf ( "ERROR :: Monitor STILL active\n" );
		}
	    }
	  else
	    {
	      printf ( "Comanda invalida\n" );
	      opt = OTHER;
	      continue;
	    }
	  
	  break;
	  
	case LIST_HUNTS:
	  
	  if ( flag_monitor_active ) // monitor active
	    {
	      if ( strcmp ( command_line, "list_hunts" ) == 0 )
		{
		  if ( ( write ( commands_file, command_line, strlen ( command_line ) ) ) != strlen ( command_line ) )
		    {
		      printf ( "Eroare la scriere in commands_file command_line\n" );
		      exit ( 1 );
		    }

		  if ( ( write ( commands_file, terminating_line_char, strlen ( terminating_line_char ) ) ) != strlen ( terminating_line_char ) )
		    {
		      printf ( "Eroare la scriere in commands_file terminating line\n" );
		      exit ( 1 );
		    }

		  kill ( pid_monitor, SIGNAL_COMMAND );
		}
	      else
		{
		  printf ( "Comanda invalida\n" );
		  opt = OTHER;
		  continue;
		}
	    }
	  else
	    {
	      printf ( "ERROR :: Monitor NOT active\n" );
	      opt = OTHER;
	    }
	  
	  break;
	  
	case LIST_TREASURES:

	  if ( flag_monitor_active ) // monitor active
	    {
	      token = strtok ( copy_command_line, sep );

	      if ( strcmp ( token, "list_treasures" ) != 0 )
		{
		  printf ( "Comanda invalida\n" );
		  opt = OTHER;
		  continue;
		}

	      token = strtok ( NULL, sep ); // choose HUNT_ID
	      token = strtok ( NULL, sep ); // should return NULL on OK command

	      if ( token != NULL ) // command not OK
		{
		  printf ( "Comanda invalida\n" );
		  opt = OTHER;
		  continue;
		}

	      if ( ( write ( commands_file, command_line, strlen ( command_line ) ) ) != strlen ( command_line ) )
		{
		  printf ( "Eroare la scriere in commands_file command_line\n" );
		  exit ( 1 );
		}
	      
	      if ( ( write ( commands_file, terminating_line_char, strlen ( terminating_line_char ) ) ) != strlen ( terminating_line_char ) )
		{
		  printf ( "Eroare la scriere in commands_file terminating line\n" );
		  exit ( 1 );
		}
	      
	      kill ( pid_monitor, SIGNAL_COMMAND );
	    }
	  else
	    {
	      printf ( "ERROR :: Monitor NOT active\n" );
	      opt = OTHER;
	    }
	  
	  break;
	  
	case VIEW_TREASURE:

	  if ( flag_monitor_active ) // monitor active
	    {
	      token = strtok ( copy_command_line, sep );

	      if ( strcmp ( token, "view_treasure" ) != 0 )
		{
		  printf ( "Comanda invalida\n" );
		  opt = OTHER;
		  continue;
		}

	      token = strtok ( NULL, sep ); // choose HUNT_ID
	      token = strtok ( NULL, sep ); // choose TREASURE_ID
	      token = strtok ( NULL, sep ); // should return NULL on OK command

	      if ( token != NULL ) // command not OK
		{
		  printf ( "Comanda invalida\n" );
		  opt = OTHER;
		  continue;
		}

	      if ( ( write ( commands_file, command_line, strlen ( command_line ) ) ) != strlen ( command_line ) )
		{
		  printf ( "Eroare la scriere in commands_file command_line\n" );
		  exit ( 1 );
		}
	      
	      if ( ( write ( commands_file, terminating_line_char, strlen ( terminating_line_char ) ) ) != strlen ( terminating_line_char ) )
		{
		  printf ( "Eroare la scriere in commands_file terminating line\n" );
		  exit ( 1 );
		}
	      
	      kill ( pid_monitor, SIGNAL_COMMAND );
	    }
	  else
	    {
	      printf ( "ERROR :: Monitor NOT active\n" );
	      opt = OTHER;
	    }
	  
	  break;
	  
	default: // shouldn't reach this point
	  
	  opt = OTHER;
	  printf ( "Eroare la switch ( opt )\n" );
	  
	  break;
	}

      usleep (  SECOND_TO_MICROSECONDS / 10 * 1 ); // to ensure menu waits for monitor to finish work
    }

 EXIT_LOOP:

  close ( commands_file );

  // on exit() fnc call, all child processes should be terminated
  // memory leak treating can be implemented, but time is short
  // so let's pretend that the computer has enough memory to not worry about those
  
  if ( flag_monitor_active && pid_monitor > 0 ) // just to be sure
    kill ( pid_monitor, SIGKILL );
  
  return 0;
}
