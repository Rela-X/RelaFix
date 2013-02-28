/*
 * RelaFixLib    Relation
 *
 * Copyright (C) Peter Berger, 2011
 */

#include <stdlib.h>
#include <string.h>

#include "relation.h"
#include "domain.h"
#include "tools.h"
#include "list.h"
#include "table.h"


int rf_relation_calc(RF_RELATION *relation, char *element_1, char *element_2)
{
	int x, y, result;

	if(!relation || !element_1 || !element_2)
	{
//		error->string = strdup("program error - some input value of relation_calc is zero");
		return 4;
	}

	/*
	 get the positions of the given elements in the domain. On fail the given element does
	 not belong to the domain. An error message is returned then.
	 */
	x = rf_domain_get_element_position(relation->domain_2, element_2);
	y = rf_domain_get_element_position(relation->domain_1, element_1);
	if(x < 0)
	{
//		error->string = rf_string_combine(5, "'", element_2, "' is not a element of domain '",
//			rf_domain_get_name(relation->domain_2), "'");
		return 4;
	}
	if(y < 0)
	{
//		error->string = rf_string_combine(5, "'", element_1, "' is not a element of domain '",
//			rf_domain_get_name(relation->domain_1), "'");
		return 4;
	}

	/* read the solution from the table. On error return an errordescription */
	result = rf_table_get_bit(relation->table, x, y);
	if(result == 0 || result == 1)
		return result;
	else
	{
//		error->string = strdup("program error - while reading bit from table");
		return 4;
	}
}


RF_RELATION * rf_relation_copy(RF_RELATION *relation)
{
	RF_RELATION *copy = 0;

	if(!relation)
		return 0;

	/* get memory */
	copy = calloc(1, sizeof(RF_RELATION));
	if(!copy)
		return 0;

	/* copy properties */
	copy->domain_1 = relation->domain_1;
	copy->domain_2 = relation->domain_2;
	copy->name = 0;
	copy->table = rf_table_copy(relation->table);

	return copy;
}


RF_RELATION * rf_relation_create(char *name, RF_DOMAIN *domain_1, RF_DOMAIN *domain_2)
{
	RF_RELATION *relation;
	RF_LIST *list;
	int width, height;

	if(!domain_1 || !domain_2)
		return 0;

	list = rf_domain_get_list(domain_1);
	height = rf_list_get_count(list);
	list = rf_domain_get_list(domain_2);
	width = rf_list_get_count(list);
	if(width <= 0 || height <= 0)
		return 0;

	relation = malloc(sizeof(RF_RELATION));
	if(!relation)
		return 0;

	relation->name = name;
	relation->domain_1 = domain_1;
	relation->domain_2 = domain_2;
	relation->table = rf_table_create(width, height, RF_TT_BINARY	);

	return relation;
}

RF_RELATION * rf_relation_create_bottom(RF_DOMAIN *domain)
{
	RF_RELATION *new_relation;
	int x, y, width, height;

	if(!domain)
	{
//		error->string = strdup("program error - argument domain is zero");
		return 0;
	}

	/* Create the new relation */
	new_relation = rf_relation_create(0, domain, domain);
	if(!new_relation)
	{
//		error->string = strdup("program error - no memory");
		return 0;
	}

	width = rf_table_get_width(new_relation->table);
	height = rf_table_get_height(new_relation->table);

	/* only the bottom triangle */
	for(y = 0; y < height; y++)
		for(x = 0; x <= y; x++)
		{
			rf_table_set_bit(new_relation->table, x, y, 1);
		}

	return new_relation;
}

RF_RELATION * rf_relation_create_complement(RF_RELATION *relation)
{
	RF_RELATION *new_relation;
	int x, y, width, height, result;

	if(!relation)
	{
//		error->string = strdup("program error - argument relation is zero");
		return 0;
	}

	/* create new_relation with conditions of the given relation */
	new_relation = rf_relation_create(0, relation->domain_1, relation->domain_2);
	if(!new_relation)
	{
//		error->string = strdup("program error - no memory");
		return 0;
	}
	else if(!new_relation->table)
	{
		rf_relation_destroy(new_relation);

//		error->string = strdup("program error - no memory");
		return 0;
	}

	width = rf_table_get_width(relation->table);
	height = rf_table_get_height(relation->table);

	/* read every bit of relation and add the reverse to new_relation */
	for(y = 0; y < height; y++)
		for(x = 0; x < width; x++)
		{
			result = rf_table_get_bit(relation->table, x, y);
			if(result == 0)
				rf_table_set_bit(new_relation->table, x, y, 1);
			else if(result == 1)
				rf_table_set_bit(new_relation->table, x, y, 0);
			else
			{
				rf_relation_destroy(new_relation);

//				error->string = strdup("program error - while set bit in table");
				return 0;
			}
		}

	return new_relation;
}

RF_RELATION * rf_relation_create_concatenation(RF_RELATION *relation_1, RF_RELATION *relation_2)
{
	RF_RELATION *new_relation = 0;
	int width1, width2, height1, height2, x1, x2, y1, result;

	if(!relation_1 || !relation_2)
	{
//		error->string = strdup("program error - argument relation is zero");
		return 0;
	}

	/* check if the Domains match */
	if(relation_1->domain_2 != relation_2->domain_1)
	{
//		error->string = rf_string_combine(5, "'", rf_domain_get_name(relation_1->domain_2), "' and '",
//			rf_domain_get_name(relation_2->domain_1), "' must be the same domain");
		return 0;
	}

	/* create new relation */
	new_relation = rf_relation_create(0, relation_1->domain_1, relation_2->domain_2);
	if(!new_relation)
	{
//		error->string = strdup("program error - no memory");
		return 0;
	}

	/* save the transitive relations */
	width1 = rf_domain_get_element_count(relation_1->domain_2);
	height1 = rf_domain_get_element_count(relation_1->domain_1);
	width2 = rf_domain_get_element_count(relation_2->domain_2);
	height2 = rf_domain_get_element_count(relation_2->domain_1);

	/* loop table of first relation */
	for(y1 = 0; y1 < height1; y1++)
		for(x1 = 0; x1 < width1; x1++)
		{
			result = rf_table_get_bit(relation_1->table, x1, y1);
			if(result < 0)
			{
//				error->string = strdup("program error - while reading bit from table");
				rf_relation_destroy(new_relation);
				return 0;
			}
			else if(result == 1)
			{
				/* loop tablerow of second relation */
				for(x2 = 0; x2 < width2; x2++)
				{
					result = rf_table_get_bit(relation_2->table, x2, x1);
					if(result < 0)
					{
//						error->string = strdup("program error - while reading bit from table");
						rf_relation_destroy(new_relation);
						return 0;
					}
					else if(result == 1)
					{
						rf_table_set_bit(new_relation->table, x2, y1, 1);
					}
				}
			}
		}

	return new_relation;
}

