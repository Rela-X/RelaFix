/*
 * RelaFixLib    Formula
 *
 * Copyright (C) Peter Berger, 2011
 */

#include "domain.h"
#include "formula.h"
#include "function.h"
#include "relation.h"
#include "negation.h"
#include "operation.h"
#include "tools.h"
#include "parse.h"
#include "array.h"
#include <stdlib.h>
#include <string.h>


/*!
Appends an argument (RF_FORMULA) to a given formula node.

@relates RF_FORMULA
@param[in] formula The formula that takes the arguments.
@param[in] argument The argument that should get appended.
*/
void rf_formula_append_argument(RF_FORMULA *formula, RF_FORMULA *argument)
{
	if(!formula || !argument)
		return;

	if(!formula->arguments)
		formula->arguments = rf_array_create();

	rf_array_append(formula->arguments, argument);
}


/*!
Calculates an formula tree.

The result is stored in RF_FORMULA_RESULT. If an error did appear, the type is set to RF_FO_ERROR.
The result will contain an errormessage and the location of the error in the sourcecode.
@relates RF_FORMULA
@param[in] formula The root of the formula tree.
@param[in] element_1 If the formula contains a variable X, X will be replaced by element_1.
@param[in] element_2 If the formula contains a variable Y, Y will be replaced by element_2.
@param[out] result A null-pointer! The result will be written here. The result must be freed by a call to rf_formula_destroy_result() by the caller!
@return 0 on success. The result of the calculation is written to result.
@return 1 on error.
*/
int rf_formula_calc(RF_FORMULA *formula, char *element_1, char *element_2, RF_FORMULA_RESULT **result)
{
	/* Used variables. For some unknown reason MSC2010 didnt allow them in the if block. But others work... */
	RF_FORMULA_RESULT *tmp, *tmp_2;
	int (*func)(RF_RELATION *relation);
	RF_RELATION * (*rel_func)(RF_RELATION *relation, RF_ERROR *error);
	RF_RELATION * (*rel_func_2)(RF_DOMAIN *domain, RF_ERROR *error);
	RF_RELATION * (*rel_func_3)(RF_DOMAIN *domain_1, RF_DOMAIN *domain_2, RF_ERROR *error);
	RF_OPERATION * (*op_func)(RF_RELATION *relation, RF_ERROR *error);
	char *el_1, *el_2, *out;
	int res;

	if(!result) /* error */
		return 1;
	else if(!formula) /* error */
	{
		*result = rf_formula_calc_error(0, rf_string_copy("program error - no formula"));
		return 1;
	}

	/* Test the type of formula and calculate it */
	if(formula->type == RF_FO_ELEMENT)
	{
		*result = rf_formula_create_result();
		(*result)->type = RF_FO_ID;
		(*result)->element = formula->element;
		(*result)->is_temporary = RF_TRUE;
		(*result)->can_delete = RF_FALSE;
		return 0;
	}

	else if(formula->type == RF_FO_DOMAIN)
	{
		*result = rf_formula_create_result();
		(*result)->type = RF_FO_DOMAIN;
		(*result)->domain = formula->domain;
		(*result)->is_temporary = RF_TRUE;
		(*result)->can_delete = RF_FALSE;
		return 0;
	}

	else if(formula->type == RF_FO_FUNCTION)
	{
		/*
		Test the function and calculate it.
		Here the relafix-function is bound to the c-function
		*/
		if(rf_function_has_name(formula->function, "is_reflexive") ||
			rf_function_has_name(formula->function, "is_irreflexive") ||
			rf_function_has_name(formula->function, "is_symmetric") ||
			rf_function_has_name(formula->function, "is_transitive") ||
			rf_function_has_name(formula->function, "is_antisymmetric") ||
			rf_function_has_name(formula->function, "is_asymmetric") ||
			rf_function_has_name(formula->function, "is_equivalent") ||
			rf_function_has_name(formula->function, "is_difunctional") ||
			rf_function_has_name(formula->function, "is_preorder") ||
			rf_function_has_name(formula->function, "is_poset"))
		{
			RF_RELATION *relation = 0;

			/* test argument count */
			if(rf_array_size(formula->arguments) != 1)
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "wrong argument count for function '",
					rf_function_get_name(formula->function), "'"));
				return 1;
			}


			/* get relation */
			rf_formula_calc(rf_array_read(formula->arguments, 0), element_1, element_2, &tmp);
			if(tmp->type == RF_FO_RELATION)
			{
				relation = tmp->relation;
			}
			else if(tmp->type == RF_FO_ELEMENT || tmp->type == RF_FO_ID)
			{
				relation = rf_parser_get_relation(tmp->element);
			}
			else if(tmp->type == RF_FO_ERROR)
			{
				*result = tmp;
				return 1;
			}

			if(!relation) /* error */
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "argument for function '",
					rf_function_get_name(formula->function), "' is not a RELATION"));
				return 1;
			}

			func = rf_function_get_func(formula->function);
			if(!func)
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "program error - function '",
					rf_function_get_name(formula->function), "' has no func"));
				return 1;
			}

			res = func(relation);
			if(res != 0 && res != 1) /* error */
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "program error while calculating function '",
					rf_function_get_name(formula->function), "'"));
				return 1;
			}

			*result = rf_formula_create_result();
			(*result)->type = RF_FO_ELEMENT;
			(*result)->element = res ? "1" : "0";
			(*result)->is_temporary = RF_FALSE;
			(*result)->can_delete = RF_FALSE;

			rf_formula_destroy_result(tmp);
			return 0;
		}
		else if(rf_function_has_name(formula->function, "max") ||
				rf_function_has_name(formula->function, "min"))
		{
			RF_ERROR error;
			char * (*rfunc)(RF_RELATION *relation, RF_ERROR *error);
			RF_RELATION *relation = 0;

			/* test argument count */
			if(rf_array_size(formula->arguments) != 1)
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "wrong argument count for function '",
					rf_function_get_name(formula->function), "'"));
				return 1;
			}


			/* get relation*/
			rf_formula_calc(rf_array_read(formula->arguments, 0), element_1, element_2, &tmp);
			if(tmp->type == RF_FO_RELATION)
			{
				relation = tmp->relation;
			}
			else if(tmp->type == RF_FO_ELEMENT || tmp->type == RF_FO_ID)
			{
				relation = rf_parser_get_relation(tmp->element);
			}
			else if(tmp->type == RF_FO_ERROR)
			{
				*result = tmp;
				return 1;
			}


			if(!relation) /* error */
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "first argument for function '",
					rf_function_get_name(formula->function), "' is not a RELATION"));
				return 1;
			}

			rfunc = rf_function_get_func(formula->function);
			if(!rfunc) /* error */
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "program error - function '",
					rf_function_get_name(formula->function), "' has no func"));
				return 1;
			}

			*result = rf_formula_create_result();
			(*result)->type = RF_FO_ELEMENT;
			(*result)->element = rfunc(relation, &error);
			(*result)->is_temporary = RF_FALSE;
			(*result)->can_delete = RF_FALSE;

			if(!(*result)->element) /* error */
			{
				rf_formula_destroy_result(*result);
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(4, "In function '", rf_function_get_name(formula->function),
						"' : ", error.string));
				if(error.string)
					free(error.string);
				return 1;
			}

			rf_formula_destroy_result(tmp);
			return 0;
		}
		else if(rf_function_has_name(formula->function, "complement") ||
				rf_function_has_name(formula->function, "converse") ||
				rf_function_has_name(formula->function, "concatenate"))
		{
			RF_ERROR error;
			RF_RELATION *relation = 0;

			/* test argument count */
			if(rf_array_size(formula->arguments) != 1)
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "wrong argument count for function '",
					rf_function_get_name(formula->function), "'"));
				return 1;
			}


			/* get relation */
			rf_formula_calc(rf_array_read(formula->arguments, 0), element_1, element_2, &tmp);
			if(tmp->type == RF_FO_RELATION)
			{
				relation = tmp->relation;
			}
			else if(tmp->type == RF_FO_ELEMENT || tmp->type == RF_FO_ID)
			{
				relation = rf_parser_get_relation(tmp->element);
			}
			else if(tmp->type == RF_FO_ERROR)
			{
				*result = tmp;
				return 1;
			}

			if(!relation) /* error */
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "argument for function '",
					rf_function_get_name(formula->function), "' is not a RELATION"));
				return 1;
			}

			rel_func = rf_function_get_func(formula->function);
			if(!rel_func) /* error */
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "program error - function '",
					rf_function_get_name(formula->function), "' has no func"));
				return 1;
			}

			*result = rf_formula_create_result();
			(*result)->type = RF_FO_RELATION;
			(*result)->relation = rel_func(relation, &error);
			(*result)->is_temporary = RF_TRUE;
			(*result)->can_delete = RF_TRUE;

			if(!(*result)->relation) /* error */
			{
				rf_formula_destroy_result(*result);
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(4, "In function '", rf_function_get_name(formula->function),
						"' : ", error.string));
				if(error.string)
					free(error.string);
				return 1;
			}

			rf_formula_destroy_result(tmp);
			return 0;
		}
		else if(rf_function_has_name(formula->function, "union") ||
				rf_function_has_name(formula->function, "intersection") ||
				rf_function_has_name(formula->function, "concat"))
		{
			RF_ERROR error;
			RF_RELATION * (*rfunc)(RF_RELATION *relation_1, RF_RELATION *relation_2, RF_ERROR *error);
			RF_RELATION *relation_1 = 0, *relation_2 = 0;

			/* test argument count */
			if(rf_array_size(formula->arguments) != 2)
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "wrong argument count for function '",
					rf_function_get_name(formula->function), "'"));
				return 1;
			}


			/* get relation 1 */
			rf_formula_calc(rf_array_read(formula->arguments, 0), element_1, element_2, &tmp);
			if(tmp->type == RF_FO_RELATION)
			{
				relation_1 = tmp->relation;
			}
			else if(tmp->type == RF_FO_ELEMENT || tmp->type == RF_FO_ID)
			{
				relation_1 = rf_parser_get_relation(tmp->element);
			}
			else if(tmp->type == RF_FO_ERROR)
			{
				*result = tmp;
				return 1;
			}

			/* get relation 2 */
			rf_formula_calc(rf_array_read(formula->arguments, 1), element_1, element_2, &tmp_2);
			if(tmp_2->type == RF_FO_RELATION)
			{
				relation_2 = tmp_2->relation;
			}
			else if(tmp_2->type == RF_FO_ELEMENT || tmp_2->type == RF_FO_ID)
			{
				relation_2 = rf_parser_get_relation(tmp_2->element);
			}
			else if(tmp_2->type == RF_FO_ERROR)
			{
				*result = tmp_2;
				return 1;
			}

			if(!relation_1 || !relation_2) /* error */
			{
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(4, relation_1 ? "second " : "first ", "argument for function '",
					rf_function_get_name(formula->function), "' is not a RELATION"));
				return 1;
			}

			rfunc = rf_function_get_func(formula->function);
			if(!rfunc) /* error */
			{
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "program error - function '",
					rf_function_get_name(formula->function), "' has no func"));
				return 1;
			}

			*result = rf_formula_create_result();
			(*result)->type = RF_FO_RELATION;
			(*result)->relation = rfunc(relation_1, relation_2, &error);
			(*result)->is_temporary = RF_TRUE;
			(*result)->can_delete = RF_TRUE;

			if(!(*result)->relation) /* error */
			{
				rf_formula_destroy_result(*result);
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(4, "In function '", rf_function_get_name(formula->function),
						"' : ", error.string));
				if(error.string)
					free(error.string);
				return 1;
			}

			rf_formula_destroy_result(tmp);
			rf_formula_destroy_result(tmp_2);
			return 0;
		}
		else if(rf_function_has_name(formula->function, "upperbound") ||
				rf_function_has_name(formula->function, "lowerbound"))
		{
			RF_ERROR error;
			RF_DOMAIN * (*rfunc)(RF_RELATION *relation, RF_DOMAIN *domain, RF_ERROR *error);
			RF_RELATION *relation = 0;
			RF_DOMAIN *domain = 0;

			/* test argument count */
			if(rf_array_size(formula->arguments) != 2)
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "wrong argument count for function '",
					rf_function_get_name(formula->function), "'"));
				return 1;
			}


			/* get relation */
			rf_formula_calc(rf_array_read(formula->arguments, 0), element_1, element_2, &tmp);
			if(tmp->type == RF_FO_RELATION)
			{
				relation = tmp->relation;
			}
			else if(tmp->type == RF_FO_ELEMENT || tmp->type == RF_FO_ID)
			{
				relation = rf_parser_get_relation(tmp->element);
			}
			else if(tmp->type == RF_FO_ERROR)
			{
				*result = tmp;
				return 1;
			}

			/* get domain */
			rf_formula_calc(rf_array_read(formula->arguments, 1), element_1, element_2, &tmp_2);
			if(tmp_2->type == RF_FO_DOMAIN)
			{
				domain = tmp_2->domain;
			}
			else if(tmp_2->type == RF_FO_ELEMENT || tmp_2->type == RF_FO_ID)
			{
				domain = rf_parser_get_domain(tmp_2->element);
			}
			else if(tmp_2->type == RF_FO_ERROR)
			{
				*result = tmp_2;
				return 1;
			}

			if(!relation) /* error */
			{
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "first argument for function '",
					rf_function_get_name(formula->function), "' is not a RELATION"));
				return 1;
			}

			if(!domain) /* error */
			{
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "second argument for function '",
					rf_function_get_name(formula->function), "' is not a DOMAIN"));
				return 1;
			}

			rfunc = rf_function_get_func(formula->function);
			if(!rfunc) /* error */
			{
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "program error - function '",
					rf_function_get_name(formula->function), "' has no func"));
				return 1;
			}

			*result = rf_formula_create_result();
			(*result)->type = RF_FO_DOMAIN;
			(*result)->domain = rfunc(relation, domain, &error);
			(*result)->is_temporary = RF_TRUE;
			(*result)->can_delete = RF_TRUE;

			if(!(*result)->domain) /* error */
			{
				rf_formula_destroy_result(*result);
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(4, "In function '", rf_function_get_name(formula->function),
						"' : ", error.string));
				if(error.string)
					free(error.string);
				return 1;
			}

			rf_formula_destroy_result(tmp);
			rf_formula_destroy_result(tmp_2);
			return 0;
		}
		else if(rf_function_has_name(formula->function, "supremum") ||
				rf_function_has_name(formula->function, "infimum"))
		{
			RF_ERROR error;
			char * (*rfunc)(RF_RELATION *relation, RF_DOMAIN *domain, RF_ERROR *error);
			RF_RELATION *relation = 0;
			RF_DOMAIN *domain = 0;

			/* test argument count */
			if(rf_array_size(formula->arguments) != 2)
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "wrong argument count for function '",
					rf_function_get_name(formula->function), "'"));
				return 1;
			}


			/* get relation */
			rf_formula_calc(rf_array_read(formula->arguments, 0), element_1, element_2, &tmp);
			if(tmp->type == RF_FO_RELATION)
			{
				relation = tmp->relation;
			}
			else if(tmp->type == RF_FO_ELEMENT || tmp->type == RF_FO_ID)
			{
				relation = rf_parser_get_relation(tmp->element);
			}
			else if(tmp->type == RF_FO_ERROR)
			{
				*result = tmp;
				return 1;
			}

			/* get domain */
			rf_formula_calc(rf_array_read(formula->arguments, 1), element_1, element_2, &tmp_2);
			if(tmp_2->type == RF_FO_DOMAIN)
			{
				domain = tmp_2->domain;
			}
			else if(tmp_2->type == RF_FO_ELEMENT || tmp_2->type == RF_FO_ID)
			{
				domain = rf_parser_get_domain(tmp_2->element);
			}
			else if(tmp_2->type == RF_FO_ERROR)
			{
				*result = tmp_2;
				return 1;
			}

			if(!relation) /* error */
			{
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "first argument for function '",
					rf_function_get_name(formula->function), "' is not a RELATION"));
				return 1;
			}

			if(!domain) /* error */
			{
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "second argument for function '",
					rf_function_get_name(formula->function), "' is not a DOMAIN"));
				return 1;
			}

			rfunc = rf_function_get_func(formula->function);
			if(!rfunc) /* error */
			{
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "program error - function '",
					rf_function_get_name(formula->function), "' has no func"));
				return 1;
			}

			*result = rf_formula_create_result();
			(*result)->type = RF_FO_ELEMENT;
			(*result)->element = rfunc(relation, domain, &error);
			(*result)->is_temporary = RF_FALSE;
			(*result)->can_delete = RF_FALSE;

			if(!(*result)->element) /* error */
			{
				rf_formula_destroy_result(*result);
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(4, "In function '", rf_function_get_name(formula->function),
						"' : ", error.string));
				if(error.string)
					free(error.string);
				return 1;
			}

			rf_formula_destroy_result(tmp);
			rf_formula_destroy_result(tmp_2);
			return 0;
		}
		else if(rf_function_has_name(formula->function, "generate_join") ||
				rf_function_has_name(formula->function, "generate_meet"))
		{
			RF_ERROR error;
			RF_RELATION *relation = 0;

			/* test argument count */
			if(rf_array_size(formula->arguments) != 1)
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "wrong argument count for function '",
					rf_function_get_name(formula->function), "'"));
				return 1;
			}


			/* get relation */
			rf_formula_calc(rf_array_read(formula->arguments, 0), element_1, element_2, &tmp);
			if(tmp->type == RF_FO_RELATION)
			{
				relation = tmp->relation;
			}
			else if(tmp->type == RF_FO_ELEMENT || tmp->type == RF_FO_ID)
			{
				relation = rf_parser_get_relation(tmp->element);
			}
			else if(tmp->type == RF_FO_ERROR)
			{
				*result = tmp;
				return 1;
			}

			if(!relation) /* error */
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "argument for function '",
					rf_function_get_name(formula->function), "' is not a RELATION"));
				return 1;
			}

			op_func = rf_function_get_func(formula->function);
			if(!op_func) /* error */
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "program error - function '",
					rf_function_get_name(formula->function), "' has no func"));
				return 1;
			}

			*result = rf_formula_create_result();
			(*result)->type = RF_FO_OPERATION;
			(*result)->operation = op_func(relation, &error);
			(*result)->is_temporary = RF_TRUE;
			(*result)->can_delete = RF_TRUE;

			if(!(*result)->operation) /* error */
			{
				rf_formula_destroy_result(*result);
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(4, "In function '", rf_function_get_name(formula->function),
						"' : ", error.string));
				if(error.string)
					free(error.string);
				return 1;
			}

			rf_formula_destroy_result(tmp);
			return 0;
		}
		else if(rf_function_has_name(formula->function, "id"))
		{
			RF_ERROR error;
			RF_DOMAIN *domain = 0;

			/* test argument count */
			if(rf_array_size(formula->arguments) != 1)
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "wrong argument count for function '",
					rf_function_get_name(formula->function), "'"));
				return 1;
			}


			/* get domain */
			rf_formula_calc(rf_array_read(formula->arguments, 0), element_1, element_2, &tmp);
			if(tmp->type == RF_FO_DOMAIN)
			{
				/* we cant use a temporary domain to create a relation */
				if(tmp->is_temporary)
				{
					rf_formula_destroy_result(tmp);

					*result = rf_formula_calc_error(&formula->location,
						rf_string_combine(3, "argument for function '",
						rf_function_get_name(formula->function), "' is not a global DOMAIN"));
					return 1;
				}
				else
					domain = tmp->domain;
			}
			else if(tmp->type == RF_FO_ELEMENT || tmp->type == RF_FO_ID)
			{
				domain = rf_parser_get_domain(tmp->element);
			}
			else if(tmp->type == RF_FO_ERROR)
			{
				*result = tmp;
				return 1;
			}

			if(!domain) /* error */
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "argument for function '",
					rf_function_get_name(formula->function), "' is not a DOMAIN"));
				return 1;
			}

			rel_func_2 = rf_function_get_func(formula->function);
			if(!rel_func_2) /* error */
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "program error - function '",
					rf_function_get_name(formula->function), "' has no func"));
				return 1;
			}

			*result = rf_formula_create_result();
			(*result)->type = RF_FO_RELATION;
			(*result)->relation = rel_func_2(domain, &error);
			(*result)->is_temporary = RF_TRUE;
			(*result)->can_delete = RF_TRUE;

			if(!(*result)->relation) /* error */
			{
				rf_formula_destroy_result(*result);
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(4, "In function '", rf_function_get_name(formula->function),
						"' : ", error.string));
				if(error.string)
					free(error.string);
				return 1;
			}

			rf_formula_destroy_result(tmp);
			return 0;
		}
		else if(rf_function_has_name(formula->function, "empty") ||
				rf_function_has_name(formula->function, "full"))
		{
			RF_ERROR error;
			RF_DOMAIN *domain_1 = 0, *domain_2 = 0;

			/* test argument count */
			if(rf_array_size(formula->arguments) != 2)
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "wrong argument count for function '",
					rf_function_get_name(formula->function), "'"));
				return 1;
			}


			/* get domain_1 */
			rf_formula_calc(rf_array_read(formula->arguments, 0), element_1, element_2, &tmp);
			if(tmp->type == RF_FO_DOMAIN)
			{
				/* we cant use a temporary domain to create a relation */
				if(tmp->is_temporary)
				{
					rf_formula_destroy_result(tmp);

					*result = rf_formula_calc_error(&formula->location,
						rf_string_combine(3, "argument for function '",
						rf_function_get_name(formula->function), "' is not a global DOMAIN"));
					return 1;
				}
				else
					domain_1 = tmp->domain;
			}
			else if(tmp->type == RF_FO_ELEMENT || tmp->type == RF_FO_ID)
			{
				domain_1 = rf_parser_get_domain(tmp->element);
			}
			else if(tmp->type == RF_FO_ERROR)
			{
				*result = tmp;
				return 1;
			}

			if(!domain_1) /* error */
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "argument for function '",
					rf_function_get_name(formula->function), "' is not a DOMAIN"));
				return 1;
			}

			/* get domain_2 */
			rf_formula_calc(rf_array_read(formula->arguments, 1), element_1, element_2, &tmp_2);
			if(tmp_2->type == RF_FO_DOMAIN)
			{
				/* we cant use a temporary domain to create a relation */
				if(tmp_2->is_temporary)
				{
					rf_formula_destroy_result(tmp);
					rf_formula_destroy_result(tmp_2);

					*result = rf_formula_calc_error(&formula->location,
						rf_string_combine(3, "argument for function '",
						rf_function_get_name(formula->function), "' is not a global DOMAIN"));
					return 1;
				}
				else
					domain_2 = tmp_2->domain;
			}
			else if(tmp_2->type == RF_FO_ELEMENT || tmp_2->type == RF_FO_ID)
			{
				domain_2 = rf_parser_get_domain(tmp_2->element);
			}
			else if(tmp_2->type == RF_FO_ERROR)
			{
				rf_formula_destroy_result(tmp);

				*result = tmp_2;
				return 1;
			}

			if(!domain_2) /* error */
			{
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "argument for function '",
					rf_function_get_name(formula->function), "' is not a DOMAIN"));
				return 1;
			}


			/* get functionpointer */
			rel_func_3 = rf_function_get_func(formula->function);
			if(!rel_func_3)
			{
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "program error - function '",
					rf_function_get_name(formula->function), "' has no func"));
				return 1;
			}

			*result = rf_formula_create_result();
			(*result)->type = RF_FO_RELATION;
			(*result)->relation = rel_func_3(domain_1, domain_2, &error);
			(*result)->is_temporary = RF_TRUE;
			(*result)->can_delete = RF_TRUE;

			if(!(*result)->relation) /* error */
			{
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(tmp_2);
				rf_formula_destroy_result(*result);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(4, "In function '", rf_function_get_name(formula->function),
						"' : ", error.string));
				if(error.string)
					free(error.string);
				return 1;
			}

			rf_formula_destroy_result(tmp);
			rf_formula_destroy_result(tmp_2);
			return 0;
		}
		else if(rf_function_has_name(formula->function, "top") ||
				rf_function_has_name(formula->function, "bottom") ||
				rf_function_has_name(formula->function, "subsetleq"))
		{
			RF_ERROR error;
			RF_RELATION * (*rfunc)(RF_DOMAIN *domain, RF_ERROR *error);
			RF_DOMAIN *domain = 0;

			/* test argument count */
			if(rf_array_size(formula->arguments) != 1)
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "wrong argument count for function '",
					rf_function_get_name(formula->function), "'"));
				return 1;
			}


			/* get domain */
			rf_formula_calc(rf_array_read(formula->arguments, 0), element_1, element_2, &tmp);
			if(tmp->type == RF_FO_DOMAIN)
			{
				/* we cant use a temporary domain to create a relation */
				if(tmp->is_temporary)
				{
					rf_formula_destroy_result(tmp);

					*result = rf_formula_calc_error(&formula->location,
						rf_string_combine(3, "argument for function '",
						rf_function_get_name(formula->function), "' is not a global DOMAIN"));
					return 1;
				}
				else
					domain = tmp->domain;
			}
			else if(tmp->type == RF_FO_ELEMENT || tmp->type == RF_FO_ID)
			{
				domain = rf_parser_get_domain(tmp->element);
			}
			else if(tmp->type == RF_FO_ERROR)
			{
				*result = tmp;
				return 1;
			}

			if(!domain) /* error */
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "argument for function '",
					rf_function_get_name(formula->function), "' is not a DOMAIN"));
				return 1;
			}


			/* get functionpointer */
			rfunc = rf_function_get_func(formula->function);
			if(!rfunc)
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "program error - function '",
					rf_function_get_name(formula->function), "' has no func"));
				return 1;
			}

			*result = rf_formula_create_result();
			(*result)->type = RF_FO_RELATION;
			(*result)->relation = rfunc(domain, &error);
			(*result)->is_temporary = RF_TRUE;
			(*result)->can_delete = RF_TRUE;

			if(!(*result)->relation) /* error */
			{
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(*result);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(4, "In function '", rf_function_get_name(formula->function),
						"' : ", error.string));
				if(error.string)
					free(error.string);
				return 1;
			}

			rf_formula_destroy_result(tmp);
			return 0;
		}
		else if(rf_function_has_name(formula->function, "pow"))
		{
			RF_ERROR error;
			RF_DOMAIN * (*dfunc)(RF_DOMAIN *domain, RF_ERROR *error);
			RF_DOMAIN *domain = 0;

			/* test argument count */
			if(rf_array_size(formula->arguments) != 1)
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "wrong argument count for function '",
					rf_function_get_name(formula->function), "'"));
				return 1;
			}


			/* get domain */
			rf_formula_calc(rf_array_read(formula->arguments, 0), element_1, element_2, &tmp);
			if(tmp->type == RF_FO_DOMAIN)
			{
				domain = tmp->domain;
			}
			else if(tmp->type == RF_FO_ELEMENT || tmp->type == RF_FO_ID)
			{
				domain = rf_parser_get_domain(tmp->element);
			}
			else if(tmp->type == RF_FO_ERROR)
			{
				*result = tmp;
				return 1;
			}

			if(!domain) /* error */
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "argument for function '",
					rf_function_get_name(formula->function), "' is not a DOMAIN"));
				return 1;
			}


			/* get functionpointer */
			dfunc = rf_function_get_func(formula->function);
			if(!dfunc)
			{
				rf_formula_destroy_result(tmp);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "program error - function '",
					rf_function_get_name(formula->function), "' has no func"));
				return 1;
			}

			*result = rf_formula_create_result();
			(*result)->type = RF_FO_DOMAIN;
			(*result)->domain = dfunc(domain, &error);
			(*result)->is_temporary = RF_TRUE;
			(*result)->can_delete = RF_TRUE;

			if(!(*result)->domain) /* error */
			{
				rf_formula_destroy_result(tmp);
				rf_formula_destroy_result(*result);

				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(4, "In function '", rf_function_get_name(formula->function),
						"' : ", error.string));
				if(error.string)
					free(error.string);
				return 1;
			}

			rf_formula_destroy_result(tmp);
			return 0;
		}
		else
		{
			*result = rf_formula_calc_error(&formula->location, rf_string_combine(3, "function '",
				rf_function_get_name(formula->function), "' is not implemented"));
			return 1;
		}
	}

	else if(formula->type == RF_FO_NEGATION)
	{
		RF_ERROR error;

		/* test argument count */
		if(rf_array_size(formula->arguments) != 1)
		{
			*result = rf_formula_calc_error(&formula->location,
				rf_string_combine(3, "wrong argument count for negation '",
				rf_negation_get_name(formula->negation), "'"));
			return 1;
		}


		/* get element from argument*/
		rf_formula_calc(rf_array_read(formula->arguments, 0), element_1, element_2, &tmp);
		if(tmp->type == RF_FO_ERROR)
		{
			*result = tmp;
			return 1;
		}
		else if(tmp->type != RF_FO_ELEMENT && tmp->type != RF_FO_ID) /* error */
		{
			rf_formula_destroy_result(tmp);

			*result = rf_formula_calc_error(&formula->location,
				rf_string_combine(3, "argument for function '",
				rf_negation_get_name(formula->negation), "' is not a ELEMENT"));
			return 1;
		}
		else if(tmp->is_temporary)
		{
			el_1 = rf_string_copy(tmp->element);
		}
		else
			el_1 = tmp->element;

		rf_formula_destroy_result(tmp);

		out = rf_negation_calc(formula->negation, el_1, &error);
		if(!out) /* error */
		{
			free(el_1);

			/* we dont free error.string here cause it is transfered to result and freed
			 * at there.
			 */
			*result = rf_formula_calc_error(&formula->location, error.string);
			error.string = 0;
			return 1;
		}

		*result = rf_formula_create_result();
		(*result)->type = RF_FO_ELEMENT;
		(*result)->element = out;
		(*result)->is_temporary = RF_FALSE;
		(*result)->can_delete = RF_FALSE;

		free(el_1);
		return 0;
	}

	else if(formula->type == RF_FO_OPERATION || formula->type == RF_FO_RELATION)
	{
		if(rf_array_size(formula->arguments) != 2)
		{
			*result = rf_formula_calc_error(&formula->location,
				rf_string_combine(3, "program error - wrong argument count for RELATION or OPERATION '",
				rf_negation_get_name(formula->negation), "'"));
			return 1;
		}


		/* extract first argument */
		rf_formula_calc(rf_array_read(formula->arguments, 0), element_1, element_2, &tmp);
		if(tmp->type == RF_FO_ERROR)
		{
			*result = tmp;
			return 1;
		}
		else if(tmp->type != RF_FO_ELEMENT && tmp->type != RF_FO_ID) /* error */
		{
			rf_formula_destroy_result(tmp);

			if(formula->type == RF_FO_RELATION)
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "left argument of relation '",
					rf_relation_get_name(formula->relation), "' is not an ELEMENT"));
			}
			else
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "left argument of relation '",
					rf_relation_get_name(formula->relation), "' is not an ELEMENT"));
			}

			return 1;
		}
		el_1 = tmp->element;
		rf_formula_destroy_result(tmp);


		/* extract second argument */
		rf_formula_calc(rf_array_read(formula->arguments, 1), element_1, element_2, &tmp);
		if(tmp->type == RF_FO_ERROR)
		{
			*result = tmp;
			return 1;
		}
		else if(tmp->type != RF_FO_ELEMENT && tmp->type != RF_FO_ID) /* error */
		{
			rf_formula_destroy_result(tmp);

			if(formula->type == RF_FO_RELATION)
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "right argument of relation '",
					rf_relation_get_name(formula->relation), "' is not an ELEMENT"));
			}
			else
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(3, "right argument of relation '",
					rf_relation_get_name(formula->relation), "' is not an ELEMENT"));
			}

			return 1;
		}
		el_2 = tmp->element;
		rf_formula_destroy_result(tmp);

		if(formula->type == RF_FO_OPERATION)
		{
			rf_operation_calc(formula->operation, el_1, el_2, &out);
			if(!out) /* error */
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(4, out, " - while calculating operation '",
					rf_operation_get_name(formula->operation), "'"));
				free(out);
				return 1;
			}

			*result = rf_formula_create_result();
			(*result)->type = RF_FO_ELEMENT;
			(*result)->element = out;
			(*result)->is_temporary = RF_FALSE;
			(*result)->can_delete = RF_FALSE;
			return 0;
		}
		else
		{
			RF_ERROR error;
			res = rf_relation_calc(formula->relation, el_1, el_2, &error);
			if(res == 0 || res == 1)
			{
				*result = rf_formula_create_result();
				(*result)->type = RF_FO_ELEMENT;
				if(res)
					(*result)->element = "1";
				else
					(*result)->element = "0";
				(*result)->is_temporary = RF_FALSE;
				(*result)->can_delete = RF_FALSE;
				return 0;
			}
			else /* error */
			{
				*result = rf_formula_calc_error(&formula->location,
					rf_string_combine(4, error.string, " - while testing relation '",
					rf_relation_get_name(formula->relation), "'"));
				free(error.string);
				return 1;
			}
		}
	}

	else if(formula->type == RF_FO_VARIABLE)
	{
		*result = rf_formula_create_result();
		(*result)->type = RF_FO_ELEMENT;
		(*result)->is_temporary = RF_TRUE;
		(*result)->can_delete = RF_FALSE;

		if(formula->variable == 'X')
		{
			if(element_1)
			{
				(*result)->element = element_1;
				return 0;
			}
			else /* error */
			{
				rf_formula_destroy_result(*result);
				*result = rf_formula_calc_error(&formula->location,
					rf_string_copy("program error - variable 'X' is not set"));
				return 1;
			}
		}
		else
		{
			if(element_2)
			{
				(*result)->element = element_2;
				return 0;
			}
			else /* error */
			{
				rf_formula_destroy_result(*result);
				*result = rf_formula_calc_error(&formula->location,
					rf_string_copy("program error - variable 'Y' is not set"));
				return 1;
			}
		}
	}

	/* if we come here, some type is not implemented yet */
	*result = rf_formula_calc_error(&formula->location,
		rf_string_copy("program error - some result is not implemented"));
	return 1;
}

