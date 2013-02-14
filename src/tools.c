/*
 * RelaFix       Tools
 *
 * Copyright (C) Peter Berger, 2011
 */

#include "tools.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


/*! \brief	Copies a string
 *
 * Creates a copy of the given string on the heap.
 * @param str pointer to source string
 * @return pointer to the new string on the heap. Must be freed by user. If an error occures, 0 will be returned.
 * @see rf_string_combine
 */
char * rf_string_copy(const char *str)
{
	char *new_string;
	
	if(!str)
		return 0;
	
	new_string = malloc((strlen(str) + 1) * sizeof(char));
	if(!new_string)
		return 0;
	
	strcpy(new_string, str);
	return new_string;
}


/*! \brief Concatenates strings
 *
 * Concatenates all strings that are given as an argument. This function can take
 * a variable list of string pointers. Every source string will be copied into the final new string.
 * @param argc count of following string pointers
 * @param ... variable list of string pointers (char *)
 * @return pointer to the new string on the heap. Must be freed by user. If an error occures, 0 will be returned.
 * @see rf_string_copy
 */
char * rf_string_combine(int argc, ...)
{
	char *tmp, *result = 0;
	int i, part_1 = 0, part_2;

	/* prepare for reading variable list */
	va_list list;
	va_start(list, argc);
		
		
	/* read the variable list and create new string*/
	for(i = 0; i < argc; i++)
	{
		tmp = va_arg(list, char *);
		if(tmp && result)
		{
			part_2 = strlen(tmp);
			result = realloc(result, part_1 + part_2 + 1);
			strcpy(result + part_1, tmp);
			result[part_1 + part_2] = 0;
			part_1 += part_2;
		}
		else if(tmp)
		{
			part_1 = strlen(tmp);
			result = realloc(result, part_1 + 1);
			strcpy(result, tmp);
			result[part_1] = 0;
		}
	}
	
	return result;
}



