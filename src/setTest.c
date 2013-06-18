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
#include "CUnit/Basic.h"
#include "set.c"

/* Something like @beforeClass
 *
 * @return 0 on success, -1 otherwise
 */
int init_suite1(void) {
	return 0;
}

/* ... and afterClass
 */
int clean_suite1(void) {
	return 0;
}

//Here follow all test cases

void test_rf_set_new() {
	char a = 'a';
	char b = 'b';
	char c = 'c';
	rf_SetElement *elems[3];

	elems[0] = rf_set_element_new_string(&a);
	elems[1] = rf_set_element_new_string(&b);
	elems[2] = rf_set_element_new_string(&c);

	rf_Set *subset = rf_set_new(3, elems);

	CU_ASSERT_EQUAL(*subset->elements[0]->value.string, a);
	CU_ASSERT_EQUAL(*subset->elements[1]->value.string, b);
	CU_ASSERT_EQUAL(*subset->elements[2]->value.string, c);

	//check for double elements
	//uncommented cause assert fails here
	
	//elems[2] = elems[0];
	//subset = rf_set_new(3, elems);
	
}

void test_rf_set_copy() {
	char a = 'a';
	char b = 'b';
	char c = 'c';

	rf_SetElement *elems1[3];
	elems1[0] = rf_set_element_new_string(&a);
	elems1[1] = rf_set_element_new_string(&b);
	elems1[2] = rf_set_element_new_string(&c);
	rf_Set *set1 = rf_set_new(3, elems1);
	
	rf_Set *result = rf_set_copy(set1);
	
	CU_ASSERT_PTR_NOT_EQUAL(set1, result);
	for(int i=0; i<result->cardinality; i++){
	  CU_ASSERT_PTR_NOT_EQUAL(set1->elements[i], result->elements[i]);
	  CU_ASSERT_EQUAL(*set1->elements[i]->value.string, *result->elements[i]->value.string);
	}
}

void test_rf_set_equal() {
	char a = 'a';
	char b = 'b';
	char c = 'c';

	rf_SetElement *elems1[3];
	elems1[0] = rf_set_element_new_string(&a);
	elems1[1] = rf_set_element_new_string(&b);
	elems1[2] = rf_set_element_new_string(&c);
	rf_Set *set1 = rf_set_new(3, elems1);

	rf_SetElement *elems2[3];
	elems2[2] = rf_set_element_new_string(&a);
	elems2[1] = rf_set_element_new_string(&b);
	elems2[0] = rf_set_element_new_string(&c);
	rf_Set *set2 = rf_set_new(3, elems2);

	rf_SetElement *elems3[3];
	elems3[0] = rf_set_element_new_string(&a);
	elems3[1] = rf_set_element_new_string(&b);
	rf_Set *set3 = rf_set_new(3, elems3);


	CU_ASSERT_TRUE(rf_set_equal(set1, set1)) //stupid case
	CU_ASSERT_TRUE(rf_set_equal(set1, set2)) //tricky case, same set with switched order
	CU_ASSERT_FALSE(rf_set_equal(set1, set3)) //false case
}

void test_rf_set_is_subset(){
  
	//watch out, in this testcase, rf_set_is_subset uses a stringcompare. That does not work with a single char
	//need a char array here which is null-byte terminated.
	char a[] = "a";
	char b[] = "b";
	char c[] = "c";
	char d[] = "d";

	rf_SetElement *elems[3];
	elems[0] = rf_set_element_new_string(a);
	elems[1] = rf_set_element_new_string(b);
	elems[2] = rf_set_element_new_string(c);

	char a2[] = "a";
	char b2[] = "b";
	char c2[] = "c";

	rf_SetElement *elems2[3];
	elems2[0] = rf_set_element_new_string(a2);
	elems2[1] = rf_set_element_new_string(b2);
	elems2[2] = rf_set_element_new_string(c2);

	rf_Set *set1 = rf_set_new(3, elems);
	rf_Set *set2 = rf_set_new(3, elems2);

	CU_ASSERT_TRUE(rf_set_is_subset(set1, set1));
	CU_ASSERT_TRUE(rf_set_is_subset(set1, set2)); //sollte mMn gehen. //kann noch perversiert werden mit untersch. Sortierung.

	rf_SetElement *elems3[2];
	elems3[0] = rf_set_element_new_string(a);
	elems3[1] = rf_set_element_new_string(d);
	rf_Set *set3 = rf_set_new(2, elems3);

	CU_ASSERT_FALSE(rf_set_is_subset(set1, set3));
}

void test_rf_bitcount() {
	unsigned int six = 6;
	int result = rf_bitcount(six);
	CU_ASSERT_EQUAL(result, 2);
	unsigned int twentyseven = 27;
	result = rf_bitcount(twentyseven);
	CU_ASSERT_EQUAL(result, 4);
}

