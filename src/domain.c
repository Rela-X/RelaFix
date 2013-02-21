/*
 * RelaFixLib    Domain
 *
 * Copyright (C) Peter Berger, 2011
 */

#include <stdlib.h>
#include <string.h>

#include "domain.h"
#include "list.h"
#include "tools.h"


int rf_domain_check_elements(RF_LIST *elements, RF_ERROR *error)
{
	RF_LIST_ITERATOR *main, *sub;
	RF_ELEMENT *main_element, *sub_element;
	char *main_name, *sub_name;

	if(!elements)
	{
		error->string = strdup("program error - list is zero");
		return 1;
	}

	/* get iterator over elements */
	main = rf_list_get_begin(elements);
	if(!main)
	{
		error->string = strdup("program error - last call: rf_list_get_begin()");
		return 1;
	}

	/* check every element with the others if it is double */
	while(rf_list_has_next(main))
	{
		/* get element */
		main_element = rf_list_next(main);
		if(!main_element)
		{
			rf_list_delete_iterator(main);

			error->string = strdup("Found an zero element in elementlist");
			return 1;
		}

		/* get the name of the element */
		main_name = rf_element_get_name(main_element);
		if(!main_name)
		{
			rf_list_delete_iterator(main);

			error->string = strdup("Found an element that has no name in the elementlist");
			return 1;
		}

		/* copy of the main iterator. We only need to check against the rest in the list */
		sub = rf_list_copy_iterator(main);

		while(rf_list_has_next(sub))
		{
			/* get element */
			sub_element = rf_list_next(sub);
			if(!sub_element)
			{
				rf_list_delete_iterator(main);
				rf_list_delete_iterator(sub);

				error->string = strdup("Found an zero element in elementlist");
				return 1;
			}

			/* get the name of the element */
			sub_name = rf_element_get_name(sub_element);
			if(!sub_name)
			{
				rf_list_delete_iterator(main);
				rf_list_delete_iterator(sub);

				error->string = strdup("Found an element that has no name in the elementlist");
				return 1;
			}

			/* check if both names match. If so return error */
			if(strcmp(main_name, sub_name) == 0)
			{
				rf_list_delete_iterator(main);
				rf_list_delete_iterator(sub);

				error->string = rf_string_combine(3, "Found '", main_name, "' more then one time in the element list");
				return 1;
			}
		}
		rf_list_delete_iterator(sub);
	}
	rf_list_delete_iterator(main);

	return 0;
}

RF_DOMAIN * rf_domain_copy(RF_DOMAIN *domain)
{
	RF_DOMAIN *new_domain = 0;
	RF_LIST_ITERATOR *elements;
	RF_ELEMENT *element;

	if(!domain)
		return 0;

	new_domain = rf_domain_create(0);
	if(!new_domain)
		return 0;

	/*copy properties */
	new_domain->name = strdup(domain->name);

	if(domain->elements)
	{
		new_domain->elements = rf_list_create();
		if(!new_domain->elements)
		{
			rf_domain_destroy(new_domain);
			return 0;
		}

		/* copy every element */
		elements = rf_list_get_begin(domain->elements);
		while(rf_list_has_next(elements))
		{
			element = rf_list_next(elements);
			if(!element)
				continue;

			element = rf_element_copy(element);
			if(!element)
			{
				rf_list_delete_iterator(elements);
				rf_domain_destroy(new_domain);
				return 0;
			}

			rf_list_append(new_domain->elements, element);
		}
		rf_list_delete_iterator(elements);
	}
	else
		domain->elements = 0;

	return new_domain;
}

RF_DOMAIN * rf_domain_create(char *name)
{
	RF_DOMAIN *domain = malloc(sizeof(RF_DOMAIN));
	if(!domain)
		return 0;

	domain->name = name;
	domain->elements = 0;

	return domain;
}

