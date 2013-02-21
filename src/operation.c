/*
 * RelaFixLib    Operator
 *
 * Copyright (C) Peter Berger, 2011
 */

#include "operation.h"
#include "domain.h"
#include "relation.h"
#include "formula.h"
#include "table.h"
#include "stack.h"
#include "tools.h"
#include "list.h"
#include "parse.h"
#include <stdlib.h>
#include <string.h>


/*!
 Calculates the the solution for to elements based on the given operation.
 @relates RF_OPERATION
 @param[in] operation The operation used for the calculation.
 @param[in] element_1 The left argument.
 @param[in] element_2 The right argument.
 @param[out] element_out If the function succeeds, it contains the resulting element.
             Then the string must not be changed or freed by the user!
 @result 0 on success. The result of the operation is stored in element_out.
 @result 1 on error. An description is written to element_out. This description must be freed by the user!
 */
int rf_operation_calc(RF_OPERATION *operation, char *element_1, char *element_2, char **element_out)
{
	int x, y;

	if(!element_out)
		return 1;

	if(!operation || !element_1 || !element_2)
	{
		*element_out = rf_string_copy("program error - some input is zero");
		return 1;
	}

	/*
	 get the positions of the given elements in the domain. On fail the given element does
	 does not belong to the domain. An error message is returned then.
	 */
	x = rf_domain_get_element_position(operation->domain_2, element_2);
	y = rf_domain_get_element_position(operation->domain_1, element_1);
	if(x < 0)
	{
		*element_out = rf_string_combine(5, "'", element_2, "' is not a element of domain '",
			rf_domain_get_name(operation->domain_2), "'");
		return 1;
	}
	if(y < 0)
	{
		*element_out = rf_string_combine(5, "'", element_1, "' is not a element of domain '",
			rf_domain_get_name(operation->domain_1), "'");
		return 1;
	}

	/* read the solution from the table. On error return an errordescription */
	if(rf_table_get_string(operation->table, x, y, element_out))
	{
		*element_out = rf_string_copy("program error - while reading string from table");
		return 1;
	}

	return 0;
}


/*!
 @relates RF_OPERATION
 @param[in] operation The operation that should be copied.
 @return The copy of the given operation.
 @return 0 on error.
 */
RF_OPERATION * rf_operation_copy(RF_OPERATION *operation)
{
	RF_OPERATION *copy = 0;

	if(!operation)
		return 0;

	/* get memory */
	copy = calloc(1, sizeof(RF_OPERATION));
	if(!copy)
		return 0;

	/* copy properties */
	copy->domain_1 = operation->domain_1;
	copy->domain_2 = operation->domain_2;
	copy->domain_3 = operation->domain_3;
	copy->name = 0;
	copy->table = rf_table_copy(operation->table);

	return copy;
}


/*!
 @relates RF_OPERATION
 @param[in] name The name of the new operation. 0 is allowed. Gets invalid for the caller.
 @param[in] domain_1 The domain of the left argument.
 @param[in] domain_2 The domain of the right argument.
 @param[in] domain_3 The domain containing the elements of the solution.
 @return The new operation
 @return 0 on error
 */
RF_OPERATION * rf_operation_create(char *name, RF_DOMAIN *domain_1, RF_DOMAIN *domain_2, RF_DOMAIN *domain_3)
{
	RF_OPERATION *operation;
	RF_LIST *list;
	int width, height;

	if(!domain_1 || !domain_2 || !domain_3)
		return 0;

	/* get element counts of the domains. Will be used to create the table */
	list = rf_domain_get_list(domain_1);
	height = rf_list_get_count(list);
	list = rf_domain_get_list(domain_2);
	width = rf_list_get_count(list);
	if(width <= 0 || height <= 0)
		return 0;

	/* allocate memeory on the heap */
	operation = malloc(sizeof(RF_OPERATION));
	if(!operation)
		return 0;

	/* initialization */
	operation->name = name;
	operation->domain_1 = domain_1;
	operation->domain_2 = domain_2;
	operation->domain_3 = domain_3;
	operation->table = rf_table_create(width, height, RF_TT_STRING);

	return operation;
}


/*!
 @relates RF_OPERATION
 @param[in] relation The less equal relation the meet is based on.
 @param[out] If the function fails an error is written here. The string must be deleted by the caller!
 @return An new meet operation.
 @return 0 on error. An description is written to error.
 */
