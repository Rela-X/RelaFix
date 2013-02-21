/*
 * RelaFixLib    Domain
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 * Functions to work on domains and elements.
 *
 * Declares functions to work on struct RF_DOMAIN. RF_DOMAIN represents the mathematical
 * domain that keeps elements (RF_ELEMENT).
 *
 * RF_DOMAIN is needed when relations and operations are defined.
 */

/* THIS FILE WAS CHANGED DUE TO THE FACT THAT TYPEDEF REDEFINITIONS
CAUSED ERRORS WITH SOME COMPILERS */

#ifndef RF_DOMAIN_H
#define RF_DOMAIN_H

#include <stdbool.h>

#include "error.h"
#include "list.h"

/*! Represents a mathematical domain
 *
 * A domain has a name and a list of elements. Here the elements are of type RF_ELEMENT.
 * After starting using the domain, the elements should not be changed or sorted! That is because other
 * structs like RF_RELATION reley on the elements in the domain.
 * An element can be a string or an other domain.
 */
typedef struct rf_domain
{
	char	*name;		/*!< domain name */

	/*! elements in the domain.
	 *
	 * A list of pointers to struct RF_ELEMENT. This list should never be changed or sorted
	 * while the domain is in use of some other struct like RF_RELATION!
	 */
	RF_LIST	*elements;
} RF_DOMAIN;

enum rf_enum_et {RF_ET_NAME, RF_ET_DOMAIN, RF_ET_GLOBAL_DOMAIN};

/*! Represents a mathematical element
 *
 * An element can be a name or a domain.
 */
typedef struct rf_element
{
	/*! Describes what is in data. A value from ::rf_enum_et.
	 */
	enum rf_enum_et type;

	/*! name or domain
	 *
	 * The data pointed to depends on the value of type.
	 *
	 * - RF_ET_NAME: data has type char*
	 * - RF_ET_DOMAIN: data has type RF_DOMAIN*
	 * - RF_ET_GLOBAL_DOMAIN: data has type RF_DOMAIN*. Will not be freed when the element is destroyed.
	 */
	void            *data;
} RF_ELEMENT;


/*!
 Checks if some element is double in the list. If so the function fails and an errormessage is
 returned.
 @param[in] elements A list containing pointers to struct RF_ELEMENT.
 @param[out] error On error a description is written into error.
 @return 0 on success.
 @return 1 on fail. A description is written to error. The errormessage must be freed by the caller.
 */
int          rf_domain_check_elements(RF_LIST *elements, RF_ERROR *error);

/*!
 @relates RF_DOMAIN
 @param domain The domain that should be copied.
 @return The copied domain.
 @return 0 on error.
 */
RF_DOMAIN *  rf_domain_copy(RF_DOMAIN *domain);

/*!
 * @param name Name of domain. Must be able to be used with free(). The pointer gets invalid for the caller!
 *				If 0 then the domain will be nameless.
 * @return Pointer to new domain. If an error occures 0 will be returned.
 */
RF_DOMAIN *  rf_domain_create(char *name);

/*!
Creates a new powerset based on the given domain.

@relates RF_DOMAIN
@param[in] domain The domain the new domain is based on
@param[out] error On fail an errormessage is written here
@return The new domain based on the elements in domain.
@return 0 on fail. An errormessage is written in error.
*/
RF_DOMAIN *  rf_domain_create_powerset(RF_DOMAIN *domain, RF_ERROR *error);

/*! @relates RF_DOMAIN
 * Frees all memory of domain. Including elements. Only destroy a domain if the domain is not
 * used by some other struct like RF_RELATION anymore!
 * @param domain Domain that needs to be destroyed.
 */
void         rf_domain_destroy(RF_DOMAIN *domain);

/*! Search element by name.
 *
 * @relates RF_DOMAIN
 * @param domain The domain from where the element should be retrieved. If 0 the function will fail.
 * @param name Name of the element searched. If 0 the function will fail.
 * @return On success the function returns a pointer to the element. That element should not be
 *			changed or destroyed!
 * @return 0 on error
 */
RF_ELEMENT * rf_domain_get_element(RF_DOMAIN *domain, char *name);

/*!
Returns an element from the given domain and position.

@relates RF_DOMAIN
@param[in] domain The domain containing the element.
@param pos The position of the element.
@return The element in question. The returned element is must not be modified by the caller! It
	is only valid as long as the domain exist!
@return 0 on error.
*/
RF_ELEMENT * rf_domain_get_element_by_position(RF_DOMAIN *domain, int pos);

/*! Get number of elements in the domain
 *
 * @relates RF_DOMAIN
 * @param domain The domain from which the elementcount should be returned
 * @return The element count for given domain.
 * @return -1 on error.
 */
