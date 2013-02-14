/*
 * RelaFixLib    Domain
 *
 * Copyright (C) Peter Berger, 2011
 */

#include "domain.h"
#include "list.h"
#include "tools.h"
#include <stdlib.h>
#include <string.h>




/*!
 Checks if some element is double in the list. If so the function fails and an errormessage is
 returned.
 @relates RF_DOMAIN
 @param[in] elements A list containing pointers to struct RF_ELEMENT.
 @param[out] error On error a description is written into error.
 @return 0 on success.
 @return 1 on fail. A description is written to error. The errormessage must be freed by the caller.
 */
int rf_domain_check_elements(RF_LIST *elements, RF_ERROR *error)
{
	RF_LIST_ITERATOR *main, *sub;
	RF_ELEMENT *main_element, *sub_element;
	char *main_name, *sub_name;
	
	if(!elements)
	{
		error->string = rf_string_copy("program error - list is zero");
		return 1;
	}
	
	/* get iterator over elements */
	main = rf_list_get_begin(elements);
	if(!main)
	{
		error->string = rf_string_copy("program error - last call: rf_list_get_begin()");
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
			
			error->string = rf_string_copy("Found an zero element in elementlist");
			return 1;
		}
		
		/* get the name of the element */
		main_name = rf_element_get_name(main_element);
		if(!main_name)
		{
			rf_list_delete_iterator(main);
			
			error->string = rf_string_copy("Found an element that has no name in the elementlist");
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
				
				error->string = rf_string_copy("Found an zero element in elementlist");
				return 1;
			}
			
			/* get the name of the element */
			sub_name = rf_element_get_name(sub_element);
			if(!sub_name)
			{
				rf_list_delete_iterator(main);
				rf_list_delete_iterator(sub);
				
				error->string = rf_string_copy("Found an element that has no name in the elementlist");
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


/*!
 @relates RF_DOMAIN
 @param[in] domain The domain that should be copied.
 @return The copied domain.
 @return 0 on error.
 */
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
	new_domain->name = rf_string_copy(domain->name);
	
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


/*! @relates RF_DOMAIN
 * @param name Name of domain. Must be able to be used with free(). The pointer gets invalid for the caller!
 *				If 0 then the domain will be nameless.
 * @return Pointer to new domain. If an error occures 0 will be returned.
 */
RF_DOMAIN * rf_domain_create(char *name)
{
	RF_DOMAIN *domain = malloc(sizeof(RF_DOMAIN));
	if(!domain)
		return 0;

	domain->name = name;
	domain->elements = 0;

	return domain;
}

/*!
Creates a new powerset based on the given domain.

@relates RF_DOMAIN
@param[in] domain The domain the new domain is based on
@param[out] error On fail an errormessage is written here
@return The new domain based on the elements in domain.
@return 0 on fail. An errormessage is written in error.
*/
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
		error->string = rf_string_copy("program error - argument domain is zero");
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
					name = rf_string_copy(tmp_name);
				sub_element = rf_element_copy(element);
				rf_list_append(tmp_elements, sub_element);
				sub_element = 0;
			}
		}
		
		/* add new element to new domain */
		if(!name)
			name = rf_string_copy("_");
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
		
		error->string = rf_string_copy("program error - no memory");
		return 0;
	}
	rf_domain_set_list(new_domain, elements);
	elements = 0;
	
	return new_domain;
}

/*! @relates RF_DOMAIN
 * Frees all memory of domain. Including elements. Only destroy a domain if the domain is not
 * used by some other struct like RF_RELATION anymore!
 * @param domain Domain that needs to be destroyed.
 */
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


/*! \brief Search element by name.
 *
 * @relates RF_DOMAIN
 * @param domain The domain from where the element should be retrieved. If 0 the function will fail.
 * @param name Name of the element searched. If 0 the function will fail.
 * @return On success the function returns a pointer to the element. That element should not be
 *			changed or destroyed!
 * @return 0 on error
 */
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


