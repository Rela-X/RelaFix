/* 
 * Copyright (C) 2008,2013 Peter Berger, Wilke Schwiedop
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
#include <stdarg.h>
#include <assert.h>

#include "relation.h"

#define N_DOMAINS 2

/*
 * table[x][y][z] = x * r->domains[0]->cardinality + y * r->domains[1]->cardinality + z
 */ 
static
size_t rf_table_idx(const rf_Relation *r, ...) {
	assert(r != NULL);

	size_t idx = 0;
	va_list argv;
	va_start(argv, r);
	int i;
	int offset; 
	for(i = 0; i < N_DOMAINS-1; ++i) {
		offset = va_arg(argv, int);
		assert(offset > r->domains[i]->cardinality);
		idx += r->domains[i]->cardinality * offset;
	}
	offset = va_arg(argv, int);
	assert(offset > r->domains[i]->cardinality);
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
rf_relation_copy(const rf_Relation *r) {
	assert(r != NULL);

	rf_Set *d[N_DOMAINS];
	size_t table_size = 1;
	for(int i = N_DOMAINS-1; i >= 0; --i) {
		d[i] = rf_set_copy(r->domains[i]);
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
		for(int y = x; y >= 0; --y) {
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

	rf_Relation *new = rf_relation_copy(r);

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
				new->table[rf_table_idx(new, z, y)] = r2->table[rf_table_idx(r2, z, y)];
			}
		}
	}

	return new;
}

rf_Relation *
rf_relation_new_converse(rf_Relation *r, rf_Error *error) {
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
	// TODO
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
	for(int x = dim-1; x >= 0; --x) {
		for(int y = dim-1; y >= 0; --y) {
			if(!r->table[rf_table_idx(r, x, y)])
				continue;
			// xRy exists
			for(int z = dim-1; z >= 0; --z) {
				if(y == z)
					continue;
				if(!r->table[rf_table_idx(r, z, y)])
					continue;
				// zRy exists
				for(int w = dim-1; w >= 0; --w) {
					if(!r->table[rf_table_idx(r, z, w)])
						continue;
					// zRw exists
					if(!r->table[rf_table_idx(r, x, w)])
						return false;
					// xRw exists
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

// TODO is this a useful procedure?
bool
rf_relation_is_irreflexive(const rf_Relation *r) {
	assert(r != NULL);

	return !rf_relation_is_reflexive(r);
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

// TODO is this a useful procedure?
bool
rf_relation_is_symmetric(const rf_Relation *r) {
	assert(r != NULL);

	return !rf_relation_is_asymmetric(r);
}

// xRy & yRz => xRz
bool
rf_relation_is_transitive(const rf_Relation *r) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r))
		return false;

	const int dim = r->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		for(int y = dim-1; y > x; --y) {
			if(x == y)
				continue;
			if(!r->table[rf_table_idx(r, x, y)])
				continue;
			// xRy exists
			assert(r->table[rf_table_idx(r, x, y)]);
			for(int z = y-1; z >= 0; --z) {
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


rf_SetElement *
rf_relation_find_maximum(const rf_Relation *r, rf_Error *error) {
	assert(r != NULL);

	rf_SetElement *max = NULL;

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL) {
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		}
		return max;
	}
	if(!rf_relation_is_partial_order(r)) {
		if(error != NULL) {
			rf_error_set(error, RF_E_REL_NOT_ORDERED, "");
		}
		return max;
	}

	const int dim = r->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		for(int y = dim-1; y >= 0; --y) {
			if(x == y)
				continue;
			if(r->table[rf_table_idx(r, x, y)])
				continue;

			if(r->table[rf_table_idx(r, y, x)])
				max = r->domains[0]->elements[y];
		}
	}

	if(max == NULL && error != NULL) {
		rf_error_set(error, RF_E_REL_NO_MAX, "");
	}

	return max;
}

rf_SetElement *
rf_relation_find_minimum(const rf_Relation *r, rf_Error *error) {
	assert(r != NULL);

	rf_SetElement *min = NULL;

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL) {
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		}
		return min;
	}
	if(!rf_relation_is_partial_order(r)) {
		if(error != NULL) {
			rf_error_set(error, RF_E_REL_NOT_ORDERED, "");
		}
		return min;
	}

	const int dim = r->domains[0]->cardinality;
	for(int x = dim-1; x >= 0; --x) {
		for(int y = dim-1; y >= 0; --y) {
			if(x == y)
				continue;
			if(r->table[rf_table_idx(r, y, x)])
				continue;

			if(r->table[rf_table_idx(r, x, y)])
				min = r->domains[0]->elements[y];
		}
	}

	if(min == NULL && error != NULL) {
		rf_error_set(error, RF_E_REL_NO_MIN, "");
	}

	return min;
}

rf_SetElement *
rf_relation_find_supremum(const rf_Relation *r, const rf_Set *domain, rf_Error *error) {
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

	// TODO
}

rf_SetElement *
rf_relation_find_infimum(const rf_Relation *r, const rf_Set *domain, rf_Error *error) {
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

	// TODO
}

rf_Set *
rf_relation_find_upperbound(const rf_Relation *r, const rf_Set *domain, rf_Error *error) {
	assert(r != NULL);

	// TODO
}

rf_Set *
rf_relation_find_lowerbound(const rf_Relation *r, const rf_Set *domain, rf_Error *error) {
	assert(r != NULL);

	// TODO
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

	// TODO
}

bool
rf_relation_make_equivalent(rf_Relation *r, bool fill, rf_Error *error) {
	assert(r != NULL);

	return rf_relation_make_reflexive(r, error) && rf_relation_make_symmetric(r, fill, error) && rf_relation_make_transitive(r, fill, error);
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

bool
rf_relation_make_partial_order(rf_Relation *r, bool fill, rf_Error *error) {
	assert(r != NULL);

	return rf_relation_make_reflexive(r, error) && rf_relation_make_antisymmetric(r, fill, error) && rf_relation_make_transitive(r, fill, error);
}

bool
rf_relation_make_preorder(rf_Relation *r, bool fill, rf_Error *error) {
	assert(r != NULL);

	return rf_relation_make_reflexive(r, error) && rf_relation_make_transitive(r, fill, error);
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

bool
rf_relation_make_transitive(rf_Relation *r, bool fill, rf_Error *error) {
	assert(r != NULL);

	if(!rf_relation_is_homogeneous(r)) {
		if(error != NULL)
			rf_error_set(error, RF_E_REL_NOT_HOMOGENEOUS, "");
		return false;
	}

	// FIXME original algorithm looked broken

	return true;
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
