/*
 * Copyright (C) 2011,2013 Peter Berger, Wilke Schwiedop, Sebastian Pospiech
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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "relation.h"
#include "tools.h"

#define N_DOMAINS 2

/*
 * table[x][y][z] = x * r->domains[0]->cardinality + y * r->domains[1]->cardinality + z
 */
size_t
rf_table_idx(const rf_Relation *r, ...) {
	assert(r != NULL);

	size_t idx = 0;
	va_list argv;
	va_start(argv, r);
	int i;
	int offset;
	for(i = 0; i < N_DOMAINS-1; ++i) {
		offset = va_arg(argv, int);
		assert(offset < r->domains[i]->cardinality);
		idx += r->domains[i]->cardinality * offset;
	}
	offset = va_arg(argv, int);
	assert(offset < r->domains[i]->cardinality);
	idx += offset;
	va_end(argv);

	return idx;
}

bool
rf_relation_calc(rf_Relation *r, rf_SetElement *e1, rf_SetElement *e2, rf_Error *error) {
	assert(r != NULL);
	assert(e1 != NULL);
	assert(e2 != NULL);

	int x = rf_set_get_element_index(r->domains[0], e1);
	if(error != NULL && x < 0) {
		rf_error_set(error, RF_E_SET_NOT_MEMBER, "");
		return false;
	}
	int y = rf_set_get_element_index(r->domains[1], e2);
	if(error != NULL && y < 0) {
		rf_error_set(error, RF_E_SET_NOT_MEMBER, "");
		return false;
	}

	return r->table[rf_table_idx(r, x, y)];
}



rf_Relation *
rf_relation_new(rf_Set *d1, rf_Set *d2, bool *table) {
	assert(d1 != NULL);
	assert(d2 != NULL);
	assert(table != NULL);

	rf_Relation *r = malloc(sizeof(*r));
	r->domains = calloc(N_DOMAINS, sizeof(*r->domains));
	r->domains[0] = d1;
	r->domains[1] = d2;
	r->table = table;

	return r;
}

rf_Relation *
rf_relation_clone(const rf_Relation *r) {
	assert(r != NULL);

	rf_Set *d[N_DOMAINS];
	size_t table_size = 1;
	for(int i = N_DOMAINS-1; i >= 0; --i) {
		d[i] = rf_set_clone(r->domains[i]);
		table_size *= r->domains[i]->cardinality;
	}
	bool *table = calloc(table_size, sizeof(*table));

	rf_Relation *new = rf_relation_new(d[0], d[1], table);
	memcpy(new->table, r->table, table_size);

	return new;
}


rf_Relation *
rf_relation_new_empty(rf_Set *d1, rf_Set *d2) {
	assert(d1 != NULL);
	assert(d2 != NULL);

	size_t table_size = d1->cardinality * d2->cardinality;
	bool *table = calloc(table_size, sizeof(*table));

	rf_Relation *new = rf_relation_new(d1, d2, table);
	memset(new->table, false, table_size);

	return new;
}

rf_Relation *
rf_relation_new_full(rf_Set *d1, rf_Set *d2) {
	assert(d1 != NULL);
	assert(d2 != NULL);

	size_t table_size = d1->cardinality * d2->cardinality;
	bool *table = calloc(table_size, sizeof(*table));

	rf_Relation *new = rf_relation_new(d1, d2, table);
	memset(new->table, true, table_size);

	return new;
}

rf_Relation *
rf_relation_new_id(rf_Set *d) {
	assert(d != NULL);

	size_t table_size = d->cardinality * d->cardinality;
	bool *table = calloc(table_size, sizeof(*table));

	rf_Relation *new = rf_relation_new_empty(d, d);

	const int dim = new->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		new->table[rf_table_idx(new, x, x)] = true;
	}

	return new;
}

// includes the (x,x) elements
rf_Relation *
rf_relation_new_top(rf_Set *d) {
	assert(d != NULL);

	rf_Relation *new = rf_relation_new_empty(d, d);

	const int dim = new->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		for(int y = dim-1; y >= x; --y) {
			new->table[rf_table_idx(new, x, y)] = true;
		}
	}

	return new;
}

// includes the (x,x) elements
rf_Relation *
rf_relation_new_bottom(rf_Set *d) {
	assert(d != NULL);

	rf_Relation *new = rf_relation_new_empty(d, d);

	const int dim = new->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		for(int y = x; y >= 0; --y) {
			new->table[rf_table_idx(new, x, y)] = true;
		}
	}

	return new;
}

