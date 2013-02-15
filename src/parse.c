/*
 * RelaFixLib, functions for the parser
 *
 * Copyright (C) Peter Berger, 2011
 */

#include "parse.h"
#include "tools.h"
#include "relation.h"
#include "formula.h"
#include "function.h"
#include "domain.h"
#include "operation.h"
#include "negation.h"
#include "table.h"
#include "array.h"
#include "list.h"
#include "parser.tab.h"
#include <stdlib.h>
#include <stdio.h>

/* globals for the parser */
enum	rf_lexer_mode rf_lexer_mode = RF_LEXER_NORMAL;
RF_BOOL				rf_parser_record = RF_FALSE;
RF_BOOL				rf_parser_with_variable = RF_FALSE;
static RF_LIST		*rf_parser_domains = 0;
static RF_LIST		*rf_parser_functions = 0;
static RF_LIST		*rf_parser_negations = 0;
static RF_LIST		*rf_parser_relations = 0;
static RF_LIST		*rf_parser_operations = 0;
static char			*rf_parser_path = 0;
char				*rf_parser_record_string = 0;
int					rf_parser_record_length = 0;
int					rf_parser_record_position = 0;



int rf_parser_add_domain(RF_DOMAIN *domain)
{
	if(!rf_parser_domains)
		rf_parser_domains = rf_list_create();
	if(!rf_parser_domains)
		return 1;

	if(!domain)
		return 2;

	rf_list_append(rf_parser_domains, domain);
	return 0;
}


int rf_parser_add_relation(RF_RELATION *relation)
{
	if(!rf_parser_relations)
		rf_parser_relations = rf_list_create();
	if(!rf_parser_relations)
		return 1;

	if(!relation)
		return 2;

	rf_list_append(rf_parser_relations, relation);
	return 0;
}

RF_PARSER_TABLE_ROW * rf_parser_create_table_row(char *name)
{
	RF_PARSER_TABLE_ROW *row = malloc(sizeof(RF_PARSER_TABLE_ROW));
	if(!row)
	{
		free(name);
		return 0;
	}

	row->element = name;
	row->values = rf_list_create();

	return row;
}

int rf_parser_add_function(RF_FUNCTION *function)
{
	if(!rf_parser_functions)
		rf_parser_functions = rf_list_create();
	if(!rf_parser_functions)
		return 1;

	if(!function)
		return 2;

	rf_list_append(rf_parser_functions, function);
	return 0;
}

int rf_parser_add_negation(RF_NEGATION *negation)
{
	if(!rf_parser_negations)
		rf_parser_negations = rf_list_create();
	if(!rf_parser_negations)
		return 1;

	if(!negation)
		return 2;

	rf_list_append(rf_parser_negations, negation);
	return 0;
}

int rf_parser_add_operation(RF_OPERATION *operation)
{
	if(!rf_parser_operations)
		rf_parser_operations = rf_list_create();
	if(!rf_parser_operations)
		return 1;

	if(!operation)
		return 2;

	rf_list_append(rf_parser_operations, operation);
	return 0;
}

