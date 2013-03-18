/* 
 * Copyright (C) 2011,2013 Peter Berger, Wilke Schwiedop
 *
 * This file is part of RelaFix.
 *
 * RelaFix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RelaFix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser Public License
 * along with RelaFix.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file
 Functions to work with relations.

 Declares functions to work on struct rf_Relation. rf_Relation represents a mathematical
 relation. In the Relafix language relations are used in the form 'a leq b'. Here leq
 is the relation.
 */

#ifndef RF_RELATION_H
#define RF_RELATION_H

#include <stdbool.h>

#include "set.h"
#include "error.h"

typedef struct _rf_relation rf_Relation;

struct _rf_relation {
        rf_Set        **domains;
        bool          *table;
};


bool            rf_relation_calc(rf_Relation *relation, rf_SetElement *element1, rf_SetElement *element2, rf_Error *error);


rf_Relation *   rf_relation_new(rf_Set *domain1, rf_Set *domain2, bool *table);
rf_Relation *   rf_relation_copy(const rf_Relation *relation);

rf_Relation *   rf_relation_new_empty(rf_Set *domain1, rf_Set *domain2);
rf_Relation *   rf_relation_new_full(rf_Set *domain1, rf_Set *domain2);
rf_Relation *   rf_relation_new_id(rf_Set *domain);
rf_Relation *   rf_relation_new_top(rf_Set *domain);
rf_Relation *   rf_relation_new_bottom(rf_Set *domain);
rf_Relation *   rf_relation_new_union(rf_Relation *relation_1, rf_Relation *relation_2, rf_Error *error);
rf_Relation *   rf_relation_new_intersection(rf_Relation *relation_1, rf_Relation *relation_2, rf_Error *error);
rf_Relation *   rf_relation_new_complement(rf_Relation *relation, rf_Error *error);
rf_Relation *   rf_relation_new_concatenation(rf_Relation *relation_1, rf_Relation *relation_2, rf_Error *error);
rf_Relation *   rf_relation_new_converse(rf_Relation *relation, rf_Error *error);
rf_Relation *   rf_relation_new_subsetleq(rf_Set *domain, rf_Error *error);

bool            rf_relation_is_homogeneous(const rf_Relation *relation);

bool            rf_relation_is_antisymmetric(const rf_Relation *relation);
bool            rf_relation_is_asymmetric(const rf_Relation *relation);
bool            rf_relation_is_difunctional(const rf_Relation *relation);
bool            rf_relation_is_equivalent(const rf_Relation *relation);
bool            rf_relation_is_irreflexive(const rf_Relation *relation);
bool            rf_relation_is_partial_order(const rf_Relation *relation);
bool            rf_relation_is_preorder(const rf_Relation *relation);
bool            rf_relation_is_reflexive(const rf_Relation *relation);
bool            rf_relation_is_symmetric(const rf_Relation *relation);
bool            rf_relation_is_transitive(const rf_Relation *relation);

rf_SetElement * rf_relation_find_infimum(const rf_Relation *relation, const rf_Set *domain, rf_Error *error);
rf_SetElement * rf_relation_find_maximum(const rf_Relation *relation, rf_Error *error);
rf_SetElement * rf_relation_find_minimum(const rf_Relation *relation, rf_Error *error);
rf_SetElement * rf_relation_find_supremum(const rf_Relation *relation, const rf_Set *domain, rf_Error *error);
rf_Set *        rf_relation_find_upperbound(const rf_Relation *relation, const rf_Set *domain, rf_Error *error);
rf_Set *        rf_relation_find_lowerbound(const rf_Relation *relation, const rf_Set *domain, rf_Error *error);

bool            rf_relation_make_antisymmetric(rf_Relation *relation, bool upper, rf_Error *error);
bool            rf_relation_make_asymmetric(rf_Relation *relation, bool upper, rf_Error *error);
bool            rf_relation_make_difunctional(rf_Relation *relation, bool fill, rf_Error *error);
bool            rf_relation_make_equivalent(rf_Relation *relation, bool fill, rf_Error *error);
bool            rf_relation_make_irreflexive(rf_Relation *relation, rf_Error *error);
bool            rf_relation_make_partial_order(rf_Relation *relation, bool fill, rf_Error *error);
bool            rf_relation_make_preorder(rf_Relation *relation, bool fill, rf_Error *error);
bool            rf_relation_make_reflexive(rf_Relation *relation, rf_Error *error);
bool            rf_relation_make_symmetric(rf_Relation *relation, bool fill, rf_Error *error);
bool            rf_relation_make_transitive(rf_Relation *relation, bool fill, rf_Error *error);

void            rf_relation_free(rf_Relation *relation);

#endif