void test_rf_set_generate_powerset() {
	char a[] = "a";
	char b[] = "b";
	char c[] = "c";
	
	char results[8][4];
	const char *expected[] = {"a", "b", "c", "ab", "ac", "bc", "abc", "\0"};

	rf_SetElement *elems[3];
	elems[0] = rf_set_element_new_string(a);
	elems[1] = rf_set_element_new_string(b);
	elems[2] = rf_set_element_new_string(c);

	rf_Set *set1 = rf_set_new(3, elems);
	CU_ASSERT_TRUE(set1!=NULL);
	
	rf_Set *powerset = rf_set_generate_powerset(set1);
	CU_ASSERT_EQUAL(powerset->cardinality, 8);
	
	
	for (int i=0; i<8; i++){
	      for (int j = 0; j<3;j++){
		  results[i][j] = 0;
		
	      }
	}
	
	for (int i=0; i<powerset->cardinality; i++){
	  rf_SetElement *tmp = powerset->elements[i];
	  for(int j=0; j<tmp->value.set->cardinality;j++){
	    rf_SetElement *tmp2 = tmp->value.set->elements[j];
	    if (tmp2 != NULL){
	      results[i][j] = *tmp2->value.string;
	    }
	  }
	}
	
	for (int i=0; i<8; i++){
	      bool contains = false;
	      for (int j = 0; j<8;j++){
		 if (strcmp(results[i], expected[j]) == 0){
		  contains = true; 
		 } 
	      }
	      CU_ASSERT_TRUE(contains);
	}
}

void test_rf_set_contains_element() {
	char a = 'a';
	char b = 'b';
	char c = 'c';
	char d = 'd';

	rf_SetElement *elems[3];
	elems[0] = rf_set_element_new_string(&a);
	elems[1] = rf_set_element_new_string(&b);
	elems[2] = rf_set_element_new_string(&c);

	rf_Set *set1 = rf_set_new(3, elems);

	for(int i=0;i<3;i++){
		CU_ASSERT_TRUE(rf_set_contains_element(set1, elems[i]));
	}
	CU_ASSERT_FALSE(rf_set_contains_element(set1, rf_set_element_new_string(&d)));

}

void test_rf_set_intersection(){
	char *a = "a";
	char *b = "b";
	char *c = "c";
	
	rf_SetElement **elems = malloc(sizeof(elems)*3);
	rf_SetElement **elems2 = malloc(sizeof(elems2)*3);

	elems[0] = rf_set_element_new_string(a);
	elems[1] = rf_set_element_new_string(b);
	elems[2] = rf_set_element_new_string(c);
	
	elems2[0] = rf_set_element_new_string(b);
	elems2[1] = rf_set_element_new_string(c);

	rf_Set *set1 = rf_set_new(3, elems);
	rf_Set *set2 = rf_set_new(2, elems2);
	
	rf_Set *result = rf_set_intersection(set1, set2);
	
	CU_ASSERT_EQUAL(result->cardinality, 2);
	CU_ASSERT_TRUE(rf_set_contains_element(result, elems2[0]));
	CU_ASSERT_TRUE(rf_set_contains_element(result, elems2[1]));
  
}

void test_rf_set_get_element_index() {
		char a[] = "a";
		char b[] = "b";
		char c[] = "c";
		char d[] = "d";
		char a2[] = "a";

		rf_SetElement *elems[4];
		elems[0] = rf_set_element_new_string(a);
		elems[1] = rf_set_element_new_string(b);
		elems[2] = rf_set_element_new_string(c);
		rf_SetElement *test = rf_set_element_new_string(d);
		rf_SetElement *test2 = rf_set_element_new_string(a2);
		rf_Set *set1 = rf_set_new(3, elems);

		for(int i=0;i<3;i++){
			CU_ASSERT_EQUAL(rf_set_get_element_index(set1, elems[i]), i);
		}
		
		CU_ASSERT_EQUAL(rf_set_get_element_index(set1, test2), 0);

		CU_ASSERT_EQUAL(rf_set_get_element_index(set1, test), -1);
}


void test_rf_set_free() {

}

void test_rf_set_element_new_string() {
	//Test valid element
	char a = 'a';
	rf_SetElement *element_a = rf_set_element_new_string(&a);
	CU_ASSERT_PTR_EQUAL(element_a->value.string, &a);
	CU_ASSERT_EQUAL(*element_a->value.string, a);

	//Test valid element with mutated vowel
	char mv = '\x94';
	rf_SetElement *element_mv = rf_set_element_new_string(&mv);
	CU_ASSERT_PTR_EQUAL(element_mv->value.string, &mv);
	CU_ASSERT_EQUAL(*element_a->value.string, a);

	//Test empty value
	char em;
	rf_SetElement *element_em = rf_set_element_new_string(&em);
	CU_ASSERT_PTR_EQUAL(element_em->value.string, &em);
	CU_ASSERT_EQUAL(*element_em->value.string, em);

	CU_ASSERT_EQUAL(element_a->type, RF_SET_ELEMENT_TYPE_STRING);
	CU_ASSERT_EQUAL(element_mv->type, RF_SET_ELEMENT_TYPE_STRING);
	CU_ASSERT_EQUAL(element_em->type, RF_SET_ELEMENT_TYPE_STRING);

	//Test Null Pointer
	//char *null = NULL;
	//rf_SetElement *element_null = rf_set_element_new_string(*null);
	//CU_ASSERT_PTR_NULL(element_null);
}