void rf_parser_delete_domain(RF_DOMAIN *domain)
{
	RF_LIST_ITERATOR *iterator;
	RF_DOMAIN *tmp;

	if(!domain)
	{
		return;
	}

	rf_parser_show_string("Deleted following:\n------------------\n");

	/* if relations depend on the domain we have to delete them also */
	if(rf_parser_relations)
	{
		RF_RELATION *relation;
		iterator = rf_list_get_begin(rf_parser_relations);
		while(rf_list_has_next(iterator))
		{
			relation = rf_list_next(iterator);
			if(!relation)
				continue;

			if(rf_relation_get_domain_1(relation) == domain)
			{
				rf_parser_show_string(rf_relation_get_name(relation));
				rf_parser_show_string("\n");
				rf_parser_delete_relation(relation);
			}
			else if(rf_relation_get_domain_2(relation) == domain)
			{
				rf_parser_show_string(rf_relation_get_name(relation));
				rf_parser_show_string("\n");
				rf_parser_delete_relation(relation);
			}
		}
		rf_list_delete_iterator(iterator);
	}

	/* if operations depend on the domain we have to delete them also */
	if(rf_parser_operations)
	{
		RF_OPERATION *operation;
		iterator = rf_list_get_begin(rf_parser_operations);
		while(rf_list_has_next(iterator))
		{
			operation = rf_list_next(iterator);
			if(!operation)
				continue;

			if(rf_operation_get_domain_1(operation) == domain)
			{
				rf_parser_show_string(rf_operation_get_name(operation));
				rf_parser_show_string("\n");
				rf_parser_delete_operation(operation);
			}
			else if(rf_operation_get_domain_2(operation) == domain)
			{
				rf_parser_show_string(rf_operation_get_name(operation));
				rf_parser_show_string("\n");
				rf_parser_delete_operation(operation);
			}
			else if(rf_operation_get_domain_3(operation) == domain)
			{
				rf_parser_show_string(rf_operation_get_name(operation));
				rf_parser_show_string("\n");
				rf_parser_delete_operation(operation);
			}
		}
		rf_list_delete_iterator(iterator);
	}

	/* if negations depend on the domain we have to delete them also */
	if(rf_parser_negations)
	{
		RF_NEGATION *negation;
		iterator = rf_list_get_begin(rf_parser_negations);
		while(rf_list_has_next(iterator))
		{
			negation = rf_list_next(iterator);
			if(!negation)
				continue;

			if(rf_negation_get_domain(negation) == domain)
			{
				rf_parser_show_string(rf_negation_get_name(negation));
				rf_parser_show_string("\n");
				rf_parser_delete_negation(negation);
			}
		}
		rf_list_delete_iterator(iterator);
	}


	/* now we can delete the domain */
	if(rf_parser_domains)
	{
		iterator = rf_list_get_begin(rf_parser_domains);
		while(rf_list_has_next(iterator))
		{
			tmp = rf_list_next(iterator);

			if(tmp == domain)
			{
				rf_parser_show_string(rf_domain_get_name(domain));
				rf_parser_show_string("\n");
				rf_list_delete_item(iterator, (void (*)(void *))rf_domain_destroy);
			}
		}
		rf_list_delete_iterator(iterator);
	}

	rf_parser_show_string("\n");
}

void rf_parser_delete_relation(RF_RELATION *relation)
{
	RF_LIST_ITERATOR *iterator;
	RF_RELATION *tmp;

	if(!relation)
		return;

	if(!rf_parser_relations)
	{
		return;
	}

	iterator = rf_list_get_begin(rf_parser_relations);
	while(rf_list_has_next(iterator))
	{
		tmp = rf_list_next(iterator);

		if(tmp == relation)
			rf_list_delete_item(iterator, (void (*)(void *))rf_relation_destroy);
	}
	rf_list_delete_iterator(iterator);
}

void rf_parser_delete_operation(RF_OPERATION *operation)
{
	RF_LIST_ITERATOR *iterator;
	RF_OPERATION *tmp;

	if(!operation)
		return;

	if(!rf_parser_operations)
	{
		return;
	}

	iterator = rf_list_get_begin(rf_parser_operations);
	while(rf_list_has_next(iterator))
	{
		tmp = rf_list_next(iterator);

		if(tmp == operation)
			rf_list_delete_item(iterator, (void (*)(void *))rf_operation_destroy);
	}
	rf_list_delete_iterator(iterator);
}

void rf_parser_delete_negation(RF_NEGATION *negation)
{
	RF_LIST_ITERATOR *iterator;
	RF_NEGATION *tmp;

	if(!negation)
		return;

	if(!rf_parser_negations)
	{
		return;
	}

	iterator = rf_list_get_begin(rf_parser_negations);
	while(rf_list_has_next(iterator))
	{
		tmp = rf_list_next(iterator);

		if(tmp == negation)
			rf_list_delete_item(iterator, (void (*)(void *))rf_negation_destroy);
	}
	rf_list_delete_iterator(iterator);
}

