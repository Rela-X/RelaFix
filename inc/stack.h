/*
 * RelaFix       Stack
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 \brief A stack based on RF_ARRAY.

 Declares functions to work on struct RF_STACK. The stack takes pointers to data.
 */

#ifndef RF_STACK_H
#define RF_STACK_H

#include "array.h"

/*!
 \brief A stack based on RF_ARRAY
 */
typedef struct rf_stack
{
    int pos;			/*!< \brief last position in the array (top of stack). */
    RF_ARRAY *array;	/*!< \brief the array that contains the data */
} RF_STACK;


RF_STACK * rf_stack_create();
void       rf_stack_destroy(RF_STACK *stack, void (*free)(void *));
void       rf_stack_push(RF_STACK *stack, void *element);
void *     rf_stack_pop(RF_STACK *stack);

#endif
