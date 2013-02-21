/*
 * RelaFixLib    Negation
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 \brief Functions to work with negation function

 Declares functions to work on struct RF_NEGATION. RF_NEGATION represents the negation function
 of the Relafix language. The user can define the negation based on a domain in that language.
 */

#ifndef RF_NEGATION_H
#define RF_NEGATION_H

#include <stdbool.h>

#include "list.h"
#include "domain.h"

/*!
 \brief Represents the negation function of the Relafix language.

 In the Relafix language the user can define the negation based on a domain. This structure is
 used in relafixlib to handle this.
 */
typedef struct rf_negation
{
	char      *name;		/*!< \brief Name of negation. */
	RF_DOMAIN *domain;	/*!< \brief The domain keeping the elements that are negated */

	/*!
	 \brief List of elementnames (char *) and their negation.

	 The list has the following format: 'original, negation, original, negation, ...'. That means,
	 that after a original always the negation to that original follows. The
	 list must have the double amount of elements then the domain has.
	 */
	RF_LIST   *items;
} RF_NEGATION;



char *        rf_negation_calc(RF_NEGATION *negation, char *element, RF_ERROR *error);
RF_NEGATION * rf_negation_create(char *name, RF_DOMAIN *domain);
void          rf_negation_destroy(RF_NEGATION *negation);
RF_DOMAIN *   rf_negation_get_domain(RF_NEGATION *negation);
RF_LIST *     rf_negation_get_items(RF_NEGATION *negation);
char *        rf_negation_get_name(RF_NEGATION *negation);
bool          rf_negation_has_name(RF_NEGATION *negation, char *name);
int           rf_negation_set_items(RF_NEGATION *negation, RF_LIST *items, RF_ERROR *error);

#endif