/*!
Returns an element from the given domain and position.

@relates RF_DOMAIN
@param[in] domain The domain containing the element.
@param pos The position of the element.
@return The element in question. The returned element is must not be modified by the caller! It
	is only valid as long as the domain exist!
@return 0 on error.
*/
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

/*! \brief Get number of elements in the domain
 *
 * @relates RF_DOMAIN
 * @param domain The domain from which the elementcount should be returned
 * @return The element count for given domain.
 * @return -1 on error.
 */
int rf_domain_get_element_count(RF_DOMAIN *domain)
{
	if(!domain)
		return -1;
	
	if(!domain->elements)
		return -1;
	
	return rf_list_get_count(domain->elements);
}


/*!
 * Returns a list that the names of the elements contains. The names must not be changed
 * or deleted! The list itself must be destroyed with rf_list_destroy(list, 0) if not needed anymore.
 * @relates RF_DOMAIN
 * @param domain The domain from which the names should be retrieved.
 * @return list with names of the elements belonging to the given domain. The names must not be changed
 * 			or deleted! The list itself must be destroyed with rf_list_destroy(list, 0) if not needed anymore.
 * @return 0 on error.
 */
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


/*!
 * \brief Returns the position of the element with the given name
 *
 * @relates RF_DOMAIN
 * @param domain The domain that contains the elements.
 * @param name The name of the element from whom the position should be retrieved.
 * @return The position of the given element.
 * @return is < 0 on error.
 */
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


/*! \brief Returns list of the elements conatined by the domain
 *
 * @relates RF_DOMAIN
 * @param domain The domain that contains the elements
 * @return The list containing the elements (RF_ELEMENT *) of the given domain. The list and its
 * containing data must not be changed or destroyed!
 * @return 0 on error.
 */
RF_LIST * rf_domain_get_list(RF_DOMAIN *domain)
{
	if(!domain)
		return 0;

	if(!domain->elements)
		domain->elements = rf_list_create();

	return domain->elements;
}


/*!
 * @relates RF_DOMAIN
 * @param domain The domain from which the name should be retrieved.
 * @return The name of the domain. The name should not be changed or deleted!
 * @return 0 on error.
 */
char * rf_domain_get_name(RF_DOMAIN *domain)
{
	if(!domain)
		return 0;

	return domain->name;
}


/*!
 @relates RF_DOMAIN
 @param domain The domain to work on.
 @param name The name of the element in question.
 @return RF_TRUE if the element belongs to the domain.
 @return RF_FALSE if the element does not belong to the domain or on error.
 */
RF_BOOL rf_domain_has_element(RF_DOMAIN *domain, char *name)
{
	RF_LIST *names;
	RF_LIST_ITERATOR *element;
	
	if(!domain || !name)
		return RF_FALSE;
	
	/* get list with names from domain */
	names = rf_domain_get_element_names(domain);
	if(!names)
		return RF_FALSE;
	
	/* get iterator over names */
	element = rf_list_get_begin(names);
	if(!element)
	{
		rf_list_destroy(names, 0);
		return RF_FALSE;
	}
	
	/* check if given name is in list */
	while(rf_list_has_next(element))
		if(strcmp(name, rf_list_next(element)) == 0)
		{
			rf_list_delete_iterator(element);
			rf_list_destroy(names, 0);
			return RF_TRUE;
		}
		
	rf_list_delete_iterator(element);
	rf_list_destroy(names, 0);
	return RF_FALSE;
}


/*!
 @relates RF_DOMAIN
 @param domain The domain to work on.
 @param The name to compare
 @return RF_TRUE if it is the name of the domain.
 @return RF_FALSE it it is not the name of the domain or on error.
 */
RF_BOOL rf_domain_has_name(RF_DOMAIN *domain, char *name)
{
	if(!domain || !name)
		return RF_FALSE;
	if(!domain->name)
		return RF_FALSE;

	if(strcmp(domain->name, name))
		return RF_FALSE;
	else
		return RF_TRUE;
}


