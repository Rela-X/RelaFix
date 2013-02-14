/*
 * RelaFix       Array
 *
 * Copyright (C) Peter Berger, 2011
 */

#include "array.h"
#include <stdlib.h>



/*!
 @relates RF_ARRAY
 @return new array
 @return 0 on error.
 */
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

/*!
 Frees the array. If a function is passed, all pointers stored in the array are passed to that
 function before the array will be freed. On this way the data can be freed fastly.
 @relates RF_ARRAY
 @param[in] array The array that should be destroyed.
 @param[in] destroy A function that takes a void pointer as an argument. Every element in the
 			array will be passed to that function before the array is be freed.
 */
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


/*!
 Appends the element at the end of the array. The array will grow automaticly.
 @relates RF_ARRAY
 @param[in] array The array where the pointer should be added.
 @param[in] element The pointer to be added.
 @return 0 on success.
 @return 1 on error.
 */
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


/*!
 Returns the pointer saved at the given position.
 @relates RF_ARRAY
 @param[in] array The array that stores the data.
 @param pos The position in the array. Array starts at position 0.
 @return The stored pointer at the given position.
 @return 0 on error.
 */
void * rf_array_read(RF_ARRAY *array, unsigned int pos)
{
	if(pos >= array->count)
	{
		return 0;
	}

	return array->array[pos];
}

/*!
 Writes the given pointer at the given position. The position must fit the range!
 @relates RF_ARRAY
 @param[in] array The array that stores the data.
 @param pos The position in the array. Array starts at position 0. The position must fit the range!
 @return The old stored pointer at the given position.
 @return 0 on error.
 */
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


/*!
 Removes that element from the array and returns it.
 @relates RF_ARRAY
 @param[in] array The array containing the data.
 @param pos The position of the data to be deleted. Must be in the range of the array!
 @return The removed data.
 @return 0 on error.
 */
void * rf_array_delete(RF_ARRAY *array, unsigned int pos)
{
	return rf_array_write(array, 0, pos);
}


/*!
 Appends the second array to the first. The second array will be empty after this.
 @relates RF_ARRAY
 @param[in] array_1 The array that should contain all data.
 @param[in] array_2 The array that gets appended to the first. Will be empty after the call.
 @return 0 on success.
 @return 1 on error.
 */
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

/*!
 Returns the size of the used array. Not the size of the pre allocated memory!
 @relates RF_ARRAY
 @param[in] array The array whoes size should be returned.
 @return The element count.
 */
unsigned int rf_array_size(RF_ARRAY *array)
{
	return array->count;
}


/*!
 @relates RF_ARRAY
 @param[in] array The array whoes element order should be swaped.
 */
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