RF_RELATION * rf_relation_create_converse(RF_RELATION *relation)
{
	RF_RELATION *new_relation;
	int x, y, width, height, result;

	if(!relation)
	{
//		error->string = strdup("program error - argument relation is zero");
		return 0;
	}

	/* check if it is homogeneous */
	if(relation->domain_1 != relation->domain_2)
	{
//		error->string = rf_string_combine(3, "'", relation->name, "' is not homogeneous");
		return 0;
	}

	/* create new_relation with conditions of the given relation */
	new_relation = rf_relation_create(0, relation->domain_1, relation->domain_2);
	if(!new_relation)
	{
//		error->string = strdup("program error - no memory");
		return 0;
	}
	else if(!new_relation->table)
	{
		rf_relation_destroy(new_relation);

//		error->string = strdup("program error - no memory");
		return 0;
	}

	width = rf_table_get_width(relation->table);
	height = rf_table_get_height(relation->table);

	/* read every bit of relation and add the converse if bit was 1 to new_relation */
	for(y = 0; y < height; y++)
		for(x = 0; x < width; x++)
		{
			result = rf_table_get_bit(relation->table, x, y);
			if(result == 0);
			else if(result == 1)
				rf_table_set_bit(new_relation->table, y, x, 1);
			else
			{
				rf_relation_destroy(new_relation);

//				error->string = strdup("program error - while set bit in table");
				return 0;
			}
		}


	return new_relation;
}

RF_RELATION * rf_relation_create_empty(RF_DOMAIN *domain_1, RF_DOMAIN *domain_2)
{
	RF_RELATION *new_relation;
	int x, y, width, height;

	if(!domain_1 || !domain_2)
	{
//		error->string = strdup("program error - argument domain is zero");
		return 0;
	}

	/* Create the new relation */
	new_relation = rf_relation_create(0, domain_1, domain_2);
	if(!new_relation)
	{
//		error->string = strdup("program error - no memory");
		return 0;
	}

	width = rf_table_get_width(new_relation->table);
	height = rf_table_get_height(new_relation->table);

	/* all empty */
	for(y = 0; y < height; y++)
		for(x = 0; x < width; x++)
		{
			rf_table_set_bit(new_relation->table, x, y, 0);
		}

	return new_relation;
}

RF_RELATION * rf_relation_create_full(RF_DOMAIN *domain_1, RF_DOMAIN *domain_2)
{
	RF_RELATION *new_relation;
	int x, y, width, height;

	if(!domain_1 || !domain_2)
	{
//		error->string = strdup("program error - argument domain is zero");
		return 0;
	}

	/* Create the new relation */
	new_relation = rf_relation_create(0, domain_1, domain_2);
	if(!new_relation)
	{
//		error->string = strdup("program error - no memory");
		return 0;
	}

	width = rf_table_get_width(new_relation->table);
	height = rf_table_get_height(new_relation->table);

	/* all empty */
	for(y = 0; y < height; y++)
		for(x = 0; x < width; x++)
		{
			rf_table_set_bit(new_relation->table, x, y, 1);
		}

	return new_relation;
}

RF_RELATION * rf_relation_create_id(RF_DOMAIN *domain)
{
	RF_RELATION *new_relation;
	int x, y, width, height;

	if(!domain)
	{
//		error->string = strdup("program error - argument domain is zero");
		return 0;
	}

	/* Create the new relation */
	new_relation = rf_relation_create(0, domain, domain);
	if(!new_relation)
	{
//		error->string = strdup("program error - no memory");
		return 0;
	}

	width = rf_table_get_width(new_relation->table);
	height = rf_table_get_height(new_relation->table);

	/* only create Relations xRx */
	for(y = 0; y < height; y++)
		for(x = 0; x < width; x++)
		{
			if(x == y)
				rf_table_set_bit(new_relation->table, x, y, 1);
			else
				rf_table_set_bit(new_relation->table, x, y, 0);
		}

	return new_relation;
}


RF_RELATION * rf_relation_create_intersection(RF_RELATION *relation_1, RF_RELATION *relation_2)
{
	RF_RELATION *new_relation;
	int x, y, width, height, result_1, result_2;

	if(!relation_1 || !relation_2)
	{
//		error->string = strdup("program error - argument relation is zero");
		return 0;
	}


	/* check if both relations have same domains */
	if(relation_1->domain_1 != relation_2->domain_1)
	{
//		error->string = rf_string_combine(5, "Relation '", relation_1->name, "' and relation '",
//			relation_2->name, "' have different domains");
		return 0;
	}
	else if(relation_1->domain_2 != relation_2->domain_2)
	{
//		error->string = rf_string_combine(5, "Relation '", relation_1->name, "' and relation '",
//			relation_2->name, "' have different domains");
		return 0;
	}

	/* create new_relation with conditions of the given relations */
	new_relation = rf_relation_create(0, relation_1->domain_1, relation_1->domain_2);
	if(!new_relation)
	{
//		error->string = strdup("program error - no memory");
		return 0;
	}
	else if(!new_relation->table)
	{
		rf_relation_destroy(new_relation);

//		error->string = strdup("program error - no memory");
		return 0;
	}

	width = rf_table_get_width(relation_1->table);
	height = rf_table_get_height(relation_1->table);

	/* read every bit of both relation and the intersection to new_relation */
	for(y = 0; y < height; y++)
		for(x = 0; x < width; x++)
		{
			result_1 = rf_table_get_bit(relation_1->table, x, y);
			result_2 = rf_table_get_bit(relation_2->table, x, y);
			if(result_1 < 0)
			{
				rf_relation_destroy(new_relation);

//				error->string = strdup("program error - while set bit in table");
				return 0;
			}
			else if(result_2 < 0)
			{
				rf_relation_destroy(new_relation);

//				error->string = strdup("program error - while set bit in table");
				return 0;
			}
			else if(result_1 == 1 && result_2 == 1)
				rf_table_set_bit(new_relation->table, x, y, 1);
			else
				rf_table_set_bit(new_relation->table, x, y, 0);
		}

	return new_relation;
}

