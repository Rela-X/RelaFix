/*
 * RelaFixLib    Formula
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
A structure that represents an expression tree.

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
Objects a formula node can take.

@relates RF_FORMULA
*/
enum rf_enum_ft {RF_FO_EMPTY, RF_FO_ELEMENT, RF_FO_ID, RF_FO_FUNCTION, RF_FO_OPERATION, RF_FO_RELATION,
                 RF_FO_VARIABLE, RF_FO_NEGATION, RF_FO_DOMAIN, RF_FO_ERROR};

/*!
The node of an formula tree.

Depending on the type (rf_enum_ft) it contains an object. arguments is a list of RF_FORMULA structs.
It can be understood as childnodes of the curent one. In location the position in the sourcefile is
been saved for later use. A formula can be calculated with a call to the function rf_formula_calc().
*/
typedef struct rf_formula
{
	enum rf_enum_ft type; /*!< The type of the object saved in the node. Must be from rf_enum_ft */
	RF_LOCATION     location; /*!< The location in the sourcecode. Will be used to create an useful errormessage */
	RF_ARRAY        *arguments; /*!< If type is RF_FO_FUNCTION, RF_FO_NEGATION, RF_FO_OPERATION or RF_FO_RELATION then arguments is a list to RF_FORMULA* that are the arguments of the object. */
	char            *element; /*!< If type is RF_FO_ELEMENT or RF_FO_ITEM */
	RF_FUNCTION     *function; /*!< If type is RF_FO_FUNCTION */
	RF_NEGATION     *negation; /*!< If type is RF_FO_NEGATION */
	RF_OPERATION    *operation; /*!< If type is RF_FO_OPERATION */
	RF_RELATION     *relation; /*!< If type is RF_FO_RELATION */
	RF_DOMAIN       *domain; /*!< If type is RF_FO_DOMAIN */
	char            variable; /*!< If type is RF_FO_VARIABLE, this can be X or Y */
} RF_FORMULA;

/*!
Represents the result of the execution of an formula.

It is important to understand the meaning of the fields is_temporary and can_delete!
is_temporary means that the object in the result is only temporary available. The user can take the ownership
of the object, but he must set can_delete to false!!! can_delete is important for the rf_formula_destroy_result() function.
The function decides on this flag if it will delete the object if it is temporary. If is_temporary is false, then this means that the
object is a global one. For example a global domain.
*/
typedef struct rf_formula_result
{
	enum rf_enum_ft type; /*!< The type of the object contained. Must be from rf_enum_ft */
	RF_ARRAY        *arguments; /*!< If the type is negation, operation or relation, then here are the arguments for them */
	char            *element; /*!< If the type is RF_FO_ELEMENT or RF_FO_ID */
	RF_NEGATION     *negation; /*!< If the type is RF_FO_NEGATION */
	RF_OPERATION    *operation; /*!< If the type is RF_FO_OPERATION */
	RF_RELATION     *relation; /*!< If the type is RF_FO_RELATION */
	RF_DOMAIN       *domain; /*!< If the type is RF_FO_DOMAIN */
	char            *error; /*!< If the type is RF_FO_ERROR, here is a pointer to an error message */
	RF_LOCATION     location; /*!< If the type is RF_FO_ERROR, here goes the location where the error did occur */
	bool            is_temporary; /*!< If true the object in this struct is only temporary available. Else it is a global object. */
	bool            can_delete; /*!< If true, a call to function rf_formula_destroy_result() will delete the object if it is temporary! */
} RF_FORMULA_RESULT;


/*!
Appends an argument (RF_FORMULA) to a given formula node.

@relates RF_FORMULA
@param[in] formula The formula that takes the arguments.
@param[in] argument The argument that should get appended.
*/
void                rf_formula_append_argument(RF_FORMULA *formula, RF_FORMULA *argument);

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
int                 rf_formula_calc(RF_FORMULA *formula, char *element_1, char *element_2, RF_FORMULA_RESULT **result);

/*!
Creates an error result.

@relates RF_FORMULA
@param[in] location The location where the error happend in the sourcecode.
@param[in] error The errormessage. The function takes the ownership of this memory! error gets invalid for the caller.
@return A result with the error data.
@return 0 on fail.
*/
RF_FORMULA_RESULT * rf_formula_calc_error(RF_LOCATION *location, char *error);

/*!
Removes all content from the structure.

@relates RF_FORMULA
@param[in] formula The formula to be cleared.
*/
void                rf_formula_clear(RF_FORMULA *formula);

/*!
Creates a empty result in memory.

@relates RF_FORMULA
@return The new result.
@return 0 on error.
*/
RF_FORMULA *        rf_formula_create();


/*!
Destroys the result.

If can_delete is true, then the object stored in the result will be deleted!
@relates RF_FORMULA
@param[in] The result to be deleted.
*/
RF_FORMULA_RESULT * rf_formula_create_result();

/*!
Creates a new empty formula

@relates RF_FORMULA
@return The new formula.
@return 0 on error.
*/
void                rf_formula_destroy_result(RF_FORMULA_RESULT *result);

/*!
Frees the given formula.

@relates RF_FORMULA
@param[in] The formula to be destroyed.
*/
void                rf_formula_destroy(RF_FORMULA *formula);

/*!
Replaces the argumentlist of the given formula.

The formulas old arguments will be destroyed!
@relates RF_FORMULA
@param[in] formula The formula whoes arguments should be replaced.
@param[in] arguments The new arguments. Must be of type RF_FORMULA *.
*/
void                rf_formula_set_arguments(RF_FORMULA *formula, RF_ARRAY *arguments);

/*!
Clears the formula and set it to the given domain.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] domain The new object.
*/
void                rf_formula_set_domain(RF_FORMULA *formula, RF_DOMAIN *domain);

/*!
Clears the formula and set it to the given element.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] element The new object.
*/
void                rf_formula_set_element(RF_FORMULA *forumula, char *element);

/*!
Clears the formula and set it to the given function.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] function The new object.
*/
void                rf_formula_set_function(RF_FORMULA *formula, RF_FUNCTION *function);

/*!
Sets the location in the sourcecode.

@relates RF_FORMULA
@param[in] formula The formula whoes location should be set.
@param first_line The line of sourcecode where the formula starts
@param first_column The column of sourcecode where the formula starts
@param last_line The line of sourcecode where the formula ends
@param last_column The column of sourcecode where the formula ends
*/
void                rf_formula_set_location(RF_FORMULA *formula, int first_line, int first_column, int last_line, int last_column);

/*!
Clears the formula and set it to the given negation.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] negation The new object.
*/
void                rf_formula_set_negation(RF_FORMULA *formula, RF_NEGATION *negation);

/*!
Clears the formula and set it to the given operation.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] operation The new object.
*/
void                rf_formula_set_operation(RF_FORMULA *formula, RF_OPERATION *operation);

/*!
Clears the formula and set it to the given relation.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] relation The new object.
*/
void                rf_formula_set_relation(RF_FORMULA *formula, RF_RELATION *relation);

/*!
Clears the formula and set it to the given variable.

@relates RF_FORMULA
@param[in] formula The formula to be set.
@param[in] variable The new object.
*/
void                rf_formula_set_variable(RF_FORMULA *formula, char variable);


#endif
