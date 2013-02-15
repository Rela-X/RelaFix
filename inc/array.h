/*
 * RelaFix       Array
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 \brief A dynamic array that keeps track of its size and reallocates its memory automaticly.

 Declares functions to work on struct RF_ARRAY. The array is an array of pointers to the data.
 That array will be rallocated automaticly if needed. It will grow to the double size every time
 a reallocation will be done.
 */

#ifndef RF_ARRAY_H
#define RF_ARRAY_H

/*!
 \brief An dynamic array that takes pointers to data.

 It will keep track of the arraysize and grow if its full. It will grow to the double of the
 size it had before. The pointers are stored in a void* buffer allocated with malloc().
 */
typedef struct rf_array
{
	unsigned int length;	/*!< \brief The allocated memory */
	unsigned int count;		/*!< \brief The elements in the memory. The size of the array */
	void **array;			/*!< \brief The array that stores the pointers to the data */
} RF_ARRAY;

RF_ARRAY *   rf_array_create();
void         rf_array_destroy(RF_ARRAY *array, void (*free)(void *));
int          rf_array_append(RF_ARRAY *array, void * element);
void *       rf_array_read(RF_ARRAY *array, unsigned int pos);
void *       rf_array_write(RF_ARRAY *array, void * element, unsigned int pos);
void *       rf_array_delete(RF_ARRAY *array, unsigned int pos);
int          rf_array_merge(RF_ARRAY *array_1, RF_ARRAY *array_2);
unsigned int rf_array_size(RF_ARRAY *array);
void			   rf_array_swap_order(RF_ARRAY *array);

#endif
