/*
  TREASURE HUB
*/

// program to create and send signal(s) to the monitor process
// this program validates commands for the monitor and sends them to intermediary file

#include "treasure_hub_monitor.h"

// flag to remember if monitor is active or not
int flag_monitor_active; // not exactly elegant, couldn't find a better way to make it work // while still maintanable
int pipe_print[2]; // pipe to get the printable results // 0 - read | 1 - write

char pipe_buffer[DISK_BUFFER_SIZE + 1]; // buffer in which operations results are stored

void transform_int_to_string ( int integer, char *string )
{
  int i = 0, pow = 1;
  
  while ( pow <= integer ) // form base 10 mask
    pow *= 10;
  pow /= 10;

  while ( pow )
    {
      string[i++] = integer / pow + '0';

      integer %= pow;
      pow /= 10;
    }
  
  string[i++] = '\0';
}

MONITOR_COMMAND menu ( char *command_line )
{
  usleep ( SECOND_TO_MICROSECONDS );
  MONITOR_COMMAND ret = OTHER;

  printf ( "\n__________________________________________\n" );
  printf ( "Optiuni Treasure Hub:\n" );
  printf ( "start_monitor\n" );
  printf ( "list_hunts\n" );
  printf ( "list_treasures\n" );
  printf ( "view_treasure\n" );
  printf ( "calculate_score\n" );
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
      else if ( strcmp ( token, "calculate_score" ) == 0 )
	{
	  ret = CALCULATE_SCORE;
	}
      else
	{
	  ret = OTHER;
	}
    }

  printf ( "\n__________________________________________\n" );
  
  return ret;
}

void handle_SIGUSR2 ( int signal ) // requires child to send SIGCHLD on termination
{
  flag_monitor_active = 0;
}

void handle_SIGUSR1 ( int signal ) // handle of the print signal // used SIGUSR1
{
  // printf ( "DEBUG\nEnter Hub level print function\n" );
  pipe_buffer[DISK_BUFFER_SIZE] = '\0'; // needed for large printable data sets

  int bytes_read;

  if ( ( bytes_read = read ( pipe_print[0], pipe_buffer, DISK_BUFFER_SIZE ) ) <= 0 )
    {
      printf ( "Eroare la citire din pipe la semnal\n" );
      return;
    }

  printf ( "%s", pipe_buffer );
  // printf ( "\nDEBUG\nExit Hub level print function %d\n", bytes_read );
}

