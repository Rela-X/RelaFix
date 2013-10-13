/*
 * Copyright (C) 2013, Sebastian Pospiech
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

#include <CUnit/CUnit.h>

#include "text_io.h"

#include "set.h"
#include "relation.h"

void
test_rf_set_from_string() {
}

void
test_rf_set_to_string() {
	rf_Set *set1 = rf_set_new(0, NULL);
	char *set1_str = rf_set_to_string(set1);

	CU_ASSERT_STRING_EQUAL(set1_str, "{}");

	rf_SetElement *elems2[] = {
		rf_set_element_new_string("a"),
		rf_set_element_new_string("b"),
		rf_set_element_new_string("c"),
	};
	rf_Set *set2 = rf_set_new(3, elems2);
	char *set2_str = rf_set_to_string(set2);

	CU_ASSERT_STRING_EQUAL(set2_str, "{ a b c }");

	rf_SetElement *elems3[] = {
		rf_set_element_new_string("a"),
		rf_set_element_new_set(set1),
		rf_set_element_new_string("b"),
		rf_set_element_new_set(set2),
		rf_set_element_new_string("c"),
	};
	rf_Set *set3 = rf_set_new(5, elems3);
	char *set3_str = rf_set_to_string(set3);

	CU_ASSERT_STRING_EQUAL(set3_str, "{ a {} b { a b c } c }");

	rf_set_free(set1);
	free(set1_str);
	rf_set_free(set2);
	free(set2_str);
	rf_set_free(set3);
	free(set3_str);
}

void
test_rf_relation_from_string() {
}

void
test_rf_relation_to_string() {
	rf_SetElement *elems1[] = {
		rf_set_element_new_string("a"),
		rf_set_element_new_string("b"),
		rf_set_element_new_string("c"),
	};
	rf_Set *set1 = rf_set_new(3, elems1);
	char *set1_str = rf_set_to_string(set1);

	rf_Relation *r1 = rf_relation_new_id(set1);
	char *r1_str = rf_relation_to_string(r1);

	CU_ASSERT_STRING_EQUAL(r1_str,
		"{ a b c } x { a b c } :"
		"\n[100|"
		"\n|010|"
		"\n|001]"
	);

	rf_set_free(set1);
	free(set1_str);
	rf_relation_free(r1);
	free(r1_str);
}

CU_ErrorCode
register_suites_text_io() {
	CU_TestInfo suite_text_io[] = {
		{ "rf_set_from_string", test_rf_set_from_string },
		{ "rf_set_to_string", test_rf_set_to_string },
		{ "rf_relation_from_string", test_rf_relation_from_string },
		{ "rf_relation_to_string", test_rf_relation_to_string },
		CU_TEST_INFO_NULL
	};

	CU_SuiteInfo suites[] = {
		{ "TextIO", NULL, NULL, suite_text_io },
		CU_SUITE_INFO_NULL
	};

	return CU_register_suites(suites);
}