RF_RELATION * rf_relation_create_subsetleq(RF_DOMAIN *domain)
{
	RF_DOMAIN *domain1, *domain2;
	RF_RELATION *new_relation;
	RF_ELEMENT *element1, *element2;
	int x, y, width, height;

	if(!domain)
	{
//		error->string = strdup("program error - argument domain is zero");
		return 0;
	}

	/* Create the new relation */
	new_relation = rf_relation_create(0, domain, domain);
	if(!new_relation)
	{
//		error->string = strdup("program error - no memory");
		return 0;
	}

	width = rf_table_get_width(new_relation->table);
	height = rf_table_get_height(new_relation->table);

	/* only subset to superset */
	for(y = 0; y < height; y++)
	{
		element1 = rf_domain_get_element_by_position(domain, y);

		if(rf_element_get_type(element1) == RF_ET_DOMAIN ||
		   rf_element_get_type(element1) == RF_ET_GLOBAL_DOMAIN)
		{
			for(x = 0; x < width; x++)
			{
				if(x == y)
				{
					rf_table_set_bit(new_relation->table, x, y, 1);
				}
				else
				{
					element2 = rf_domain_get_element_by_position(domain, x);

					if(rf_element_get_type(element2) == RF_ET_DOMAIN ||
						rf_element_get_type(element2) == RF_ET_GLOBAL_DOMAIN)
					{
						domain1 = rf_element_get_data(element1);
						domain2 = rf_element_get_data(element2);

						if(rf_domain_is_partof(domain1, domain2))
							rf_table_set_bit(new_relation->table, x, y, 1);
					}
				}
			}
		}
	}

	return new_relation;
}

RF_RELATION * rf_relation_create_top(RF_DOMAIN *domain)
{
	RF_RELATION *new_relation;
	int x, y, width, height;

	if(!domain)
	{
//		error->string = strdup("program error - argument domain is zero");
		return 0;
	}

	/* Create the new relation */
	new_relation = rf_relation_create(0, domain, domain);
	if(!new_relation)
	{
//		error->string = strdup("program error - no memory");
		return 0;
	}

	width = rf_table_get_width(new_relation->table);
	height = rf_table_get_height(new_relation->table);

	/* only the top triangle */
	for(y = 0; y < height; y++)
		for(x = y; x < width; x++)
		{
			rf_table_set_bit(new_relation->table, x, y, 1);
		}

	return new_relation;
}

RF_RELATION * rf_relation_create_union(RF_RELATION *relation_1, RF_RELATION *relation_2)
{
	RF_RELATION *new_relation;
	int x, y, width, height, result_1, result_2;

	if(!relation_1 || !relation_2)
	{
//		error->string = strdup("program error - argument relation is zero");
		return 0;
	}


	/* check if both relations have same domains */
	if(relation_1->domain_1 != relation_2->domain_1)
	{
//		error->string = rf_string_combine(5, "Relation '", relation_1->name, "' and relation '",
//			relation_2->name, "' have different domains");
		return 0;
	}
	else if(relation_1->domain_2 != relation_2->domain_2)
	{
//		error->string = rf_string_combine(5, "Relation '", relation_1->name, "' and relation '",
//			relation_2->name, "' have different domains");
		return 0;
	}

	/* create new_relation with conditions of the given relations */
	new_relation = rf_relation_create(0, relation_1->domain_1, relation_1->domain_2);
	if(!new_relation)
	{
//		error->string = strdup("program error - no memory");
		return 0;
	}
	else if(!new_relation->table)
	{
		rf_relation_destroy(new_relation);

//		error->string = strdup("program error - no memory");
		return 0;
	}

	width = rf_table_get_width(relation_1->table);
	height = rf_table_get_height(relation_1->table);

	/* read every bit of both relation and the union to new_relation */
	for(y = 0; y < height; y++)
		for(x = 0; x < width; x++)
		{
			result_1 = rf_table_get_bit(relation_1->table, x, y);
			result_2 = rf_table_get_bit(relation_2->table, x, y);
			if(result_1 < 0)
			{
				rf_relation_destroy(new_relation);

//				error->string = strdup("program error - while set bit in table");
				return 0;
			}
			else if(result_2 < 0)
			{
				rf_relation_destroy(new_relation);

//				error->string = strdup("program error - while set bit in table");
				return 0;
			}
			else if(result_1 == 1 || result_2 == 1)
				rf_table_set_bit(new_relation->table, x, y, 1);
			else
				rf_table_set_bit(new_relation->table, x, y, 0);
		}

	return new_relation;
}

void rf_relation_destroy(RF_RELATION *relation)
{
	if(!relation)
		return;

	if(relation->name)
		free(relation->name);

	if(relation->table)
		rf_table_destroy(relation->table);

	free(relation);
}

char * rf_relation_find_infimum(RF_RELATION *relation, RF_DOMAIN *domain)
{
	RF_LIST *names;
	RF_LIST_ITERATOR *element1, *element2;
	char *name1 = 0, *name2 = 0;
	bool isInfimum;
	int x, y, result;

	if(!relation)
	{
//		error->string = strdup("program error - relation is zero");
		return 0;
	}

	if(!domain)
	{
//		error->string = strdup("program error - domain is zero");
		return 0;
	}

	/* check if the relation is homogeneous */
	if(relation->domain_1 != relation->domain_2)
	{
//		error->string = rf_string_combine(3, "'", relation->name, "' is not homogeneous.");
		return 0;
	}

	/* check if relation is a poset */
	if(rf_relation_is_poset(relation) != 1)
	{
//		error->string = rf_string_combine(3, "'", relation->name, "' is not a ordered set.");
		return 0;
	}

	/* check if the domain is part of relation->domain_1 */
	if(rf_domain_is_partof(domain, relation->domain_1) == false)
	{
//		error->string = rf_string_combine(5, "'", rf_domain_get_name(domain), "' is not part of '",
//			rf_domain_get_name(relation->domain_1), "'");
		return 0;
	}

	/* find the infimum */
	names = rf_domain_get_element_names(domain);
	element1 = rf_list_get_begin(names);
	while(rf_list_has_next(element1))
	{
		/* get position in table */
		name1 = rf_list_next(element1);
		y = rf_domain_get_element_position(relation->domain_1, name1);

		/* check if element1 is bigger then all others */
		element2 = rf_list_get_begin(names);
		isInfimum = true;
		while(rf_list_has_next(element2))
		{
			/* get positions in table */
			name2 = rf_list_next(element2);
			if(name1 == name2) /* we dont test against the same element! */
				continue;
			x = rf_domain_get_element_position(relation->domain_2, name2);

			result = rf_table_get_bit(relation->table, x, y);
			if(result < 0)
			{
				rf_list_delete_iterator(element1);
				rf_list_delete_iterator(element2);
				rf_list_destroy(names, 0);

//				error->string = strdup("program error - while reading bit in table");
				return 0;
			}
			else if(result == 1) /* element1 is not bigger then element2 */
			{
				isInfimum = false;
				break;
			}
			else if(result == 0)
			{
				/* we need to check if it is realy bigger then element2 */
				result = rf_table_get_bit(relation->table, y, x);
				if(result < 0)
				{
					rf_list_delete_iterator(element1);
					rf_list_delete_iterator(element2);
					rf_list_destroy(names, 0);

//					error->string = strdup("program error - while reading bit in table");
					return 0;
				}
				else if(result == 0) /* element1 is not bigger then element2 (we dont know) */
				{
					isInfimum = false;
					break;
				}
			}
		}
		rf_list_delete_iterator(element2);

		/* check if infimum was found */
		if(isInfimum == true)
		{
			rf_list_delete_iterator(element1);
			rf_list_destroy(names, 0);

			/* we take the name from the relations domain, cause the other will be destroyed soon */
			names = rf_domain_get_element_names(relation->domain_1);
			name1 = rf_list_get_by_position(names, y);
			rf_list_destroy(names, 0);
			return name1;
		}
	}

	/* if we arrive here, no infimum exists */
	rf_list_delete_iterator(element1);
	rf_list_destroy(names, 0);

	return "no infimum";
}