void rf_parser_destroy_table_data(RF_PARSER_TABLE_DATA *table_data)
{
	if(!table_data)
		return;

	if(table_data->type == RF_PTD_RELATION)
	{
		if(table_data->relation)
			rf_relation_destroy(table_data->relation);
	}
	else if(table_data->type == RF_PTD_DATA)
	{
		rf_list_destroy(table_data->columns, free);
		rf_list_destroy(table_data->rows, (void (*)(void *))rf_parser_destroy_table_row);
	}
	else if(table_data->type == RF_PTD_FORMULA)
	{
		rf_formula_destroy(table_data->formula);
	}

	free(table_data);
}

void rf_parser_destroy_table_row(RF_PARSER_TABLE_ROW *row)
{
	if(!row)
		return;

	free(row->element);
	rf_list_destroy(row->values, free);

	free(row);
}

RF_DOMAIN * rf_parser_get_domain(char *name)
{
	RF_LIST_ITERATOR *iterator;
	RF_DOMAIN *domain;

	if(!name || !rf_parser_domains)
		return 0;

	iterator = rf_list_get_begin(rf_parser_domains);
	while(rf_list_has_next(iterator))
	{
		domain = rf_list_next(iterator);
		if(rf_domain_has_name(domain, name))
		{
			rf_list_delete_iterator(iterator);
			return domain;
		}
	}

	rf_list_delete_iterator(iterator);
	return 0;
}

RF_FUNCTION * rf_parser_get_function(char *name)
{
	RF_LIST_ITERATOR *iterator;
	RF_FUNCTION *function;

	if(!name || !rf_parser_functions)
		return 0;

	iterator = rf_list_get_begin(rf_parser_functions);
	while(rf_list_has_next(iterator))
	{
		function = rf_list_next(iterator);
		if(rf_function_has_name(function, name))
		{
			rf_list_delete_iterator(iterator);
			return function;
		}
	}

	rf_list_delete_iterator(iterator);
	return 0;
}

RF_NEGATION * rf_parser_get_negation(char *name)
{
	RF_LIST_ITERATOR *iterator;
	RF_NEGATION *negation;

	if(!name || !rf_parser_negations)
		return 0;

	iterator = rf_list_get_begin(rf_parser_negations);
	while(rf_list_has_next(iterator))
	{
		negation = rf_list_next(iterator);
		if(rf_negation_has_name(negation, name))
		{
			rf_list_delete_iterator(iterator);
			return negation;
		}
	}

	rf_list_delete_iterator(iterator);
	return 0;
}

RF_RELATION * rf_parser_get_relation(char *name)
{
	RF_LIST_ITERATOR *iterator;
	RF_RELATION *relation;

	if(!name || !rf_parser_relations)
		return 0;

	iterator = rf_list_get_begin(rf_parser_relations);
	while(rf_list_has_next(iterator))
	{
		relation = rf_list_next(iterator);
		if(rf_relation_has_name(relation, name))
		{
			rf_list_delete_iterator(iterator);
			return relation;
		}
	}

	rf_list_delete_iterator(iterator);
	return 0;
}

RF_OPERATION * rf_parser_get_operation(char *name)
{
	RF_LIST_ITERATOR *iterator;
	RF_OPERATION *operation;

	if(!name || !rf_parser_operations)
		return 0;

	iterator = rf_list_get_begin(rf_parser_operations);
	while(rf_list_has_next(iterator))
	{
		operation = rf_list_next(iterator);
		if(rf_operation_has_name(operation, name))
		{
			rf_list_delete_iterator(iterator);
			return operation;
		}
	}

	rf_list_delete_iterator(iterator);
	return 0;
}

