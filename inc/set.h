/*
 * RelaFix Set
 *
 * Copyright (C) Wilke Schwiedop 2013
 */

#ifndef RF_SET_H
#define RF_SET_H

#include <stdbool.h>

enum _rf_set_element_type {
        RF_SET_ELEMENT_TYPE_STRING,
        RF_SET_ELEMENT_TYPE_SET,
};

typedef struct _rf_set                  rf_Set;
typedef struct _rf_set_element          rf_SetElement;
typedef enum _rf_set_element_type       rf_SetElementType;

struct _rf_set {
        unsigned int    cardinality;    /*!< Number of Members */
        rf_SetElement   **elements;     /*!< Members */
};

struct _rf_set_element {
        rf_SetElementType       type;
        union {
                char    *string;
                rf_Set  *set;
        } value;
};


rf_Set *        rf_set_new(int n, rf_SetElement **elements);
rf_Set *        rf_set_copy(const rf_Set *set);

bool            rf_set_equal(const rf_Set *a, const rf_Set *b);
/*! Checks if subset is a strict subset of superset */
bool            rf_set_is_subset(const rf_Set *subset, const rf_Set *superset);

rf_Set *        rf_set_generate_powerset(const rf_Set *set);

bool            rf_set_contains_element(const rf_Set *set, const rf_SetElement *element);
int             rf_set_get_element_index(const rf_Set *set, const rf_SetElement *element);

void            rf_set_free(rf_Set *set);

#if __STDC_VERSION__ >= 201112L
#define rf_set_element_new(value) _Generic((value),     \
        char    : rf_set_element_new_string,            \
        rf_Set  : rf_set_element_new_set,               \
        default : ) (value)
#endif
rf_SetElement * rf_set_element_new_string(char *value);
rf_SetElement * rf_set_element_new_set(rf_Set *value);
rf_SetElement * rf_set_element_copy(const rf_SetElement *element);

bool            rf_set_element_equal(const rf_SetElement *a, const rf_SetElement *b);

void            rf_set_element_free(rf_SetElement *element);

#endif