RF_DOMAIN * rf_relation_find_lowerbound(RF_RELATION *relation, RF_DOMAIN *domain)
{
	int x, y, height, result;
	RF_LIST *names, *new_elements;
	RF_LIST_ITERATOR *element;
	RF_ELEMENT *new_element;
	RF_DOMAIN *new_domain;
	bool isLowerbound;
	char *name;

	if(!relation)
	{
//		error->string = strdup("program error - relation is zero");
		return 0;
	}

	if(!domain)
	{
//		error->string = strdup("program error - domain is zero");
		return 0;
	}

	/* check if the relation is homogeneous */
	if(relation->domain_1 != relation->domain_2)
	{
//		error->string = rf_string_combine(3, "'", relation->name, "' is not homogeneous.");
		return 0;
	}

	/* check if relation is a poset */
	if(rf_relation_is_poset(relation) != 1)
	{
//		error->string = rf_string_combine(3, "'", relation->name, "' is not a ordered set.");
		return 0;
	}

	/* check if the domain is part of relation->domain_1 */
	if(rf_domain_is_partof(domain, relation->domain_1) == false)
	{
//		error->string = rf_string_combine(5, "'", rf_domain_get_name(domain), "' is not part of '",
//			rf_domain_get_name(relation->domain_1), "'");
		return 0;
	}

	/* find all lower elements */
	height = rf_table_get_height(relation->table);
	names = rf_domain_get_element_names(domain);
	new_elements = rf_list_create();
	for(y = 0; y < height; y++)
	{
		/* we compare element y against every element from domain */
		element = rf_list_get_begin(names);
		isLowerbound = true;
		while(rf_list_has_next(element))
		{
			name = rf_list_next(element);

			x = rf_domain_get_element_position(relation->domain_1, name);
			result = rf_table_get_bit(relation->table, x, y);
			if(result < 0)
			{
				rf_list_delete_iterator(element);
				rf_list_destroy(names, 0);
				rf_list_destroy(new_elements, 0);

//				error->string = strdup("program error - while reading bit from table");
				return 0;
			}
			else if(result == 0) /* y is not an lowerbound */
			{
				isLowerbound = false;
			}
		}
		rf_list_delete_iterator(element);
		element = 0;

		/* if y is lowerbound we add it to the new domain */
		if(isLowerbound == true)
		{
			new_element = rf_element_copy(rf_domain_get_element_by_position(relation->domain_1, y));
			rf_list_append(new_elements, new_element);
		}
	}

	rf_list_destroy(names, 0);

	/* add the list to the domain and return the new domain */
	new_domain = rf_domain_create(strdup("no name"));
	rf_domain_set_list(new_domain, new_elements);
	return new_domain;
}

char * rf_relation_find_maximum(RF_RELATION *relation)
{
	int x, y, width, height, max, result;
	RF_LIST *names;
	char *name;

	if(!relation)
	{
//		error->string = strdup("program error - relation is zero");
		return 0;
	}

	/* check if the relation is homogeneous */
	if(relation->domain_1 != relation->domain_2)
	{
//		error->string = rf_string_combine(3, "'", relation->name, "' is not homogeneous.");
		return 0;
	}

	/* check if relation is a poset */
	if(rf_relation_is_poset(relation) != 1)
	{
//		error->string = rf_string_combine(3, "'", relation->name, "' is not a ordered set.");
		return 0;
	}

	/* search the maximum */
	width = rf_table_get_width(relation->table);
	height = rf_table_get_height(relation->table);
	for(y = 0, max = -1; y < height; y++)
	{
		for(x = 0, max = -1; x < width; x++)
		{
			if(x == y) /* we dont compare the element with itself */
				continue;

			result = rf_table_get_bit(relation->table, x, y); /* check if relation exist */
			if(result < 0)
			{
//				error->string = strdup("program error - while reading bit from table");
				return 0;
			}
			else if(result == 0) /* y is maybe a posssible maximum */
			{
				result = rf_table_get_bit(relation->table, y, x); /* if it exists -> y > x */
				if(result < 0)
				{
//					error->string = strdup("program error - while reading bit from table");
					return 0;
				}
				else if(result == 1) /* y > x */
				{
					/* we remember y as a possible maximum */
					max = y;
				}
			}
			else if(result == 1) /* y is < or = x --> not a maximum */
			{
				max = -1; /* makes it invalid */
				break;
			}
		}

		/*if max >= 0 then the maximum is found and we return it*/
		if(max >= 0)
		{
			names = rf_domain_get_element_names(relation->domain_1);
			name = rf_list_get_by_position(names, max);
			rf_list_destroy(names, 0);
			return name;
		}
	}

	/* if we come here, there exists no maximum and we create an error */
//	error->string = rf_string_combine(3, "For relation '", relation->name, "' exists no maximum.");
	return 0;
}

char * rf_relation_find_minimum(RF_RELATION *relation)
{
	int x, y, width, height, min, result;
	RF_LIST *names;
	char *name;

	if(!relation)
	{
//		error->string = strdup("program error - relation is zero");
		return 0;
	}

	/* check if the relation is homogeneous */
	if(relation->domain_1 != relation->domain_2)
	{
//		error->string = rf_string_combine(3, "'", relation->name, "' is not homogeneous.");
		return 0;
	}

	/* check if relation is a poset */
	if(rf_relation_is_poset(relation) != 1)
	{
//		error->string = rf_string_combine(3, "'", relation->name, "' is not a ordered set.");
		return 0;
	}

	/* search the minimum */
	width = rf_table_get_width(relation->table);
	height = rf_table_get_height(relation->table);
	for(y = 0, min = -1; y < height; y++)
	{
		for(x = 0, min = -1; x < width; x++)
		{
			if(x == y) /* we dont compare the element with itself */
				continue;

			result = rf_table_get_bit(relation->table, y, x); /* check if relation exist */
			if(result < 0)
			{
//				error->string = strdup("program error - while reading bit from table");
				return 0;
			}
			else if(result == 0) /* y is maybe a posssible minimum */
			{
				result = rf_table_get_bit(relation->table, x, y); /* if it exists -> y < x */
				if(result < 0)
				{
//					error->string = strdup("program error - while reading bit from table");
					return 0;
				}
				else if(result == 1) /* y > x */
				{
					/* we remember y as a possible minimum */
					min = y;
				}
			}
			else if(result == 1) /* x is < or = y --> not a minimum */
			{
				min = -1; /* makes it invalid */
				break;
			}
		}

		/*if min >= 0 then the minimum is found and we return it*/
		if(min >= 0)
		{
			names = rf_domain_get_element_names(relation->domain_1);
			name = rf_list_get_by_position(names, min);
			rf_list_destroy(names, 0);
			return name;
		}
	}

	/* if we come here, there exists no minimum and we create an error */
//	error->string = rf_string_combine(3, "For relation '", relation->name, "' exists no minimum.");
	return 0;
}

