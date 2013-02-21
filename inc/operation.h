/*
 * RelaFixLib    Operation
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 Functions to work with operations.

 Declares functions to work on struct RF_OPERATION. RF_OPERATION represents a mathematical
 operation. In the Relafix language operations are used in the form 'a + b'.
 */

#ifndef RF_OPERATION_H
#define RF_OPERATION_H

#include <stdbool.h>

#include "error.h"
#include "relation.h"

/*! Represents a mathematical operation

 An operation has a name and is based on 3 domains (domain_1 X domain_2 -> domain_3). The operation also has a table with
 all possible solutions. If the operation needs to be calculated, the solution is just picked
 from the right position in the table. The operation has the form "left_element operation right_element" in the Relafix language.
 */
typedef struct rf_operation
{
	char		  *name;		/*!< The name of the operation */
	RF_DOMAIN *domain_1;	/*!< The domain from which the left element must be. */
	RF_DOMAIN *domain_2;	/*!< The domain from which the right element must be. */
	RF_DOMAIN *domain_3;	/*!< The domain from which the element of the solution must be. */
	RF_TABLE  *table;		/*!< Contains the solutions in form of element names from domain_3. The type of the table is RF_TT_STRING. */
} RF_OPERATION;


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
int            rf_operation_calc(RF_OPERATION *operation, char *element_1, char *element_2, char **element_out);

/*!
 @relates RF_OPERATION
 @param[in] operation The operation that should be copied.
 @return The copy of the given operation.
 @return 0 on error.
 */
RF_OPERATION * rf_operation_copy(RF_OPERATION *operation);

/*!
 @relates RF_OPERATION
 @param[in] name The name of the new operation. 0 is allowed. Gets invalid for the caller.
 @param[in] domain_1 The domain of the left argument.
 @param[in] domain_2 The domain of the right argument.
 @param[in] domain_3 The domain containing the elements of the solution.
 @return The new operation
 @return 0 on error
 */
RF_OPERATION * rf_operation_create(char *name, RF_DOMAIN *domain_1, RF_DOMAIN *domain_2, RF_DOMAIN *domain_3);

/*!
 @relates RF_OPERATION
 @param[in] relation The less equal relation the meet is based on.
 @param[out] If the function fails an error is written here. The string must be deleted by the caller!
 @return An new meet operation.
 @return 0 on error. An description is written to error.
 */
RF_OPERATION * rf_operation_create_meet(RF_RELATION *relation, RF_ERROR *error);

/*!
 @relates RF_OPERATION
 @param[in] relation The less equal relation the join is based on.
 @param[out] If the function fails an error is written here. The string must be deleted by the caller!
 @return An new join operation.
 @return 0 on error. An description is written to error.
 */
RF_OPERATION * rf_operation_create_join(RF_RELATION *relation, RF_ERROR *error);

/*!
 @relates RF_OPERATION
 @param[in] operation The operation that should be freed. Gets invalid for the caller.
 */
void           rf_operation_destroy(RF_OPERATION *operation);

/*!
 Generates the join operation for a given less equal relation.
 @relates RF_OPERATION
 @param[in] leq The less equal relation the join operation will be based on.
 @return The new join operation.
 @return 0 on error.
 */
RF_OPERATION * rf_operation_generate_join(RF_RELATION *leq);

/*!
 Generates the meet operation for a given less equal relation.
 @relates RF_OPERATION
 @param[in] leq The less equal relation the meet operation will be based on.
 @return The new meet operation.
 @return 0 on error.
 */
RF_OPERATION * rf_operation_generate_meet(RF_RELATION *leq);

/*!
 Returns the domain the first element in an operation must be from.
 @relates RF_OPERATION
 @param[in] operation The operation whoes domain should be returned.
 @return The domain. Should not be changed by the caller!
 @return 0 on error.
 */
RF_DOMAIN *    rf_operation_get_domain_1(RF_OPERATION *operation);

/*!
 Returns the domain the second element in an operation must be from.
 @relates RF_OPERATION
 @param[in] operation The operation whoes domain should be returned.
 @return The domain. Should not be changed by the caller!
 @return 0 on error.
 */
RF_DOMAIN *    rf_operation_get_domain_2(RF_OPERATION *operation);

/*!
 Returns the domain the resulting element of an operation must be from.
 @relates RF_OPERATION
 @param[in] operation The operation whoes domain should be returned.
 @return The domain. Should not be changed by the caller!
 @return 0 on error.
 */
RF_DOMAIN *    rf_operation_get_domain_3(RF_OPERATION *operation);

/*!
 @relates RF_OPERATION
 @param[in] operation The operation whoes name should be returned.
 @return The name. Should not be changed by the caller!
 @return 0 on error.
 */
char *         rf_operation_get_name(RF_OPERATION *operation);

RF_TABLE *     rf_operation_get_table(RF_OPERATION *operation);

/*!
 Checks if a given name matches the name of the given operation.
 @relates RF_OPERATION
 @param[in] operation The operation whoes name is in question.
 @param[in] The name that should be compared.
 @return true if the name matches the name of the operation.
 @return false if the name does not match and on error.
 */
bool           rf_operation_has_name(RF_OPERATION *operation, char *name);

/*!
 The function will free the old name if exists.
 @relates RF_OPERATION
 @param[in] operation The operation whoes name should be set.
 @param[in] name The name to set. Gets invalid for the caller!
 */
void           rf_operation_set_name(RF_OPERATION *operation, char *name);

/*!
 The function will destroy the old table if exists.
 @relates RF_OPERATION
 @param[in] operation The operation whoes table should be set.
 @param[in] table The table to set. Gets invalid for caller!
 */
void           rf_operation_set_table(RF_OPERATION *operation, RF_TABLE *table);

#endif