char * rf_parser_get_path()
{
	return rf_parser_path;
}

void rf_parser_list_domains()
{
	RF_LIST_ITERATOR *domains;
	RF_DOMAIN *domain;

	if(!rf_parser_domains)
	{
		rf_parser_show_string("no domain exist\n\n");
		return;
	}

	domains = rf_list_get_begin(rf_parser_domains);
	while(rf_list_has_next(domains))
	{
		domain = rf_list_next(domains);
		if(!domain)
			continue;

		rf_parser_show_string(rf_domain_get_name(domain));
		rf_parser_show_string("\n");
	}

	rf_parser_show_string("\n");
	rf_list_delete_iterator(domains);
}

void rf_parser_list_relations()
{
	RF_LIST_ITERATOR *relations;
	RF_RELATION *relation;

	if(!rf_parser_relations)
	{
		rf_parser_show_string("no relation exist\n\n");
		return;
	}

	relations = rf_list_get_begin(rf_parser_relations);
	while(rf_list_has_next(relations))
	{
		relation = rf_list_next(relations);
		if(!relation)
			continue;

		rf_parser_show_string(rf_relation_get_name(relation));
		rf_parser_show_string("\n");
	}

	rf_parser_show_string("\n");
	rf_list_delete_iterator(relations);
}

void rf_parser_list_operations()
{
	RF_LIST_ITERATOR *operations;
	RF_OPERATION *operation;

	if(!rf_parser_operations)
	{
		rf_parser_show_string("no operation exist\n\n");
		return;
	}

	operations = rf_list_get_begin(rf_parser_operations);
	while(rf_list_has_next(operations))
	{
		operation = rf_list_next(operations);
		if(!operation)
			continue;

		rf_parser_show_string(rf_operation_get_name(operation));
		rf_parser_show_string("\n");
	}

	rf_parser_show_string("\n");
	rf_list_delete_iterator(operations);
}

void rf_parser_list_negations()
{
	RF_LIST_ITERATOR *negations;
	RF_NEGATION *negation;

	if(!rf_parser_negations)
	{
		rf_parser_show_string("no negation exist\n\n");
		return;
	}

	negations = rf_list_get_begin(rf_parser_negations);
	while(rf_list_has_next(negations))
	{
		negation = rf_list_next(negations);
		if(!negation)
			continue;

		rf_parser_show_string(rf_negation_get_name(negation));
		rf_parser_show_string("\n");
	}

	rf_parser_show_string("\n");
	rf_list_delete_iterator(negations);
}

void rf_parser_list_functions()
{
	RF_LIST_ITERATOR *functions;
	RF_FUNCTION *function;

	if(!rf_parser_functions)
	{
		rf_parser_show_string("no function exist\n\n");
		return;
	}

	functions = rf_list_get_begin(rf_parser_functions);
	while(rf_list_has_next(functions))
	{
		function = rf_list_next(functions);
		if(!function)
			continue;

		rf_parser_show_string(rf_function_get_name(function));
		rf_parser_show_string("\n");
	}

	rf_parser_show_string("\n");
	rf_list_delete_iterator(functions);
}

/*! \brief records the input. This is used to print the expression after the calculation */
void rf_parser_record_append(char c)
{
	if(!rf_parser_record_string)
	{
		rf_parser_record_length = 20;
		rf_parser_record_position = 0;
		rf_parser_record_string = calloc(rf_parser_record_length, sizeof(char));
		if(!rf_parser_record_string)
			return;
	}

	if(rf_parser_record_position >= rf_parser_record_length - 1)
	{
		rf_parser_record_length *= 2;
		rf_parser_record_string = realloc(rf_parser_record_string, rf_parser_record_length);
		if(!rf_parser_record_string)
			return;
	}

	if((c == '\t' || c == ' ') && rf_parser_record_position == 0)
		return;
	if(c == ';')
		return;

	rf_parser_record_string[rf_parser_record_position++] = c;
	rf_parser_record_string[rf_parser_record_position] = '\0';
}