char * rf_relation_find_supremum(RF_RELATION *relation, RF_DOMAIN *domain)
{
	RF_LIST *names;
	RF_LIST_ITERATOR *element1, *element2;
	char *name1 = 0, *name2 = 0;
	bool isSupremum;
	int x, y, result;

	if(!relation)
	{
//		error->string = strdup("program error - relation is zero");
		return 0;
	}

	if(!domain)
	{
//		error->string = strdup("program error - domain is zero");
		return 0;
	}

	/* check if the relation is homogeneous */
	if(relation->domain_1 != relation->domain_2)
	{
//		error->string = rf_string_combine(3, "'", relation->name, "' is not homogeneous.");
		return 0;
	}

	/* check if relation is a poset */
	if(rf_relation_is_poset(relation) != 1)
	{
//		error->string = rf_string_combine(3, "'", relation->name, "' is not a ordered set.");
		return 0;
	}

	/* check if the domain is part of relation->domain_1 */
	if(rf_domain_is_partof(domain, relation->domain_1) == false)
	{
//		error->string = rf_string_combine(5, "'", rf_domain_get_name(domain), "' is not part of '",
//			rf_domain_get_name(relation->domain_1), "'");
		return 0;
	}

	/* find the supremum */
	names = rf_domain_get_element_names(domain);
	element1 = rf_list_get_begin(names);
	while(rf_list_has_next(element1))
	{
		/* get position in table */
		name1 = rf_list_next(element1);
		y = rf_domain_get_element_position(relation->domain_1, name1);

		/* check if element1 is smaller then all otheres */
		element2 = rf_list_get_begin(names);
		isSupremum = true;
		while(rf_list_has_next(element2))
		{
			/* get positions in table */
			name2 = rf_list_next(element2);
			if(name1 == name2) /* we dont test against the same element! */
				continue;
			x = rf_domain_get_element_position(relation->domain_2, name2);

			result = rf_table_get_bit(relation->table, x, y);
			if(result < 0)
			{
				rf_list_delete_iterator(element1);
				rf_list_delete_iterator(element2);
				rf_list_destroy(names, 0);

//				error->string = strdup("program error - while reading bit in table");
				return 0;
			}
			else if(result == 0) /* element1 is not smaller then element2 */
			{
				isSupremum = false;
				break;
			}
			else if(result == 1)
			{
				/* we need to check if it is realy smaller then element2 */
				result = rf_table_get_bit(relation->table, y, x);
				if(result < 0)
				{
					rf_list_delete_iterator(element1);
					rf_list_delete_iterator(element2);
					rf_list_destroy(names, 0);

//					error->string = strdup("program error - while reading bit in table");
					return 0;
				}
				else if(result == 1) /* element1 is not smaller then element2 (we dont know) */
				{
					isSupremum = false;
					break;
				}
			}
		}
		rf_list_delete_iterator(element2);

		/* check if Supremum was found */
		if(isSupremum == true)
		{
			rf_list_delete_iterator(element1);
			rf_list_destroy(names, 0);

			/* we take the name from the relations domain, cause the other will be destroyed soon */
			names = rf_domain_get_element_names(relation->domain_1);
			name1 = rf_list_get_by_position(names, y);
			rf_list_destroy(names, 0);
			return name1;
		}
	}

	/* if we arrive here, no supremum exists */
	rf_list_delete_iterator(element1);
	rf_list_destroy(names, 0);

	return "no supremum";
}

RF_DOMAIN * rf_relation_find_upperbound(RF_RELATION *relation, RF_DOMAIN *domain)
{
	int x, y, height, result;
	RF_LIST *names, *new_elements;
	RF_LIST_ITERATOR *element;
	RF_ELEMENT *new_element;
	RF_DOMAIN *new_domain;
	bool isUpperbound;
	char *name;

	if(!relation)
	{
//		error->string = strdup("program error - relation is zero");
		return 0;
	}

	if(!domain)
	{
//		error->string = strdup("program error - domain is zero");
		return 0;
	}

	/* check if the relation is homogeneous */
	if(relation->domain_1 != relation->domain_2)
	{
//		error->string = rf_string_combine(3, "'", relation->name, "' is not homogeneous.");
		return 0;
	}

	/* check if relation is a poset */
	if(rf_relation_is_poset(relation) != 1)
	{
//		error->string = rf_string_combine(3, "'", relation->name, "' is not a ordered set.");
		return 0;
	}

	/* check if the domain is part of relation->domain_1 */
	if(rf_domain_is_partof(domain, relation->domain_1) == false)
	{
//		error->string = rf_string_combine(5, "'", rf_domain_get_name(domain), "' is not part of '",
//			rf_domain_get_name(relation->domain_1), "'");
		return 0;
	}

	/* find all upper elements */
	height = rf_table_get_height(relation->table);
	names = rf_domain_get_element_names(domain);
	new_elements = rf_list_create();
	for(y = 0; y < height; y++)
	{
		/* we compare element y against every element from domain */
		element = rf_list_get_begin(names);
		isUpperbound = true;
		while(rf_list_has_next(element))
		{
			name = rf_list_next(element);

			x = rf_domain_get_element_position(relation->domain_1, name);
			result = rf_table_get_bit(relation->table, y, x);
			if(result < 0)
			{
				rf_list_delete_iterator(element);
				rf_list_destroy(names, 0);
				rf_list_destroy(new_elements, 0);

//				error->string = strdup("program error - while reading bit from table");
				return 0;
			}
			else if(result == 0) /* y is not an upperbound */
			{
				isUpperbound = false;
			}
		}
		rf_list_delete_iterator(element);
		element = 0;

		/* if y is upperbound we add it to the new domain */
		if(isUpperbound == true)
		{
			new_element = rf_element_copy(rf_domain_get_element_by_position(relation->domain_1, y));
			rf_list_append(new_elements, new_element);
		}
	}

	rf_list_destroy(names, 0);

	/* add the list to the domain and return the new domain */
	new_domain = rf_domain_create(strdup("no name"));
	rf_domain_set_list(new_domain, new_elements);
	return new_domain;
}


