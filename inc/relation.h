/*
 * RelaFixLib    Relation
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 Functions to work with relations.

 Declares functions to work on struct RF_RELATION. RF_RELATION represents a mathematical
 relation. In the Relafix language relations are used in the form 'a leq b'. Here leq
 is the relation.
 */

#ifndef RF_RELATION_H
#define RF_RELATION_H

#include <stdbool.h>

#include "error.h"
#include "table.h"
#include "domain.h"

/*! Represents a mathematical relation

 An relation has a name and is based on 2 domains (domain_1 -> domain_2). The relation also has a table with
 all possible solutions. If the relation needs to be calculated, the solution is just picked
 from the right position in the table. The relation has the form "left_element relation right_element" in the Relafix language.
 */
typedef struct rf_relation
{
  char      *name;        /*!< The name of the relation */
  RF_DOMAIN *domain_1;    /*!< The domain from which the left element must be. */
  RF_DOMAIN *domain_2;    /*!< The domain from which the right element must be. */
  RF_TABLE  *table;        /*!< Contains the solutions in form of element names from domain boolean. The type of the table is RF_TT_BINARY. */
} RF_RELATION;


/*!
 Checks if a given relation of form aRb exists.
 @relates RF_RELATION
 @param[in] relation The relation used for the calculation.
 @param[in] element_1 The left argument.
 @param[in] element_2 The right argument.
 @param[out] element_out If the function succeeds, it contains the resulting element.
             Then the string must not be changed or freed by the user!
 @result on success 0 (no) or 1 (yes)
 @result Any other number on error. An description is written to element_out. This description must be freed by the user!
 */
int           rf_relation_calc(RF_RELATION *relation, char *element_1, char *element_2, RF_ERROR *error);

/*!
 @relates RF_RELATION
 @param[in] relation The relation that should be copied.
 @return The copy of the given relation.
 @return 0 on error.
 */
RF_RELATION * rf_relation_copy(RF_RELATION *relation);


/*!
 @relates RF_RELATION
 @param[in] name The name of the new relation. 0 is allowed. Gets invalid for the caller.
 @param[in] domain_1 The domain of the left argument.
 @param[in] domain_2 The domain of the right argument.
 @param[in] domain_3 The domain containing the elements of the solution.
 @return The new relation
 @return 0 on error
 */
RF_RELATION * rf_relation_create(char *name, RF_DOMAIN *domain_1, RF_DOMAIN *domain_2);

/*!
Creates a new relation with the bottom triangle of the table filled.

@relates RF_RELATION
@param[in] domain The domain the new relation is based on
@param[out] error On fail an errormessage is written here
@return The new relation based on domain.
@return 0 on fail. An errormessage is written in error.
*/
RF_RELATION * rf_relation_create_bottom(RF_DOMAIN *domain, RF_ERROR *error);

/*!
Creates the complement of the given relation

@relates RF_RELATION
@param[in] relation The relation from which the complement should be created
@param[out] error On fail an errormessage will be written here. The message must be freed by the caller.
@return On success a new relation, the complement of the given, is returned.
@return 0 on fail. An errormessage is written to error.
*/
RF_RELATION * rf_relation_create_complement(RF_RELATION *relation, RF_ERROR *error);

/*!
Creates the concatenation of two given relations.

This means, that: S = {<x, z> | x -> A, z -> C, it exists y -> B: xRy and yRz}
The second domain of the first relation and the first domain of the second relation must be the same!
@relates RF_RELATION
@param[in] relation_1 the first relation used in the concatenation
@param[in] relation_2 the second relation used in the concatenation
@param[out] error If the function fails an errormessage is written here
@return The new relation that is the concatenation of the given ones.
@return 0 on fail. An errormessage is written to error.
*/
RF_RELATION * rf_relation_create_concatenation(RF_RELATION *relation_1, RF_RELATION *relation_2, RF_ERROR *error);

/*!
Creates the converse of the given relation

@relates RF_RELATION
@param[in] relation The relation from which the complement should be created
@param[out] error On fail an errormessage will be written here. The message must be freed by the caller.
@return On success a new relation, the converse of the given, is returned.
@return 0 on fail. An errormessage is written to error.
*/
RF_RELATION * rf_relation_create_converse(RF_RELATION *relation, RF_ERROR *error);