/*!
Creates an error result.

@relates RF_FORMULA
@param[in] location The location where the error happend in the sourcecode.
@param[in] error The errormessage. The function takes the ownership of this memory! error gets invalid for the caller.
@return A result with the error data.
@return 0 on fail.
*/
RF_FORMULA_RESULT * rf_formula_calc_error(RF_LOCATION *location, char *error)
{
	RF_FORMULA_RESULT *result;

	result = rf_formula_create_result();
	if(!result)
		return 0;

	result->type = RF_FO_ERROR;
	result->error = error;
	if(location)
		result->location = *location;
	result->is_temporary = RF_TRUE;
	result->can_delete = RF_TRUE;

	return result;
}


/*!
Removes all content from the structure.

@relates RF_FORMULA
@param[in] formula The formula to be cleared.
*/
void rf_formula_clear(RF_FORMULA *formula)
{
	if(!formula)
		return;

	formula->type = RF_FO_EMPTY;

	/* arguments has to be destroyed */
	if(formula->arguments)
	{
		rf_array_destroy(formula->arguments, (void (*)(void *))rf_formula_destroy);
		formula->arguments = 0;
	}
	if(formula->element) /* element is a string that must be freed here */
	{
		free(formula->element);
		formula->element = 0;
	}
	if(formula->domain) /* domain is a temporary domain that must be freed here. A global domain would be only a string in element here.*/
	{
		rf_domain_destroy(formula->domain);
		formula->domain = 0;
	}

	formula->function = 0;
	formula->negation = 0;
	formula->operation = 0;
	formula->relation = 0;
	formula->variable = 0;
}