RF_DOMAIN * rf_relation_get_domain_1(RF_RELATION *relation)
{
	if(!relation)
		return 0;

	return relation->domain_1;
}

RF_DOMAIN * rf_relation_get_domain_2(RF_RELATION *relation)
{
	if(!relation)
		return 0;

	return relation->domain_2;
}


char * rf_relation_get_name(RF_RELATION *relation)
{
	if(!relation)
		return 0;
	else if(!relation->name)
		return "no name";

	return relation->name;
}


RF_TABLE * rf_relation_get_table(RF_RELATION *relation)
{
	if(!relation)
		return 0;

	return relation->table;
}

bool rf_relation_has_name(RF_RELATION *relation, char *name)
{
	if(!relation || !name)
		return false;
	if(!relation->name)
		return false;

	if(strcmp(relation->name, name))
		return false;
	else
		return true;
}


int rf_relation_is_antisymmetric(RF_RELATION *relation)
{
	int x, y, width, result;

	if(!relation)
		return 2;

	/* test if relation is homogeneous */
	if(relation->domain_1 != relation->domain_2)
		return 3;

	/* get size of the table */
	width = rf_table_get_width(relation->table);

	/* loop the table and test for antiymmetrie */
	for(y = 0; y < width; y++)
		for(x = 0; x < width; x++)
		{
			if(x == y) /* no need to test xRx */
				continue;

			result = rf_table_get_bit(relation->table, x, y);
			if(result < 0) /* error */
				return 4;
			else if(result == 1) /* now need to test if the other relation exist or not */
			{
				result = rf_table_get_bit(relation->table, y, x);
				if(result < 0) /* error */
					return 4;
				else if(result == 1) /* exists -> relation is not antisymmetric */
					return 0;
			}
		}

	/* if we come here, the relation is antisymmetric */
	return 1;
}

int rf_relation_is_asymmetric(RF_RELATION *relation)
{
	int x, y, width, result_1, result_2;

	if(!relation)
		return 2;

	/* test if relation is homogeneous */
	if(relation->domain_1 != relation->domain_2)
		return 3;

	/* get table size */
	width = rf_table_get_width(relation->table);

	/* loop the table and check if relation is asymmetric */
	for(y = 0; y < width; y++)
		for(x = y; x < width; x++)
		{
			if(x == y) /* xRx */
			{
				result_1 = rf_table_get_bit(relation->table, x, y);
				if(result_1 == 1) /* xRx exist -> not asymmetric */
					return 0;
				else if(result_1 == 0)
					continue;
				else
					return 4;
			}
			else
			{
				result_1 = rf_table_get_bit(relation->table, x, y);
				result_2 = rf_table_get_bit(relation->table, y, x);
				if((result_1 != 0 && result_1 != 1) || (result_2 != 0 && result_2 != 1)) /* error */
					return 4;
				else if(result_1 == result_2) /* xRy and yRx exist -> not asymmetric */
					return 0;
			}
		}

	/* if we come here, the relation is asymmetric */
	return 1;
}


int rf_relation_is_difunctional(RF_RELATION *relation)
{
	int x, y, dx, dy, width, result;

	if(!relation)
		return 2;

	/* test if relation is homogeneous */
	if(relation->domain_1 != relation->domain_2)
		return 3;

	/* get table size */
	width = rf_table_get_width(relation->table);

	/* loop table and check if relation is difunctional */
	for(y = 0; y < width; y++)
		for(x = 0; x < width; x++)
		{
			result = rf_table_get_bit(relation->table, x, y);
			if(result != 0 && result != 1) /* error */
				return 4;
			else if(result == 1) /* yRx exist */
			{
				/* now need to find all zRx */
				for(dy = 0; dy < width; dy++)
				{
					if(y == dy) /* --> yRx = zRx */
						continue;

					result = rf_table_get_bit(relation->table, x, dy);
					if(result != 0 && result != 1) /* error */
						return 4;
					else if(result == 1) /* zRx exist */
					{
						/* now need to find all zRw (w = dx)*/
						for(dx = 0; dx < width; dx++)
						{
							result = rf_table_get_bit(relation->table, dx, dy);
							if(result != 0 && result != 1) /* error */
								return 4;
							else if(result == 1) /* zRw exist */
							{
								/* now also yRw must exist! */
								result = rf_table_get_bit(relation->table, dx, y);
								if(result != 0 && result != 1) /* error */
									return 4;
								else if(result == 0) /* yRw does not exist --> relation not difunctional */
									return 0;
							}
						}
					}

				}
			}
		}

	/* if we come here, the relation is difunctional */
	return 1;
}

int rf_relation_is_equivalent(RF_RELATION *relation)
{
	int reflexive, symmetric, transitive;

	if(!relation)
		return 2;

	reflexive = rf_relation_is_reflexive(relation);
	symmetric = rf_relation_is_symmetric(relation);
	transitive = rf_relation_is_transitive(relation);

	if(reflexive > 1 || reflexive < 0)
		return 4;
	if(symmetric > 1 || symmetric < 0)
		return 4;
	if(transitive > 1 || transitive < 0)
		return 4;

	if(reflexive && symmetric && transitive)
		return 1;
	else
		return 0;
}

int rf_relation_is_irreflexive(RF_RELATION *relation)
{
	int x, y, width, result;

	if(!relation)
		return 2;

	/* test if relation is homogeneous */
	if(relation->domain_1 != relation->domain_2)
		return 3;

	/* get table size */
	width = rf_table_get_width(relation->table);

	/* loop all xRx and test if relation is irreflexive */
	for(x = 0, y = 0; x < width; x++, y++)
	{
		result = rf_table_get_bit(relation->table, x, y);
		if(result != 0 && result != 1) /* error */
			return 4;
		else if(result == 1) /* --> not irreflexive */
			return 0;
	}

	/* if we come here the relation is irreflexive */
	return 1;
}

int rf_relation_is_poset(RF_RELATION *relation)
{
	int reflexive, antisymmetric, transitive;

	if(!relation)
		return 2;

	reflexive = rf_relation_is_reflexive(relation);
	antisymmetric = rf_relation_is_antisymmetric(relation);
	transitive = rf_relation_is_transitive(relation);

	if(reflexive > 1 || reflexive < 0) /* error */
		return 4;
	if(antisymmetric > 1 || antisymmetric < 0) /* error */
		return 4;
	if(transitive > 1 || transitive < 0) /* error */
		return 4;

	if(reflexive && antisymmetric && transitive)
		return 1;
	else
		return 0;
}

