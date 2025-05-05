/*
  TREASURE HUB
*/

// program to create and send signal(s) to the monitor process
// this program validates commands for the monitor and sends them to intermediary file

// communication signals:
// SIGUSR1 - normal feature
// SIGUSR2 - stop_monitor


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>

// monitor program :: 
#define MONITOR_PROGRAM_LAUNCH "./treasure_hub_monitor.exe"

// flag to remember if monitor is active or not
int flag_monitor_active;


#define MONITOR_COMMAND_MAX_SIZE 128 // sa ajunga

// commands taken by the treasure_hub program
typedef enum
  {
    START_MONITOR, LIST_HUNTS, LIST_TREASURES, VIEW_TREASURE, STOP_MONITOR, EXIT,
    OTHER // to be left as last
  } MONITOR_COMMAND;

MONITOR_COMMAND menu( char *command_line )
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

      token = strtok ( command_line, sep );

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
  
  return ret;
}

void handle_SIGCHLD ( int signal ) // requires child to send SIGCHLD on termination
{
  flag_monitor_active = 0;
}

int main ( void )
{
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
      perror ( "sigaction" );
      exit ( -1 ) ;
    }
  
  while ( ( opt = menu ( command_line ) ) != OTHER ) // should go on forever (the condition)
    {
      // first the monitor process related options // as in influencing the process itself
      switch (opt)
	{
	case START_MONITOR:
	  if ( strcmp ( command_line, "start_monitor" ) == 0 )
	    {
	      if ( pid_monitor > 0 ) // monitor already exists
		{
		  printf ( "Monitor already active\n" );
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
		      printf ( "Eroare la create monitor process -- exec\n" );
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
		  printf ( "Monitor not active\n" );
		  opt = OTHER;
		  continue;
		}
	      else
		{
		  kill ( pid_monitor, SIGUSR2 );
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
	  if (strcmp(command_line, "exit") == 0)
	    {
      // logica de iesire, dacă este necesară
	    }
	  else
	    {
	      printf("Comanda invalida\n");
	      opt = OTHER;
	      continue;
	    }
	  break;
	  
	case LIST_HUNTS:
	  // logica pentru listarea hunt-urilor
	  break;
	  
	case LIST_TREASURES:
	  // logica pentru listarea comoarelor
	  break;
	  
	case VIEW_TREASURE:
	  // logica pentru vizualizarea unei comori
	  break;
	  
	default:
	  // opțiune necunoscută
    break;
	}
    }
  
  return 0;
}
