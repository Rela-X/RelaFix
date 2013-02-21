/*
 * RelaFix       Array
 *
 * Copyright (C) Peter Berger, 2011
 */

#include <stdlib.h>

#include "array.h"


RF_ARRAY * rf_array_create()
{
	RF_ARRAY *array = malloc(sizeof(RF_ARRAY));

	if(!array)
	{
		return 0;
	}

	else
	{
		array->count = 0;
		array->length = 2;
		array->array = malloc(array->length * sizeof(void *));
		if(!array->array)
		{
			array->length = 0;
		}

		return array;
	}
}

void rf_array_destroy(RF_ARRAY *array, void (*destroy)(void *))
{
	unsigned int i;

	if(!array)
		return;

	if(destroy)
	{
		for(i = 0; i < array->count; i++)
		{
			if(array->array[i])
			{
				destroy(array->array[i]);
			}
		}
	}

	free(array->array);
	free(array);
}

int rf_array_append(RF_ARRAY *array, void * element)
{
	void *memory;

	if(!array)
		return 1;

	/* check if array is full. If so reallocate to the double size */
	if(array->count == array->length)
	{
		memory = realloc(array->array, sizeof(void *) * array->length * 2);
		if(!memory)
		{
			return 1;
		}

		array->array = (void**)memory;
		array->length *= 2;
	}

	/* add element */
	array->array[array->count] = element;
	array->count++;
	return 0;
}

void * rf_array_read(RF_ARRAY *array, unsigned int pos)
{
	if(pos >= array->count)
	{
		return 0;
	}

	return array->array[pos];
}

void * rf_array_write(RF_ARRAY *array, void * element, unsigned int pos)
{
	void *oldElement;

	if(pos >= array->count)
	{
		return 0;
	}

	oldElement = array->array[pos];
	array->array[pos] = element;
	return oldElement;
}

void * rf_array_delete(RF_ARRAY *array, unsigned int pos)
{
	return rf_array_write(array, 0, pos);
}

int rf_array_merge(RF_ARRAY *array_1, RF_ARRAY *array_2)
{
	int i, count;

	if(!array_1 || !array_2)
		return 1;

	/* Append all the items of the second array to the first one */
	for(i = 0, count = rf_array_size(array_2); i < count; i++)
		rf_array_append(array_1, rf_array_delete(array_2, i));

	return 0;
}

unsigned int rf_array_size(RF_ARRAY *array)
{
	return array->count;
}

void rf_array_swap_order(RF_ARRAY *array)
{
	int a, b;
	void *tmp, **data = array->array;

	for(a = 0, b = array->count - 1; a < b; a++, b--)
	{
		tmp = data[a];
		data[a] = data[b];
		data[b] = tmp;
	}
}