int rf_relation_is_preorder(RF_RELATION *relation)
{
	int reflexive, transitive;

	if(!relation)
		return 2;

	reflexive = rf_relation_is_reflexive(relation);
	transitive = rf_relation_is_transitive(relation);

	if(reflexive > 1 || reflexive < 0) /* error */
		return 4;
	if(transitive > 1 || transitive < 0) /* error */
		return 4;

	if(reflexive && transitive)
		return 1;
	else
		return 0;
}

int rf_relation_is_reflexive(RF_RELATION *relation)
{
	int x, y, width, result;

	if(!relation)
		return 2;

	/* test if relation is homogeneous */
	if(relation->domain_1 != relation->domain_2)
		return 3;

	/* get table size */
	width = rf_table_get_width(relation->table);

	/* loop all xRx and test for reflexivity */
	for(x = 0, y = 0; x < width; x++, y++)
	{
		result = rf_table_get_bit(relation->table, x, y);
		if(result != 0 && result != 1) /* error */
			return 4;
		else if(result == 0) /* --> not reflexive */
			return 0;
	}

	/* if we come here the relation is reflexive */
	return 1;
}

int rf_relation_is_symmetric(RF_RELATION *relation)
{
	int x, y, width, result_1, result_2;

	if(!relation)
		return 2;

    /* test if the relation is homogeneous */
	if(relation->domain_1 != relation->domain_2)
		return 3;

	/* get table size */
	width = rf_table_get_width(relation->table);

	/* loop top triangle of table and test if the relation is symmetric */
	for(y = 0; y < width - 1; y++)
		for(x = y + 1; x < width; x++)
		{
			result_1 = rf_table_get_bit(relation->table, x, y);
			result_2 = rf_table_get_bit(relation->table, y, x);
			if((result_1 != 0 && result_1 != 1) || (result_2 != 0 && result_2 != 1)) /* error */
				return 4;
			else if(result_1 != result_2) /* --> not symmetric */
				return 0;
		}

	/* if we come here, the relation is symmetric */
	return 1;
}

int rf_relation_is_transitive(RF_RELATION *relation)
{
	int x, dx, y, dy, width, result_1, result_2;

	if(!relation)
		return 2;

	/* test if relation is homogeneous */
	if(relation->domain_1 != relation->domain_2)
		return 3;

	/* get table size */
	width = rf_table_get_width(relation->table);

	/* loop table and test if the relation is transitive */
	for(y = 0; y < width; y++)
		for(x = 0; x < width; x++)
		{
			if(x == y) /* no need to test xRx */
				continue;

			result_1 = rf_table_get_bit(relation->table, x, y);
			if(result_1 != 0 && result_1 != 1) /* error */
				return 4;

			if(result_1) /* yRx exist */
			{
				/* now we need to test every xRz */
				for(dx = 0, dy = x; dx < width; dx++)
				{
					if(x == dx || dx == dy)
						continue;

					result_1 = rf_table_get_bit(relation->table, dx, dy);
					if(result_1 != 0 && result_1 != 1) /* error */
						return 4;

					if(result_1 == 1) /* xRz exist */
					{
						/* now also yRz must exist */
						result_2 = rf_table_get_bit(relation->table, dx, y);
						if(result_2 != 0 && result_2 != 1) /* error */
							return 4;
						if(result_2 == 0) /* xRz does not exist --> relation is not transitive */
							return 0;
					}
				}
			}
		}

	/* if we come here, the relation is transitive */
	return 1;
}

void rf_relation_set_name(RF_RELATION *relation, char *name)
{
	if(!relation)
	{
		if(name)
			free(name);
	}

	if(relation->name)
		free(relation->name);

	relation->name = name;
}

void rf_relation_set_table(RF_RELATION *relation, RF_TABLE *table)
{
	if(!relation || !table)
		return;

	if(relation->table)
		rf_table_destroy(relation->table);

	relation->table = 0;
}




int rf_relation_gain_symmetric(RF_RELATION* relation, bool fill)
{
    int res = rf_relation_is_symmetric(relation);
    if (res != 0)
        return res;

    //proceed only if the relation is not already symmetric
    int x, y, width, result_1, result_2;


    /* loop top triangle of table and test if the relation is symmetric */
    width = rf_table_get_width(relation->table);
    for (y = 0; y < width - 1; y++)
        for (x = y + 1; x < width; x++)
        {
            result_1 = rf_table_get_bit(relation->table, x, y);
            result_2 = rf_table_get_bit(relation->table, y, x);
            //because is_symmetric already checked result
            //no need to check it again
            if (result_1 != result_2){
                if (fill){
                    rf_table_set_bit(relation->table, x, y, 1);
                    rf_table_set_bit(relation->table, y, x, 1);
                }else{
                    rf_table_set_bit(relation->table, x, y, 0);
                    rf_table_set_bit(relation->table, y, x, 0);
                }

            }
        }

    //check again. Will terminate if rf_relation_is_symmetric reutrns 1 or an error
    return rf_relation_gain_symmetric(relation, fill);
}


int rf_relation_gain_antisymmetric(RF_RELATION* relation, bool upper)
{
    int res = rf_relation_is_antisymmetric(relation);
    if (res != 0)
        return res;

    //proceed only if the relation is not already antisymmetric
    int x, y, width, result;


    //loop the table and test for antiymmetrie
    width = rf_table_get_width(relation->table);
    for (y = 0; y < width; y++)
        for (x = 0; x < width; x++)
        {
            if (x == y) /* no need to test xRx */
                continue;

            result = rf_table_get_bit(relation->table, x, y);
            if (result == 1) /* now need to test if the other relation exist or not */
            {
                result = rf_table_get_bit(relation->table, y, x);
                if (result == 1){
                    if (upper){
                        rf_table_set_bit(relation->table, y, x, 0);
                    }else{
                        rf_table_set_bit(relation->table, x, y, 0);
                    }
                }
            }
        }

    //check again. Will terminate if rf_relation_is_antisymmetric reutrns 1 or an error
    return rf_relation_gain_antisymmetric(relation, upper);
}


int rf_relation_gain_irreflexive(RF_RELATION* relation)
{
    int res = rf_relation_is_irreflexive(relation);
    if (res != 0)
        return res;

    //proceed only if the relation is not already irreflexive
    int x, y, width, result;


    // loop all xRx and test if relation is irreflexive
    width = rf_table_get_width(relation->table);
    for (x = 0, y = 0; x < width; x++, y++)
    {
        result = rf_table_get_bit(relation->table, x, y);
        //because is_irreflexive already checked result
        //no need to check it again
        if (result == 1)
            rf_table_set_bit(relation->table, x, y, 0);
    }

    //check again. Will terminate if rf_relation_is_irreflexive reutrns 1 or an error
    return rf_relation_gain_irreflexive(relation);
}