RF_OPERATION * rf_operation_create_meet(RF_RELATION *relation, RF_ERROR *error)
{
	RF_OPERATION *operation;

	if(!relation)
	{
		error->string = rf_string_copy("program error - relation is zero");
		return 0;
	}

	if(rf_relation_get_domain_1(relation) != rf_relation_get_domain_2(relation))
	{
		error->string = rf_string_combine(3, "'", rf_relation_get_name(relation), "' is not homogeneous.");
		return 0;
	}

	if(!rf_relation_is_poset(relation))
	{
		error->string = rf_string_copy("Given relation is not a poset");
		return 0;
	}

	if(!(operation = rf_operation_generate_meet(relation)))
	{
		error->string = rf_string_copy("program error - while generating meet");
		return 0;
	}

	rf_operation_set_name(operation, rf_string_copy("no name"));
	return operation;
}


/*!
 @relates RF_OPERATION
 @param[in] relation The less equal relation the join is based on.
 @param[out] If the function fails an error is written here. The string must be deleted by the caller!
 @return An new join operation.
 @return 0 on error. An description is written to error.
 */
RF_OPERATION * rf_operation_create_join(RF_RELATION *relation, RF_ERROR *error)
{
	RF_OPERATION *operation;

	if(!relation)
	{
		error->string = rf_string_copy("program error - relation is zero");
		return 0;
	}

	if(rf_relation_get_domain_1(relation) != rf_relation_get_domain_2(relation))
	{
		error->string = rf_string_combine(3, "'", rf_relation_get_name(relation), "' is not homogeneous.");
		return 0;
	}

	if(!rf_relation_is_poset(relation))
	{
		error->string = rf_string_copy("Given relation is not a poset");
		return 0;
	}

	if(!(operation = rf_operation_generate_join(relation)))
	{
		error->string = rf_string_copy("program error - while generating join");
		return 0;
	}

	rf_operation_set_name(operation, rf_string_copy("no name"));
	return operation;
}


/*!
 @relates RF_OPERATION
 @param[in] operation The operation that should be freed. Gets invalid for the caller.
 */
void rf_operation_destroy(RF_OPERATION *operation)
{
	if(!operation)
		return;

	if(operation->name)
		free(operation->name);

	if(operation->table)
		rf_table_destroy(operation->table);

	free(operation);
}


/*!
 This function is is used by the function rf_operation_generate_meet() to find the meet.

 Checks the relation xRy and yRx. If only one of them exist, the position of the first element
 is returned. If none or both exist, a special algorithm searches the element which is the meet
 to the given ones. The retrieved element's position will be returned then.
 @relates RF_OPERATION
 @param[in] table Represents the less equal relation. (Homogenous).
 @param x Domainposition of first element. x in xRy.
 @param y Domainposition of second element. y in xRy.
 @return The position in the domain of the element that is the meet to the given ones.
 @return 0 on error.
 */