rf_Relation *
rf_relation_new_union(rf_Relation *r1, rf_Relation *r2, rf_Error *error) {
	assert(r1 != NULL);
	assert(r2 != NULL);

	for(int i = N_DOMAINS-1; i >= 0; --i) {
		if(!rf_set_equal(r1->domains[i], r2->domains[i])) {
			if(error != NULL) {
				rf_error_set(error, RF_E_GENERIC, "Domains of r1 and r2 differ");
			}
			return NULL;
		}
	}

	rf_Relation *new = rf_relation_new_empty(r1->domains[0], r1->domains[1]);

	const int dim = new->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		for(int y = dim-1	; y >= 0; --y) {
			new->table[rf_table_idx(new, x, y)] = r1->table[rf_table_idx(r1, x, y)] || r2->table[rf_table_idx(r2, x, y)];
		}
	}

	return new;
}

rf_Relation *
rf_relation_new_intersection(rf_Relation *r1, rf_Relation *r2, rf_Error *error) {
	assert(r1 != NULL);
	assert(r2 != NULL);

	for(int i = N_DOMAINS-1; i >= 0; --i) {
		if(!rf_set_equal(r1->domains[i], r2->domains[i])) {
			if(error != NULL) {
				rf_error_set(error, RF_E_GENERIC, "Domains of r1 and r2 differ");
			}
			return NULL;
		}
	}

	rf_Relation *new = rf_relation_new_empty(r1->domains[0], r1->domains[1]);

	const int dim = new->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		for(int y = x; y >= 0; --y) {
			new->table[rf_table_idx(new, x, y)] = r1->table[rf_table_idx(r1, x, y)] && r2->table[rf_table_idx(r2, x, y)];
		}
	}

	return new;
}

rf_Relation *
rf_relation_new_complement(rf_Relation *r, rf_Error *error) {
	assert(r != NULL);

	rf_Relation *new = rf_relation_clone(r);

	const size_t table_size = new->domains[0]->cardinality * new->domains[1]->cardinality;
	for(int i = table_size-1; i >= 0; --i) {
		new->table[i] = !new->table[i];
	}

	return new;
}

rf_Relation *
rf_relation_new_concatenation(rf_Relation *r1, rf_Relation *r2, rf_Error *error) {
	assert(r1 != NULL);
	assert(r2 != NULL);

	if(!rf_set_equal(r1->domains[1], r2->domains[0])) {
		if(error != NULL) {
			rf_error_set(error, RF_E_GENERIC, "Domains of r1->domain1 and r2->domain0 differ");
		}
		return NULL;
	}

	rf_Relation *new = rf_relation_new_empty(r1->domains[0], r2->domains[1]);

	for(int x = r1->domains[1]->cardinality-1; x >= 0; --x) {
		for(int y = r1->domains[0]->cardinality-1; y >= 0; --y) {
			if(!r1->table[rf_table_idx(r1, x, y)])
				continue;

			for(int z = r2->domains[1]->cardinality-1; z >= 0; --z) {
				if(r2->table[rf_table_idx(r2, y,z)]) {
						new->table[rf_table_idx(new,x,z)] = true;
				}
			}
		}
	}

	return new;
}

rf_Relation *
rf_relation_new_converse(const rf_Relation *r, rf_Error *error) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL)
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		return NULL;
	}

	rf_Relation *new = rf_relation_new_empty(r->domains[0], r->domains[1]);

	const int dim = new->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		for(int y = dim-1; y >= 0; --y) {
			new->table[rf_table_idx(new, x, y)] = r->table[rf_table_idx(r, y, x)];
		}
	}

	return new;
}

rf_Relation *
rf_relation_new_subsetleq(rf_Set *d, rf_Error *error) {

	if(d == NULL) {
		if(error != NULL)
			rf_error_set(error, RF_E_NULL_ARG, "Can't create a relation without a set");
		return NULL;
	}

	rf_Relation *subsetleq = rf_relation_new_empty(d, d);
	for(int x = 0; x < d->cardinality; x++) {
			if(d->elements[x]->type == RF_SET_ELEMENT_TYPE_SET) {
				for(int y = 0; y < d->cardinality; y++) {
		if(x==y) {
			subsetleq->table[rf_table_idx(subsetleq,x,y)] = true;
		}
		else if(d->elements[y]->type == RF_SET_ELEMENT_TYPE_SET) {
			rf_Set *set1 = d->elements[y]->value.set;
			rf_Set *set2 = d->elements[x]->value.set;
			subsetleq->table[rf_table_idx(subsetleq,x,y)] = rf_set_is_subset(set1, set2) ? true : false;
		}
				}
			}

	}

	return subsetleq;
}


bool
rf_relation_is_homogeneous(const rf_Relation *r) {
	assert(r != NULL);

	for(int i = N_DOMAINS-2; i >= 0; --i)
		if(!rf_set_equal(r->domains[i], r->domains[i+1]))
			return false;

	return true;
}

