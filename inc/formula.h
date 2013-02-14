/*
 * RelaFixLib    Formula
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
\brief A structure that represents an expression tree.

Declares functions to work on struct RF_FORMULA. It will be used to store expressions for late
execution.
*/

#ifndef RF_FORMULA_H
#define RF_FORMULA_H

#include "array.h"
#include "domain.h"
#include "function.h"
#include "negation.h"
#include "operation.h"

/*!
\brief Objects a formula node can take.

@relates RF_FORMULA
*/
enum rf_enum_ft {RF_FO_EMPTY, RF_FO_ELEMENT, RF_FO_ID, RF_FO_FUNCTION, RF_FO_OPERATION, RF_FO_RELATION,
                 RF_FO_VARIABLE, RF_FO_NEGATION, RF_FO_DOMAIN, RF_FO_ERROR};

/*!
\brief The node of an formula tree.

Depending on the type (rf_enum_ft) it contains an object. arguments is a list of RF_FORMULA structs.
It can be understood as childnodes of the curent one. In location the position in the sourcefile is
been saved for later use. A formula can be calculated with a call to the function rf_formula_calc().
*/
typedef struct rf_formula
{
	enum rf_enum_ft type; /*!< \brief The type of the object saved in the node. Must be from rf_enum_ft */
	RF_LOCATION     location; /*!< \brief The location in the sourcecode. Will be used to create an useful errormessage */
	RF_ARRAY        *arguments; /*!< \brief If type is RF_FO_FUNCTION, RF_FO_NEGATION, RF_FO_OPERATION or RF_FO_RELATION then arguments is a list to RF_FORMULA* that are the arguments of the object. */
	char            *element; /*!< \brief If type is RF_FO_ELEMENT or RF_FO_ITEM */
	RF_FUNCTION     *function; /*!< \brief If type is RF_FO_FUNCTION */
	RF_NEGATION     *negation; /*!< \brief If type is RF_FO_NEGATION */
	RF_OPERATION    *operation; /*!< \brief If type is RF_FO_OPERATION */
	RF_RELATION     *relation; /*!< \brief If type is RF_FO_RELATION */
	RF_DOMAIN       *domain; /*!< \brief If type is RF_FO_DOMAIN */
	char            variable; /*!< \brief If type is RF_FO_VARIABLE, this can be X or Y */
} RF_FORMULA;

/*!
\brief Represents the result of the execution of an formula.

It is important to understand the meaning of the fields is_temporary and can_delete!
is_temporary means that the object in the result is only temporary available. The user can take the ownership
of the object, but he must set can_delete to false!!! can_delete is important for the rf_formula_destroy_result() function.
The function decides on this flag if it will delete the object if it is temporary. If is_temporary is false, then this means that the
object is a global one. For example a global domain.
*/
typedef struct rf_formula_result
{
	enum rf_enum_ft type; /*!< \brief The type of the object contained. Must be from rf_enum_ft */
	RF_ARRAY        *arguments; /*!< \brief If the type is negation, operation or relation, then here are the arguments for them */
	char            *element; /*!< \brief If the type is RF_FO_ELEMENT or RF_FO_ID */
	RF_NEGATION     *negation; /*!< \brief If the type is RF_FO_NEGATION */
	RF_OPERATION    *operation; /*!< \brief If the type is RF_FO_OPERATION */
	RF_RELATION     *relation; /*!< \brief If the type is RF_FO_RELATION */
	RF_DOMAIN       *domain; /*!< \brief If the type is RF_FO_DOMAIN */
	char            *error; /*!< \brief If the type is RF_FO_ERROR, here is a pointer to an error message */
	RF_LOCATION     location; /*!< \brief If the type is RF_FO_ERROR, here goes the location where the error did occur */
	RF_BOOL         is_temporary; /*!< \brief If true the object in this struct is only temporary available. Else it is a global object. */
	RF_BOOL         can_delete; /*!< \brief If true, a call to function rf_formula_destroy_result() will delete the object if it is temporary! */
} RF_FORMULA_RESULT;


void                rf_formula_append_argument(RF_FORMULA *formula, RF_FORMULA *argument);
int                 rf_formula_calc(RF_FORMULA *formula, char *element_1, char *element_2, RF_FORMULA_RESULT **result);
RF_FORMULA_RESULT * rf_formula_calc_error(RF_LOCATION *location, char *error);
void                rf_formula_clear(RF_FORMULA *formula);
RF_FORMULA *        rf_formula_create();
RF_FORMULA_RESULT * rf_formula_create_result();
void                rf_formula_destroy_result(RF_FORMULA_RESULT *result);
void                rf_formula_destroy(RF_FORMULA *formula);
void                rf_formula_set_arguments(RF_FORMULA *formula, RF_ARRAY *arguments);
void                rf_formula_set_domain(RF_FORMULA *formula, RF_DOMAIN *domain);
void                rf_formula_set_element(RF_FORMULA *forumula, char *element);
void                rf_formula_set_function(RF_FORMULA *formula, RF_FUNCTION *function);
void                rf_formula_set_location(RF_FORMULA *formula, int first_line, int first_column, int last_line, int last_column);
void                rf_formula_set_negation(RF_FORMULA *formula, RF_NEGATION *negation);
void                rf_formula_set_operation(RF_FORMULA *formula, RF_OPERATION *operation);
void                rf_formula_set_relation(RF_FORMULA *formula, RF_RELATION *relation);
void                rf_formula_set_variable(RF_FORMULA *formula, char variable);


#endif