void rf_parser_record_clear()
{
	rf_parser_record_position = 0;

	if(rf_parser_record_string)
		rf_parser_record_string[0] = '\0';
}

char * rf_parser_record_read()
{
	return rf_parser_record_string;
}


void yyerror(char const *str)
{
	rf_parser_error(yylloc.first_line, yylloc.first_column, str, 0);
}

RF_LIST * rf_parser_table_row_get_list(RF_PARSER_TABLE_ROW *row)
{
	if(!row)
		return 0;

	return row->values;
}

void rf_parser_register_system_functions()
{
	RF_FUNCTION *function;
	function = rf_function_create(rf_string_copy("is_reflexive"), rf_relation_is_reflexive, 1);
	if(function)
	{
		rf_function_set_description(function,
			"is_reflexive(Relation): Boolean\n\n"
			"Takes a relation and returns an element of domain boolean.\n"
			"Works only with homogeneous relations!\n"
			"A relation is reflexiv if for all x element of domain,  xRx is true");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function is_reflexive to parser", 0);

	function = rf_function_create(rf_string_copy("is_irreflexive"), rf_relation_is_irreflexive, 1);
	if(function)
	{
		rf_function_set_description(function,
			"is_irreflexive(Relation): Boolean\n\n"
			"Takes a relation and tests if this relation is irreflexive. For all x in R there exist no xRx.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function is_irreflexive to parser", 0);

	function = rf_function_create(rf_string_copy("is_symmetric"), rf_relation_is_symmetric, 1);
	if(function)
	{
		rf_function_set_description(function,
			"is_symmetric(Relation): Boolean\n\n"
			"Takes a relation and tests if this relation is symmetric. For all x,y in R, xRy -> yRx");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function is_symmetric to parser", 0);

	function = rf_function_create(rf_string_copy("is_transitive"), rf_relation_is_transitive, 1);
	if(function)
	{
		rf_function_set_description(function,
			"is_transitive(Relation): Boolean\n\n"
			"Takes a relation and tests if this relation is transitive. For all x,y,z in R, xRy and yRz -> xRz");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function is_transitive to parser", 0);

	function = rf_function_create(rf_string_copy("is_antisymmetric"), rf_relation_is_antisymmetric, 1);
	if(function)
	{
		rf_function_set_description(function,
			"is_antisymmetric(Relation): Boolean\n\n"
			"Takes a relation and tests if this relation is antisymmetric. For all x,y in R, xRy and yRx -> x = y");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function is_antisymmetric to parser", 0);

	function = rf_function_create(rf_string_copy("is_asymmetric"), rf_relation_is_asymmetric, 1);
	if(function)
	{
		rf_function_set_description(function,
			"is_asymmetric(Relation): Boolean\n\n"
			"Takes a relation and tests if this relation is asymmetric. For all x,y in R, xRy -> not yRx");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function is_asymmetric to parser", 0);

	function = rf_function_create(rf_string_copy("is_equivalent"), rf_relation_is_equivalent, 1);
	if(function)
	{
		rf_function_set_description(function,
			"is_equivalent(Relation): Boolean\n\n"
			"Takes a relation and tests if this relation is equivalent. A relation is equivalent "
			"if the relation is reflexive, symmetric and transitive.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function is_equivalent to parser", 0);

	function = rf_function_create(rf_string_copy("is_difunctional"), rf_relation_is_difunctional, 1);
	if(function)
	{
		rf_function_set_description(function,
			"is_difunctional(Relation): Boolean\n\n"
			"Takes a relation and tests if this relation is difunctional. For all w,x,y,z in R, xRy and zRy and zRw -> xRw");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function is_difunctional to parser", 0);

	function = rf_function_create(rf_string_copy("is_preorder"), rf_relation_is_preorder, 1);
	if(function)
	{
		rf_function_set_description(function,
			"is_preorder(Relation): Boolean\n\n"
			"Takes a relation and tests if this relation is preorder. A relation is preorder if the relation "
			"is reflexive and transitive.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function is_preorder to parser", 0);

	function = rf_function_create(rf_string_copy("is_poset"), rf_relation_is_poset, 1);
	if(function)
	{
		rf_function_set_description(function,
			"is_poset(Relation): Boolean\n\n"
			"Takes a relation and tests if this relation is a poset. A relation is a poset if it is"
			"reflexive, antisymmetric and transitive.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function is_poset to parser", 0);

	function = rf_function_create(rf_string_copy("complement"), rf_relation_create_complement, 1);
	if(function)
	{
		rf_function_set_description(function,
			"complement(Relation): Relation\n\n"
			"Takes a relation and returns a new relation. The new relation is the complement of the given.\n\n"
			"The complement relation S is for given relation R:  xSy when not xRy");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function complement to parser", 0);

	function = rf_function_create(rf_string_copy("converse"), rf_relation_create_converse, 1);
	if(function)
	{
		rf_function_set_description(function,
			"converse(Relation): Relation\n\n"
			"Takes a relation and returns a new relation. The new relation is the converse of the given.\n\n"
			"The converse relation S is for given relation R:  if xRy then ySx");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function converse to parser", 0);

	function = rf_function_create(rf_string_copy("union"), rf_relation_create_union, 2);
	if(function)
	{
		rf_function_set_description(function,
			"union(Relation, Relation): Relation\n\n"
			"Takes two relations and returns a new relation. The new relation is the union of the given ones. (A or B)");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function union to parser", 0);

	function = rf_function_create(rf_string_copy("intersection"), rf_relation_create_intersection, 2);
	if(function)
	{
		rf_function_set_description(function,
			"intersection(Relation, Relation): Relation\n\n"
			"Takes two relations and returns a new relation. The new relation is the intersection of the given ones. (A and B)");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function intersection to parser", 0);

	function = rf_function_create(rf_string_copy("generate_meet"), rf_operation_create_meet, 1);
	if(function)
	{
		rf_function_set_description(function,
			"generate_meet(Relation): Operation\n\n"
			"Takes a relation and returns a new operation.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function intersection to parser", 0);

	function = rf_function_create(rf_string_copy("generate_join"), rf_operation_create_join, 1);
	if(function)
	{
		rf_function_set_description(function,
			"generate_join(Relation): Operation\n\n"
			"Takes a relation and returns a new operation.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function intersection to parser", 0);

	function = rf_function_create(rf_string_copy("id"), rf_relation_create_id, 1);
	if(function)
	{
		rf_function_set_description(function,
			"id(Domain): Relation\n\n"
			"Takes a domain and returns a new relation. In the relation all pairs xRx exist.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function id to parser", 0);

	function = rf_function_create(rf_string_copy("empty"), rf_relation_create_empty, 2);
	if(function)
	{
		rf_function_set_description(function,
			"empty(Domain, Domain): Relation\n\n"
			"Takes two domains and returns a new empty relation. In xRy x is from the first domain and y from the second.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function empty to parser", 0);

	function = rf_function_create(rf_string_copy("full"), rf_relation_create_full, 2);
	if(function)
	{
		rf_function_set_description(function,
			"full(Domain, Domain): Relation\n\n"
			"Takes two domains and returns a new relation. All xRy exist.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function full to parser", 0);

	function = rf_function_create(rf_string_copy("concat"), rf_relation_create_concatenation, 2);
	if(function)
	{
		rf_function_set_description(function,
			"concat(Relation, Relation): Relation\n\n"
			"Takes two relations and returns a new relation which is the concatenation of the given ones."
			"S = {<x, z> | x -> A, z -> C, it exists y -> B: xRy and yRz}");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function concat to parser", 0);

	function = rf_function_create(rf_string_copy("max"), rf_relation_find_maximum, 1);
	if(function)
	{
		rf_function_set_description(function,
			"max(Relation): Element\n\n"
			"Takes one ordered relation and returns the maximum. If no maximum exists, an error will appear");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function max to parser", 0);

	function = rf_function_create(rf_string_copy("min"), rf_relation_find_minimum, 1);
	if(function)
	{
		rf_function_set_description(function,
			"min(Relation): Element\n\n"
			"Takes one ordered relation and returns the minimum. If no minimum exists, an error will appear");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function min to parser", 0);

	function = rf_function_create(rf_string_copy("upperbound"), rf_relation_find_upperbound, 2);
	if(function)
	{
		rf_function_set_description(function,
			"upperbound(Relation, Domain): Domain\n\n"
			"Takes one ordered relation and a Domain with elements that must exist in the relation! "
			"Returns a new temporary domain containing all upperbounds of the given elements.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function upperbound to parser", 0);

	function = rf_function_create(rf_string_copy("lowerbound"), rf_relation_find_lowerbound, 2);
	if(function)
	{
		rf_function_set_description(function,
			"lowerbound(Relation, Domain): Domain\n\n"
			"Takes one ordered relation and a Domain with elements that must exist in the relation! "
			"Returns a new temporary domain containing all lowerbounds of the given elements.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function lowerbound to parser", 0);

	function = rf_function_create(rf_string_copy("supremum"), rf_relation_find_supremum, 2);
	if(function)
	{
		rf_function_set_description(function,
			"supremum(Relation, Domain): Element\n\n"
			"Takes one ordered relation and a Domain with elements that must exist in the relation! "
			"Returns an element that is the supremum of the given ones. If no supremum exists, \"no supremum\" will be returned.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function supremum to parser", 0);

	function = rf_function_create(rf_string_copy("infimum"), rf_relation_find_infimum, 2);
	if(function)
	{
		rf_function_set_description(function,
			"infimum(Relation, Domain): Element\n\n"
			"Takes one ordered relation and a Domain with elements that must exist in the relation! "
			"Returns an element that is the infimum of the given ones. If no infimum exists, \"no infimum\" will be returned.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function infimum to parser", 0);

	function = rf_function_create(rf_string_copy("top"), rf_relation_create_top, 1);
	if(function)
	{
		rf_function_set_description(function,
			"top(Domain): Relation\n\n"
			"Takes one domain and returns a Relation whoes top triangle is filled.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function top to parser", 0);

	function = rf_function_create(rf_string_copy("bottom"), rf_relation_create_bottom, 1);
	if(function)
	{
		rf_function_set_description(function,
			"bottom(Domain): Relation\n\n"
			"Takes one domain and returns a Relation whoes bottom triangle is filled.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function bottom to parser", 0);

	function = rf_function_create(rf_string_copy("pow"), rf_domain_create_powerset, 1);
	if(function)
	{
		rf_function_set_description(function,
			"pow(Domain): Domain\n\n"
			"Takes one domain and returns a new Domain based on the elements of the given one.");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function pow to parser", 0);

	function = rf_function_create(rf_string_copy("subsetleq"), rf_relation_create_subsetleq, 1);
	if(function)
	{
		rf_function_set_description(function,
			"subsetleq(Domain): Relation\n\n"
			"Takes one domain and returns a new Relation. All subsets stand in relation to there superset");
		rf_parser_add_function(function);
	}
	else
		rf_parser_error(0, 0, "could not add function subsetleq to parser", 0);
}

void rf_parser_set_location(RF_LOCATION *location)
{
	if(!location)
		return;

	/* yylloc is a bison internal ! */
	yylloc.first_line = location->first_line;
	yylloc.last_line = location->last_line;
	yylloc.first_column = location->first_column;
	yylloc.last_column = location->last_column;
}

void rf_parser_set_path(char *path)
{
	if(rf_parser_path)
		free(rf_parser_path);

	rf_parser_path = path;
}