// xRy => !yRx
bool
rf_relation_is_antisymmetric(const rf_Relation *r) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r))
		return false;

	const int dim = r->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		for(int y = dim-1; y > x; --y) {
			if(r->table[rf_table_idx(r, x, y)] && r->table[rf_table_idx(r, y, x)])
				return false;
			// xRy exists but yRx doesn't or yRx exists but xRy doesn't
		}
	}
	return true;
}

bool
rf_relation_is_asymmetric(const rf_Relation *r) {
	assert(r != NULL);

	return !rf_relation_is_reflexive(r) && rf_relation_is_antisymmetric(r);
}

// xRy & zRy & zRw => xRw
bool
rf_relation_is_difunctional(const rf_Relation *r) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r))
		return false;

	const int dim = r->domains[0]->cardinality;

	for(int y = 0; y < dim; y++) {
		for(int x = 0; x < dim; x++) {
				if(r->table[rf_table_idx(r,x,y)] == true) {
		for(int z=x+1;z<dim;z++) {
			if(r->table[rf_table_idx(r,z,y)] == true) {
				//here we have xRy & zRy, now we test for the images of x and z
				for(int i = 0;i<dim;i++) {
					if(r->table[rf_table_idx(r,z,i)] != r->table[rf_table_idx(r,x,i)]) {
			return false;
					}
				}
			}
		}
				}
		}
	}

	return true;
}

bool
rf_relation_is_equivalent(const rf_Relation *r) {
	assert(r != NULL);

	return rf_relation_is_reflexive(r) && rf_relation_is_symmetric(r) && rf_relation_is_transitive(r);
}

bool
rf_relation_is_irreflexive(const rf_Relation *r) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r))
		return false;

	const int dim = r->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		if(r->table[rf_table_idx(r, x, x)])
			return false;
	}

	return true;
}

bool
rf_relation_is_partial_order(const rf_Relation *r) {
	assert(r != NULL);

	return rf_relation_is_reflexive(r) && rf_relation_is_antisymmetric(r) && rf_relation_is_transitive(r);
}

bool
rf_relation_is_preorder(const rf_Relation *r) {
	assert(r != NULL);

	return rf_relation_is_reflexive(r) && rf_relation_is_transitive(r);
}

// xRx
bool
rf_relation_is_reflexive(const rf_Relation *r) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r))
		return false;

	const int dim = r->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		if(!r->table[rf_table_idx(r, x, x)])
			return false;
		// xRx exists
		assert(r->table[rf_table_idx(r, x, x)]);
	}

	return true;
}

bool
rf_relation_is_symmetric(const rf_Relation *r) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r))
		return false;

	const int dim = r->domains[0]->cardinality;

	for(int x = 0; x < dim; x++) {
		for(int y = x; y < dim; y++) { //don't need to check mirror elements twice
			if(x == y) //don't need to check elements related to themself
				continue;
			if(r->table[rf_table_idx(r,x,y)] && !r->table[rf_table_idx(r,y,x)])
				return false;
		}
	}

	return true;
}

// xRy & yRz => xRz
bool
rf_relation_is_transitive(const rf_Relation *r) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r))
		return false;

	const int dim = r->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		for(int y = dim-1; y >= 0; --y) {
			if(x == y)
				continue;
			if(!r->table[rf_table_idx(r, x, y)])
				continue;
			// xRy exists
			assert(r->table[rf_table_idx(r, x, y)]);
			for(int z = dim-1; z >= 0; --z) {
				if(!r->table[rf_table_idx(r, y, z)])
					continue;
				// yRz exists
				assert(r->table[rf_table_idx(r, y, z)]);
				if(!r->table[rf_table_idx(r, x, z)])
					return false;
				// xRz exists
				assert(r->table[rf_table_idx(r, x, z)]);
			}
		}
	}

	return true;
}

rf_Set *
rf_relation_find_minimal_elements(const rf_Relation *r, rf_Set *s, rf_Error *error) {
	assert(r != NULL);

	rf_SetElement **mins = malloc(sizeof(mins)*s->cardinality);
	rf_Set *returnSet = rf_set_new(0, mins);

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL) {
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		}
		return returnSet;
	}
	if(!rf_relation_is_partial_order(r)) {
		if(error != NULL) {
			rf_error_set(error, RF_E_REL_NOT_ORDERED, "");
		}
		return returnSet;
	}

	if(s->cardinality == 1)
		return s;

	const int dimS = s->cardinality;
	for(int x = 0; x < dimS; x++) {
		bool xRx = false;
		bool zRx = false;
		for(int y = 0; y < dimS; y++) {
			int xr = rf_set_get_element_index(r->domains[0],s->elements[x]);
			int yr = rf_set_get_element_index(r->domains[1],s->elements[y]);
			if(r->table[rf_table_idx(r,xr,yr)]) {
				if(xr == yr)
					xRx = true;
				if(xr !=yr)
					zRx = true;
			}
		}
		if(xRx & !zRx) {
			mins[returnSet->cardinality] = rf_set_element_clone(s->elements[x]);
			returnSet->cardinality++;
		}
	}

	return returnSet;
}