static int rf_operation_generate_meet_helper(RF_TABLE *table, int x, int y)
{
	RF_STACK *stack;
	int result_1, result_2;
	int dy, width, last = -1;
	int *tmp;

	if(!table || x < 0 || y < 0)
		return 0;

	/* if both elements are the same, the meet is the element itself */
	if(x == y)
		return x;


	/* read xRy and yRx */
	result_1 = rf_table_get_bit(table, y, x);
	result_2 = rf_table_get_bit(table, x, y);
	if(result_1 < 0 || result_2 < 0)
		return 0;

	/* If only xRy exists return position of y in the domain */
	if(result_1 == 1 && result_2 == 0)
	{
		return x;
	}

	/* If only yRx exists return position of x in the domain */
	else if(result_1 == 0 && result_2 == 1)
	{
		return y;
	}

	/* If both exist or both do not exist, search and return the meet of them */
	else
	{
		/* Stack needed to remember possible meets */
		stack = rf_stack_create();
		if(!stack)
			return 0;

		/*
		 x and y describe the column position! dy the line.
		 We compare for every line if x and y are 1. If so, the element
		 described by dy is a possible meet and is pushed on the stack if
		 it is smaller to the last found possible meet.
		 */
		for(dy = 0, width = rf_table_get_width(table); dy < width; dy++)
		{
			result_1 = rf_table_get_bit(table, x, dy);
			result_2 = rf_table_get_bit(table, y, dy);
			if(result_1 < 0 || result_2 < 0)
			{
				rf_stack_destroy(stack, free);
				return 0;
			}

			/* If dyRx and dyRy exist then dy is a possible meet */
			if(result_1 == 1 && result_2 == 1)
			{
				/* If there exist a possible meet already, we do more checks */
				if(last >= 0)
				{
					/* check if relation dyRlast and lastRdy exist */
					result_1 = rf_table_get_bit(table, last, dy);
					result_2 = rf_table_get_bit(table, dy, last);
					if(result_1 < 0 || result_2 < 0)
					{
						rf_stack_destroy(stack, free);
						return 0;
					}

					/* if only dyRlast exists push dy on the stack and remain last as last. */
					else if(result_1 == 1 && result_2 == 0)
					{
						tmp = malloc(sizeof(int));
						if(!tmp)
						{
							rf_stack_destroy(stack, free);
							return 0;
						}

						*tmp = dy;
						rf_stack_push(stack, tmp);
						tmp = 0;
					}

					/* if only lastRdy exists push last on the stack and remember dy as last. */
					else if(result_1 == 0 && result_2 == 1)
					{
						tmp = malloc(sizeof(int));
						if(!tmp)
						{
							rf_stack_destroy(stack, free);
							return 0;
						}

						*tmp = last;
						rf_stack_push(stack, tmp);
						tmp = 0;
						last = dy;
					}

					/* if both dont exist, forget them and take the element from the stack as last. */
					else
					{
						tmp = rf_stack_pop(stack);
						if(!tmp)
						{
							last = -1;	/* -1 means no last possible meet exist */
						}
						else
						{
							last = *tmp;
							free(tmp);
							tmp = 0;
						}
					}
				}

				/* dy becomes the last possible meet. */
				else
				{
					last = dy;
				}
			}
		}

		rf_stack_destroy(stack, free);
		return last;
	}
}


/*!
 Generates the meet operation for a given less equal relation.
 @relates RF_OPERATION
 @param[in] leq The less equal relation the meet operation will be based on.
 @return The new meet operation.
 @return 0 on error.
 */
RF_OPERATION * rf_operation_generate_meet(RF_RELATION *leq)
{
	RF_OPERATION *operation = 0;
	RF_DOMAIN *domain_1 = 0, *domain_2 = 0;
	RF_TABLE *table = 0, *op_table = 0;
	RF_LIST *names = 0;
	int pos_x, pos_y, count, result;


	/* Get the domains of the realtion */
	domain_1 = rf_relation_get_domain_1(leq);
	domain_2 = rf_relation_get_domain_2(leq);
	/* if domains are not equal the relation is not homogeneous and we quit */
	if(domain_1 != domain_2 || domain_1 == 0)
		return 0;

	/* create new operation */
	operation = rf_operation_create(0, domain_1, domain_1, domain_1);
	if(!operation)
		return 0;

	/* count the elements in the domain */
	count = rf_domain_get_element_count(domain_1);
	if(count <= 0)
	{
		rf_operation_destroy(operation);
		return 0;
	}


	/* get tables and elements to work on */
	table = rf_relation_get_table(leq);
	op_table = rf_operation_get_table(operation);
	names = rf_domain_get_element_names(domain_1);
	if(!table || !op_table || !names)
	{
		rf_operation_destroy(operation);
		rf_list_destroy(names, 0);
		return 0;
	}


	/* calculate the meet for every xRy and save it in the table*/
	for(pos_y = 0; pos_y < count; pos_y++)
	{
		for(pos_x = 0; pos_x < count; pos_x++)
		{
			result = rf_operation_generate_meet_helper(table, pos_x, pos_y);
			if(result < 0)
			{
				rf_operation_destroy(operation);
				rf_list_destroy(names, 0);
				return 0;
			}

			rf_table_set_string(op_table, pos_x, pos_y, rf_list_get_by_position(names, result));
		}
	}

	rf_list_destroy(names, 0);
	return operation;
}


/*!
 This function is is used by the function rf_operation_generate_join() to find the join.

 Checks the relation xRy and yRx. If only one of them exist, the position of the second element
 is returned. If none or both exist, a special algorithm searches the element which is the join
 to the given ones. The retrieved element's position will be returned then.
 @relates RF_OPERATION
 @param[in] table Represents the less equal relation. (Homogenous).
 @param x Domainposition of first element. x in xRy.
 @param y Domainposition of second element. y in xRy.
 @return The position in the domain of the element that is the join to the given ones.
 @return 0 on error.
 */
