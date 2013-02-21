/*
 * RelaFix       Function
 *
 * Copyright (C) Peter Berger, 2011
 */

#include "function.h"
#include <stdlib.h>
#include <string.h>


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

void rf_function_destroy(RF_FUNCTION *function)
{
	if(!function)
		return;

	if(function->name)
		free(function->name);
	free(function);
}

int rf_function_get_argument_count(RF_FUNCTION *function)
{
	if(!function)
		return -1;

	return function->argument_count;
}

char * rf_function_get_description(RF_FUNCTION *function)
{
	if(!function)
		return 0;

	return function->description;
}

void * rf_function_get_func(RF_FUNCTION *function)
{
	if(!function)
		return 0;

	return function->function;
}

char * rf_function_get_name(RF_FUNCTION *function)
{
	if(!function)
		return 0;

	return function->name;
}

bool rf_function_has_name(RF_FUNCTION *function, char *name)
{
	if(!function || !name)
		return false;

	if(strcmp(function->name, name) == 0)
		return true;
	else
		return false;
}

void rf_function_set_description(RF_FUNCTION *function, char *description)
{
	if(!function)
		return;

	function->description = description;
}