rf_SetElement *
rf_relation_find_minimum_within_subset(const rf_Relation *r, rf_Set *s, rf_Error *error) {
	assert(r != NULL);
	assert(s != NULL);

	rf_Set *mins = rf_relation_find_minimal_elements(r, s, error);

	if(mins->cardinality == 1)
		return mins->elements[0];

	return NULL;
}

/**
 * finds the minimum in the given relation, taking all elements of the set into consideration
 */
rf_SetElement *
rf_relation_find_minimum(const rf_Relation *r, rf_Error *error) {
	return rf_relation_find_minimum_within_subset(r,r->domains[0] ,error);
}

rf_Set *
rf_relation_find_maximal_elements(const rf_Relation *r, rf_Set *s, rf_Error *error) {
	assert(r != NULL);

	rf_SetElement **maxs = malloc(sizeof(maxs)*s->cardinality);
	rf_Set *returnSet = rf_set_new(0, maxs);

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL) {
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		}
		return returnSet;
	}
	if(!rf_relation_is_partial_order(r)) {
		if(error != NULL) {
			rf_error_set(error, RF_E_REL_NOT_ORDERED, "");
		}
		return returnSet;
	}

	if(s->cardinality == 1)
		return s;

	const int dimS = s->cardinality;

	for(int y = 0; y < dimS; y++) {
		bool xRx = false;
		bool zRx = false;
		for(int x = 0; x < dimS; x++) {
			int xr = rf_set_get_element_index(r->domains[0],s->elements[x]);
			int yr = rf_set_get_element_index(r->domains[1],s->elements[y]);
			if(r->table[rf_table_idx(r,xr,yr)]) {
				if(xr == yr)
					xRx = true;
				if(xr !=yr)
					zRx = true;
			}
		}
		if(xRx & !zRx) {
			maxs[returnSet->cardinality] = rf_set_element_clone(s->elements[y]);
			returnSet->cardinality++;
		}
	}

	return returnSet;
}

rf_SetElement *
rf_relation_find_maximum_within_subset(const rf_Relation *r, rf_Set *s, rf_Error *error) {
	assert(r != NULL);
	assert(s != NULL);

	rf_Set *maxs = rf_relation_find_maximal_elements(r, s, error);

	if(maxs->cardinality == 1)
		return maxs->elements[0];


	return NULL;
}

/**
 * finds the maximum in the given relation, taking all elements of the set into consideration
 */
rf_SetElement *
rf_relation_find_maximum(const rf_Relation *r, rf_Error *error) {
	return rf_relation_find_maximum_within_subset(r,r->domains[0] ,error);
}


rf_SetElement *
rf_relation_find_supremum(const rf_Relation *r, const rf_Set *domain, rf_Error *error) {
	rf_Set *upperbound = rf_relation_find_upperbound(r, domain, error);

	if(upperbound == NULL)
		return NULL;

	return rf_relation_find_minimum_within_subset(r, upperbound, error);
}

rf_SetElement *
rf_relation_find_infimum(const rf_Relation *r, const rf_Set *domain, rf_Error *error) {
	rf_Set *lowerbound = rf_relation_find_lowerbound(r, domain, error);

	if(lowerbound == NULL) {
		return NULL;
	}

	return rf_relation_find_maximum_within_subset(r, lowerbound, error);
}

rf_Set *
rf_relation_find_upperbound(const rf_Relation *r, const rf_Set *domain, rf_Error *error) {
	assert(r != NULL);
	assert(domain != NULL);

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL) {
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		}
		return NULL;
	}

	if(!rf_relation_is_partial_order(r)) {
		if(error != NULL) {
			rf_error_set(error, RF_E_REL_NOT_ORDERED, "");
		}
		return NULL;
	}

	if(!rf_set_is_subset(domain, r->domains[0])) {
		if(error != NULL) {
			rf_error_set(error, RF_E_SET_NOT_SUBSET, "");
		}
		return NULL;
	}

	int dim = r->domains[0]->cardinality;
	int dimSubset = domain->cardinality;
	int goalPosition = 0;

	rf_SetElement **tmp = malloc(sizeof(tmp)*dim);

	for(int x = 0; x < dim; x++) {
		bool isUpperbound = true;
		for(int y = 0; y < dimSubset; y++) {
			int index = rf_set_get_element_index(r->domains[0], domain->elements[y]);
			if(!r->table[rf_table_idx(r,x,index)]) {
				isUpperbound = false;
			}
		}
		if(isUpperbound) {
			tmp[goalPosition] = rf_set_element_clone(r->domains[0]->elements[x]);
			goalPosition++;
		}
	}

	return rf_set_new(goalPosition, tmp);
}