/*!
Creates a empty result in memory.

@relates RF_FORMULA
@return The new result.
@return 0 on error.
*/
RF_FORMULA_RESULT * rf_formula_create_result()
{
	RF_FORMULA_RESULT *result = calloc(1, sizeof(RF_FORMULA_RESULT));
	if(!result)
		return 0;

	result->is_temporary = RF_FALSE;
	result->can_delete = RF_FALSE;
	return result;
}

/*!
Destroys the result.

If can_delete is true, then the object stored in the result will be deleted!
@relates RF_FORMULA
@param[in] The result to be deleted.
*/
void rf_formula_destroy_result(RF_FORMULA_RESULT *result)
{
	if(!result)
		return;

	if(result->can_delete == RF_TRUE)
	{
		switch(result->type)
		{
			case RF_FO_ELEMENT:
				if(result->element)
					free(result->element);
				break;

			case RF_FO_ID:
				if(result->element)
					free(result->element);
				break;

			case RF_FO_OPERATION:
				rf_operation_destroy(result->operation);
				break;

			case RF_FO_RELATION:
				rf_relation_destroy(result->relation);
				break;

			case RF_FO_NEGATION:
				rf_negation_destroy(result->negation);
				break;

			case RF_FO_DOMAIN:
				rf_domain_destroy(result->domain);
				break;
		}
	}

	free(result);
}