static int rf_operation_generate_join_helper(RF_TABLE *table, int x, int y)
{
	RF_STACK *stack;
	int result_1, result_2;
	int dy, width, last = -1;
	int *tmp;

	if(!table || x < 0 || y < 0)
		return 0;

	/* if both elements are the same, the join is the element itself */
	if(x == y)
		return x;

	/* read xRy and yRx */
	result_1 = rf_table_get_bit(table, y, x);
	result_2 = rf_table_get_bit(table, x, y);
	if(result_1 < 0 || result_2 < 0)
		return 0;

	/* If only xRy exists return position of y in the domain */
	if(result_1 == 1 && result_2 == 0)
	{
		return y;
	}

	/* If only yRx exists return position of x in the domain */
	else if(result_1 == 0 && result_2 == 1)
	{
		return x;
	}

	/* If both exist or both do not exist, search and return the join of them */
	else
	{
		/* Stack needed to remember possible joints */
		stack = rf_stack_create();
		if(!stack)
			return 0;

		/*
		 x and y describe the column position! dy the line.
		 We compare for every line if x and y are 0. If so, the element
		 described by dy is a possible join and is pushed on the stack if
		 it is bigger to the last found possible join.
		 */
		for(dy = 0, width = rf_table_get_width(table); dy < width; dy++)
		{
			result_1 = rf_table_get_bit(table, x, dy);
			result_2 = rf_table_get_bit(table, y, dy);
			if(result_1 < 0 || result_2 < 0)
			{
				rf_stack_destroy(stack, free);
				return 0;
			}

			/* If dyRx and dyRy does not exist then dy is a possible join */
			if(result_1 == 0 && result_2 == 0)
			{
				/* If there exist a possible join already, we do more checks */
				if(last >= 0)
				{
					/* check if relation dyRlast and lastRdy exist */
					result_1 = rf_table_get_bit(table, last, dy);
					result_2 = rf_table_get_bit(table, dy, last);
					if(result_1 < 0 || result_2 < 0)
					{
						rf_stack_destroy(stack, free);
						return 0;
					}

					/* if only lastRdy exists push dy on the stack and remain last as last. */
					else if(result_1 == 0 && result_2 == 1)
					{
						tmp = malloc(sizeof(int));
						if(!tmp)
						{
							rf_stack_destroy(stack, free);
							return 0;
						}

						*tmp = dy;
						rf_stack_push(stack, tmp);
						tmp = 0;
					}

					/* if only dyRlast exists push last on the stack and remember dy as last. */
					else if(result_1 == 1 && result_2 == 0)
					{
						tmp = malloc(sizeof(int));
						if(!tmp)
						{
							rf_stack_destroy(stack, free);
							return 0;
						}

						*tmp = last;
						rf_stack_push(stack, tmp);
						tmp = 0;
						last = dy;
					}

					/* if both dont exist, forget them and take the element from the stack as last. */
					else
					{
						tmp = rf_stack_pop(stack);
						if(!tmp)
						{
							last = -1;	/* -1 means no last possible meet exist */
						}
						else
						{
							last = *tmp;
							free(tmp);
							tmp = 0;
						}
					}
				}

				/* dy becomes the last possible join. */
				else
				{
					last = dy;
				}
			}
		}

		rf_stack_destroy(stack, free);
		return last;
	}
}


/*!
 Generates the join operation for a given less equal relation.
 @relates RF_OPERATION
 @param[in] leq The less equal relation the join operation will be based on.
 @return The new join operation.
 @return 0 on error.
 */