/*!
Creates a new empty relation

@relates RF_RELATION
@param [in] x in xRy is from this domain
@param [in] y in xRy is from this domain
@param [out] error On fail an errormessage is written here
@return The new empty relation based on domain_1 und domain_2.
@return 0 on fail. An errormessage is written in error.
*/
RF_RELATION * rf_relation_create_empty(RF_DOMAIN *domain_1, RF_DOMAIN *domain_2, RF_ERROR *error);

/*!
Creates a new relation with all xRy existing.

@relates RF_RELATION
@param [in] x in xRy is from this domain
@param [in] y in xRy is from this domain
@param [out] error On fail an errormessage is written here
@return The new relation based on domain_1 und domain_2.
@return 0 on fail. An errormessage is written in error.
*/
RF_RELATION * rf_relation_create_full(RF_DOMAIN *domain_1, RF_DOMAIN *domain_2, RF_ERROR *error);

/*!
Creates a homogeneous relation with only xRx

@relates RF_RELATION
@param[in] domain The domain the new relation is based on
@param[out] error On Error a errormessage will be written
@return The new relation
@return 0 on fail. error will contain a message.
*/
RF_RELATION * rf_relation_create_id(RF_DOMAIN *domain, RF_ERROR *error);

/*!
Creates the intersection of two given relations.

Both relations must have the same domains!
@relates RF_RELATION
@param[in] relation_1 The first relation to intersect
@param[in] relation_2 The second relation to intersect
@param[out] error On fail an errormessage will be written here. This message must be freed by the caller then.
@return A new relation, that is the intersection of the given relations.
@return 0 on fail. An errormessage is written to error.
*/
RF_RELATION * rf_relation_create_intersection(RF_RELATION *relation_1, RF_RELATION *relation_2, RF_ERROR *error);

/*!
Creates a new relation with with subsets leq.

@relates RF_RELATION
@param[in] domain The domain the new relation is based on
@param[out] error On fail an errormessage is written here
@return The new relation based on domain.
@return 0 on fail. An errormessage is written in error.
*/
RF_RELATION * rf_relation_create_subsetleq(RF_DOMAIN *domain, RF_ERROR *error);

/*!
Creates a new relation with the top of the table filled.

@relates RF_RELATION
@param[in] domain The domain the new relation is based on
@param[out] error On fail an errormessage is written here
@return The new relation based on domain.
@return 0 on fail. An errormessage is written in error.
*/
RF_RELATION * rf_relation_create_top(RF_DOMAIN *domain, RF_ERROR *error);

/*!
Creates the union of two given relations.

Both relations must have the same domains!
@relates RF_RELATION
@param[in] relation_1 The first relation to unite
@param[in] relation_2 The second relation to unite
@param[out] error On fail an errormessage will be written here. This message must be freed by the caller then.
@return A new relation, that is the union of the given relations.
@return 0 on fail. An errormessage is written to error.
*/
RF_RELATION * rf_relation_create_union(RF_RELATION *relation_1, RF_RELATION *relation_2, RF_ERROR *error);

/*!
Frees all data of the relation.

@relates RF_RELATION
@param[in] relation The relations to be freed.
*/
void          rf_relation_destroy(RF_RELATION *relation);

/*!
Finds the infimum for the given elements if it exists.

@relates RF_RELATION
@param[in] relation The less equal relation the elements must be from.
@param[in] domain The elements with infimum in question.
@param[out] error If the function fails an errormessage is written here
@return The name of the infimum. The name must not be modified by the user and will get invalid when the given relation gets invalid.
@return If no infimum exist "no infimum" is returned.
@return 0 on fail. An errormessage is written to error.
*/
char *        rf_relation_find_infimum(RF_RELATION *relation, RF_DOMAIN *domain, RF_ERROR *error);

