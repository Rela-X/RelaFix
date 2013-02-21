/*
 * RelaFix       Stack
 *
 * Copyright (C) Peter Berger, 2011
 */

#include <stdlib.h>
#include <stdio.h>

#include "stack.h"
#include "array.h"


RF_STACK * rf_stack_create()
{
	RF_STACK *stack = malloc(sizeof(RF_STACK));
	if(!stack)
	{
		perror("Stack konnte nicht erzeugt werden.");
		return 0;
	}

	stack->array = rf_array_create();
	stack->pos = 0;

	return stack;
}


void rf_stack_destroy(RF_STACK *stack, void (*destroy)(void *))
{
	void *tmp;

	if(!stack)
		return;

	if(destroy)
	{
		while((tmp = rf_stack_pop(stack)))
			destroy(tmp);
	}

	rf_array_destroy(stack->array, 0);
	free(stack);
}


void rf_stack_push(RF_STACK *stack, void *element)
{
	if(!element)
		return;

	/* If the used array is not full we write the element at the next position */
	if((int)rf_array_size(stack->array) > stack->pos)
	{
		rf_array_write(stack->array, element, stack->pos);
	}

	/* if the used array is full, we append the element */
	else
	{
		rf_array_append(stack->array, element);
	}

	/* one more element on the stack */
	stack->pos++;
}


void * rf_stack_pop(RF_STACK *stack)
{
	if(stack->pos == 0) /* stack is empty */
		return 0;

	stack->pos--; /* one element less on the stack */
	return rf_array_delete(stack->array, stack->pos);
}