/*!
Tests if the domain1 is subdomain of domain2.

@relates RF_DOMAIN
@param[in] domain1 The subdomain.
@param[in] domain2 The domain the subdomain is part of.
@return RF_TRUE if domain is subdomain of domain2.
@return RF_FALSE if not or error.
*/
RF_BOOL rf_domain_is_partof(RF_DOMAIN *domain1, RF_DOMAIN *domain2)
{
	RF_LIST *names;
	RF_LIST_ITERATOR *element;
	char *name;
	
	if(!domain1 || !domain2)
	{
		return RF_FALSE;
	}
	
	/* get names in question */
	names = rf_domain_get_element_names(domain1);
	element = rf_list_get_begin(names);
	while(rf_list_has_next(element))
	{
		name = rf_list_next(element);
		if(rf_domain_has_element(domain2, name) == RF_FALSE)
		{
			rf_list_delete_iterator(element);
			rf_list_destroy(names, 0);
			
			return RF_FALSE;
		}
	}
	
	/* if we arrive here, domain1 is in domain2 */
	rf_list_delete_iterator(element);
	rf_list_destroy(names, 0);
	return RF_TRUE;
}

/*! \brief Sets the elements for the given domain.
 @relates RF_DOMAIN
 @param domain The domain to work on.
 @param list A list with elements (RF_ELEMENT *) inside. The list and its contained data is not
 		valid for the user anymore after the function returned!
 */
void rf_domain_set_list(RF_DOMAIN *domain, RF_LIST *list)
{
	if(!domain || !list)
		return;

	if(domain->elements)
		rf_list_destroy(domain->elements, (void (*)(void *))rf_element_destroy);

	domain->elements = list;
}


/*!
 @relates RF_DOMAIN
 @param domain The domain whoms name is to be set.
 @param name The new name. If 0 the domain will have no name.
 */
void rf_domain_set_name(RF_DOMAIN *domain, char *name)
{
	if(!domain)
		return;

	if(domain->name)
		free(domain->name);

	domain->name = name;
}


/*!
 @relates RF_ELEMENT
 @param[in] element The element that should be copied.
 @return The copy of the given element.
 @return 0 on error.
 */
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
		new_element->data = rf_string_copy(element->data);
	}
	else
	{
		free(new_element);
		return 0;
	}
	
	return new_element;
}


/*!
 @relates RF_ELEMENT
 @param type The type of the new element. Must be a value from ::rf_enum_et.
 @param data Depends on the value of type:
 			- RF_ET_DOMAIN: pointer to domain (RF_DOMAIN *)
 			- RF_ET_GLOBAL_DOMAIN: pointer to domain (RF_DOMAIN *). This domain will not be destroyed by a call to rf_element_destroy().
 			- RF_ET_NAME: pointer to name (char *)
 @return New element. The user must destroy it with rf_element_destroy() if it is not needed anymore!
 */
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


/*!
 @relates RF_ELEMENT
 @param element The element to be destroyed.
 */
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


/*!
 @relates RF_ELEMENT
 @param element The element containing the data.
 @return Depends on the type (::rf_enum_et) of the given element. The data should not be
 		changed or deleted by the user! For
 	- RF_ET_DOMAIN: pointer to domain (RF_DOMAIN *)
 	- RF_ET_GLOBAL_DOMAIN: pointer to global domain (RF_DOMAIN *)
 	- RF_ET_NAME: pointer to name (char *)
 @return 0 on error.
 */
void * rf_element_get_data(RF_ELEMENT *element)
{
	if(!element)
		return 0;

	return element->data;
}


/*!
 @relates RF_ELEMENT
 @param element The element whoms name should be retrieved.
 @return The name of the element. Should not be changed or deleted!
 @return 0 on error.
 */
char * rf_element_get_name(RF_ELEMENT *element)
{
	if(!element)
		return 0;
	
	if(element->type == RF_ET_NAME)
		return element->data;
	else
		return rf_domain_get_name(element->data);
}


/*!
 @relates RF_ELEMENT
 @param element The element whoms type should be retrieved.
 @return value of ::rf_enum_et.
 @return is < 0 on error.
 */
int rf_element_get_type(RF_ELEMENT *element)
{
	if(!element)
		return -1;

	return element->type;
}