/*!
Searches a set of lowerbounds for the given elements.

@relates RF_RELATION
@param[in] relation An lessequal relation the elements must be from
@param[in] domain The elements for which the lowerbound should be searched
@param[out] error On fail an errormessage is written here
@return A temporary domain with the elements that are a lowerbound to the given elements.
@return 0 on fail. An errormessage is writen to error.
*/
RF_DOMAIN *   rf_relation_find_lowerbound(RF_RELATION *relation, RF_DOMAIN *domain, RF_ERROR *error);

/*!
Finds the maximum of an partial ordered relation.

If there does not exist a maximum, an error will appear.
@relates RF_RELATION
@param[in] relation a partial ordered relation whoes maximum is in question
@param[out] error on fail, an errormessage is written here
@return If a maximum exists, a pointer to the elementname is returned. The name must not be changed
		or freed by the user! The name is only valid as long as the relation exists.
@return 0 on fail or when no maximum exists. An errormessage is written to error.
*/
char *        rf_relation_find_maximum(RF_RELATION *relation, RF_ERROR *error);

/*!
Finds the minimum of an partial ordered relation.

If there does not exist a minimum, an error will appear.
@relates RF_RELATION
@param[in] relation a partial ordered relation whoes minimum is in question
@param[out] error on fail, an errormessage is written here
@return If a minimum exists, a pointer to the elementname is returned. The name must not be changed
		or freed by the user! The name is only valid as long as the relation exists.
@return 0 on fail or when no minimum exists. An errormessage is written to error.
*/
char *        rf_relation_find_minimum(RF_RELATION *relation, RF_ERROR *error);

/*!
Finds the supremum for the given elements if it exists.

@relates RF_RELATION
@param[in] relation The less equal relation the elements must be from.
@param[in] domain The elements with supremum in question.
@param[out] error If the function fails an errormessage is written here
@return The name of the supremum. The name must not be modified by the user and will get invalid when the given relation gets invalid.
@return If no supremum exist "no supremum" is returned.
@return 0 on fail. An errormessage is written to error.
*/
char *        rf_relation_find_supremum(RF_RELATION *relation, RF_DOMAIN *domain, RF_ERROR *error);

/*!
Searches a set of upperbounds for the given elements.

@relates RF_RELATION
@param[in] relation An lessequal relation the elements must be from
@param[in] domain The elements for which the upperbound should be searched
@param[out] error On fail an errormessage is written here
@return A temporary domain with the elements that are an upperbound to the given elements.
@return 0 on fail. An errormessage is writen to error.
*/
RF_DOMAIN *   rf_relation_find_upperbound(RF_RELATION *relation, RF_DOMAIN *domain, RF_ERROR *error);

/*!
Returns the domain x is from in xRy.

@relates RF_RELATION
@param[in] relation The relation whoes domain is in question.
@return The first domain.
@return 0 on error
*/
RF_DOMAIN *   rf_relation_get_domain_1(RF_RELATION *relation);

/*!
Returns the domain y is from in xRy.

@relates RF_RELATION
@param[in] relation The relation whoes domain is in question.
@return The second domain.
@return 0 on error
*/
RF_DOMAIN *   rf_relation_get_domain_2(RF_RELATION *relation);

/*!
Returns the name of the relation.

@relates RF_RELATION
@param[in] relation The relation whoes name is in question.
@return A pointer to the name. Must not be modified by the caller! Is only valid as long as the relation exists.
@return If the relation has no name 'no name' is returned as name.
@return 0 on error.
*/
char *        rf_relation_get_name(RF_RELATION *relation);

/*!
Returns the table of the relation.

The table contains the set of relations.
@relates RF_RELATION
@param[in] relation The relation whoes table is in question.
@return The table that contains the set of relations. Must not be freed by the caller! Is only
		valid as long as the relation is valid.
@return 0 on fail.
*/
RF_TABLE *    rf_relation_get_table(RF_RELATION *relation);

/*!
Tests if the given name is the name of the relation.

@relates RF_RELATION
@param[in] relation The relation whoes name should be compared.
@param[in] name The name to be compared.
@return true if the given name is the name of the relation.
@return false if not or on error.
*/
bool          rf_relation_has_name(RF_RELATION *relation, char *name);

/*!
Tests if a relation is antisymmetric.

@relates RF_RELATION
@param[in] relation The relation to be tested.
@return 1 if the relation is antisymmetric.
@return 0 if the relation is not antisymmetric.
@return >1 on error
*/
int           rf_relation_is_antisymmetric(RF_RELATION *relation);