int rf_relation_gain_reflexive(RF_RELATION* relation)
{
    int res = rf_relation_is_reflexive(relation);
    if (res != 0)
        return res;

    //proceed only if the relation is not already reflexive
    int x, y, width, result;


    // loop all xRx and test for reflexivity
    width = rf_table_get_width(relation->table);
    for (x = 0, y = 0; x < width; x++, y++)
    {
        result = rf_table_get_bit(relation->table, x, y);
        //because is_reflexive already checked result
        //no need to check it again
        if (result == 0)
            rf_table_set_bit(relation->table, x, y, 1);
    }

    //check again. Will terminate if rf_relation_is_reflexive reutrns 1 or an error
    return rf_relation_gain_reflexive(relation);
}




int rf_relation_gain_transitive(RF_RELATION* relation, bool fill)
{
    int res = rf_relation_is_transitive(relation);
    if (res != 0)
        return res;

    //proceed only if the relation is not already transitive
    int x, dx, y, dy, width, result_1, result_2;

    //because is_transitive already checked result
    //they will not be checked in the following code
    // loop table and test if the relation is transitive
    width = rf_table_get_width(relation->table);
    for (y = 0; y < width; y++)
        for (x = 0; x < width; x++)
        {
            if (x == y) /* no need to test xRx */
                continue;

            result_1 = rf_table_get_bit(relation->table, x, y);
            if (result_1) /* yRx exist */
            {
                /* now we need to test every xRz */
                for (dx = 0, dy = x; dx < width; dx++)
                {
                    if (x == dx || dx == dy)
                        continue;

                    result_1 = rf_table_get_bit(relation->table, dx, dy);
                    if (result_1 == 1) /* xRz exist */
                    {
                        /* now also yRz must exist */
                        result_2 = rf_table_get_bit(relation->table, dx, y);
                        if (result_2 == 0){
                            if (fill){
                                rf_table_set_bit(relation->table, dx, y, 1);
                                break;
                            }else{
                                rf_table_set_bit(relation->table, dx, dy, 0);
                                break;
                            }
                        }
                    }
                }
            }
        }
    //check again. Will terminate if rf_relation_is_transitive reutrns 1 or an error
    return rf_relation_gain_transitive(relation, fill);
}




int rf_relation_gain_difunctional(RF_RELATION* relation, bool fill)
{
    int res = rf_relation_is_difunctional(relation);
    if (res != 0)
        return res;

    //proceed only if the relation is not already transitive
    int x, y, dx, dy, width, result;

    //because is_difunctional already checked result
    //they will not be checked in the following code

    //loop table and check if relation is difunctional
    width = rf_table_get_width(relation->table);
    for (y = 0; y < width; y++)
        for (x = 0; x < width; x++)
        {
            result = rf_table_get_bit(relation->table, x, y);
            if (result == 1) /* yRx exist */
            {
                /* now need to find all zRx */
                for (dy = 0; dy < width; dy++)
                {
                    if (y == dy) /* --> yRx = zRx */
                        continue;

                    result = rf_table_get_bit(relation->table, x, dy);
                    if (result == 1) /* zRx exist */
                    {
                        /* now need to find all zRw (w = dx)*/
                        for (dx = 0; dx < width; dx++)
                        {
                            result = rf_table_get_bit(relation->table, dx, dy);
                            if (result == 1) /* zRw exist */
                            {
                                /* now also yRw must exist! */
                                result = rf_table_get_bit(relation->table, dx, y);
                                if (result == 0){ /* yRw does not exist --> relation not difunctional */
                                    if (fill){
                                        rf_table_set_bit(relation->table, dx, y, 1);
                                    }else{
                                        rf_table_set_bit(relation->table, dx, dy, 0);
                                    }
                                }
                            }
                        }
                    }

                }
            }
        }

    //check again. Will terminate if rf_relation_is_difunctional reutrns 1 or an error
    return rf_relation_gain_difunctional(relation, fill);
}


int rf_relation_gain_equivalent(RF_RELATION* relation, bool fill)
{
    int res = rf_relation_is_equivalent(relation);
    if (res != 0)
        return res;

    rf_relation_gain_reflexive(relation);
    rf_relation_gain_symmetric(relation, fill);
    rf_relation_gain_transitive(relation, fill);


    //check again. Will terminate if rf_relation_is_equivalent reutrns 1 or an error
    return rf_relation_gain_equivalent(relation, fill);
}


int rf_relation_gain_poset(RF_RELATION* relation, bool fill)
{
    int res = rf_relation_is_poset(relation);
    if (res != 0)
        return res;

    rf_relation_gain_reflexive(relation);
    rf_relation_gain_antisymmetric(relation, fill);
    rf_relation_gain_transitive(relation, fill);


    //check again. Will terminate if rf_relation_is_poset reutrns 1 or an error
    return rf_relation_gain_poset(relation, fill);
}


int rf_relation_gain_preorder(RF_RELATION* relation, bool fill)
{
    int res = rf_relation_is_preorder(relation);
    if (res != 0)
        return res;

    rf_relation_gain_reflexive(relation);
    rf_relation_gain_transitive(relation, fill);


    //check again. Will terminate if rf_relation_is_preorder reutrns 1 or an error
    return rf_relation_gain_preorder(relation, fill);
}

int rf_relation_gain_asymmetric(RF_RELATION* relation, bool upper)
{
    int res = rf_relation_is_asymmetric(relation);
    if (res != 0)
        return res;

    int x, y, width, result_1, result_2;

    //loop the table and check if relation is asymmetric
    width = rf_table_get_width(relation->table);
    for (y = 0; y < width; y++)
        for (x = y; x < width; x++)
        {
            if (x == y) /* xRx */
            {
                result_1 = rf_table_get_bit(relation->table, x, y);
                if (result_1 == 1){ /* xRx exist -> not asymmetric */
                    rf_table_set_bit(relation->table, x, y, 0);
                }
                else if (result_1 == 0)
                    continue;
            }
            else
            {
                result_1 = rf_table_get_bit(relation->table, x, y);
                result_2 = rf_table_get_bit(relation->table, y, x);

                if (result_1 == result_2){ /* xRy and yRx exist -> not asymmetric */
                    if (upper){
                        rf_table_set_bit(relation->table, x, y, 0);
                        rf_table_set_bit(relation->table, y, x, 1);
                    }else{
                        rf_table_set_bit(relation->table, x, y, 1);
                        rf_table_set_bit(relation->table, y, x, 0);
                    }
                }
            }
        }

    //check again. Will terminate if rf_relation_is_asymmetric reutrns 1 or an error
    return rf_relation_gain_asymmetric(relation, upper);
}