rf_Set *
rf_relation_find_lowerbound(const rf_Relation *r, const rf_Set *domain, rf_Error *error) {
	assert(r != NULL);

	//To increase performance, just copy lowerbound algorithm into this function and
	//switch indizes.

	const rf_Relation *converse = rf_relation_new_converse(r, error);

	return rf_relation_find_upperbound(converse, domain, error);
}


bool
rf_relation_make_antisymmetric(rf_Relation *r, bool upper, rf_Error *error) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL)
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		return false;
	}

	const int dim = r->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		for(int y = dim-1; y > x; --y) {
			if(r->table[rf_table_idx(r, x, y)] && r->table[rf_table_idx(r, y, x)]) {
				if(upper)
					r->table[rf_table_idx(r, y, x)] = false;
				else
					r->table[rf_table_idx(r, x, y)] = false;
			}
		}
	}
	return true;
}

bool
rf_relation_make_asymmetric(rf_Relation *r, bool upper, rf_Error *error) {
	assert(r != NULL);

	return rf_relation_make_irreflexive(r, error) && rf_relation_make_antisymmetric(r, upper, error);
}

bool
rf_relation_make_difunctional(rf_Relation *r, bool fill, rf_Error *error) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r))
		return false;

	const int dim = r->domains[0]->cardinality;

	for(int y = 0; y < dim; y++) {
		for(int x = 0; x < dim; x++) {
			if(r->table[rf_table_idx(r, x, y)] == true) {
				for(int z = x+1; z < dim; z++) {
					if(r->table[rf_table_idx(r,z,y)] == true) {
						//here we have xRy & zRy, now we equalize the images of x and z
						if(!fill) {
							r->table[rf_table_idx(r,z,y)] = false;
						} else {
							for(int i = 0; i < dim; i++) {
								if(r->table[rf_table_idx(r, z, i)] == true) {
									r->table[rf_table_idx(r, x, i)] = true;
								} else if(r->table[rf_table_idx(r, x, i)] == true) {
									r->table[rf_table_idx(r, z, i)] = true;
								}
							}
						}
					}
				}
			}
		}
	}

	return true;
}

/*
 * Bezeichnung irreführend. Ist im falle fill = false ein "try_make_equivalent"
 */
bool
rf_relation_make_equivalent(rf_Relation *r, bool fill, rf_Error *error) {
	assert(r != NULL);

	rf_relation_make_reflexive(r, error) && rf_relation_make_symmetric(r, fill, error) && rf_relation_make_transitive(r, fill, error);

	return rf_relation_is_reflexive(r) && rf_relation_is_symmetric(r) && rf_relation_is_transitive(r);
}

bool
rf_relation_make_irreflexive(rf_Relation *r, rf_Error *error) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL)
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		return false;
	}

	const int dim = r->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		r->table[rf_table_idx(r, x, x)] = false;
	}

	return true;
}

/*
 * Bezeichnung irreführend. Ist im Falle fill = false ein "try_make_partial_order"
 */
bool
rf_relation_make_partial_order(rf_Relation *r, bool fill, rf_Error *error) {
	assert(r != NULL);

	rf_relation_make_reflexive(r, error) && rf_relation_make_antisymmetric(r, fill, error) && rf_relation_make_transitive(r, fill, error);

	return rf_relation_is_reflexive(r) && rf_relation_is_antisymmetric(r) && rf_relation_is_transitive(r);
}

/*
 * Bezeichnung irreführend. Ist im Falle fill = false ein "try_make_preorder"
 */
bool
rf_relation_make_preorder(rf_Relation *r, bool fill, rf_Error *error) {
	assert(r != NULL);

	rf_relation_make_reflexive(r, error) && rf_relation_make_transitive(r, fill, error);

	return rf_relation_is_reflexive(r) && rf_relation_is_transitive(r);
}

bool
rf_relation_make_reflexive(rf_Relation *r, rf_Error *error) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL)
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		return false;
	}

	const int dim = r->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		r->table[rf_table_idx(r, x, x)] = true;
	}

	return true;
}

bool
rf_relation_make_symmetric(rf_Relation *r, bool fill, rf_Error *error) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL)
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		return false;
	}

	const int dim = r->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		for(int y = dim-1; y > x; --y) {
			if(r->table[rf_table_idx(r, x, y)] == r->table[rf_table_idx(r, y, x)])
				continue;

			r->table[rf_table_idx(r, x, y)] = fill;
			r->table[rf_table_idx(r, y, x)] = fill;
		}
	}

	return true;
}
/*
 * if fill is true, rf_relation_make_transitive computes the transitive closure according to marshall-algorithm
 */