void test_rf_set_element_new_set() {
	char a = 'a';
	char b = 'b';
	char c = 'c';

	rf_SetElement *elems[3];

	elems[0] = rf_set_element_new_string(&a);
	elems[1] = rf_set_element_new_string(&b);
	elems[2] = rf_set_element_new_string(&c);
	rf_Set *subset = rf_set_new(3, elems);
	rf_SetElement *elem = rf_set_element_new_set(subset);

	CU_ASSERT_EQUAL(elem->type, RF_SET_ELEMENT_TYPE_SET);
	CU_ASSERT_PTR_EQUAL(elem->value.set, subset);
}

void test_rf_set_element_copy() {
	//case, element is a char
	char a = 'a';
	rf_SetElement *source = rf_set_element_new_string(&a);
	rf_SetElement *dest = rf_set_element_copy(source);

	CU_ASSERT_PTR_NOT_EQUAL(dest, source);
	CU_ASSERT_EQUAL(*dest->value.string, *source->value.string);

	//case element is a subset
	char b = 'b';
	char c = 'c';

	rf_SetElement *elems[3];

	elems[0] = rf_set_element_new_string(&a);
	elems[1] = rf_set_element_new_string(&b);
	elems[2] = rf_set_element_new_string(&c);

	rf_Set *subset = rf_set_new(3, elems);

	rf_SetElement *source2 = rf_set_element_new_set(subset);
	rf_SetElement *dest2 = rf_set_element_copy(source2);

	//Must not work on same elements by copying pointers
	CU_ASSERT_PTR_NOT_EQUAL(dest2, source2);
	CU_ASSERT_PTR_NOT_EQUAL(dest2->value.set, subset);

	rf_Set *destSet = dest2->value.set;

	rf_SetElement *temp = elems[0];
	rf_SetElement *temp1 = elems[1];
	rf_SetElement *temp2 = elems[2];

	CU_ASSERT_EQUAL(*destSet->elements[0]->value.string, *temp->value.string);
	CU_ASSERT_EQUAL(*destSet->elements[1]->value.string, *temp1->value.string);
	CU_ASSERT_EQUAL(*destSet->elements[2]->value.string, *temp2->value.string);
}

void test_rf_set_element_equal() {
	char a[] = "a";
	char b[] = "b";
	char a2[] = "a";
	rf_SetElement *elem_a = rf_set_element_new_string(a);
	rf_SetElement *elem_b = rf_set_element_new_string(b);
	rf_SetElement *elem_a2 = rf_set_element_new_string(a2);

	CU_ASSERT_TRUE(rf_set_element_equal(elem_a, elem_a));

	//Strcmp liefert hier einen Wert >0, kset_copyeine Ahnung wieso.
	CU_ASSERT_TRUE(rf_set_element_equal(elem_a, elem_a2));

	CU_ASSERT_FALSE(rf_set_element_equal(elem_a, elem_b));

	//also test for sets and maybe different types
}

void test_rf_set_element_free() {
	// How to test this? Test for null?
}

int add_suites(CU_pSuite *suite) {

	//Maybe suite 1
	CU_add_test(*suite, "rf_set_new", test_rf_set_new);
	CU_add_test(*suite, "rf_set_equal", test_rf_set_equal);
	CU_add_test(*suite, "rf_set_contains_element", test_rf_set_contains_element);
	CU_add_test(*suite, "rf_get_element_index", test_rf_set_get_element_index);
	CU_add_test(*suite, "rf_set_is_subset", test_rf_set_is_subset);
	CU_add_test(*suite, "rf_set_generate_powerset", test_rf_set_generate_powerset);
	CU_add_test(*suite, "rf_set_intersection", test_rf_set_intersection);
	CU_add_test(*suite, "rf_set_copy", test_rf_set_copy);
	CU_add_test(*suite, "rf_bitcount", test_rf_bitcount);
	
	
	//Maybe suite 2
	CU_add_test(*suite, "rf_element_new_string", test_rf_set_element_new_string);
	CU_add_test(*suite, "rf_set_element_new_set", test_rf_set_element_new_set);
	CU_add_test(*suite, "rf_element_copy", test_rf_set_element_copy);
	CU_add_test(*suite, "rf_element_equal", test_rf_set_element_equal);

	return 0;
}

int main() {
	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	CU_pSuite pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	add_suites(&pSuite);

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