RF_OPERATION * rf_operation_generate_join(RF_RELATION *leq)
{
	RF_OPERATION *operation = 0;
	RF_DOMAIN *domain_1 = 0, *domain_2 = 0;
	RF_TABLE *table = 0, *op_table = 0;
	RF_LIST *names = 0;
	int pos_x, pos_y, count, result;


	/* Get the domains of the realtion */
	domain_1 = rf_relation_get_domain_1(leq);
	domain_2 = rf_relation_get_domain_2(leq);
	/* if domains are not equal the relation is not homogeneous and we quit */
	if(domain_1 != domain_2 || domain_1 == 0)
		return 0;

	/* create new operation */
	operation = rf_operation_create(0, domain_1, domain_1, domain_1);
	if(!operation)
		return 0;

	/* count the elements in the domain */
	count = rf_domain_get_element_count(domain_1);
	if(count <= 0)
	{
		rf_operation_destroy(operation);
		return 0;
	}


	/* get tables and elements to work on */
	table = rf_relation_get_table(leq);
	op_table = rf_operation_get_table(operation);
	names = rf_domain_get_element_names(domain_1);
	if(!table || !op_table || !names)
	{
		rf_operation_destroy(operation);
		rf_list_destroy(names, 0);
		return 0;
	}


	/* calculate the join for every xRy and save it in the table*/
	for(pos_y = 0; pos_y < count; pos_y++)
	{
		for(pos_x = 0; pos_x < count; pos_x++)
		{
			result = rf_operation_generate_join_helper(table, pos_x, pos_y);
			if(result < 0)
			{
				rf_operation_destroy(operation);
				rf_list_destroy(names, 0);
				return 0;
			}

			rf_table_set_string(op_table, pos_x, pos_y, rf_list_get_by_position(names, result));
		}
	}

	rf_list_destroy(names, 0);
	return operation;
}

/*!
 Returns the domain the first element in an operation must be from.
 @relates RF_OPERATION
 @param[in] operation The operation whoes domain should be returned.
 @return The domain. Should not be changed by the caller!
 @return 0 on error.
 */
RF_DOMAIN * rf_operation_get_domain_1(RF_OPERATION *operation)
{
	if(!operation)
		return 0;

	return operation->domain_1;
}

/*!
 Returns the domain the second element in an operation must be from.
 @relates RF_OPERATION
 @param[in] operation The operation whoes domain should be returned.
 @return The domain. Should not be changed by the caller!
 @return 0 on error.
 */
RF_DOMAIN * rf_operation_get_domain_2(RF_OPERATION *operation)
{
	if(!operation)
		return 0;

	return operation->domain_2;
}

/*!
 Returns the domain the resulting element of an operation must be from.
 @relates RF_OPERATION
 @param[in] operation The operation whoes domain should be returned.
 @return The domain. Should not be changed by the caller!
 @return 0 on error.
 */
RF_DOMAIN * rf_operation_get_domain_3(RF_OPERATION *operation)
{
	if(!operation)
		return 0;

	return operation->domain_3;
}

/*!
 @relates RF_OPERATION
 @param[in] operation The operation whoes name should be returned.
 @return The name. Should not be changed by the caller!
 @return 0 on error.
 */
char * rf_operation_get_name(RF_OPERATION *operation)
{
	if(!operation)
		return 0;

	return operation->name;
}

/*!
 Returns the table storing the solutions for all possible operations.
 @relates RF_OPERATION
 @param[in] operation The operation whoes table should be returned.
 @return Should not be changed by the caller!
 @return 0 on error.
 */
RF_TABLE * rf_operation_get_table(RF_OPERATION *operation)
{
	if(!operation)
		return 0;

	return operation->table;
}

/*!
 Checks if a given name matches the name of the given operation.
 @relates RF_OPERATION
 @param[in] operation The operation whoes name is in question.
 @param[in] The name that should be compared.
 @return true if the name matches the name of the operation.
 @return false if the name does not match and on error.
 */
bool rf_operation_has_name(RF_OPERATION *operation, char *name)
{
	if(!operation || !name)
		return false;
	if(!operation->name)
		return false;

	if(strcmp(name, operation->name))
		return false;
	else
		return true;
}

/*!
 The function will free the old name if exists.
 @relates RF_OPERATION
 @param[in] operation The operation whoes name should be set.
 @param[in] name The name to set. Gets invalid for the caller!
 */
void rf_operation_set_name(RF_OPERATION *operation, char *name)
{
	if(!operation)
	{
		if(name)
			free(name);
		return;
	}

	if(operation->name)
		free(operation->name);

	operation->name = name;
}

/*!
 The function will destroy the old table if exists.
 @relates RF_OPERATION
 @param[in] operation The operation whoes table should be set.
 @param[in] table The table to set. Gets invalid for caller!
 */
void rf_operation_set_table(RF_OPERATION *operation, RF_TABLE *table)
{
	if(!operation)
		return;

	if(operation->table)
		rf_table_destroy(operation->table);

	operation->table = table;
}
