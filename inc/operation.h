/*
 * RelaFixLib    Operation
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 \brief Functions to work with operations.

 Declares functions to work on struct RF_OPERATION. RF_OPERATION represents a mathematical
 operation. In the Relafix language operations are used in the form 'a + b'.
 */

#ifndef RF_OPERATION_H
#define RF_OPERATION_H

#include <stdbool.h>

#include "error.h"
#include "relation.h"

/*! \brief Represents a mathematical operation

 An operation has a name and is based on 3 domains (domain_1 X domain_2 -> domain_3). The operation also has a table with
 all possible solutions. If the operation needs to be calculated, the solution is just picked
 from the right position in the table. The operation has the form "left_element operation right_element" in the Relafix language.
 */
typedef struct rf_operation
{
	char		  *name;		/*!< \brief The name of the operation */
	RF_DOMAIN *domain_1;	/*!< \brief The domain from which the left element must be. */
	RF_DOMAIN *domain_2;	/*!< \brief The domain from which the right element must be. */
	RF_DOMAIN *domain_3;	/*!< \brief The domain from which the element of the solution must be. */
	RF_TABLE  *table;		/*!< \brief Contains the solutions in form of element names from domain_3. The type of the table is RF_TT_STRING. */
} RF_OPERATION;


int            rf_operation_calc(RF_OPERATION *operation, char *element_1, char *element_2, char **element_out);
RF_OPERATION * rf_operation_copy(RF_OPERATION *operation);
RF_OPERATION * rf_operation_create(char *name, RF_DOMAIN *domain_1, RF_DOMAIN *domain_2, RF_DOMAIN *domain_3);
RF_OPERATION * rf_operation_create_meet(RF_RELATION *relation, RF_ERROR *error);
RF_OPERATION * rf_operation_create_join(RF_RELATION *relation, RF_ERROR *error);
void           rf_operation_destroy(RF_OPERATION *operation);
RF_OPERATION * rf_operation_generate_join(RF_RELATION *leq);
RF_OPERATION * rf_operation_generate_meet(RF_RELATION *leq);
RF_DOMAIN *    rf_operation_get_domain_1(RF_OPERATION *operation);
RF_DOMAIN *    rf_operation_get_domain_2(RF_OPERATION *operation);
RF_DOMAIN *    rf_operation_get_domain_3(RF_OPERATION *operation);
char *         rf_operation_get_name(RF_OPERATION *operation);
RF_TABLE *     rf_operation_get_table(RF_OPERATION *operation);
bool           rf_operation_has_name(RF_OPERATION *operation, char *name);
void           rf_operation_set_name(RF_OPERATION *operation, char *name);
void           rf_operation_set_table(RF_OPERATION *operation, RF_TABLE *table);

#endif
