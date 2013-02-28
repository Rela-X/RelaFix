/*
 * RelaFixLib    Negation
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 Functions to work with negation function

 Declares functions to work on struct RF_NEGATION. RF_NEGATION represents the negation function
 of the Relafix language. The user can define the negation based on a domain in that language.
 */

#ifndef RF_NEGATION_H
#define RF_NEGATION_H

#include <stdbool.h>

#include "list.h"
#include "domain.h"

/*!
 Represents the negation function of the Relafix language.

 In the Relafix language the user can define the negation based on a domain. This structure is
 used in relafixlib to handle this.
 */
typedef struct rf_negation
{
	char      *name;		/*!< Name of negation. */
	RF_DOMAIN *domain;	/*!< The domain keeping the elements that are negated */

	/*!
	 List of elementnames (char *) and their negation.

	 The list has the following format: 'original, negation, original, negation, ...'. That means,
	 that after a original always the negation to that original follows. The
	 list must have the double amount of elements then the domain has.
	 */
	RF_LIST   *items;
} RF_NEGATION;


/*!
 Returns the negated input

 @relates RF_NEGATION
 @param negation The negation to use.
 @param element The name of the element that should be negated.
 @return The name of the negated element.
 @return 0 when the function did fail. an errordescription is written to error.
 */
char *        rf_negation_calc(RF_NEGATION *negation, char *element);

/*!
 @relates RF_NEGATION
 @param name The name of the new negation. 0 is not allowed.
 @param domain The domain whoes elements are negated.
 @return New negation.
 @return 0 on error.
 */
RF_NEGATION * rf_negation_create(char *name, RF_DOMAIN *domain);

/*!
 @relates RF_NEGATION
 @param negation The negation to be destroyed.
 */
void          rf_negation_destroy(RF_NEGATION *negation);

/*!
 @relates RF_NEGATION
 @param negation The negation whoes domain should be returned.
 @return The domain the given negation is based on.
 @return 0 on error.
 */
RF_DOMAIN *   rf_negation_get_domain(RF_NEGATION *negation);

/*!
 @relates RF_NEGATION
 @param negation The negation whoes elementlist should be returned.
 @return A list with element names (char *). The list has a special format (See RF_NEGATION).
         The list and its contained data must not be changed or deleted by the user!
 @return 0 on error.
 */
RF_LIST *     rf_negation_get_items(RF_NEGATION *negation);

/*!
 @relates RF_NEGATION
 @param negation The negation whoes name should be returned.
 @return The name of the negation. Should not be changed or deleted by the user!
 @return 0 on error or if the negation has no name.
 */
char *        rf_negation_get_name(RF_NEGATION *negation);

/*! Compares the given name with the name of the negation.

 @relates RF_NEGATION
 @param negation The negation whoes name should be compared.
 @param name The name in question.
 @return true if the name matchs the name of the negation.
 @return false if the name does not match or on error.
 */
bool          rf_negation_has_name(RF_NEGATION *negation, char *name);

/*!
 Sets the negation items. If the negation had a list before, that list will be destroyed first.
 Before the list is set, the contained data is checked against the domain and if all elements
 have a negation.
 @relates RF_NEGATION
 @param negation The negation whoes items should be set.
 @param items A list with element names (char *). The list must follow a special format (See RF_NEGATION).
        The list gets invalid for the caller!
 @param error If the function fails an error description is written in error. See RF_ERROR. 0 is not allowed.
 @return 0 on success.
 @return 1 on fail. An error description is written into error.
 */
int           rf_negation_set_items(RF_NEGATION *negation, RF_LIST *items);

#endif
