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
#include <string.h>
#include <assert.h>

#include "set.h"
#include "tools.h"

rf_Set *
rf_set_new(int n, rf_SetElement *elements[n]) {
	assert(n >= 0);
	assert(elements != NULL);
	
	for(int i = 0; i < n; i++) {
		for(int j = i + 1; j < n; j++) {
			assert(elements[i] != elements[j]);
		}
	}

	rf_Set *s = malloc(sizeof(*s));
	s->cardinality = n;
	s->elements = calloc(n, sizeof(*s->elements));
	for(int i = n-1; i >= 0; --i) {
		s->elements[i] = elements[i];
	}
	
	return s;
}

rf_Set *
rf_set_clone(const rf_Set *s) {
	assert(s != NULL);

	int n = s->cardinality;
	rf_SetElement *elements[n];
	for(int i = n-1; i >= 0; --i) {
		elements[i] = rf_set_element_clone(s->elements[i]);
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
		for(int j = ps_elem_n -1 ; j >= 0; --b) {
			assert(b >= 0);
			// bit b is 1
			if(i & (1 << b)) {
				ps_elem_elems[j] = rf_set_element_clone(s->elements[b]);
				j--;
			} 	
		}
		rf_Set *ps_elem = rf_set_new(ps_elem_n, ps_elem_elems);
		ps_elems[i] = rf_set_element_new_set(ps_elem);
	}

	rf_Set *powerset = rf_set_new(ps_n, ps_elems);

	return powerset;
}

rf_Set *
rf_set_intersection(const rf_Set *s1, const rf_Set *s2) {
	assert(s1 != NULL);
	assert(s2 != NULL);
	
	int maxElements = (s1->cardinality < s2->cardinality) ? s1->cardinality : s2->cardinality; 
	rf_SetElement *elements[maxElements];

	int elemCount = 0;
	for(int i = 0; i < s1->cardinality; i++) {
		for(int j = 0; j < s2->cardinality; j++) {
			if(rf_set_element_equal(s1->elements[i], s2->elements[j])) {
				elements[elemCount] = rf_set_element_clone(s1->elements[i]);
				elemCount++;
			}
		}
	}
	
	return rf_set_new(elemCount, elements);
}

bool
rf_set_contains_element(const rf_Set *s, const rf_SetElement *e) {
	assert(s != NULL);
	assert(e != NULL);

	return rf_set_get_element_index(s, e) != -1;
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
	free(s->elements);
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
	e->value.string = strdup(value);

	return e;
}

rf_SetElement *
rf_set_element_new_set(rf_Set *value) {
	assert(value != NULL);

	rf_SetElement *e = malloc(sizeof(*e));
	e->type = RF_SET_ELEMENT_TYPE_SET;
	e->value.set = rf_set_clone(value);

	return e;
}

rf_SetElement *
rf_set_element_clone(const rf_SetElement *e) {
	assert(e != NULL);

	rf_SetElement *c;
	switch(e->type) {
	case RF_SET_ELEMENT_TYPE_STRING:
		c = rf_set_element_new_string(e->value.string);
		break;
	case RF_SET_ELEMENT_TYPE_SET:
		c = rf_set_element_new_set(e->value.set);
		break;
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
	{
		return strcmp(a->value.string, b->value.string) == 0;
	}
		
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