bool
rf_relation_make_transitive(rf_Relation *r, bool fill, rf_Error *error) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL)
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		return false;
	}

	if(rf_relation_is_transitive(r))
		return true;

	const int dim = r->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		for(int y = dim-1; y >= 0; --y) {
			if(x == y)
				continue;
			if(!r->table[rf_table_idx(r, x, y)])
				continue;
			// xRy exists
			assert(r->table[rf_table_idx(r, x, y)]);
			for(int z = dim-1; z >= 0; --z) {
				if(!r->table[rf_table_idx(r, y, z)])
					continue;
				// yRz exists
				assert(r->table[rf_table_idx(r, y, z)]);
				if(fill) {
					r->table[rf_table_idx(r, x, z)] = true;
				} else{
					r->table[rf_table_idx(r, y, z)] = false;
				}
			}
		}
	}

	return rf_relation_make_transitive(r, fill, error);
}

int
rf_relation_find_transitive_gaps(rf_Relation *r, int *occurrences, rf_Set *gaps, rf_Error *error) {
	assert(r != NULL);
	assert(occurrences!=NULL);

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL)
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		return -1;
	}
	const int dim = r->domains[0]->cardinality;
	rf_SetElement **elems = malloc(sizeof(elems)*dim*dim*2);

	int numOfGaps = 0;
	int elemCount = 0;

	for(int x = dim-1; x >= 0; --x) {
		for(int y = dim-1; y >= 0; --y) {
			if(x == y)
				continue;
			if(!r->table[rf_table_idx(r, x, y)])
				continue;
			// xRy exists
			assert(r->table[rf_table_idx(r, x, y)]);
			for(int z = dim-1; z >= 0; --z) {
				if(!r->table[rf_table_idx(r, y, z)])
					continue;
				// yRz exists
				assert(r->table[rf_table_idx(r, y, z)]);
				if(!r->table[rf_table_idx(r, x, z)]) {
					//transitive gap
					occurrences[rf_table_idx(r, x, y)]++;
					occurrences[rf_table_idx(r, y, z)]++;
					//printf("transitive gap: %i, %i\n", x,z);
					if(occurrences[rf_table_idx(r, x, y)] == 1) {
						rf_SetElement **tupel = malloc(sizeof(tupel)*2);
						tupel[0] = r->domains[0]->elements[x];
						tupel[1] = r->domains[0]->elements[y];
						elems[elemCount] = rf_set_element_new_set(rf_set_new(2, tupel));
						elemCount++;
					}
					if(occurrences[rf_table_idx(r, y, z)] == 1) {
						rf_SetElement **tupel = malloc(sizeof(tupel)*2);
						tupel[0] = r->domains[0]->elements[y];
						tupel[1] = r->domains[0]->elements[z];
						elems[elemCount] = rf_set_element_new_set(rf_set_new(2, tupel));
						elemCount++;
					}
					numOfGaps++;
				}
			}
		}
	}
	if(gaps != NULL) {
		gaps->cardinality = elemCount;
		gaps->elements = elems;
	}

	return numOfGaps;
}

bool
rf_relation_guess_transitive_core(rf_Relation *r, rf_Error *error) {
	assert(r!=NULL);

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL)
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		return NULL;
	}

	int n = r->domains[0]->cardinality*r->domains[0]->cardinality;

	int *occurrences = malloc(sizeof(int)*n);

	int numOfGaps =rf_relation_find_transitive_gaps(r, occurrences, NULL, error);

	while (numOfGaps>0) {
		int biggestOccurrenceIndex = 0;
		for(int i=1;i<r->domains[0]->cardinality*r->domains[0]->cardinality;i++) {
			if(occurrences[i] > occurrences[biggestOccurrenceIndex]) {
				biggestOccurrenceIndex = i;

			}
		}
		r->table[biggestOccurrenceIndex] = 0;
		numOfGaps = numOfGaps - occurrences[biggestOccurrenceIndex];
		occurrences[biggestOccurrenceIndex] = -1;
	}
	if(rf_relation_is_transitive(r)	&& numOfGaps <= 0) {
		 return true;
	}

	while(!rf_relation_is_transitive(r)) {
		int biggestOccurrenceIndex = 0;
		for(int i=1;i<r->domains[0]->cardinality*r->domains[0]->cardinality;i++) {
			if(occurrences[i] > occurrences[biggestOccurrenceIndex]) {
				biggestOccurrenceIndex = i;
			}
		}
		r->table[biggestOccurrenceIndex] = 0;
		occurrences[biggestOccurrenceIndex] = -1;
	}

	return false;

}

/**
 * This is probably an NP-C problem. Computation may take a long time.
 * 9x9 relation computation takes about ten seconds, 10x10 over 2 minutes.
 *
 */