/*!
Tests if a relation is asymmetric.

@relates RF_RELATION
@param[in] relation The relation to be tested.
@return 1 if the relation is asymmetric.
@return 0 if the relation is not asymmetric.
@return >1 on error
*/
int           rf_relation_is_asymmetric(RF_RELATION *relation);

/*!
Tests if a relation is difunctional.

@relates RF_RELATION
@param[in] relation The relation to be tested.
@return 1 if the relation is difunctional.
@return 0 if the relation is not difunctional.
@return >1 on error
*/
int           rf_relation_is_difunctional(RF_RELATION *relation);

/*!
Tests if a relation is equivalent.

@relates RF_RELATION
@param[in] relation The relation to be tested.
@return 1 if the relation is equivalent.
@return 0 if the relation is not equivalent.
@return >1 on error
*/
int           rf_relation_is_equivalent(RF_RELATION *relation);

/*!
Tests if a relation is irreflexive.

@relates RF_RELATION
@param[in] relation The relation to be tested.
@return 1 if the relation is irreflexive.
@return 0 if the relation is not irreflexive.
@return >1 on error
*/
int           rf_relation_is_irreflexive(RF_RELATION *relation);

/*!
Tests if a relation is poset.

@relates RF_RELATION
@param[in] relation The relation to be tested.
@return 1 if the relation is poset.
@return 0 if the relation is not poset.
@return >1 on error
*/
int           rf_relation_is_poset(RF_RELATION *relation);

/*!
Tests if a relation is preorder.

@relates RF_RELATION
@param[in] relation The relation to be tested.
@return 1 if the relation is preorder.
@return 0 if the relation is not preorder.
@return >1 on error
*/
int           rf_relation_is_preorder(RF_RELATION *relation);

/*!
Tests if a relation is reflexive.

@relates RF_RELATION
@param[in] relation The relation to be tested.
@return 1 if the relation is reflexive.
@return 0 if the relation is not reflexive.
@return >1 on error
*/
int           rf_relation_is_reflexive(RF_RELATION *relation);

/*!
Tests if a relation is symmetric.

@relates RF_RELATION
@param[in] relation The relation to be tested.
@return 1 if the relation is symmetric.
@return 0 if the relation is not symmetric.
@return >1 on error
*/
int           rf_relation_is_symmetric(RF_RELATION *relation);

/*!
Tests if a relation is transitive.

@relates RF_RELATION
@param[in] relation The relation to be tested.
@return 1 if the relation is transitive.
@return 0 if the relation is not transitive.
@return >1 on error
*/
int           rf_relation_is_transitive(RF_RELATION *relation);

/*!
Set the name of the given relation.

The old name gets invalid!
@relates RF_RELATION
@param[in] relation The relation whoes name should be set.
@param[in] name The new name for the relation. The name will be freed by the relation!
*/
void          rf_relation_set_name(RF_RELATION *relation, char *name);

/*!
Set the table of the given relation.

The old table gets invalid!
@relates RF_RELATION
@param[in] relation The relation whoes table should be set.
@param[in] name The new table for the relation. The table will be freed by the relation!
*/
void          rf_relation_set_table(RF_RELATION *relation, RF_TABLE *table);

int           rf_relation_gain_symmetric(RF_RELATION* relation, bool fill);

int           rf_relation_gain_antisymmetric(RF_RELATION* relation, bool upper);

int           rf_relation_gain_irreflexive(RF_RELATION* relation);

int           rf_relation_gain_reflexive(RF_RELATION* relation);

int           rf_relation_gain_transitive(RF_RELATION* relation, bool fill);

int           rf_relation_gain_difunctional(RF_RELATION* relation, bool fill);

int           rf_relation_gain_equivalent(RF_RELATION* relation, bool fill);

int           rf_relation_gain_poset(RF_RELATION* relation, bool fill);

int           rf_relation_gain_preorder(RF_RELATION* relation, bool fill);

int           rf_relation_gain_asymmetric(RF_RELATION* relation, bool upper);


#endif