int          rf_domain_get_element_count(RF_DOMAIN *domain);

/*!
 * Returns a list that the names of the elements contains. The names must not be changed
 * or deleted! The list itself must be destroyed with rf_list_destroy(list, 0) if not needed anymore.
 * @relates RF_DOMAIN
 * @param domain The domain from which the names should be retrieved.
 * @return list with names of the elements belonging to the given domain. The names must not be changed
 * 			or deleted! The list itself must be destroyed with rf_list_destroy(list, 0) if not needed anymore.
 * @return 0 on error.
 */
RF_LIST *    rf_domain_get_element_names(RF_DOMAIN *domain);

/*!
 * Returns the position of the element with the given name
 *
 * @relates RF_DOMAIN
 * @param domain The domain that contains the elements.
 * @param name The name of the element from whom the position should be retrieved.
 * @return The position of the given element.
 * @return is < 0 on error.
 */
int          rf_domain_get_element_position(RF_DOMAIN *domain, char *element);

/*! Returns list of the elements conatined by the domain
 *
 * @relates RF_DOMAIN
 * @param domain The domain that contains the elements
 * @return The list containing the elements (RF_ELEMENT *) of the given domain. The list and its
 * containing data must not be changed or destroyed!
 * @return 0 on error.
 */
RF_LIST *    rf_domain_get_list(RF_DOMAIN *domain);

/*!
 * @relates RF_DOMAIN
 * @param domain The domain from which the name should be retrieved.
 * @return The name of the domain. The name should not be changed or deleted!
 * @return 0 on error.
 */
char *       rf_domain_get_name(RF_DOMAIN *domain);

/*!
 @relates RF_DOMAIN
 @param domain The domain to work on.
 @param name The name of the element in question.
 @return true if the element belongs to the domain.
 @return false if the element does not belong to the domain or on error.
 */
bool         rf_domain_has_element(RF_DOMAIN *domain, char *name);

/*!
 @relates RF_DOMAIN
 @param domain The domain to work on.
 @param The name to compare
 @return true if it is the name of the domain.
 @return false it it is not the name of the domain or on error.
 */
bool         rf_domain_has_name(RF_DOMAIN *domain, char *name);

/*!
Tests if the domain1 is subdomain of domain2.

@relates RF_DOMAIN
@param[in] domain1 The subdomain.
@param[in] domain2 The domain the subdomain is part of.
@return true if domain is subdomain of domain2.
@return false if not or error.
*/
bool         rf_domain_is_partof(RF_DOMAIN *domain1, RF_DOMAIN *domain2);

/*! Sets the elements for the given domain.
 @relates RF_DOMAIN
 @param domain The domain to work on.
 @param list A list with elements (RF_ELEMENT *) inside. The list and its contained data is not
        valid for the user anymore after the function returned!
 */
void         rf_domain_set_list(RF_DOMAIN *domain, RF_LIST *list);

/*!
 @relates RF_DOMAIN
 @param domain The domain whoms name is to be set.
 @param name The new name. If 0 the domain will have no name.
 */
void         rf_domain_set_name(RF_DOMAIN *domain, char *name);


/*!
 @relates RF_ELEMENT
 @param[in] element The element that should be copied.
 @return The copy of the given element.
 @return 0 on error.
 */
RF_ELEMENT * rf_element_copy(RF_ELEMENT *element);

/*!
 @relates RF_ELEMENT
 @param type The type of the new element. Must be a value from ::rf_enum_et.
 @param data Depends on the value of type:
            - RF_ET_DOMAIN: pointer to domain (RF_DOMAIN *)
            - RF_ET_GLOBAL_DOMAIN: pointer to domain (RF_DOMAIN *). This domain will not be destroyed by a call to rf_element_destroy().
            - RF_ET_NAME: pointer to name (char *)
 @return New element. The user must destroy it with rf_element_destroy() if it is not needed anymore!
 */
RF_ELEMENT * rf_element_create(int type, void *data);

/*!
 @relates RF_ELEMENT
 @param element The element to be destroyed.
 */
void         rf_element_destroy(RF_ELEMENT *element);

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
void *       rf_element_get_data(RF_ELEMENT *element);

/*!
 @relates RF_ELEMENT
 @param element The element whoms name should be retrieved.
 @return The name of the element. Should not be changed or deleted!
 @return 0 on error.
 */
char *       rf_element_get_name(RF_ELEMENT *element);

/*!
 @relates RF_ELEMENT
 @param element The element whoms type should be retrieved.
 @return value of ::rf_enum_et.
 @return is < 0 on error.
 */
int          rf_element_get_type(RF_ELEMENT *element);


#endif
