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
#include <string.h>
#include <assert.h>

#include "set.h"

rf_Set *
rf_set_new(int n, rf_SetElement *elements[n]) {
	assert(n >= 0);
	assert(elements != NULL);

	rf_Set *s = malloc(sizeof(*s));
	s->cardinality = n;
	s->elements = elements;

	if(false /* TODO check for duplicate elements */) {
		;
	}

	return s;
}

rf_Set *
rf_set_copy(const rf_Set *s) {
	assert(s != NULL);

	int n = s->cardinality;
	rf_SetElement **elements = calloc(n, sizeof(*elements));
	for(int i = n-1; i >= 0; --i) {
		elements[i] = rf_set_element_copy(s->elements[i]);
	}

	return rf_set_new(n, elements);
}


bool
rf_set_equal(const rf_Set *a, const rf_Set *b) {
	assert(a != NULL);
	assert(b != NULL);

	if(a->cardinality != b->cardinality)
		return false;

	for(int i = b->cardinality-1; i >= 0; --i) {
		if(!rf_set_contains_element(a, b->elements[i]))
			return false;
	}

	return true;
}

bool
rf_set_is_subset(const rf_Set *subset, const rf_Set *superset) {
	assert(subset != NULL);
	assert(superset != NULL);

	// strict subset
	if(subset->cardinality > superset->cardinality)
		return false;

	for(int i = subset->cardinality-1; i >= 0; --i) {
		if(!rf_set_contains_element(superset, subset->elements[i]))
			return false;
	}

	return true;
}

/*
 * Counts the number of bits in an int (32-bit value). Beware of voodoo.
 * See http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
 */
static unsigned int
rf_bitcount(unsigned int v) {
	v = v - ((v >> 1) & 0x55555555); // reuse input as temporary
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
	return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
}

rf_Set *
rf_set_generate_powerset(const rf_Set *s) {
	assert(s != NULL);
//	assert(SIZE_MAX >> s->cardinality > 0); // size_t has enough bits

	size_t ps_n = 1 << s->cardinality; // powerset has 2^n members
	rf_SetElement **ps_elems = calloc(ps_n, sizeof(**ps_elems));

	for(int i = ps_n-1; i >= 0; --i) {
		// i's bits are interpreted as an array of boolean values
		// that indicare whether the element should become part
		// of the powerset element.
		// So if i is 6 (little-endian: 0101) the elements at
		// index 1 and 3 will form the new powerset element.

		// The bitcount of i is the cardinality of that element.
		size_t ps_elem_n = rf_bitcount(i);
		rf_SetElement **ps_elem_elems = calloc(ps_elem_n, sizeof(*ps_elem_elems));
		int b = s->cardinality-1;
		for(int j = ps_elem_n; j >= 0; --b) {
			assert(b >= 0);
			// bit b is 1
			if(i & (1 << b)) {
				ps_elem_elems[--j] = rf_set_element_copy(s->elements[b]);
			}
		}
		rf_Set *ps_elem = rf_set_new(ps_elem_n, ps_elem_elems);
		ps_elems[i] = rf_set_element_new_set(ps_elem);
	}

	rf_Set *powerset = rf_set_new(ps_n, ps_elems);

	return powerset;
}

bool
rf_set_contains_element(const rf_Set *s, const rf_SetElement *e) {
	assert(s != NULL);
	assert(e != NULL);

	for(int i = s->cardinality-1; i >= 0; --i) {
		if(rf_set_element_equal(s->elements[i], e))
			return true;
	}

	return false;
}

int
rf_set_get_element_index(const rf_Set *s, const rf_SetElement *e) {
	assert(s != NULL);
	assert(e != NULL);

	int i;
	for(i = s->cardinality-1; i >= 0; --i) {
		if(rf_set_element_equal(s->elements[i], e))
			return i;
	}

	return i;
}

void
rf_set_free(rf_Set *s) {
	assert(s != NULL);

	for(int i = s->cardinality-1; i >= 0; --i) {
		rf_set_element_free(s->elements[i]);
	}
	free(s);
}


/*
 * Element procedures
 */

rf_SetElement *
rf_set_element_new_string(char *value) {
	assert(value != NULL);

	rf_SetElement *e = malloc(sizeof(*e));
	e->type = RF_SET_ELEMENT_TYPE_STRING;
	e->value.string = value;

	return e;
}

rf_SetElement *
rf_set_element_new_set(rf_Set *value) {
	assert(value != NULL);

	rf_SetElement *e = malloc(sizeof(*e));
	e->type = RF_SET_ELEMENT_TYPE_SET;
	e->value.set = value;

	return e;
}

rf_SetElement *
rf_set_element_copy(const rf_SetElement *e) {
	assert(e != NULL);

	rf_SetElement *c;
	switch(e->type) {
	case RF_SET_ELEMENT_TYPE_STRING: {
		char *str = strdup(e->value.string);
		c = rf_set_element_new_string(str);
		break;
	}
	case RF_SET_ELEMENT_TYPE_SET: {
		rf_Set *set = rf_set_copy(e->value.set);
		c = rf_set_element_new_set(set);
		break;
	}
	default:
		assert(false); // all cases must be handled
	}

	return c;
}


bool
rf_set_element_equal(const rf_SetElement *a, const rf_SetElement *b) {
	assert(a != NULL);
	assert(b != NULL);

	if(a->type != b->type)
		return false;

	switch(a->type) {
	case RF_SET_ELEMENT_TYPE_STRING:
		return strcmp(a->value.string, b->value.string) == 0;
	case RF_SET_ELEMENT_TYPE_SET:
		return rf_set_equal(a->value.set, b->value.set);
	default:
		assert(false); // all cases must be handled
	}

	return false;
}

void
rf_set_element_free(rf_SetElement *e) {
	assert(e != NULL);

	switch(e->type) {
	case RF_SET_ELEMENT_TYPE_STRING:
		free(e->value.string);
		break;
	case RF_SET_ELEMENT_TYPE_SET:
		rf_set_free(e->value.set);
		break;
	default:
		assert(false); // all cases must be handled
	}
	free(e);
}