int main ( void )
{
  if ( pipe ( pipe_print ) != 0 )
    {
      printf ( "Eroare la creare print pipe\n" );
      exit ( -1 );
    }

  // close ( pipe_print[1] ); // closes write end of pipe
  
  char pipe_print_write[NUMBER_DIGITS_LIMIT + 1];
  transform_int_to_string ( pipe_print[1], pipe_print_write );

  // printf ( "DEBUG HUB\n%d %s %d\nENDDEBUG\n", pipe_print[0], pipe_print_write, pipe_print[1] );
  
  int commands_file = open ( COMMANDS_FILENAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR ); // always reset the file to empty-file

  if ( commands_file == -1 )
    {
      printf ( "Eroare la creare fisier comenzi -- hub-level\n" );
      exit ( -2 );
    }
  
  char command_line[MONITOR_COMMAND_MAX_SIZE];
  MONITOR_COMMAND opt;

  pid_t pid_monitor = -1;
  flag_monitor_active = 0;

  struct sigaction sa;
  sa.sa_handler = &handle_SIGUSR2;
  sigemptyset ( &sa.sa_mask );
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;  // SA_RESTART evita întreruperea apelurilor blocante

  struct sigaction sa_print;
  sa_print.sa_handler = &handle_SIGUSR1;
  sigemptyset ( &sa_print.sa_mask );
  sa_print.sa_flags = SA_RESTART | SA_NOCLDSTOP;  // SA_RESTART evita întreruperea apelurilor blocante
  
  if ( sigaction ( SIGUSR2, &sa, NULL ) == -1 )
    {
      printf ( "Eroare la alocare sigaction monitor dies -- hub level\n" );
      close ( commands_file );
      exit ( -3 ) ;
    }
  
  if ( sigaction ( SIGUSR1, &sa_print, NULL ) == -1 )
    {
      printf ( "Eroare la alocare sigaction print -- hub level\n" );
      close ( commands_file );
      exit ( -3 ) ;
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
		      execlp ( MONITOR_PROGRAM_LAUNCH, MONITOR_PROGRAM_LAUNCH, pipe_print_write, NULL );

		      printf ( "Eroare la create monitor process -- exec -- %d\n", errno );
		    }
		  else // hub ( parent ) process
		    {
		      flag_monitor_active = 1;
		      // printf ( "Monitor started as process %d\n", pid_monitor ); // for debug
		      printf ( "Monitor started\n" ); // for final draft
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

	case CALCULATE_SCORE:

	  if ( strcmp ( command_line, "calculate_score" ) == 0 )
	    {
	      int flag_no_hunt_found = 1;
	      
	      DIR *dir = opendir ( CURRENT_FOLDER ); // DIR struct already exists :D
	      DIRENT *entry;
	      
	      if ( dir == NULL )
		{
		  printf ( "Eroare la deschiderea directorului curent\n" );
		  opt = OTHER;
		  continue;
		}

	      printf ( "Calculating score per hunt:\n" );

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
		      opt = OTHER;
		      continue;
		    }
		  
		  if ( S_ISDIR ( statbuf.st_mode ) )
		    {
		      flag_no_hunt_found = 0;
		      printf( "\tHUNT: %s\n", entry->d_name );
		      
		      strcat ( fullpath, "/" );
		      strcat ( fullpath, TREASURE_GENERAL_FILENAME );
		      
		      if ( stat ( fullpath, &statbuf ) != 0 )
			{
			  printf ( "\n\tEroare la deschidere treasure data file in Hunt\n" );
			  opt = OTHER;
			  continue;
			}
		      else
			{
			  int current_pid = getpid(); // get pid of hub before fork()
			  pid_t calculate_score_pid = fork();

			  if ( calculate_score_pid == -1 )
			    {
			      printf ( "Eroare la fork pentru calculate score\n" );
			      opt = OTHER;
			      continue;
			    }

			  if ( calculate_score_pid == 0 ) // calculate score ( child ) process
			    {
			      char current_pid_string[NUMBER_DIGITS_LIMIT];
			      transform_int_to_string ( current_pid, current_pid_string );
			      
			      execlp ( CALCULATE_SCORE_PROGRAM_LAUNCH, CALCULATE_SCORE_PROGRAM_LAUNCH, fullpath, pipe_print_write, current_pid_string, NULL );
			      printf ( "Eroare la exec pentru calculate score\n" );
			      opt = OTHER;
			      continue;
			    }
			  
			  int status;
			  waitpid(calculate_score_pid, &status, 0);
			}
		    }
		}

	      if ( flag_no_hunt_found )
		{
		  printf ( "No hunts found\n" );
		}
	    }
	  else
	    {
	      printf ( "Comanda invalida\n" );
	      opt = OTHER;
	      continue;
	    }
	  
	  break;
	  
	default: // shouldn't reach this point
	  
	  opt = OTHER;
	  printf ( "Eroare la switch ( opt )\n" );
	  
	  break;
	}
      
    }

 EXIT_LOOP:

  close ( pipe_print[0] );
  close ( pipe_print[1] );
  close ( commands_file );

  // on exit() fnc call, all child processes should be terminated
  // memory leak treating can be implemented, but time is short
  // so let's pretend that the computer has enough memory to not worry about those
  
  if ( flag_monitor_active && pid_monitor > 0 ) // just to be sure
    kill ( pid_monitor, SIGKILL );
  
  return 0;
}
