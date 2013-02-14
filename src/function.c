/*
 * RelaFix       Function
 *
 * Copyright (C) Peter Berger, 2011
 */

#include "function.h"
#include <stdlib.h>
#include <string.h>


/*!
 @relates RF_FUNCTION
 @param name Name of the new function. Pointer to heap. The name gets invalid for the user! If 0
 			the new function wont have a name.
 @param fpointer Pointer to a function. Should be the function that represents the equivalent Relafix language function.
 @param argc The number of arguments the function takes in the Relafix language.
 @return New function. Must be destroyed with rf_function_destroy() if not needed anymore.
 @return 0 on error.
 */
RF_FUNCTION * rf_function_create(char *name, void *fpointer, int argc)
{
	RF_FUNCTION *function = malloc(sizeof(RF_FUNCTION));
	if(!function)
		return 0;
	
	function->name = name;
	function->argument_count = argc;
	function->function = fpointer;
	function->description = 0;
	
	return function;
}


/*! \brief Frees all included data except the description!
 
 @relates RF_FUNCTION
 @param function The function to be destroyed.
 */
void rf_function_destroy(RF_FUNCTION *function)
{
	if(!function)
		return;
	
	if(function->name)
		free(function->name);
	free(function);
}


/*! \brief Return the number of arguments the function takes in the Relafix language.
 
 @relates RF_FUNCTION
 @param function The function whoms argument count should be retrieved.
 @return The number of arguments the function takes in the Relafix language.
 @return is < 0 on error.
 */
int rf_function_get_argument_count(RF_FUNCTION *function)
{
	if(!function)
		return -1;
	
	return function->argument_count;
}


/*!
 @relates RF_FUNCTION
 @param function The function whoms description should be retrieved.
 @return The description. The description will also be used by the struct! But the description
 		will not be deleted by rf_function_destroy().
 @return 0 on error or if no description exists.
 */
char * rf_function_get_description(RF_FUNCTION *function)
{
	if(!function)
		return 0;
	
	return function->description;
}


/*!
 @relates RF_FUNCTION
 @param function The function whoms func should be retrieved.
 @return The function that represents the equivalent Relafix language function.
 @return 0 on error
 */
void * rf_function_get_func(RF_FUNCTION *function)
{
	if(!function)
		return 0;
	
	return function->function;
}


/*!
 @relates RF_FUNCTION
 @param function The function whoes name should be retrieved.
 @return The name of the function. Should not be changed or freed by the user.
 @return 0 if no name exists or on error
 */
char * rf_function_get_name(RF_FUNCTION *function)
{
	if(!function)
		return 0;
	
	return function->name;
}


/*!
 @relates RF_FUNCTION
 @param function The function whose name should be compared.
 @param name The name in question.
 @return RF_TRUE if name is the name of the function.
 @return RF_FALSE if not or error.
 */
RF_BOOL rf_function_has_name(RF_FUNCTION *function, char *name)
{
	if(!function || !name)
		return RF_FALSE;
	
	if(strcmp(function->name, name) == 0)
		return RF_TRUE;
	else
		return RF_FALSE;
}


/*!
 @relates RF_FUNCTION
 @param function The function whose description is to be set.
 @param description Will be used by the RF_FUNCTION, but not freed with rf_function_destroy(). If 0 then
 		the function will not have a description.
 */
void rf_function_set_description(RF_FUNCTION *function, char *description)
{
	if(!function)
		return;
	
	function->description = description;
}