/*!
Creates a new empty formula

@relates RF_FORMULA
@return The new formula.
@return 0 on error.
*/
RF_FORMULA * rf_formula_create()
{
	RF_FORMULA *formula = calloc(1, sizeof(RF_FORMULA));
	if(!formula)
		return 0;

	formula->type = RF_FO_EMPTY;

	return formula;
}

/*!
Frees the given formula.

@relates RF_FORMULA
@param[in] The formula to be destroyed.
*/
void rf_formula_destroy(RF_FORMULA *formula)
{
	if(!formula)
		return;

	rf_formula_clear(formula);
	free(formula);
}

/*!
Replaces the argumentlist of the given formula.

The formulas old arguments will be destroyed!
@relates RF_FORMULA
@param[in] formula The formula whoes arguments should be replaced.
@param[in] arguments The new arguments. Must be of type RF_FORMULA *.
*/
void rf_formula_set_arguments(RF_FORMULA *formula, RF_ARRAY *arguments)
{
	if(!formula || !arguments)
		return;

	if(formula->arguments)
		rf_array_destroy(formula->arguments, (void (*)(void *))rf_formula_destroy);

	formula->arguments = arguments;
}

/*!
Clears the formula and set it to the given domain.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] domain The new object.
*/
void rf_formula_set_domain(RF_FORMULA *formula, RF_DOMAIN *domain)
{
	if(!formula || !domain)
		return;

	rf_formula_clear(formula);

	formula->type = RF_FO_DOMAIN;
	formula->domain = domain;
}

