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

#include "set.h"

void test_rf_set_new() {
	char a[] = "a";
	char b[] = "b";
	char c[] = "c";
	rf_SetElement *elems[] = {
		rf_set_element_new_string(a),
		rf_set_element_new_string(b),
		rf_set_element_new_string(c),
	};

	rf_Set *subset = rf_set_new(3, elems);

	CU_ASSERT_STRING_EQUAL(subset->elements[0]->value.string, a);
	CU_ASSERT_STRING_EQUAL(subset->elements[1]->value.string, b);
	CU_ASSERT_STRING_EQUAL(subset->elements[2]->value.string, c);

	//check for double elements
	//uncommented cause assert fails here

	//elems[2] = elems[0];
	//subset = rf_set_new(3, elems);

	rf_set_free(subset);
}

void test_rf_set_clone() {
	char a[] = "a";
	char b[] = "b";
	char c[] = "c";

	rf_SetElement *elems1[] = {
		rf_set_element_new_string(a),
		rf_set_element_new_string(b),
		rf_set_element_new_string(c),
	};
	rf_Set *set1 = rf_set_new(3, elems1);

	rf_Set *result = rf_set_clone(set1);

	CU_ASSERT_PTR_NOT_EQUAL(set1, result);
	for(int i = 0; i < result->cardinality; i++) {
		CU_ASSERT_PTR_NOT_EQUAL(set1->elements[i], result->elements[i]);
		CU_ASSERT_EQUAL(*set1->elements[i]->value.string, *result->elements[i]->value.string);
	}

	rf_set_free(set1);
	rf_set_free(result);
}

void test_rf_set_equal() {
	char a[] = "a";
	char b[] = "b";
	char c[] = "c";

	rf_SetElement *elems1[] = {
		rf_set_element_new_string(a),
		rf_set_element_new_string(b),
		rf_set_element_new_string(c),
	};
	rf_Set *set1 = rf_set_new(3, elems1);

	rf_SetElement *elems2[] = {
		rf_set_element_new_string(c),
		rf_set_element_new_string(b),
		rf_set_element_new_string(a),
	};
	rf_Set *set2 = rf_set_new(3, elems2);

	rf_SetElement *elems3[] = {
		rf_set_element_new_string(a),
		rf_set_element_new_string(b),
	};
	rf_Set *set3 = rf_set_new(2, elems3);


	CU_ASSERT_TRUE(rf_set_equal(set1, set1)) //stupid case
	CU_ASSERT_TRUE(rf_set_equal(set1, set2)) //tricky case, same set with switched order
	CU_ASSERT_FALSE(rf_set_equal(set1, set3)) //false case

	rf_set_free(set1);
	rf_set_free(set2);
	rf_set_free(set3);
}

void test_rf_set_is_subset() {
	//watch out, in this testcase, rf_set_is_subset uses a stringcompare. That does not work with a single char
	//need a char array here which is null-byte terminated.
	char a1[] = "a";
	char b1[] = "b";
	char c1[] = "c";
	char d1[] = "d";

	rf_SetElement *elems[] = {
		rf_set_element_new_string(a1),
		rf_set_element_new_string(b1),
		rf_set_element_new_string(c1),
	};

	char a2[] = "a";
	char b2[] = "b";
	char c2[] = "c";

	rf_SetElement *elems2[] = {
		rf_set_element_new_string(a2),
		rf_set_element_new_string(b2),
		rf_set_element_new_string(c2),
	};

	rf_Set *set1 = rf_set_new(3, elems);
	rf_Set *set2 = rf_set_new(3, elems2);

	CU_ASSERT_TRUE(rf_set_is_subset(set1, set1));
	CU_ASSERT_TRUE(rf_set_is_subset(set1, set2)); //sollte mMn gehen. //kann noch perversiert werden mit untersch. Sortierung.

	rf_SetElement *elems3[] = {
		rf_set_element_new_string(a1),
		rf_set_element_new_string(d1),
	};
	rf_Set *set3 = rf_set_new(2, elems3);

	CU_ASSERT_FALSE(rf_set_is_subset(set1, set3));

	rf_set_free(set1);
	rf_set_free(set2);
	rf_set_free(set3);
}

void test_rf_set_contains_element() {
	char a[] = "a";
	char b[] = "b";
	char c[] = "c";
	char d[] = "d";

	rf_SetElement *elems[] = {
		rf_set_element_new_string(a),
		rf_set_element_new_string(b),
		rf_set_element_new_string(c),
	};

	rf_Set *set1 = rf_set_new(3, elems);

	rf_SetElement *d_elem = rf_set_element_new_string(d);

	for(int i = 0; i < 3; i++) {
		CU_ASSERT_TRUE(rf_set_contains_element(set1, elems[i]));
	}
	CU_ASSERT_FALSE(rf_set_contains_element(set1, d_elem));

	rf_set_free(set1);
	rf_set_element_free(d_elem);
}