rf_Relation *
rf_relation_find_transitive_hard_core(rf_Relation *relation, rf_Error *error) {
	assert(relation != NULL);

	if(!rf_relation_is_homogeneous(relation)) {
		if(error != NULL)
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		return NULL;
	}

	rf_Relation *arbeitsrelation = rf_relation_clone(relation);
	rf_Relation *transitiveCore = NULL;

	int *occurrences = malloc(sizeof(int)*arbeitsrelation->domains[0]->cardinality*arbeitsrelation->domains[0]->cardinality);
	rf_Set *gaps = rf_set_new(0, malloc(0));
	rf_relation_find_transitive_gaps(arbeitsrelation, occurrences, gaps, error);

	int minCombi = 1 << gaps->cardinality;
	for(int i = (1 << gaps->cardinality)-1; i >= 0; --i) {
		size_t ps_elem_n = rf_bitcount(i);
		rf_SetElement **ps_elem_elems = calloc(ps_elem_n, sizeof(*ps_elem_elems));
		int b = gaps->cardinality-1;
		//generate current combination
		for(int j = ps_elem_n -1 ; j >= 0; --b) {
			assert(b >= 0);
			if(i & (1 << b)) {
				ps_elem_elems[j] = gaps->elements[b];
				j--;
			}
		}
		rf_Set *currentCombi = rf_set_new(ps_elem_n, ps_elem_elems);
		//try current combination
		for(int j = 0; j < currentCombi->cardinality; j++) {
			rf_SetElement *tmp = currentCombi->elements[j];
			int x = rf_set_get_element_index(arbeitsrelation->domains[0], tmp->value.set->elements[0]);
			int y = rf_set_get_element_index(arbeitsrelation->domains[0], tmp->value.set->elements[1]);
			arbeitsrelation->table[rf_table_idx(arbeitsrelation, x, y)] = false;
		}
		//is it a possible core?
		if(rf_relation_is_transitive(arbeitsrelation) && currentCombi->cardinality < minCombi) {
			transitiveCore = rf_relation_clone(arbeitsrelation);
			minCombi = currentCombi->cardinality;
		}
		//rollback
		for(int j = 0; j < currentCombi->cardinality; j++) {
			rf_SetElement *tmp = currentCombi->elements[j];
			int x = rf_set_get_element_index(arbeitsrelation->domains[0], tmp->value.set->elements[0]);
			int y = rf_set_get_element_index(arbeitsrelation->domains[0], tmp->value.set->elements[1]);
			arbeitsrelation->table[rf_table_idx(arbeitsrelation, x, y)] = true;
		}
	}

	return transitiveCore;
}

void
rf_relation_free(rf_Relation *r) {
	assert(r != NULL);

	for(int i = N_DOMAINS-1; i >= 0; --i)
		rf_set_free(r->domains[i]);
	free(r->domains);
	free(r->table);
	free(r);
}

bool
rf_relation_is_lefttotal(const rf_Relation *relation) {
	assert(relation != NULL);

	//each x has at least one y

	for(int x = 0; x < relation->domains[0]->cardinality; x++) {
		bool foundAPartner = false;
		for(int y = 0; y < relation->domains[1]->cardinality; y++) {
			if(relation->table[rf_table_idx(relation, x, y)]) {
				foundAPartner = true;
				break;
			}
		}
		if(!foundAPartner) {
			return false;
		}
	}

	return true;
}

bool
rf_relation_is_functional(const rf_Relation *relation) {
	assert(relation !=NULL);

	for(int x = 0; x < relation->domains[0]->cardinality; x++) {
		int partnerCount = 0;
		for(int y = 0; y < relation->domains[1]->cardinality; y++) {
			if(relation->table[rf_table_idx(relation, x, y)]) {
				partnerCount++;
			}
		}
		if(partnerCount > 1) {
			return false;
		}
	}

	return true;
}

bool
rf_relation_is_function(const rf_Relation *relation) {
	assert(relation !=NULL);

	for(int x = 0; x < relation->domains[0]->cardinality; x++) {
		int partnerCount = 0;
		for(int y = 0; y < relation->domains[1]->cardinality; y++) {
			if(relation->table[rf_table_idx(relation, x, y)]) {
				partnerCount++;
			}
		}
		if(partnerCount != 1) {
			return false;
		}
	}

	return true;
}

bool
rf_relation_is_surjective(const rf_Relation *relation) {

	assert(relation !=NULL);

	//each x has at least one y

	for(int y = 0; y < relation->domains[1]->cardinality; y++) {
		int partnerCount = 0;
		for(int x = 0; x < relation->domains[0]->cardinality; x++) {
			if(relation->table[rf_table_idx(relation, x, y)]) {
				partnerCount++;
			}
		}
		if(partnerCount == 0) {
			return false;
		}
	}

	return true;
}