RF_DOMAIN * rf_domain_create_powerset(RF_DOMAIN *domain, RF_ERROR *error)
{
	RF_DOMAIN *new_domain = 0, *tmp_domain = 0;
	RF_ELEMENT *element = 0, *sub_element = 0, *new_element = 0;
	RF_LIST *elements = 0;
	RF_LIST *tmp_elements = 0;
	int scount, element_count, run;
	char *subset, *name, *tmp_name;

	if(!domain)
	{
		error->string = strdup("program error - argument domain is zero");
		return 0;
	}


	/* put every element from domain as subdomain into the new domain */
	elements = rf_list_create();
	element_count = rf_domain_get_element_count(domain);
	subset = calloc(element_count, sizeof(char));
	for(run = 1; run;)
	{
		/* build element */
		tmp_elements = rf_list_create();
		name = 0;
		for(scount = 0, name = 0; scount < element_count; scount++)
		{
			if(subset[scount] == 1)
			{
				element = rf_domain_get_element_by_position(domain, scount);
				tmp_name = rf_element_get_name(element);
				if(name)
				{
					tmp_name = rf_string_combine(3, name, "_", tmp_name);
					free(name);
					name = tmp_name;
				}
				else
					name = strdup(tmp_name);
				sub_element = rf_element_copy(element);
				rf_list_append(tmp_elements, sub_element);
				sub_element = 0;
			}
		}

		/* add new element to new domain */
		if(!name)
			name = strdup("_");
		tmp_domain = rf_domain_create(name);
		rf_domain_set_list(tmp_domain, tmp_elements);
		tmp_elements = 0;
		new_element = rf_element_create(RF_ET_DOMAIN, tmp_domain);
		rf_list_append(elements, new_element);
		new_element = 0;

		/* set subset to next subset (bit addierer)*/
		for(scount = 0; scount < element_count; scount++)
		{
			if(subset[scount] == 1)
			{
				subset[scount] = 0;

				if(scount == element_count - 1)
					run = 0;
			}
			else
			{
				subset[scount] = 1;
				break;
			}
		}
	}

	/* Create the new domain */
	new_domain = rf_domain_create(0);
	if(!new_domain)
	{
		rf_list_destroy(elements, (void (*)(void *))rf_element_destroy);

		error->string = strdup("program error - no memory");
		return 0;
	}
	rf_domain_set_list(new_domain, elements);
	elements = 0;

	return new_domain;
}

void rf_domain_destroy(RF_DOMAIN *domain)
{
	if(!domain)
		return;

	if(domain->name)
		free(domain->name);

	if(domain->elements)
		rf_list_destroy(domain->elements, (void (*)(void *))rf_element_destroy);

	free(domain);
}

RF_ELEMENT * rf_domain_get_element(RF_DOMAIN *domain, char *name)
{
	RF_LIST_ITERATOR *pos;
	RF_ELEMENT *element;
	RF_DOMAIN *tmp_domain;
	char *tmp;

	if(!domain || !name)
		return 0;

	/* get iterator */
	pos = rf_list_get_begin(domain->elements);
	if(!pos)
		return 0;

	/* move through list */
	while(rf_list_has_next(pos))
	{

		/* get next element */
		element = rf_list_next(pos);

		/* check type of element and name. if name matches, return the element */
		if(element)
		{
			if(element->type == RF_ET_NAME)
			{
				if(strcmp(name, element->data) == 0)
				{
					rf_list_delete_iterator(pos);
					return element;
				}
			}
			else if(element->type == RF_ET_DOMAIN)
			{
				tmp_domain = rf_element_get_data(element);
				tmp = rf_domain_get_name(tmp_domain);
				if(strcmp(name, tmp) == 0)
				{
					rf_list_delete_iterator(pos);
					return element;
				}
			}
		}
	}

	rf_list_delete_iterator(pos);
	return 0;
}

RF_ELEMENT * rf_domain_get_element_by_position(RF_DOMAIN *domain, int pos)
{
	if(!domain || pos < 0)
		return 0;

	/* check if outofbounds */
	if(pos >= rf_list_get_count(domain->elements))
		return 0;

	/* find the element */
	return rf_list_get_by_position(domain->elements, pos);
}

int rf_domain_get_element_count(RF_DOMAIN *domain)
{
	if(!domain)
		return -1;

	if(!domain->elements)
		return -1;

	return rf_list_get_count(domain->elements);
}

RF_LIST * rf_domain_get_element_names(RF_DOMAIN *domain)
{
	RF_LIST *names;
	RF_LIST_ITERATOR *pos;
	RF_ELEMENT *element;
	RF_DOMAIN *tmp_domain;

	if(!domain)
		return 0;

	/* create new list */
	names = rf_list_create();
	if(!names)
		return 0;

	/* get iterator */
	pos = rf_list_get_begin(domain->elements);
	if(!pos)
		return 0;

	/* fill list with the names of the elements */
	while(rf_list_has_next(pos))
	{
		element = rf_list_next(pos);
		if(element)
		{
			if(element->type == RF_ET_NAME)
				rf_list_append(names, element->data);
			else if(element->type == RF_ET_DOMAIN)
			{
				tmp_domain = rf_element_get_data(element);
				rf_list_append(names, rf_domain_get_name(tmp_domain));
			}
		}
	}

	rf_list_delete_iterator(pos);
	return names;
}

int rf_domain_get_element_position(RF_DOMAIN *domain, char *name)
{
	RF_LIST_ITERATOR *iterator = 0;
	RF_ELEMENT *element = 0;
	int i;

	if(!domain || !name)
		return -1;

	iterator = rf_list_get_begin(domain->elements);
	for(i = 0; rf_list_has_next(iterator); i++)
	{
		element = rf_list_next(iterator);
		if(!element)
			continue;
		else if(element->type == RF_ET_NAME)
		{
			if(strcmp(element->data, name) == 0)
			{
				rf_list_delete_iterator(iterator);
				return i;
			}
		}
		else if(element->type == RF_ET_DOMAIN)
		{
			if(strcmp(rf_domain_get_name(element->data), name) == 0)
			{
				rf_list_delete_iterator(iterator);
				return i;
			}
		}
	}

	rf_list_delete_iterator(iterator);
	return -2;
}

