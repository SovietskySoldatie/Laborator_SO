#include "treasure_manager.h" // include toate dependentele

int main ( int argc, char **args )
{
  if ( argc != 3 && argc != 4 ) // check number of arguments
    {
      printf ( "Numar invalid de argumente\n" );
      exit ( -1 );
    }

  if ( strlen ( args[1] ) >= OPERATION_STRING_SIZE ) // check that args[1] can be a valid operation
    {
      printf ( "Argumentul corespunzator operatiei e prea lung\n" );
      exit ( -1 );
    }

  if ( strlen ( args[2] ) >= HUNT_ID_SIZE ) // check that args[2] can be a valid hunt_id
    {
      printf ( "Argumentul corespunzator hunt ID e prea lung\n" );
      exit ( -1 );
    }

  OPERATION operation = OTHER, starting_operation = ADD_TREASURE;

  char operations[OPERATIONS_NUMBER][OPERATION_STRING_SIZE] = // hard-coded operations
    {
      "--add",
      "--list",
      "--view",
      "--remove_treasure",
      "--remove_hunt"
    };

  for ( uint32_t i = 0; i < OPERATIONS_NUMBER; i++ )
    if ( strcmp ( operations[i], args[1] ) == 0 ) // match operations with args
      {
	operation = starting_operation + i;
	break;
      }

  int operation_succesful = 1; // should stay 1 for operation succesful, reset to 0 for not succesful operation
  // considered not succesful on error; succesful even on data not found

  int log_file_descriptor = -1;

  switch ( operation )
    {
    case ADD_TREASURE:
      
      if ( argc != 3 )
	{
	  operation = OTHER;
	  printf ( "Numar invalid de argumente pentru aceasta comanda\n" );
	}
      
      operation_succesful -= add_treasure ( args[2] );
      
      log_file_descriptor = get_log_file_descriptor ( args[2], ~CREATE_FILE );
      
      break;
      
    case LIST_HUNT:
      
      if ( argc != 3 )
	{
	  operation = OTHER;
	  printf ( "Numar invalid de argumente pentru aceasta comanda\n" );
	}
      
      operation_succesful -= list_hunt ( args[2] );
      
      log_file_descriptor = get_log_file_descriptor ( args[2], ~CREATE_FILE );
      
      break;
      
    case VIEW_TREASURE:
      
      if ( argc != 4 )
	{
	  operation = OTHER;
	  printf ( "Numar invalid de argumente pentru aceasta comanda\n" );
	}
      
      if ( strlen ( args[3] ) >= TREASURE_ID_SIZE ) // check that args[3] can be a valid treasure_id
	{
	  printf ( "Argumentul corespunzator treasure ID e prea lung\n" );
	  exit ( -1 );
	}
      
      operation_succesful -= view_treasure ( args[2], args[3] );
      
      log_file_descriptor = get_log_file_descriptor ( args[2], ~CREATE_FILE );
      
      break;
      
    case REMOVE_TREASURE:
      
      if ( argc != 4 )
	{
	  operation = OTHER;
	  printf ( "Numar invalid de argumente pentru aceasta comanda\n" );
	}
      
      if ( strlen ( args[3] ) >= TREASURE_ID_SIZE ) // check that args[3] can be a valid treasure_id
	{
	  printf ( "Argumentul corespunzator treasure ID e prea lung\n" );
	  exit ( -1 );
	}
      
      operation_succesful -= remove_treasure ( args[2], args[3] );
      
      log_file_descriptor = get_log_file_descriptor ( args[2], ~CREATE_FILE );
      
      break;
      
    case REMOVE_HUNT:
      
      if ( argc != 3 )
	{
	  operation = OTHER;
	  printf ( "Numar invalid de argumente pentru aceasta comanda\n" );
	}
      
      operation_succesful -= remove_hunt ( args[2] );
      
      break;
      
    case OTHER:
    default:
      
      printf ( "Comanda folosita este invalida\n" );
      
      break;
    }

  char string_command[COMMAND_LINE_SIZE]; // to make writing and checking easier

  if ( log_file_descriptor >= 0 ) // log file was found
    {
      if ( operation != REMOVE_HUNT && operation != OTHER )
	{
	  // form command string

	  strcpy ( string_command, "" );
	  
	  if ( !operation_succesful )
	    strcat ( string_command, "ERROR |" );
	  
	  for ( int i = 0; i < argc; i++ )
	    {
	      strcat ( string_command, " " );
	      strcat ( string_command, args[i] );
	    }
	  
	  strcat ( string_command, "\n" );

	  if ( write ( log_file_descriptor, string_command, strlen ( string_command ) + 1 ) != strlen ( string_command ) + 1 )
	    {
	      printf ( "Eroare la scrierea in log file\n" );
	      close ( log_file_descriptor );
	      exit ( -1 );
	    }

	  close ( log_file_descriptor );
	}
    }
  else
    {
      if ( operation == REMOVE_HUNT )
	{
	  if ( operation_succesful )
	    printf ( "SUCCESS | Hunt was removed\n" );
	  else
	    printf ( "FAILURE | Hunt was NOT removed\n" );
	}
    }
  
  printf ( "\n" );
  
  return 0;
}