bool
rf_relation_is_injective(const rf_Relation *relation) {
	assert(relation !=NULL);

	//each y has not more then one x

	for(int y = 0; y < relation->domains[1]->cardinality; y++) {
		int partnerCount = 0;
		for(int x = 0; x < relation->domains[0]->cardinality; x++) {
			if(relation->table[rf_table_idx(relation,x,y)]) {
				partnerCount++;
			}
		}
		if(partnerCount > 1) {
			return false;
		}
	}

	return true;
}

bool
rf_relation_is_bijective(const rf_Relation *relation) {
	return rf_relation_is_injective(relation) && rf_relation_is_surjective(relation);
}

/**
 * Checks, whether the relation is a lattice or not
 */
bool
rf_relation_is_lattice(const rf_Relation *relation, rf_Error *error) {
	//ex. supremum and infimum for all a,b with a,b element of domain

	assert(relation != NULL);
	if(!rf_relation_is_homogeneous(relation)) {
		if(error != NULL) {
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		}
		return false;
	}

	if(!rf_relation_is_partial_order(relation)) {
		if(error != NULL) {
			rf_error_set(error, RF_E_REL_NOT_ORDERED, "");
		}
		return false;
	}

	int dim = relation->domains[0]->cardinality;

	for(int x = 0; x < dim; x++) {
		for(int y = x + 1; y < dim; y++) {
			//check for supremum and infimum

			rf_SetElement **elems = malloc(sizeof(elems)*2);
			elems[0] = relation->domains[0]->elements[x];
			elems[1] = relation->domains[0]->elements[y];
			rf_Set *subset = rf_set_new(2, elems);

			rf_SetElement *sup = rf_relation_find_supremum(relation, subset, error);
			rf_SetElement *inf = rf_relation_find_infimum(relation, subset, error);

			if(sup == NULL || inf == NULL)
				return false;
			free(elems);
		}
	}

	return true;
}

bool
rf_relation_is_sublattice(rf_Relation *superlattice, rf_Relation *sublattice, rf_Error *error) {
	assert(superlattice != NULL);
	assert(sublattice != NULL);

	if(!rf_relation_is_lattice(superlattice, error))
		return false;

	if(!rf_relation_is_lattice(sublattice, error))
		return false;

	if(!rf_set_is_subset(superlattice->domains[0], sublattice->domains[0]))
		return false;

	int dimSub = sublattice->domains[0]->cardinality;

	for(int x = 0; x < dimSub; x++) {
		for(int y = 0; y < dimSub; y++) {
			int xSuper = rf_set_get_element_index(superlattice->domains[0], sublattice->domains[0]->elements[x]);
			int ySuper = rf_set_get_element_index(superlattice->domains[1], sublattice->domains[1]->elements[y]);
			if(sublattice->table[rf_table_idx(sublattice, x, y)] != superlattice->table[rf_table_idx(superlattice, xSuper, ySuper)]) {
				return false;
			}
		}
	}

	return true;
}

/**
 * The image is a subset of the second domain, containing elements that are referenced by some x.
 */
rf_Set *
rf_relation_get_image(const rf_Relation *relation, rf_Set *subrelation) {
	assert(relation != NULL);
	assert(subrelation != NULL);

	rf_SetElement **elems = malloc(sizeof(elems)*relation->domains[0]->cardinality);

	int elemCount = 0;
	for(int y = 0; y < relation->domains[1]->cardinality; y++) {
		if(!rf_set_contains_element(subrelation, relation->domains[1]->elements[y]))
			continue;
		for(int x = 0; x < relation->domains[0]->cardinality; x++) {
			if(!rf_set_contains_element(subrelation, relation->domains[0]->elements[x]))
				continue;
			if(relation->table[rf_table_idx(relation,x,y)]) {
				elems[elemCount] = rf_set_element_clone(relation->domains[1]->elements[y]);
				elemCount++;
				break;
			}
		}
	}

	return rf_set_new(elemCount, elems);
}

/**
 *The preimage is a set of elements, that have at least one partner in the relation
 */
rf_Set *
rf_relation_get_preImage(const rf_Relation *relation, rf_Set *subrelation) {
	assert(relation != NULL);
	assert(subrelation != NULL);

	rf_SetElement **elems = malloc(sizeof(elems)*relation->domains[0]->cardinality);

	int elemCount = 0;
	for(int x = 0; x < relation->domains[0]->cardinality; x++) {
		if(!rf_set_contains_element(subrelation, relation->domains[0]->elements[x]))
	continue;
		for(int y = 0; y < relation->domains[1]->cardinality; y++) {
			if(!rf_set_contains_element(subrelation, relation->domains[1]->elements[y]))
				continue;
			if(relation->table[rf_table_idx(relation,x,y)]) {
				elems[elemCount] = rf_set_element_clone(relation->domains[0]->elements[x]);
				elemCount++;
				break;
			}
		}
	}

	return rf_set_new(elemCount, elems);
}