/*!
Clears the formula and set it to the given element.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] element The new object.
*/
void rf_formula_set_element(RF_FORMULA *formula, char *element)
{
	if(!formula || !element)
		return;

	rf_formula_clear(formula);

	formula->type = RF_FO_ELEMENT;
	formula->element = element;
}

/*!
Clears the formula and set it to the given function.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] function The new object.
*/
void rf_formula_set_function(RF_FORMULA *formula, RF_FUNCTION *function)
{
	if(!formula || !function)
		return;

	rf_formula_clear(formula);

	formula->type = RF_FO_FUNCTION;
	formula->function = function;
}

/*!
Sets the location in the sourcecode.

@relates RF_FORMULA
@param[in] formula The formula whoes location should be set.
@param first_line The line of sourcecode where the formula starts
@param first_column The column of sourcecode where the formula starts
@param last_line The line of sourcecode where the formula ends
@param last_column The column of sourcecode where the formula ends
*/
void rf_formula_set_location(RF_FORMULA *formula, int first_line, int first_column, int last_line, int last_column)
{
	if(!formula)
		return;

	formula->location.first_line = first_line;
	formula->location.first_column = first_column;
	formula->location.last_line = last_line;
	formula->location.last_column = last_column;
}

/*!
Clears the formula and set it to the given negation.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] negation The new object.
*/
void rf_formula_set_negation(RF_FORMULA *formula, RF_NEGATION *negation)
{
	if(!formula || !negation)
		return;

	rf_formula_clear(formula);

	formula->type = RF_FO_NEGATION;
	formula->negation = negation;
}

/*!
Clears the formula and set it to the given operation.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] operation The new object.
*/
void rf_formula_set_operation(RF_FORMULA *formula, RF_OPERATION *operation)
{
	if(!formula || !operation)
		return;

	rf_formula_clear(formula);

	formula->type = RF_FO_OPERATION;
	formula->operation = operation;
}

/*!
Clears the formula and set it to the given relation.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] relation The new object.
*/
void rf_formula_set_relation(RF_FORMULA *formula, RF_RELATION *relation)
{
	if(!formula || !relation)
		return;

	rf_formula_clear(formula);

	formula->type = RF_FO_RELATION;
	formula->relation = relation;
}

/*!
Clears the formula and set it to the given variable.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] variable The new object.
*/
void rf_formula_set_variable(RF_FORMULA *formula, char variable)
{
	if(!formula || !variable)
		return;

	rf_formula_clear(formula);

	formula->type = RF_FO_VARIABLE;
	formula->variable = variable;
}

