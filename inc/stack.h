/*
 * RelaFix       Stack
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 A stack based on RF_ARRAY.

 Declares functions to work on struct RF_STACK. The stack takes pointers to data.
 */

#ifndef RF_STACK_H
#define RF_STACK_H

#include "array.h"

/*!
 A stack based on RF_ARRAY
 */
typedef struct rf_stack
{
    int pos;			/*!< last position in the array (top of stack). */
    RF_ARRAY *array;	/*!< the array that contains the data */
} RF_STACK;


/*!
 @relates RF_STACK
 @return The new stack
 @return 0 on error.
 */
RF_STACK * rf_stack_create();

/*!
 @relates RF_STACK
 @param[in] stack The stack to be destroyed.
 @param[in] destroy This function will be called with every element in the stack, before the
            stack gets freed. Can be used to fastly free data.
 */
void       rf_stack_destroy(RF_STACK *stack, void (*free)(void *));

/*!
 Put the element on top of the stack.
 @relates RF_STACK
 @param[in] stack The element will be put on this stack.
 @param[in] element The element that will be pushed on the stack.
 */
void       rf_stack_push(RF_STACK *stack, void *element);

/*!
 @relates RF_STACK
 @param[in] stack The stack that contains the data.
 @return The element on top of the stack.
 @return 0 on error or if stack is empty.
 */
void *     rf_stack_pop(RF_STACK *stack);

#endif