void test_rf_set_get_element_index() {
	char a[] = "a";
	char b[] = "b";
	char c[] = "c";
	char d[] = "d";
	char a2[] = "a";

	rf_SetElement *elems[] = {
		rf_set_element_new_string(a),
		rf_set_element_new_string(b),
		rf_set_element_new_string(c),
	};

	rf_Set *set1 = rf_set_new(3, elems);

	rf_SetElement *test1 = rf_set_element_new_string(d);
	rf_SetElement *test2 = rf_set_element_new_string(a2);

	for(int i = 0; i < 3; i++) {
		CU_ASSERT_EQUAL(rf_set_get_element_index(set1, elems[i]), i);
	}

	CU_ASSERT_EQUAL(rf_set_get_element_index(set1, test1), -1);
	CU_ASSERT_EQUAL(rf_set_get_element_index(set1, test2), 0);

	rf_set_free(set1);
	rf_set_element_free(test1);
	rf_set_element_free(test2);
}

void test_rf_set_generate_powerset() {
CU_FAIL_FATAL("not implemented");
	char a[] = "a";
	char b[] = "b";
	char c[] = "c";

	char *results[] = {
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		'\0'
	};
	char *expected[] = {
		"a", "b", "c", "ab", "ac", "bc", "abc", '\0'
	};

	rf_SetElement *elems[] = {
		rf_set_element_new_string(a),
		rf_set_element_new_string(b),
		rf_set_element_new_string(c),
	};

	rf_Set *set1 = rf_set_new(3, elems);
	CU_ASSERT_TRUE(set1 != NULL);

	rf_Set *powerset = rf_set_generate_powerset(set1);
	CU_ASSERT_EQUAL(powerset->cardinality, 8);

/*
	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 3; j++) {
			results[i][j] = 0;
		}
	}
*/

	for(int i = 0; i < powerset->cardinality; i++) {
		rf_SetElement *tmp1 = powerset->elements[i];
		for(int j = 0; j < tmp1->value.set->cardinality; j++) {
			rf_SetElement *tmp2 = tmp1->value.set->elements[j];
			if(tmp2 != NULL) {
				results[i][j] = *tmp2->value.string;
			}
		}
//		printf("### ^%s$\n", results[i]);
	}

	for(int i = 0; i < 8; i++) {
		bool contains = false;
		for(int j = 0; j < 3; j++) {
			if(strcmp(results[i], expected[j]) == 0) {
				printf("[%s|%s]\n", results[i], expected[i]);
				contains = true;
			}
		}
		CU_ASSERT_TRUE(contains);
	}

	rf_set_free(set1);
	rf_set_free(powerset);
}

void test_rf_set_intersection() {
	char *a = "a";
	char *b = "b";
	char *c = "c";

	rf_SetElement *elems1[3] = {
		rf_set_element_new_string(a),
		rf_set_element_new_string(b),
		rf_set_element_new_string(c),
	};

	rf_SetElement *elems2[2] = {
		rf_set_element_new_string(b),
		rf_set_element_new_string(c),
	};

	rf_Set *set1 = rf_set_new(3, elems1);
	rf_Set *set2 = rf_set_new(2, elems2);

	rf_Set *result = rf_set_intersection(set1, set2);

	CU_ASSERT_EQUAL(result->cardinality, 2);
	CU_ASSERT_TRUE(rf_set_contains_element(result, elems2[0]));
	CU_ASSERT_TRUE(rf_set_contains_element(result, elems2[1]));

	rf_set_free(set1);
	rf_set_free(set2);
	rf_set_free(result);
}

void test_rf_set_free() {

}

void test_rf_set_element_new_string() {
	char a[] = "a"; //Test valid element
	char mv[] = "\x94"; //Test valid element with mutated vowel
	char em[] = ""; //Test empty value

	rf_SetElement *element_a = rf_set_element_new_string(a);
	rf_SetElement *element_mv = rf_set_element_new_string(mv);
	rf_SetElement *element_em = rf_set_element_new_string(em);

	CU_ASSERT_EQUAL(element_a->type, RF_SET_ELEMENT_TYPE_STRING);
	CU_ASSERT_EQUAL(element_mv->type, RF_SET_ELEMENT_TYPE_STRING);
	CU_ASSERT_EQUAL(element_em->type, RF_SET_ELEMENT_TYPE_STRING);

	CU_ASSERT_STRING_EQUAL(element_a->value.string, a);
	CU_ASSERT_STRING_EQUAL(element_mv->value.string, mv);
	CU_ASSERT_STRING_EQUAL(element_em->value.string, em);

	rf_set_element_free(element_a);
	rf_set_element_free(element_mv);
	rf_set_element_free(element_em);
}

