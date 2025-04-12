#include "treasure_manager.h" // include toate dependentele

int main ( int argc, char **args )
{
  if ( argc != 3 && argc != 4 ) // check number of arguments
    {
      printf ( "Numar invalid de argumente\n" );
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

  switch ( operation )
    {
    case ADD_TREASURE:
      
      if ( argc != 3 )
	{
	  operation = OTHER;
	  printf ( "Numar invalid de argumente pentru aceasta comanda\n" );
	}

      add_treasure ( args[2] );
      
      break;
      
    case LIST_HUNT:
      
      if ( argc != 3 )
	{
	  operation = OTHER;
	  printf ( "Numar invalid de argumente pentru aceasta comanda\n" );
	}
      
      list_hunt ( args[2] );
      
      break;
      
    case VIEW_TREASURE:
      
      if ( argc != 4 )
	{
	  operation = OTHER;
	  printf ( "Numar invalid de argumente pentru aceasta comanda\n" );
	}

      view_treasure ( args[2], args[3] );
      
      break;
      
    case REMOVE_TREASURE:
      
      if ( argc != 4 )
	{
	  operation = OTHER;
	  printf ( "Numar invalid de argumente pentru aceasta comanda\n" );
	}
      
      break;
      
    case REMOVE_HUNT:
      
      if ( argc != 3 )
	{
	  operation = OTHER;
	  printf ( "Numar invalid de argumente pentru aceasta comanda\n" );
	}
      
      break;
      
    case OTHER:
    default:
      printf ( "Comanda folosita este invalida\n" );
      
      break;
    }
  
  return 0;
}