RF_LIST * rf_domain_get_list(RF_DOMAIN *domain)
{
	if(!domain)
		return 0;

	if(!domain->elements)
		domain->elements = rf_list_create();

	return domain->elements;
}

char * rf_domain_get_name(RF_DOMAIN *domain)
{
	if(!domain)
		return 0;

	return domain->name;
}

bool rf_domain_has_element(RF_DOMAIN *domain, char *name)
{
	RF_LIST *names;
	RF_LIST_ITERATOR *element;

	if(!domain || !name)
		return false;

	/* get list with names from domain */
	names = rf_domain_get_element_names(domain);
	if(!names)
		return false;

	/* get iterator over names */
	element = rf_list_get_begin(names);
	if(!element)
	{
		rf_list_destroy(names, 0);
		return false;
	}

	/* check if given name is in list */
	while(rf_list_has_next(element))
		if(strcmp(name, rf_list_next(element)) == 0)
		{
			rf_list_delete_iterator(element);
			rf_list_destroy(names, 0);
			return true;
		}

	rf_list_delete_iterator(element);
	rf_list_destroy(names, 0);
	return false;
}

bool rf_domain_has_name(RF_DOMAIN *domain, char *name)
{
	if(!domain || !name)
		return false;
	if(!domain->name)
		return false;

	if(strcmp(domain->name, name))
		return false;
	else
		return true;
}

bool rf_domain_is_partof(RF_DOMAIN *domain1, RF_DOMAIN *domain2)
{
	RF_LIST *names;
	RF_LIST_ITERATOR *element;
	char *name;

	if(!domain1 || !domain2)
	{
		return false;
	}

	/* get names in question */
	names = rf_domain_get_element_names(domain1);
	element = rf_list_get_begin(names);
	while(rf_list_has_next(element))
	{
		name = rf_list_next(element);
		if(rf_domain_has_element(domain2, name) == false)
		{
			rf_list_delete_iterator(element);
			rf_list_destroy(names, 0);

			return false;
		}
	}

	/* if we arrive here, domain1 is in domain2 */
	rf_list_delete_iterator(element);
	rf_list_destroy(names, 0);
	return true;
}

void rf_domain_set_list(RF_DOMAIN *domain, RF_LIST *list)
{
	if(!domain || !list)
		return;

	if(domain->elements)
		rf_list_destroy(domain->elements, (void (*)(void *))rf_element_destroy);

	domain->elements = list;
}

void rf_domain_set_name(RF_DOMAIN *domain, char *name)
{
	if(!domain)
		return;

	if(domain->name)
		free(domain->name);

	domain->name = name;
}



RF_ELEMENT * rf_element_copy(RF_ELEMENT *element)
{
	RF_ELEMENT *new_element;

	if(!element)
		return 0;

	new_element = calloc(1, sizeof(RF_ELEMENT));
	if(!new_element)
		return 0;

	/* copy properties */
	new_element->type = element->type;

	if(element->type == RF_ET_DOMAIN)
	{
		new_element->data = rf_domain_copy(element->data);
		if(!new_element->data)
		{
			free(new_element);
			return 0;
		}
	}
	else if(element->type == RF_ET_GLOBAL_DOMAIN)
	{
		new_element->data = element->data;  /* element is not the owner */
	}
	else if(element->type == RF_ET_NAME)
	{
		new_element->data = strdup(element->data);
	}
	else
	{
		free(new_element);
		return 0;
	}

	return new_element;
}

RF_ELEMENT * rf_element_create(int type, void *data)
{
	RF_ELEMENT *element;

	if(!data || !(type == RF_ET_DOMAIN || type == RF_ET_NAME || type == RF_ET_GLOBAL_DOMAIN))
		return 0;

	element = malloc(sizeof(RF_ELEMENT));
	if(!element)
		return 0;

	element->type = type;
	element->data = data;

	return element;
}

void rf_element_destroy(RF_ELEMENT *element)
{
	if(!element)
		return;

	if(element->type == RF_ET_NAME)
		free(element->data);
	else if(element->type == RF_ET_DOMAIN)
		rf_domain_destroy(element->data);

	free(element);
}

void * rf_element_get_data(RF_ELEMENT *element)
{
	if(!element)
		return 0;

	return element->data;
}

char * rf_element_get_name(RF_ELEMENT *element)
{
	if(!element)
		return 0;

	if(element->type == RF_ET_NAME)
		return element->data;
	else
		return rf_domain_get_name(element->data);
}

int rf_element_get_type(RF_ELEMENT *element)
{
	if(!element)
		return -1;

	return element->type;
}
