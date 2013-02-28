/*
 * RelaFixLib    Negation
 *
 * Copyright (C) Peter Berger, 2011
 */

#include <stdlib.h>
#include <string.h>

#include "negation.h"
#include "tools.h"
#include "list.h"
#include "domain.h"


char * rf_negation_calc(RF_NEGATION *negation, char *element)
{
	RF_LIST_ITERATOR *pos;
	char *tmp;

	if(!negation || !element)
	{
//		error->string = strdup("program error - no negation or element in negation_calc");
		return 0;
	}


	/* check if given element is in domain */
	if(!rf_domain_has_element(rf_negation_get_domain(negation), element))
	{
//		error->string = rf_string_combine(5, "'", element, "' is not a element of domain '",
//			rf_domain_get_name(rf_negation_get_domain(negation)), "'");
		return 0;
	}


	/* get iterator over the negationlist */
	pos = rf_list_get_begin(negation->items);
	if(!pos)
	{
//		error->string = strdup("program error");
		return 0;
	}

	/* iterate over list */
	while(rf_list_has_next(pos))
	{
		/* get next element */
		tmp = rf_list_next(pos);
		if(!tmp)
		{
			rf_list_delete_iterator(pos);
//			error->string = strdup("program error");
			return 0;
		}

		/* compare given element with element from list. On match return the negation */
		if(strcmp(tmp, element) == 0)
		{
			/* get the following name from the list. Its the name of the negated element */
			if(rf_list_has_next(pos))
			{
				tmp = rf_list_next(pos);

				rf_list_delete_iterator(pos);
				if(!tmp)
				{
//					error->string = strdup("program error");
					return 0;
				}
				else
					return tmp;
			}
			else
			{
				rf_list_delete_iterator(pos);
//				error->string = strdup("program error - reached end of list in negation_calc");
				return 0;
			}
		}

		/* skip every second element, cause its the negated of the previous. */
		if(rf_list_has_next(pos))
			rf_list_next(pos);
	}

	rf_list_delete_iterator(pos);
//	error->string = strdup("program error - reached end of list in negation_calc");
	return 0;
}

RF_NEGATION * rf_negation_create(char *name, RF_DOMAIN *domain)
{
	RF_NEGATION *negation;

	if(!name)
		return 0;
	if(!domain)
	{
		free(name);
		return 0;
	}

	negation = malloc(sizeof(RF_NEGATION));
	if(!negation)
	{
		free(name);
		return 0;
	}

	negation->name = name;
	negation->domain = domain;
	negation->items = 0;

	return negation;
}

void rf_negation_destroy(RF_NEGATION *negation)
{
	if(!negation)
		return;

	if(negation->items)
		rf_list_destroy(negation->items, free);
	if(negation->name)
		free(negation->name);

	free(negation);
}

RF_DOMAIN * rf_negation_get_domain(RF_NEGATION *negation)
{
	if(!negation)
		return 0;

	return negation->domain;
}

RF_LIST * rf_negation_get_items(RF_NEGATION *negation)
{
	if(!negation)
		return 0;

	return negation->items;
}

char * rf_negation_get_name(RF_NEGATION *negation)
{
	if(!negation)
		return 0;

	return negation->name;
}

bool rf_negation_has_name(RF_NEGATION *negation, char *name)
{
	if(!negation || !name)
		return false;

	if(strcmp(negation->name, name) == 0)
		return true;
	else
		return false;
}


int rf_negation_set_items(RF_NEGATION *negation, RF_LIST *items)
{
	RF_LIST_ITERATOR *element, *rest;
	char *tmp, *tmp_rest;

	if(!negation)
	{
//		error->string = strdup("program error - argument negation is zero in negation_set_items");
		return 1;
	}
	if(!items)
	{
//		error->string = strdup("program error - argument items is zero in negation_set_items");
		return 1;
	}

	if(negation->items)
	{
		rf_list_destroy(negation->items, free);
		negation->items = 0;
	}


	/* form of items-list is 'original, negation, original, negation, ...'. That means,
	 * that after a original always the negation to that original follows. That means, that the
	 * list must have the double count of elements then the domain has.
	 */

	/* Check if id count is double of domain element-count */
	if((rf_domain_get_element_count(negation->domain) * 2) != rf_list_get_count(items))
	{
//		error->string = strdup("wrong count of elements");
		return 1;
	}


	/* compare every second element if it is unique and if every element is from domain*/
	element = rf_list_get_begin(items);
	while(rf_list_has_next(element))
	{
		/* compare every second element if it is unique and element of domain*/
		tmp = rf_list_next(element);
		if(!tmp)
		{
			rf_list_delete_iterator(element);

//			error->string = strdup("program error - found id with null pointer");
			return 1;
		}
		if(!rf_domain_has_element(negation->domain, tmp))
		{
			rf_list_delete_iterator(element);

//			error->string = rf_string_combine(5, "element '", tmp, "' is not from domain '",
//				rf_domain_get_name(negation->domain), "'");
			return 1;
		}
		rest = rf_list_copy_iterator(element);
		while(rf_list_has_next(rest))
		{
			/* skip one */
			tmp_rest = rf_list_next(rest);
			if(!tmp_rest)
			{
				rf_list_delete_iterator(element);
				rf_list_delete_iterator(rest);

//				error->string = strdup("program error - found id with null pointer");
				return 1;
			}
			if(!rf_list_has_next(rest))
				break;

			/* test one */
			tmp_rest = rf_list_next(rest);
			if(!tmp_rest)
			{
				rf_list_delete_iterator(element);
				rf_list_delete_iterator(rest);

//				error->string = strdup("program error - found id with null pointer");
				return 1;
			}

			if(strcmp(tmp, tmp_rest) == 0)
			{
				rf_list_delete_iterator(rest);
				rf_list_delete_iterator(element);

//				error->string = rf_string_combine(3, "element '", tmp_rest, "' is redefined");
				return 1;
			}
		}
		rf_list_delete_iterator(rest);


		/* check if skip element is in domain */
		if(rf_list_has_next(element))
		{
			tmp = rf_list_next(element);
			if(!tmp)
			{
				rf_list_delete_iterator(element);

//				error->string = strdup("program error - found id with null pointer");
				return 1;
			}

			if(!rf_domain_has_element(negation->domain, tmp))
			{
				rf_list_delete_iterator(element);

//				error->string = rf_string_combine(5, "element '", tmp, "' is not from domain '",
//					rf_domain_get_name(negation->domain), "'");
				return 1;
			}
		}
		else
			break;
	}
	rf_list_delete_iterator(element);

	negation->items = items;
	return 0;
}