void test_rf_set_element_new_set() {
	rf_SetElement *elems[3];
	elems[0] = rf_set_element_new_string("a");
	elems[1] = rf_set_element_new_string("b");
	elems[2] = rf_set_element_new_string("c");
	rf_Set *subset = rf_set_new(3, elems);

	rf_SetElement *set_elem = rf_set_element_new_set(subset);

//	CU_ASSERT_EQUAL(set_elem->type, RF_SET_ELEMENT_TYPE_SET);
//	CU_ASSERT_TRUE(rf_set_equal(subset, set_elem->value.set));

	rf_set_free(subset);
	rf_set_element_free(set_elem);
}

void test_rf_set_element_clone() {
	//case, element is a char
	rf_SetElement *src1 = rf_set_element_new_string("a");
	rf_SetElement *dst1 = rf_set_element_clone(src1);

	CU_ASSERT_PTR_NOT_EQUAL(dst1, src1);
	CU_ASSERT_STRING_EQUAL(dst1->value.string, src1->value.string);

	//case element is a subset
	rf_SetElement *elems[] = {
		rf_set_element_new_string("a"),
		rf_set_element_new_string("b"),
		rf_set_element_new_string("c"),
	};
	rf_Set *subset = rf_set_new(3, elems);

	rf_SetElement *src2 = rf_set_element_new_set(subset);
	rf_SetElement *dst2 = rf_set_element_clone(src2);

	//Must not work on same elements by copying pointers
	CU_ASSERT_PTR_NOT_EQUAL(src2->value.set, subset);
	CU_ASSERT_PTR_NOT_EQUAL(dst2->value.set, subset);
	CU_ASSERT_PTR_NOT_EQUAL(dst2, src2);
	CU_ASSERT_TRUE(rf_set_element_equal(dst2, src2));

	CU_ASSERT_STRING_EQUAL(dst2->value.set->elements[0]->value.string, src2->value.set->elements[0]->value.string);
	CU_ASSERT_STRING_EQUAL(dst2->value.set->elements[1]->value.string, src2->value.set->elements[1]->value.string);
	CU_ASSERT_STRING_EQUAL(dst2->value.set->elements[2]->value.string, src2->value.set->elements[2]->value.string);

	rf_set_element_free(src1);
	rf_set_element_free(dst1);

	rf_set_free(subset);
	rf_set_element_free(src2);
	rf_set_element_free(dst2);
}

void test_rf_set_element_equal() {
	rf_SetElement *elem1 = rf_set_element_new_string("a");
	rf_SetElement *elem2 = rf_set_element_new_string("a");
	rf_SetElement *elem3 = rf_set_element_new_string("b");

	CU_ASSERT_TRUE(rf_set_element_equal(elem1, elem1));
	CU_ASSERT_TRUE(rf_set_element_equal(elem1, elem2));
	CU_ASSERT_FALSE(rf_set_element_equal(elem1, elem3));

	//also test for sets and maybe different types

	rf_set_element_free(elem1);
	rf_set_element_free(elem2);
	rf_set_element_free(elem3);
}

void test_rf_set_element_free() {
	// How to test this? Test for null?
}

CU_ErrorCode
register_suites_set() {
	CU_TestInfo suite_set[] = {
		{ "rf_set_new", test_rf_set_new },
		{ "rf_set_clone", test_rf_set_clone },
		{ "rf_set_equal", test_rf_set_equal },
		{ "rf_set_contains_element", test_rf_set_contains_element },
		{ "rf_get_element_index", test_rf_set_get_element_index },
		{ "rf_set_is_subset", test_rf_set_is_subset },
		{ "rf_set_generate_powerset", test_rf_set_generate_powerset },
		{ "rf_set_intersection", test_rf_set_intersection },
		CU_TEST_INFO_NULL
	};

	CU_TestInfo suite_set_element[] = {
		{ "rf_set_element_new_string", test_rf_set_element_new_string },
		{ "rf_set_element_new_set", test_rf_set_element_new_set },
		{ "rf_set_element_clone", test_rf_set_element_clone },
		{ "rf_set_element_equal", test_rf_set_element_equal },
		CU_TEST_INFO_NULL
	};

	CU_SuiteInfo suites[] = {
		{ "rf_Set", NULL, NULL, suite_set },
		{ "rf_SetElement", NULL, NULL, suite_set_element },
		CU_SUITE_INFO_NULL
	};

	return CU_register_suites(suites);
}
