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

#include "error.h"
#include "set.h"
#include "relation.h"

extern size_t rf_table_idx(rf_Relation *, int, int);

char a[] = "a";
char b[] = "b";
char c[] = "c";
char d[] = "d";
char e[] = "e";
char g[] = "g";
char i[] = "i";

rf_Set *set;
rf_Set *set2;

void generateTestElements(int n, rf_SetElement *elements[n]){
	char buf[] = "a";
	for(int i = 0; i < n; i++) {
		buf[0] = 'a' + i;
		elements[i] = rf_set_element_new_string(buf);
	}
}

int init_suite(void) {
	rf_SetElement *elems[3];
	rf_SetElement *elems2[1];

	generateTestElements(3, elems);

	elems2[0] = rf_set_element_new_string(d);

	set = rf_set_new(3, elems);
	set2 = rf_set_new(1, elems2);

	return 0;
}


int clean_suite(void) {
	rf_set_free(set);
	rf_set_free(set2);

	return 0;
}

void test_rf_relation_calc() {
	rf_Relation * rel = rf_relation_new_id(set);

	bool result1 = rf_relation_calc(rel, rf_set_element_new_string(b), rf_set_element_new_string(b), NULL);
	bool result2 = rf_relation_calc(rel, rf_set_element_new_string(c), rf_set_element_new_string(b), NULL);

	CU_ASSERT_TRUE(result1);
	CU_ASSERT_FALSE(result2);
}

void test_rf_relation_new_empty(){
	rf_Relation *empty_relation = rf_relation_new_empty(set, set);

	CU_ASSERT_PTR_NOT_NULL(empty_relation);

	CU_ASSERT_PTR_EQUAL(set, empty_relation->domains[0]);
	CU_ASSERT_PTR_EQUAL(set, empty_relation->domains[1]);

	int n = (set->cardinality) * (set->cardinality);
	for(int i = 0; i < n; i++){
		CU_ASSERT_FALSE(empty_relation->table[i]);
	}
}

void test_rf_relation_new_full(){
	rf_Relation *full_relation = rf_relation_new_full(set, set);

	CU_ASSERT_PTR_NOT_NULL(full_relation);

	CU_ASSERT_PTR_EQUAL(set, full_relation->domains[0]);
	CU_ASSERT_PTR_EQUAL(set, full_relation->domains[1]);

	int n = (set->cardinality) * (set->cardinality);
	for(int i = 0; i < n; i++){
		CU_ASSERT_TRUE(full_relation->table[i]);
	}
}

void test_rf_relation_new(){
	size_t n = set->cardinality * set->cardinality;
	bool *table = calloc(n, sizeof(table));

	table[0] = table[4] = table[8] = true;

	rf_Relation *relation = rf_relation_new(set, set, table);
	CU_ASSERT_PTR_NOT_NULL(relation);

	CU_ASSERT_PTR_EQUAL(set, relation->domains[0]);
	CU_ASSERT_PTR_EQUAL(set, relation->domains[1]);

	for(int i = 0; i < n; i++){
		if(i % (set->cardinality+1) == 0){
			CU_ASSERT_TRUE(relation->table[i]);
		} else {
			CU_ASSERT_FALSE(relation->table[i]);
		}
	}
}

void test_rf_relation_clone(){
	size_t n = set->cardinality * set->cardinality;
	bool *table = calloc(n, sizeof(table));

	table[0] = table[4] = table[8] = true;

	rf_Relation *source = rf_relation_new(set, set, table);
	rf_Relation *result = rf_relation_clone(source);

	CU_ASSERT_PTR_NOT_EQUAL(source, result);
	CU_ASSERT_PTR_NOT_EQUAL(set, result->domains[0]);
	CU_ASSERT_PTR_NOT_EQUAL(set, result->domains[1]);
	CU_ASSERT_PTR_NOT_EQUAL(table, result->table);

	CU_ASSERT_EQUAL(set->cardinality, result->domains[0]->cardinality);

	for(int i = 0; i < n; i++){
		if(i % (set->cardinality+1) == 0){
			CU_ASSERT_TRUE(result->table[i]);
		} else {
			CU_ASSERT_FALSE(result->table[i]);
		}
	}
}

void test_rf_relation_new_id(){
	rf_Relation * relation = rf_relation_new_id(set);
	CU_ASSERT_PTR_NOT_NULL(relation);

	CU_ASSERT_PTR_EQUAL(relation->domains[0], set);
	CU_ASSERT_PTR_EQUAL(relation->domains[1], set);

	int n = set->cardinality * set->cardinality;
	for(int i = 0; i < n; i++){
		if(i % (set->cardinality+1) == 0){
			CU_ASSERT_TRUE(relation->table[i]);
		} else {
			CU_ASSERT_FALSE(relation->table[i]);
		}
	}
}

void test_rf_table_idx(){
	rf_Relation *relation = rf_relation_new_full(set, set);

	for(int x=0;x<set->cardinality;x++){
		for(int y=0;y<set->cardinality;y++){
			CU_ASSERT_EQUAL(rf_table_idx(relation, x,y), (x*set->cardinality)+y);
		}
	}
}

void test_rf_relation_new_top(){
	rf_Relation *relation = rf_relation_new_top(set);

	CU_ASSERT_TRUE(relation->table[rf_table_idx(relation, 0, 0)]);
	CU_ASSERT_TRUE(relation->table[rf_table_idx(relation, 0, 1)]);
	CU_ASSERT_TRUE(relation->table[rf_table_idx(relation, 0, 2)]);

	CU_ASSERT_FALSE(relation->table[rf_table_idx(relation, 1, 0)]);
	CU_ASSERT_TRUE(relation->table[rf_table_idx(relation, 1, 1)]);
	CU_ASSERT_TRUE(relation->table[rf_table_idx(relation, 1, 2)]);

	CU_ASSERT_FALSE(relation->table[rf_table_idx(relation, 2, 0)]);
	CU_ASSERT_FALSE(relation->table[rf_table_idx(relation, 2, 1)]);
	CU_ASSERT_TRUE(relation->table[rf_table_idx(relation, 2, 2)]);

}

void test_rf_relation_new_bottom(){
	rf_Relation *relation = rf_relation_new_bottom(set);

	CU_ASSERT_TRUE(relation->table[rf_table_idx(relation, 0, 0)]);
	CU_ASSERT_FALSE(relation->table[rf_table_idx(relation, 0, 1)]);
	CU_ASSERT_FALSE(relation->table[rf_table_idx(relation, 0, 2)]);

	CU_ASSERT_TRUE(relation->table[rf_table_idx(relation, 1, 0)]);
	CU_ASSERT_TRUE(relation->table[rf_table_idx(relation, 1, 1)]);
	CU_ASSERT_FALSE(relation->table[rf_table_idx(relation, 1, 2)]);

	CU_ASSERT_TRUE(relation->table[rf_table_idx(relation, 2, 0)]);
	CU_ASSERT_TRUE(relation->table[rf_table_idx(relation, 2, 1)]);
	CU_ASSERT_TRUE(relation->table[rf_table_idx(relation, 2, 2)]);

}

void test_rf_relation_new_union(){
	 //inits
	rf_Error error;
	rf_Relation *idR = rf_relation_new_id(set);
	rf_Relation *r1 = rf_relation_new_empty(set, set);
	rf_Relation *result;

	rf_Relation *idRSet2 = rf_relation_new_id(set2);

	//TESTS

	//Union between id-Relation and an empty relation
	result = rf_relation_new_union(idR, r1, &error);
	int n = set->cardinality * set->cardinality;
	for(int i=0;i<n;i++){
		CU_ASSERT_EQUAL(idR->table[i], result->table[i]);
	}

	//Union id-Relation with itself
	result = rf_relation_new_union(idR, idR, &error);
	for(int i=0;i<n;i++){
		CU_ASSERT_EQUAL(idR->table[i], result->table[i]);
	}

	//Union between id-R and its complement
	result = rf_relation_new_union(idR, rf_relation_new_complement(idR, &error), &error);

	for(int i=0;i<n;i++){
		CU_ASSERT_TRUE(result->table[i]);
	}

	//Union between two relations that have different domains
	result = rf_relation_new_union(idR, idRSet2, &error);
	CU_ASSERT_TRUE(result == NULL);
}

void test_rf_relation_new_intersection(){
	//inits
	rf_Error error;
	rf_Relation *idR = rf_relation_new_id(set);
	rf_Relation *r1 = rf_relation_new_empty(set, set);
	rf_Relation *r2 = rf_relation_new_empty(set, set);
	r2->table[rf_table_idx(r1,1,1)] = true;
	rf_Relation *result;

	rf_Relation *idRSet2 = rf_relation_new_id(set2);

	//Intersection between id-Relation and an empty relation
	result = rf_relation_new_intersection(idR, r1, &error);
	int n = set->cardinality * set->cardinality;
	for(int i=0;i<n;i++){
		CU_ASSERT_FALSE(result->table[i]);
	}

	//Intersection id-Relation with itself
	result = rf_relation_new_intersection(idR, idR, &error);
	for(int i=0;i<n;i++){
		CU_ASSERT_EQUAL(idR->table[i], result->table[i]);
	}

	//Intersection id-Relation with a relation having one intersection element
	result = rf_relation_new_intersection(idR, r2, &error);
	for(int i=0;i<n;i++){
		if(i == rf_table_idx(result, 1,1)){
			CU_ASSERT_TRUE(result->table[i])
		} else{
			CU_ASSERT_FALSE(result->table[i]);
		}
	}

	//Intersection between two relations that have different domains
	result = rf_relation_new_intersection(idR, idRSet2, &error);
	CU_ASSERT_TRUE(result == NULL);
}

void test_rf_relation_new_complement(){
	rf_Error error;
	rf_Relation *idR = rf_relation_new_id(set);

	//The error function is never used in this method...consider refactoring
	rf_Relation *result = rf_relation_new_complement(idR, &error);

	int n = set->cardinality * set->cardinality;
	for(int i=0;i<n;i++){
		CU_ASSERT_NOT_EQUAL(idR->table[i], result->table[i]);
	}
}

void test_rf_relation_new_concatenation(){
	//often also called composition, which is I think the more correct terminus

	rf_Error error;
	rf_Relation *r1 = rf_relation_new_empty(set, set);
	rf_Relation *r2 = rf_relation_new_empty(set, set);

	r1->table[rf_table_idx(r1,0,1)] = r1->table[rf_table_idx(r1,1,1)] = r1->table[rf_table_idx(r1,2,1)] = true;
	r2->table[rf_table_idx(r2,1,0)] = r2->table[rf_table_idx(r2,1,1)] = r2->table[rf_table_idx(r2,1,2)] = true;

	rf_Relation *result = rf_relation_new_concatenation(r1, r2, &error);

	//should be the full relation in this example
	int n = set->cardinality * set->cardinality;

	for(int i=0;i<n;i++){
	CU_ASSERT_TRUE(result->table[i]);
	}

	//TODO: Need more testcases: Other concacts with no full relation result and for heterogeneous relations, too

	}

void test_rf_relation_new_converse(){
	rf_Error error;
	rf_Relation *r1 = rf_relation_new_empty(set, set);
	r1->table[rf_table_idx(r1,0,2)] = true;
	r1->table[rf_table_idx(r1,1,1)] = true;

	rf_Relation *result = rf_relation_new_converse(r1, &error);
	CU_ASSERT_TRUE(result->table[rf_table_idx(result,2,0)]);
	CU_ASSERT_TRUE(result->table[rf_table_idx(result,1,1)]);
}

void test_rf_relation_new_subsetleq(){
	//trivial case
	int n = set->cardinality * set->cardinality;
	rf_Relation *empty = rf_relation_new_subsetleq(set, NULL);

	for(int i=0;i<n;i++){
		CU_ASSERT_FALSE(empty->table[i]);
	}

	/*
	* testSet = (a,(1,2,3),(1,2))
	*
	* result of subsetleq(testSet) should be:
	*
	*  0 0 0
	*  0 1 1
	*  0 0 1
	*/

	char one[] = "1";
	char two[] = "2";
	char three[] = "3";

	rf_SetElement *elemsSubset1[3];
	rf_SetElement *elemsSubset2[2];

	elemsSubset1[0] = rf_set_element_new_string(one);
	elemsSubset1[1] = rf_set_element_new_string(two);
	elemsSubset1[2] = rf_set_element_new_string(three);

	elemsSubset2[0] = rf_set_element_new_string(one);
	elemsSubset2[1] = rf_set_element_new_string(two);

	rf_Set *subset1 = rf_set_new(3, elemsSubset1);
	rf_Set *subset2 = rf_set_new(2, elemsSubset2);

	rf_SetElement *elemsSet[3];
	elemsSet[0] = rf_set_element_new_string(a);
	elemsSet[1] = rf_set_element_new_set(subset1);
	elemsSet[2] = rf_set_element_new_set(subset2);

	CU_ASSERT_TRUE(elemsSet[1]->type == RF_SET_ELEMENT_TYPE_SET);

	rf_Set *testSet = rf_set_new(3, elemsSet);

	rf_SetElement *tmp = *testSet->elements;
	CU_ASSERT_TRUE(elemsSet[1]->type == RF_SET_ELEMENT_TYPE_SET);
	CU_ASSERT_TRUE(tmp[1].type == RF_SET_ELEMENT_TYPE_STRING);

	rf_Relation *expected = rf_relation_new_empty(testSet, testSet);
	expected->table[rf_table_idx(expected,1,1)] = true;
	expected->table[rf_table_idx(expected,1,2)] = true;
	expected->table[rf_table_idx(expected,2,2)] = true;

	rf_Relation *result = rf_relation_new_subsetleq(testSet, NULL);
	for(int i=0;i<n;i++){
	CU_ASSERT_EQUAL(expected->table[i], result->table[i]);
	}
	}

void test_rf_relation_is_homogeneous(){
	rf_Relation *homogeneous = rf_relation_new_empty(set, set);
	rf_Relation *heterogeneous = rf_relation_new_empty(set, set2);

	CU_ASSERT_TRUE(rf_relation_is_homogeneous(homogeneous));
	CU_ASSERT_FALSE(rf_relation_is_homogeneous(heterogeneous));
	}

void test_rf_relation_is_antisymmetric(){
	// R schnitt RT <= I

	rf_Relation *r1 = rf_relation_new_id(set);
	CU_ASSERT_TRUE(rf_relation_is_antisymmetric(r1));

	r1->table[rf_table_idx(r1,0,2)] = true;
	CU_ASSERT_TRUE(rf_relation_is_antisymmetric(r1));

	r1->table[rf_table_idx(r1,2,0)] = true;
	CU_ASSERT_FALSE(rf_relation_is_antisymmetric(r1));
	}

void test_rf_relation_is_asymmetric(){
	// R schnitt RT <= I

	rf_Relation *r1 = rf_relation_new_id(set);
	CU_ASSERT_FALSE(rf_relation_is_asymmetric(r1));

	r1 = rf_relation_new_empty(set, set);
	r1->table[rf_table_idx(r1,0,2)] = true;
	CU_ASSERT_TRUE(rf_relation_is_asymmetric(r1));

	r1->table[rf_table_idx(r1,2,0)] = true;
	CU_ASSERT_FALSE(rf_relation_is_asymmetric(r1));
}

void test_rf_relation_is_difunctional(){
	rf_SetElement *elems[4];
	generateTestElements(4, elems);

	rf_Set *mySet = rf_set_new(4, elems);

	rf_Relation *rel = rf_relation_new_empty(mySet, mySet);

	rel->table[rf_table_idx(rel,0,2)] = true;
	rel->table[rf_table_idx(rel,0,3)] = true;
	rel->table[rf_table_idx(rel,1,0)] = true;
	rel->table[rf_table_idx(rel,2,1)] = true;
	rel->table[rf_table_idx(rel,3,2)] = true;

	//Not difunctional, should be false
	bool returnCode = rf_relation_is_difunctional(rel);
	CU_ASSERT_FALSE(returnCode);

	//make it difunctional
	rel->table[rf_table_idx(rel,3,3)] = true;

	//now it should return true
	returnCode = rf_relation_is_difunctional(rel);
	CU_ASSERT_TRUE(returnCode);

}

void test_rf_relation_is_equivalent(){
	rf_Relation *full = rf_relation_new_full(set, set);
	rf_Relation *id = rf_relation_new_id(set);
	rf_Relation *false1 = rf_relation_new_full(set, set);
	false1->table[rf_table_idx(false1, 0,0)] = false;
	rf_Relation *false2 = rf_relation_new_id(set);
	false2->table[rf_table_idx(false2, 0,2)] = true;

	CU_ASSERT_TRUE(rf_relation_is_equivalent(full));
	CU_ASSERT_TRUE(rf_relation_is_equivalent(id));
	CU_ASSERT_FALSE(rf_relation_is_equivalent(false1));
	CU_ASSERT_FALSE(rf_relation_is_equivalent(false2));
}

void test_rf_relation_is_symmetric(){
	rf_Relation *false1 = rf_relation_new_id(set);
	false1->table[rf_table_idx(false1, 0,2)] = true;

	CU_ASSERT_FALSE(rf_relation_is_symmetric(false1));

	false1->table[rf_table_idx(false1, 2,0)] = true;

	CU_ASSERT_TRUE(rf_relation_is_symmetric(false1));
}

void test_rf_relation_is_irreflexive(){
	//true examples
	rf_Relation *true1 = rf_relation_new_empty(set, set);
	rf_Relation *true2 = rf_relation_new_empty(set, set);
	true2->table[rf_table_idx(true2, 0,2)] = true;

	//false examples
	rf_Relation *false2 = rf_relation_new_full(set, set);
	false2->table[rf_table_idx(false2, 0,0)] = false;
	rf_Relation *false3 = rf_relation_new_id(set);
	rf_Relation *false4 = rf_relation_new_empty(set, set2);

	CU_ASSERT_TRUE(rf_relation_is_irreflexive(true1));
	CU_ASSERT_TRUE(rf_relation_is_irreflexive(true2));

	CU_ASSERT_FALSE(rf_relation_is_irreflexive(false2));
	CU_ASSERT_FALSE(rf_relation_is_irreflexive(false3));
	CU_ASSERT_FALSE(rf_relation_is_irreflexive(false4));
}

void test_rf_relation_is_reflexive(){
	rf_Relation *id = rf_relation_new_id(set);
	rf_Relation *full = rf_relation_new_full(set, set);
	rf_Relation *true3 = rf_relation_new_id(set);
	true3->table[rf_table_idx(true3, 0,2)] = true;

	rf_Relation *false1 = rf_relation_new_full(set, set);
	false1->table[rf_table_idx(false1, 0,0)] = false;
	rf_Relation *empty = rf_relation_new_empty(set, set);

	CU_ASSERT_TRUE(rf_relation_is_reflexive(id));
	CU_ASSERT_TRUE(rf_relation_is_reflexive(full));
	CU_ASSERT_TRUE(rf_relation_is_reflexive(true3));

	CU_ASSERT_FALSE(rf_relation_is_reflexive(false1));
	CU_ASSERT_FALSE(rf_relation_is_reflexive(empty));
}

void test_rf_relation_is_partial_order(){
	//just depends on reflexive, transitive and antisymetrie
	//if those three are correct this method works correct,
	//as long as it is implemented like this.
}

void test_rf_relation_is_preoder(){
	//depends on reflexive and transitive
}

void test_rf_relation_is_transitive(){
	rf_Relation *full = rf_relation_new_full(set, set);
	rf_Relation *empty = rf_relation_new_empty(set, set);
	rf_Relation *variation = rf_relation_new_empty(set, set);
	variation->table[rf_table_idx(variation, 0,1)] = true;

	CU_ASSERT_TRUE(rf_relation_is_transitive(full));
	CU_ASSERT_TRUE(rf_relation_is_transitive(empty));

	CU_ASSERT_TRUE(rf_relation_is_transitive(variation));

	variation->table[rf_table_idx(variation, 1,2)] = true;
	CU_ASSERT_FALSE(rf_relation_is_transitive(variation));

	variation->table[rf_table_idx(variation, 0,2)] = true;
	CU_ASSERT_TRUE(rf_relation_is_transitive(variation));
}

void test_rf_relation_find_maximum(){
	rf_Relation *id = rf_relation_new_id(set);

	id->table[rf_table_idx(id, 0,1)] = true;
	id->table[rf_table_idx(id, 1,2)] = true;
	id->table[rf_table_idx(id, 1,0)] = true;

	CU_ASSERT_EQUAL(rf_relation_find_maximum(id, NULL), NULL);
	id->table[rf_table_idx(id, 0,1)] = false;
	CU_ASSERT_TRUE(rf_set_element_equal(rf_relation_find_maximum(id, NULL), id->domains[0]->elements[1]));
}

void test_rf_relation_find_minimum(){
	rf_Relation *id = rf_relation_new_id(set);
	id->table[rf_table_idx(id, 0,2)] = true;
	id->table[rf_table_idx(id, 1,2)] = true;
	CU_ASSERT_TRUE(rf_set_element_equal(rf_relation_find_minimum(id, NULL),id->domains[0]->elements[2]));

	id->table[rf_table_idx(id, 0,1)] = true;
	id->table[rf_table_idx(id, 0,2)] = false;
	id->table[rf_table_idx(id, 1,0)] = true;
	CU_ASSERT_EQUAL(rf_relation_find_minimum(id, NULL), NULL);
	id->table[rf_table_idx(id, 0,1)] = false;
	CU_ASSERT_EQUAL(rf_relation_find_minimum(id, NULL), NULL);
}

void test_rf_relation_find_maximum_within_subset(){

	rf_SetElement *elems[5];

	elems[0] = rf_set_element_new_string(a);
	elems[1] = rf_set_element_new_string(b);
	elems[2] = rf_set_element_new_string(c);
	elems[3] = rf_set_element_new_string(d);
	elems[4] = rf_set_element_new_string(e);

	rf_SetElement *elems2[1];
	elems2[0] = rf_set_element_new_string(e);

	rf_Set *superSet = rf_set_new(5, elems);
	rf_Set *subset = rf_set_new(1, elems2);

	rf_Relation *relation2 = rf_relation_new_id(superSet);
	relation2->table[rf_table_idx(relation2, 0,2)] = true;
	relation2->table[rf_table_idx(relation2, 0,3)] = true;
	relation2->table[rf_table_idx(relation2, 0,4)] = true;
	relation2->table[rf_table_idx(relation2, 1,2)] = true;
	relation2->table[rf_table_idx(relation2, 1,3)] = true;
	relation2->table[rf_table_idx(relation2, 1,4)] = true;
	relation2->table[rf_table_idx(relation2, 2,4)] = true;
	relation2->table[rf_table_idx(relation2, 3,4)] = true;

	rf_SetElement *expected = rf_relation_find_maximum_within_subset(relation2, subset, NULL);

	CU_ASSERT_TRUE(rf_set_element_equal(expected, elems[4]));

	//another test

	  rf_SetElement *elems6[9];
	  generateTestElements(9, elems6);

	  rf_Set *superSet2 = rf_set_new(9, elems6);
	  relation2 = rf_relation_new_top(superSet2);

	  relation2->table[rf_table_idx(relation2, 1, 2)] = false;
	  relation2->table[rf_table_idx(relation2, 1, 5)] = false;
	  relation2->table[rf_table_idx(relation2, 2, 3)] = false;
	  relation2->table[rf_table_idx(relation2, 3, 4)] = false;
	  relation2->table[rf_table_idx(relation2, 3, 5)] = false;
	  relation2->table[rf_table_idx(relation2, 3, 7)] = false;
	  relation2->table[rf_table_idx(relation2, 4, 5)] = false;
	  relation2->table[rf_table_idx(relation2, 5, 6)] = false;
	  relation2->table[rf_table_idx(relation2, 6, 7)] = false;

	  rf_SetElement *elems5[3];

	  elems5[0] = rf_set_element_new_string(d);
	  elems5[1] = rf_set_element_new_string(g);
	  elems5[2] = rf_set_element_new_string(i);

	  expected = rf_relation_find_maximum_within_subset(relation2, rf_set_new(3, elems5), NULL);
	  CU_ASSERT_TRUE(rf_set_element_equal(expected, elems5[0]));


}

void test_rf_relation_find_minimal_elements(){
	rf_SetElement *elems6[9];
	generateTestElements(9, elems6);

	rf_Set *superSet2 = rf_set_new(9, elems6);
	rf_Relation *relation2 = rf_relation_new_top(superSet2);

	relation2->table[rf_table_idx(relation2, 1, 2)] = false;
	relation2->table[rf_table_idx(relation2, 1, 5)] = false;
	relation2->table[rf_table_idx(relation2, 2, 3)] = false;
	relation2->table[rf_table_idx(relation2, 3, 4)] = false;
	relation2->table[rf_table_idx(relation2, 3, 5)] = false;
	relation2->table[rf_table_idx(relation2, 3, 7)] = false;
	relation2->table[rf_table_idx(relation2, 4, 5)] = false;
	relation2->table[rf_table_idx(relation2, 5, 6)] = false;
	relation2->table[rf_table_idx(relation2, 6, 7)] = false;

	rf_SetElement *elems5[3];

	elems5[0] = rf_set_element_new_string(d);
	elems5[1] = rf_set_element_new_string(g);
	elems5[2] = rf_set_element_new_string(i);

	rf_SetElement *elems7[2];
	elems7[0] = rf_set_element_new_string(b);
	elems7[1] = rf_set_element_new_string(d);
	elems7[2] = rf_set_element_new_string(e);

	rf_Set *expected = rf_relation_find_minimal_elements(relation2, rf_set_new(3, elems5), NULL);
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems5[2]));
	CU_ASSERT_EQUAL(expected->cardinality, 1);

	expected = rf_relation_find_minimal_elements(relation2, rf_set_new(3, elems7), NULL);
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems7[1]));
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems7[2]));
	CU_ASSERT_EQUAL(expected->cardinality, 2);
}

void test_rf_relation_find_maximal_elements(){
	rf_SetElement *elems6[9];
	generateTestElements(9, elems6);

	rf_Set *superSet2 = rf_set_new(9, elems6);
	rf_Relation *relation2 = rf_relation_new_top(superSet2);

	relation2->table[rf_table_idx(relation2, 1, 2)] = false;
	relation2->table[rf_table_idx(relation2, 1, 5)] = false;
	relation2->table[rf_table_idx(relation2, 2, 3)] = false;
	relation2->table[rf_table_idx(relation2, 3, 4)] = false;
	relation2->table[rf_table_idx(relation2, 3, 5)] = false;
	relation2->table[rf_table_idx(relation2, 3, 7)] = false;
	relation2->table[rf_table_idx(relation2, 4, 5)] = false;
	relation2->table[rf_table_idx(relation2, 5, 6)] = false;
	relation2->table[rf_table_idx(relation2, 6, 7)] = false;

	rf_SetElement *elems5[4];

	elems5[0] = rf_set_element_new_string(d);
	elems5[1] = rf_set_element_new_string(g);
	elems5[2] = rf_set_element_new_string(i);

	rf_SetElement *elems7[2];
	elems7[0] = rf_set_element_new_string(b);
	elems7[1] = rf_set_element_new_string(d);
	elems7[2] = rf_set_element_new_string(c);

	rf_Set *expected = rf_relation_find_maximal_elements(relation2, rf_set_new(3, elems5), NULL);
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems5[0]));
	CU_ASSERT_EQUAL(expected->cardinality, 1);

	expected = rf_relation_find_minimal_elements(relation2, rf_set_new(3, elems7), NULL);
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems7[1]));
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems7[2]));
	CU_ASSERT_EQUAL(expected->cardinality, 2)
}


void test_rf_relation_find_supremum(){
	/*
	 *   a b c d
	 * a 1     1
	 * b 1 1   1
	 * c     1
	 * d       1
	 *
	 * M = (a,b,c,d)
	 * S = (b,c,d)
	 * S2 = (b,d)
	 */
	rf_SetElement *elems[4];
	rf_SetElement *elems3[2];

	generateTestElements(4, elems);

	elems3[0] = rf_set_element_new_string(a);
	elems3[1] = rf_set_element_new_string(d);

	rf_Set *superSet = rf_set_new(4, elems);
	rf_Set *subset2 = rf_set_new(2, elems3);

	rf_Relation *relation = rf_relation_new_id(superSet);
	relation->table[rf_table_idx(relation, 0,3)] = true;
	relation->table[rf_table_idx(relation, 1,0)] = true;
	relation->table[rf_table_idx(relation, 1,3)] = true;

	rf_SetElement *expected = rf_relation_find_supremum(relation, subset2, NULL);

	CU_ASSERT_TRUE(rf_set_element_equal(expected, elems[0]));

	//another test

	rf_SetElement *elems4[9];
	generateTestElements(9, elems4);

	rf_Set *superSet2 = rf_set_new(9, elems4);
	rf_Relation *relation2 = rf_relation_new_top(superSet2);

	relation2->table[rf_table_idx(relation2, 1, 2)] = false;
	relation2->table[rf_table_idx(relation2, 1, 5)] = false;
	relation2->table[rf_table_idx(relation2, 2, 3)] = false;
	relation2->table[rf_table_idx(relation2, 3, 4)] = false;
	relation2->table[rf_table_idx(relation2, 3, 5)] = false;
	relation2->table[rf_table_idx(relation2, 3, 7)] = false;
	relation2->table[rf_table_idx(relation2, 4, 5)] = false;
	relation2->table[rf_table_idx(relation2, 5, 6)] = false;
	relation2->table[rf_table_idx(relation2, 6, 7)] = false;

	rf_SetElement *elems5[2];

	elems5[0] = rf_set_element_new_string(b);
	elems5[1] = rf_set_element_new_string(d);

	expected = rf_relation_find_supremum(relation2, rf_set_new(2, elems5), NULL);
	CU_ASSERT_TRUE(rf_set_element_equal(expected, elems4[1]));
}

void test_rf_relation_find_infimum(){
	rf_SetElement *elems[5];
	rf_SetElement *elems2[2];
	rf_SetElement *elems3[2];
	rf_SetElement *elems4[3];

	generateTestElements(5, elems);
	generateTestElements(2, elems2);

	elems3[0] = rf_set_element_new_string(c);
	elems3[1] = rf_set_element_new_string(d);

	generateTestElements(3, elems4);

	rf_Set *superSet = rf_set_new(5, elems);
	rf_Set *subset2 = rf_set_new(2, elems2);
	rf_Set *subset3 = rf_set_new(2, elems3);
	rf_Set *subset4 = rf_set_new(3, elems4);

	rf_Relation *relation = rf_relation_new_id(superSet);
	relation->table[rf_table_idx(relation, 0,2)] = true;
	relation->table[rf_table_idx(relation, 1,2)] = true;
	relation->table[rf_table_idx(relation, 0,3)] = true;
	relation->table[rf_table_idx(relation, 1,3)] = true;
	relation->table[rf_table_idx(relation, 0,4)] = true;
	relation->table[rf_table_idx(relation, 1,4)] = true;
	relation->table[rf_table_idx(relation, 2,4)] = true;
	relation->table[rf_table_idx(relation, 3,4)] = true;

	//ab
	rf_SetElement *expected = rf_relation_find_infimum(relation, subset2, NULL);
	//cd
	rf_SetElement *expected2 = rf_relation_find_infimum(relation, subset3, NULL);
	//abc
	rf_SetElement *expected3 = rf_relation_find_infimum(relation, subset4, NULL);

	CU_ASSERT_TRUE(expected == NULL);
	CU_ASSERT_TRUE(rf_set_element_equal(expected2, elems[4]));
	CU_ASSERT_TRUE(rf_set_element_equal(expected3, elems[2]));

	//another test

	rf_SetElement *elems6[9];
	generateTestElements(9, elems6);

	rf_Set *superSet2 = rf_set_new(9, elems6);
	rf_Relation *relation2 = rf_relation_new_top(superSet2);

	relation2->table[rf_table_idx(relation2, 1, 2)] = false;
	relation2->table[rf_table_idx(relation2, 1, 5)] = false;
	relation2->table[rf_table_idx(relation2, 2, 3)] = false;
	relation2->table[rf_table_idx(relation2, 3, 4)] = false;
	relation2->table[rf_table_idx(relation2, 3, 5)] = false;
	relation2->table[rf_table_idx(relation2, 3, 7)] = false;
	relation2->table[rf_table_idx(relation2, 4, 5)] = false;
	relation2->table[rf_table_idx(relation2, 5, 6)] = false;
	relation2->table[rf_table_idx(relation2, 6, 7)] = false;

	rf_SetElement *elems5[2];

	elems5[0] = rf_set_element_new_string(b);
	elems5[1] = rf_set_element_new_string(d);

	expected = rf_relation_find_infimum(relation2, rf_set_new(2, elems5), NULL);
	CU_ASSERT_TRUE(rf_set_element_equal(expected, elems6[3]));
}

void test_rf_relation_find_upperbound(){
		/*
		 *   a b c d
		 * a 1     1
		 * b 1 1   1
		 * c     1
		 * d       1
		 *
		 * M = (a,b,c,d)
		 * S = (b,c,d)
		 * S2 = (b,d)
		 */

	rf_SetElement *elems[4];
	rf_SetElement *elems2[3];
	rf_SetElement *elems3[2];

	generateTestElements(4, elems);

	elems2[0] = rf_set_element_new_string(b);
	elems2[1] = rf_set_element_new_string(c);
	elems2[2] = rf_set_element_new_string(d);

	elems3[0] = rf_set_element_new_string(a);
	elems3[1] = rf_set_element_new_string(d);

	rf_Set *superSet = rf_set_new(4, elems);
	rf_Set *subset1 = rf_set_new(3, elems2);
	rf_Set *subset2 = rf_set_new(2, elems3);

	rf_Relation *relation = rf_relation_new_id(superSet);
	relation->table[rf_table_idx(relation, 0,3)] = true;
	relation->table[rf_table_idx(relation, 1,0)] = true;
	relation->table[rf_table_idx(relation, 1,3)] = true;

	rf_Set *expected = rf_relation_find_upperbound(relation, subset1, NULL);
	rf_Set *expected2 = rf_relation_find_upperbound(relation, subset2, NULL);

	CU_ASSERT_EQUAL(expected->cardinality, 0);
	CU_ASSERT_EQUAL(expected2->cardinality, 2);
	CU_ASSERT_TRUE(rf_set_contains_element(expected2, elems[0]));
	CU_ASSERT_TRUE(rf_set_contains_element(expected2, elems[1]));

	//additional test

	rf_SetElement *elems4[5];
	generateTestElements(5, elems4);

	rf_SetElement *elems5[2];
	generateTestElements(2, elems5);

	rf_Set *superSet2 = rf_set_new(5, elems4);
	rf_Set *subset3 = rf_set_new(2, elems5);

	rf_Relation *relation2 = rf_relation_new_id(superSet2);
	relation2->table[rf_table_idx(relation2, 2,0)] = true;
	relation2->table[rf_table_idx(relation2, 2,1)] = true;
	relation2->table[rf_table_idx(relation2, 3,0)] = true;
	relation2->table[rf_table_idx(relation2, 3,1)] = true;
	relation2->table[rf_table_idx(relation2, 4,0)] = true;
	relation2->table[rf_table_idx(relation2, 4,1)] = true;
	relation2->table[rf_table_idx(relation2, 4,2)] = true;
	relation2->table[rf_table_idx(relation2, 4,3)] = true;

	rf_Set *expected3 = rf_relation_find_upperbound(relation2, subset3, NULL);

	CU_ASSERT_EQUAL(expected3->cardinality, 3);
}

void test_rf_relation_find_lowerbound(){

	rf_SetElement *elems[5];

	generateTestElements(5, elems);

	rf_SetElement *elems2[2];
	generateTestElements(2, elems2);

	rf_SetElement *elems3[3];
	generateTestElements(3, elems3);

	rf_SetElement *elems4[2];
	elems4[0] = rf_set_element_new_string(c);
	elems4[1] = rf_set_element_new_string(d);

	rf_Set *superSet = rf_set_new(5, elems);
	rf_Set *subset = rf_set_new(2, elems2);
	rf_Set *subset2 = rf_set_new(3, elems3);
	rf_Set *subset3 = rf_set_new(2, elems4);

	rf_Relation *relation2 = rf_relation_new_id(superSet);
	relation2->table[rf_table_idx(relation2, 0,2)] = true;
	relation2->table[rf_table_idx(relation2, 0,3)] = true;
	relation2->table[rf_table_idx(relation2, 0,4)] = true;
	relation2->table[rf_table_idx(relation2, 1,2)] = true;
	relation2->table[rf_table_idx(relation2, 1,3)] = true;
	relation2->table[rf_table_idx(relation2, 1,4)] = true;
	relation2->table[rf_table_idx(relation2, 2,4)] = true;
	relation2->table[rf_table_idx(relation2, 3,4)] = true;

	rf_Set *expected = rf_relation_find_lowerbound(relation2, subset, NULL);

	CU_ASSERT_EQUAL(expected->cardinality, 3);
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems[2]));
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems[3]));
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems[4]));

	expected = rf_relation_find_lowerbound(relation2, subset2, NULL);

	CU_ASSERT_EQUAL(expected->cardinality, 2);
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems[2]));
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems[4]));

	expected = rf_relation_find_lowerbound(relation2, subset3, NULL);

	CU_ASSERT_EQUAL(expected->cardinality, 1);
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems[4]));

	//another test
	rf_SetElement *elems6[9];
	generateTestElements(9, elems6);

	rf_Set *superSet2 = rf_set_new(9, elems6);
	relation2 = rf_relation_new_top(superSet2);

	relation2->table[rf_table_idx(relation2, 1, 2)] = false;
	relation2->table[rf_table_idx(relation2, 1, 5)] = false;
	relation2->table[rf_table_idx(relation2, 2, 3)] = false;
	relation2->table[rf_table_idx(relation2, 3, 4)] = false;
	relation2->table[rf_table_idx(relation2, 3, 5)] = false;
	relation2->table[rf_table_idx(relation2, 3, 7)] = false;
	relation2->table[rf_table_idx(relation2, 4, 5)] = false;
	relation2->table[rf_table_idx(relation2, 5, 6)] = false;
	relation2->table[rf_table_idx(relation2, 6, 7)] = false;

	rf_SetElement *elems5[2];

	elems5[0] = rf_set_element_new_string(b);
	elems5[1] = rf_set_element_new_string(d);

	expected = rf_relation_find_lowerbound(relation2, rf_set_new(2, elems5), NULL);
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems6[3]));
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems6[6]));
	CU_ASSERT_TRUE(rf_set_contains_element(expected, elems6[8]));
}

void test_rf_relation_make_transitive(){
	rf_Relation *variation = rf_relation_new_empty(set, set);
	variation->table[rf_table_idx(variation, 0,1)] = true;
	variation->table[rf_table_idx(variation, 1,2)] = true;

	//take intransitive relation
	CU_ASSERT_FALSE(rf_relation_is_transitive(variation));

	//make it transitive by adding missing relations
	bool returnCode = rf_relation_make_transitive(variation, true, NULL);

	//test it
	CU_ASSERT_TRUE(returnCode);
	CU_ASSERT_TRUE(rf_relation_is_transitive(variation));
	CU_ASSERT_TRUE(variation->table[rf_table_idx(variation, 0,2)]);

	//undo changes (make it intransitive again)
	variation->table[rf_table_idx(variation, 0,2)] = false;
	CU_ASSERT_FALSE(rf_relation_is_transitive(variation));

	//make it transitive by deleting relations that would cause intransitivity
	returnCode = rf_relation_make_transitive(variation, false, NULL);

	//test it again
	CU_ASSERT_TRUE(returnCode);
	CU_ASSERT_TRUE(rf_relation_is_transitive(variation));
	CU_ASSERT_FALSE(variation->table[rf_table_idx(variation, 1,2)]);

	//another example

	rf_SetElement *elems[4];
	generateTestElements(4,elems);

	rf_Set *mySet = rf_set_new(4, elems);

	rf_Relation *rel = rf_relation_new_empty(mySet, mySet);

	rel->table[rf_table_idx(rel,0,2)] = true;
	rel->table[rf_table_idx(rel,0,3)] = true;
	rel->table[rf_table_idx(rel,1,0)] = true;
	rel->table[rf_table_idx(rel,2,1)] = true;
	rel->table[rf_table_idx(rel,3,2)] = true;

	returnCode = rf_relation_make_transitive(rel, true, NULL);

	CU_ASSERT_TRUE(returnCode);
	CU_ASSERT_TRUE(rf_relation_is_transitive(rel));
}

void test_rf_relation_is_lefttotal(){
	//Jedes A hat mindestens ein B

	rf_Relation *rel = rf_relation_new_id(set);
	CU_ASSERT_TRUE(rf_relation_is_lefttotal(rel));

	rel->table[rf_table_idx(rel, 0, 0)] = false;
	CU_ASSERT_FALSE(rf_relation_is_lefttotal(rel));

	rel->table[rf_table_idx(rel, 0, 2)] = true;
	rel->table[rf_table_idx(rel, 0, 1)] = true;

	CU_ASSERT_TRUE(rf_relation_is_lefttotal(rel));
}

void test_rf_relation_is_functional(){
	//Jedes A hat höchstens ein B

	rf_Relation *rel = rf_relation_new_id(set);
	CU_ASSERT_TRUE(rf_relation_is_functional(rel));

	rel->table[rf_table_idx(rel, 0, 0)] = false;
	CU_ASSERT_TRUE(rf_relation_is_functional(rel));

	rel->table[rf_table_idx(rel, 0, 2)] = true;
	rel->table[rf_table_idx(rel, 0, 1)] = true;

	CU_ASSERT_FALSE(rf_relation_is_functional(rel));

}

void test_rf_relation_is_function(){
	//Jedes A hat genau ein B

	rf_Relation *rel = rf_relation_new_id(set);
	CU_ASSERT_TRUE(rf_relation_is_function(rel));

	rel->table[rf_table_idx(rel, 0, 0)] = false;
	CU_ASSERT_FALSE(rf_relation_is_function(rel));

	rel->table[rf_table_idx(rel, 0, 2)] = true;
	rel->table[rf_table_idx(rel, 0, 1)] = true;

	CU_ASSERT_FALSE(rf_relation_is_function(rel));

}

void test_rf_relation_is_surjective(){
	 //Jedes B hat mindestens ein A

	rf_Relation *rel = rf_relation_new_id(set);
	CU_ASSERT_TRUE(rf_relation_is_surjective(rel));

	rel->table[rf_table_idx(rel, 0, 0)] = false;
	CU_ASSERT_FALSE(rf_relation_is_surjective(rel));

	rel->table[rf_table_idx(rel, 0, 2)] = true;
	rel->table[rf_table_idx(rel, 0, 1)] = true;

	CU_ASSERT_FALSE(rf_relation_is_surjective(rel));

	rel->table[rf_table_idx(rel, 2, 0)] = true;
	CU_ASSERT_TRUE(rf_relation_is_surjective(rel));

}

void test_rf_relation_is_injective(){
	//Jedes B hat höchstens ein A

	rf_Relation *rel = rf_relation_new_id(set);
	CU_ASSERT_TRUE(rf_relation_is_injective(rel));

	rel->table[rf_table_idx(rel, 0, 0)] = false;
	CU_ASSERT_TRUE(rf_relation_is_injective(rel));

	rel->table[rf_table_idx(rel, 2, 0)] = true;
	rel->table[rf_table_idx(rel, 2, 1)] = true;
	CU_ASSERT_FALSE(rf_relation_is_injective(rel));
}

void test_rf_relation_is_bijective(){
	//Jedes B hat genau ein A

	rf_Relation *rel = rf_relation_new_id(set);
	CU_ASSERT_TRUE(rf_relation_is_bijective(rel));

	rel->table[rf_table_idx(rel, 0, 0)] = false;
	CU_ASSERT_FALSE(rf_relation_is_bijective(rel));

	rel->table[rf_table_idx(rel, 2, 0)] = true;
	CU_ASSERT_TRUE(rf_relation_is_bijective(rel));

	rel->table[rf_table_idx(rel, 2, 1)] = true;
	CU_ASSERT_FALSE(rf_relation_is_bijective(rel));
}

void test_rf_relation_find_transitive_gaps(){
	rf_SetElement *elems[4];
	generateTestElements(4, elems);

	rf_Set *mySet = rf_set_new(4, elems);

	rf_Relation *rel = rf_relation_new_empty(mySet, mySet);
	rel->table[rf_table_idx(rel,0,2)] = true;
	rel->table[rf_table_idx(rel,0,3)] = true;
	rel->table[rf_table_idx(rel,1,0)] = true;
	rel->table[rf_table_idx(rel,2,1)] = true;
	rel->table[rf_table_idx(rel,3,2)] = true;

	const int expectedNumOfGaps = 5;
	const int expected[16] = {0,0,2,1,3,0,0,0,0,3,0,0,0,0,1,0};

	int *occurrences = malloc(sizeof(int)*rel->domains[0]->cardinality*rel->domains[0]->cardinality);
	rf_Set *gaps = rf_set_new(0, malloc(0));

	int numOfGaps = rf_relation_find_transitive_gaps(rel, occurrences, gaps, NULL);

	CU_ASSERT_FALSE(gaps == NULL);
	CU_ASSERT_EQUAL(numOfGaps, expectedNumOfGaps);

	int sum = 0;
	for(int i = 0; i < 16; i++) {
		sum += occurrences[i];
		CU_ASSERT_EQUAL(occurrences[i], expected[i]);
	}
	CU_ASSERT_EQUAL(numOfGaps*2, sum);
}

void test_rf_relation_guess_transitive_core(){
	rf_SetElement *elems[4];
	generateTestElements(4, elems);

	rf_Set *mySet = rf_set_new(4, elems);

	rf_Relation *rel = rf_relation_new_empty(mySet, mySet);

	rel->table[rf_table_idx(rel,0,2)] = true;
	rel->table[rf_table_idx(rel,0,3)] = true;
	rel->table[rf_table_idx(rel,1,0)] = true;
	rel->table[rf_table_idx(rel,2,1)] = true;
	rel->table[rf_table_idx(rel,3,2)] = true;

	bool success = rf_relation_guess_transitive_core(rel, NULL);

	//In this example the algorithm should not know if he has found the tc, so he returns false
	CU_ASSERT_TRUE(success);
	CU_ASSERT_TRUE(rf_relation_is_transitive(rel));

	int dim = 8;
	rf_SetElement *elems2[dim];
	generateTestElements(8, elems2);

	for(int testsize=1; testsize<dim;testsize++){
		rf_Set *bigSet = rf_set_new(testsize, elems2);
		rf_Relation *bigRel = rf_relation_new_empty(bigSet, bigSet);

		for(int i=0;i<testsize-1;i++){
		  bigRel->table[rf_table_idx(bigRel, i,testsize-1)] = true;
		  bigRel->table[rf_table_idx(bigRel, testsize-1,i)] = true;
		}

		bool success = rf_relation_guess_transitive_core(bigRel, NULL);
		CU_ASSERT_TRUE(success);
		CU_ASSERT_TRUE(rf_relation_is_transitive(bigRel));
	}

}

void test_rf_relation_find_transitive_hard_core(){
	rf_SetElement *elems[4];
	generateTestElements(4, elems);

	rf_Set *mySet = rf_set_new(4, elems);

	rf_Relation *rel = rf_relation_new_empty(mySet, mySet);

	rel->table[rf_table_idx(rel,0,2)] = true;
	rel->table[rf_table_idx(rel,0,3)] = true;
	rel->table[rf_table_idx(rel,1,0)] = true;
	rel->table[rf_table_idx(rel,2,1)] = true;
	rel->table[rf_table_idx(rel,3,2)] = true;

	rf_Relation *result = rf_relation_find_transitive_hard_core(rel, NULL);

	int relationsCount = 0;
	CU_ASSERT_TRUE(rf_relation_is_transitive(result));
	for(int i = 0; i<16;i++){
	if(result->table[i])
		relationsCount++;
	}
	CU_ASSERT_EQUAL(3, relationsCount);


	//BIG EXAMPLE -WORST CASE SCENARIO

	int dim = 8;
	rf_SetElement *elems2[dim];
	generateTestElements(dim, elems2);

	for(int testsize = 5; testsize < dim; testsize++) {
		rf_Set *bigSet = rf_set_new(testsize, elems2);
		rf_Relation *bigRel = rf_relation_new_empty(bigSet, bigSet);

		for(int i = 0; i < testsize-1; i++){
			bigRel->table[rf_table_idx(bigRel, i,testsize-1)] = true;
			bigRel->table[rf_table_idx(bigRel, testsize-1,i)] = true;
		}

		result = rf_relation_find_transitive_hard_core(bigRel, NULL);

		relationsCount = 0;
		CU_ASSERT_TRUE(rf_relation_is_transitive(result));
		for(int i = 0; i < testsize*testsize; i++){
			if(result->table[i])
				relationsCount++;
		}
		CU_ASSERT_EQUAL(testsize-1, relationsCount);
	}
}

void test_rf_relation_get_image(){
	rf_Relation *rel = rf_relation_new_id(set);

	//Test with domain(rel) = subdomain
	rf_Set *result = rf_relation_get_image(rel, rel->domains[0]);
	CU_ASSERT_TRUE(result->cardinality == 3);
	CU_ASSERT_TRUE(rf_set_equal(set, result));

	rel->table[4] = false;

	result = rf_relation_get_image(rel, rel->domains[0]);
	CU_ASSERT_TRUE(result->cardinality == 2);
	CU_ASSERT_FALSE(rf_set_contains_element(result, set->elements[1]));
	CU_ASSERT_TRUE(rf_set_contains_element(result, set->elements[0]));
	CU_ASSERT_TRUE(rf_set_contains_element(result, set->elements[2]));

	//Test with real subset

	rf_SetElement *elems[2];
	elems[0] = rf_set_element_new_string(a);
	elems[1] = rf_set_element_new_string(c);
	rf_Set *subset = rf_set_new(2, elems);

	result = rf_relation_get_image(rel, subset);
	CU_ASSERT_TRUE(result->cardinality == 2);
	CU_ASSERT_TRUE(rf_set_contains_element(result, set->elements[0]));
	CU_ASSERT_TRUE(rf_set_contains_element(result, set->elements[2]));

	rel->table[4] = true;

	result = rf_relation_get_image(rel, subset);
	CU_ASSERT_TRUE(result->cardinality == 2);
	CU_ASSERT_TRUE(rf_set_contains_element(result, set->elements[0]));
	CU_ASSERT_TRUE(rf_set_contains_element(result, set->elements[2]));
}

void test_rf_relation_get_preimage(){
	rf_Relation *rel = rf_relation_new_id(set);

	//Test with domain(rel) = subdomain
	rf_Set *result = rf_relation_get_preImage(rel, rel->domains[0]);
	CU_ASSERT_TRUE(result->cardinality == 3);
	CU_ASSERT_TRUE(rf_set_equal(set, result));

	rel->table[0] = false;

	result = rf_relation_get_preImage(rel, rel->domains[0]);
	CU_ASSERT_TRUE(result->cardinality == 2);
	CU_ASSERT_FALSE(rf_set_contains_element(result, set->elements[0]));
	CU_ASSERT_TRUE(rf_set_contains_element(result, set->elements[1]));
	CU_ASSERT_TRUE(rf_set_contains_element(result, set->elements[2]));

	//Test with real subset

	rf_SetElement *elems[2];
	elems[0] = rf_set_element_new_string(a);
	elems[1] = rf_set_element_new_string(c);
	rf_Set *subset = rf_set_new(2, elems);

	result = rf_relation_get_preImage(rel, subset);
	CU_ASSERT_TRUE(result->cardinality == 1);
	CU_ASSERT_TRUE(rf_set_contains_element(result, set->elements[2]));

	rel->table[0] = true;

	result = rf_relation_get_preImage(rel, subset);
	CU_ASSERT_TRUE(result->cardinality == 2);
	CU_ASSERT_TRUE(rf_set_contains_element(result, set->elements[0]));
	CU_ASSERT_TRUE(rf_set_contains_element(result, set->elements[2]));
}

void test_rf_relation_is_lattice(){
	rf_SetElement *elems[4];

	elems[0] = rf_set_element_new_string(a);
	elems[1] = rf_set_element_new_string(b);
	elems[2] = rf_set_element_new_string(c);
	elems[3] = rf_set_element_new_string(d);

	rf_Relation *rel = rf_relation_new_id(rf_set_new(4, elems));

	rel->table[rf_table_idx(rel,0,2)] = true;
	rel->table[rf_table_idx(rel,1,2)] = true;
	rel->table[rf_table_idx(rel,3,0)] = true;
	rel->table[rf_table_idx(rel,3,1)] = true;
	rel->table[rf_table_idx(rel,3,2)] = true;

	bool result = rf_relation_is_lattice(rel, NULL);
	CU_ASSERT_TRUE(result);

	rf_Relation *relFalse = rf_relation_new_id(set);

	relFalse->table[rf_table_idx(rel,0,2)] = true;
	rel->table[rf_table_idx(rel,1,2)] = true;

	result = rf_relation_is_lattice(relFalse, NULL);
	CU_ASSERT_FALSE(result);
}

//make methods

void test_rf_relation_make_antisymmetric(){
	//change upper half
	rf_Relation *test = rf_relation_new_full(set, set);

	CU_ASSERT_FALSE(rf_relation_is_antisymmetric(test));

	bool result = rf_relation_make_antisymmetric(test, true, NULL);
	CU_ASSERT_TRUE(result);
	CU_ASSERT_TRUE(rf_relation_is_antisymmetric(test));

	//change lower half
	test = rf_relation_new_full(set, set);

	CU_ASSERT_FALSE(rf_relation_is_antisymmetric(test));

	result = rf_relation_make_antisymmetric(test, false, NULL);
	CU_ASSERT_TRUE(result);
	CU_ASSERT_TRUE(rf_relation_is_antisymmetric(test));
}

void test_rf_relation_make_asymmetric(){
	//change upper half
	rf_Relation *test = rf_relation_new_full(set, set);

	CU_ASSERT_FALSE(rf_relation_is_asymmetric(test));

	bool result = rf_relation_make_asymmetric(test, true, NULL);
	CU_ASSERT_TRUE(result);
	CU_ASSERT_TRUE(rf_relation_is_asymmetric(test));

	//change lower half
	test = rf_relation_new_full(set, set);

	CU_ASSERT_FALSE(rf_relation_is_asymmetric(test));

	result = rf_relation_make_asymmetric(test, false, NULL);
	CU_ASSERT_TRUE(result);
	CU_ASSERT_TRUE(rf_relation_is_asymmetric(test));

}

void test_rf_relation_make_difunctional(){
	rf_SetElement *elems[4];
	generateTestElements(4, elems);
	rf_Set *mySet = rf_set_new(4, elems);

	rf_Relation *rel = rf_relation_new_empty(mySet, mySet);

	rel->table[rf_table_idx(rel,0,2)] = true;
	rel->table[rf_table_idx(rel,0,3)] = true;
	rel->table[rf_table_idx(rel,1,0)] = true;
	rel->table[rf_table_idx(rel,2,1)] = true;
	rel->table[rf_table_idx(rel,3,2)] = true;

	bool returnCode = rf_relation_make_difunctional(rel, true, NULL);

	CU_ASSERT_TRUE(returnCode);

	CU_ASSERT_TRUE(rel->table[rf_table_idx(rel,3,3)]);
	CU_ASSERT_TRUE(rf_relation_is_difunctional(rel));

	rel->table[rf_table_idx(rel,3,3)] = false;

	CU_ASSERT_FALSE(rf_relation_is_difunctional(rel));
	returnCode = rf_relation_make_difunctional(rel, false, NULL);

	CU_ASSERT_TRUE(returnCode);
	CU_ASSERT_TRUE(rf_relation_is_difunctional(rel));
	CU_ASSERT_FALSE(rel->table[rf_table_idx(rel,3,2)]);
}

void test_rf_relation_make_equivalent(){
	rf_SetElement *elems[4];
	generateTestElements(4, elems);

	rf_Set *mySet = rf_set_new(4, elems);

	rf_Relation *rel = rf_relation_new_empty(mySet, mySet);

	rel->table[rf_table_idx(rel,0,2)] = true;
	rel->table[rf_table_idx(rel,0,3)] = true;
	rel->table[rf_table_idx(rel,1,0)] = true;
	rel->table[rf_table_idx(rel,2,1)] = true;
	rel->table[rf_table_idx(rel,3,2)] = true;

	CU_ASSERT_FALSE(rf_relation_is_equivalent(rel));

	bool result = rf_relation_make_equivalent(rel, true, NULL);
	CU_ASSERT_TRUE(result);
	CU_ASSERT_TRUE(rf_relation_is_equivalent(rel));

	rel = rf_relation_new_empty(mySet, mySet);
	rel->table[rf_table_idx(rel,0,2)] = true;
	rel->table[rf_table_idx(rel,0,3)] = true;
	rel->table[rf_table_idx(rel,1,0)] = true;
	rel->table[rf_table_idx(rel,2,1)] = true;
	rel->table[rf_table_idx(rel,3,2)] = true;

	CU_ASSERT_FALSE(rf_relation_is_equivalent(rel));

	result = rf_relation_make_equivalent(rel, false, NULL);
	CU_ASSERT_TRUE(result);
	CU_ASSERT_TRUE(rf_relation_is_equivalent(rel));
}

void test_rf_relation_make_irreflexive(){
	rf_Relation *test = rf_relation_new_id(set);

	CU_ASSERT_FALSE(rf_relation_is_irreflexive(test));

	bool result = rf_relation_make_irreflexive(test, NULL);
	CU_ASSERT_TRUE(result);
	CU_ASSERT_TRUE(rf_relation_is_irreflexive(test));
}

void test_rf_relation_make_partial_order(){
	rf_Relation *rel = rf_relation_new_full(set, set);
	CU_ASSERT_FALSE(rf_relation_is_partial_order(rel));

	rf_relation_make_partial_order(rel, false, NULL);
	CU_ASSERT_TRUE(rf_relation_is_partial_order(rel));

	rel = rf_relation_new_id(set);
	rel->table[rf_table_idx(rel, 0,1)] = true;
	rel->table[rf_table_idx(rel, 2,2)] = false;
	CU_ASSERT_FALSE(rf_relation_is_partial_order(rel));

	rf_relation_make_partial_order(rel, true, NULL);
	CU_ASSERT_TRUE(rf_relation_is_partial_order(rel));
}

void test_rf_relation_make_preorder(){
	rf_SetElement *elems[4];
	generateTestElements(4, elems);

	rf_Set *mySet = rf_set_new(4, elems);

	rf_Relation *rel = rf_relation_new_empty(mySet, mySet);

	rel->table[rf_table_idx(rel,0,2)] = true;
	rel->table[rf_table_idx(rel,0,3)] = true;
	rel->table[rf_table_idx(rel,1,0)] = true;
	rel->table[rf_table_idx(rel,2,1)] = true;
	rel->table[rf_table_idx(rel,3,2)] = true;

	CU_ASSERT_FALSE(rf_relation_is_preorder(rel));

	bool result = rf_relation_make_preorder(rel, true, NULL);
	CU_ASSERT_TRUE(result);
	CU_ASSERT_TRUE(rf_relation_is_preorder(rel));

	rel = rf_relation_new_empty(mySet, mySet);
	rel->table[rf_table_idx(rel,0,2)] = true;
	rel->table[rf_table_idx(rel,0,3)] = true;
	rel->table[rf_table_idx(rel,1,0)] = true;
	rel->table[rf_table_idx(rel,2,1)] = true;
	rel->table[rf_table_idx(rel,3,2)] = true;

	CU_ASSERT_FALSE(rf_relation_is_preorder(rel));

	//In this case, the preorder algorithm fails. That can happen in make_preorder / make_partial_order / make_equivalent
	//Problem is, that e.g. make_transitive and make_reflexive don't take care of each other in case fill = false
	result = rf_relation_make_preorder(rel, false, NULL);
	CU_ASSERT_FALSE(result);
	CU_ASSERT_FALSE(rf_relation_is_preorder(rel));

}

void test_rf_relation_make_reflexive(){
	rf_Relation *test = rf_relation_new_empty(set, set);

	CU_ASSERT_FALSE(rf_relation_is_reflexive(test));

	bool result = rf_relation_make_reflexive(test, NULL);
	CU_ASSERT_TRUE(result);
	CU_ASSERT_TRUE(rf_relation_is_reflexive(test));
}

void test_rf_relation_make_symmetric(){
	//fill
	rf_Relation *test = rf_relation_new_full(set, set);
	CU_ASSERT_TRUE(rf_relation_make_asymmetric(test, true, NULL));

	CU_ASSERT_FALSE(rf_relation_is_symmetric(test));

	bool result = rf_relation_make_symmetric(test, true, NULL);
	CU_ASSERT_TRUE(result);
	CU_ASSERT_TRUE(rf_relation_is_symmetric(test));

	//no fill
	test = rf_relation_new_full(set, set);
	CU_ASSERT_TRUE(rf_relation_make_asymmetric(test, true, NULL));

	CU_ASSERT_FALSE(rf_relation_is_symmetric(test));

	result = rf_relation_make_symmetric(test, false, NULL);
	CU_ASSERT_TRUE(result);
	CU_ASSERT_TRUE(rf_relation_is_symmetric(test));
}

CU_ErrorCode
register_suites_relation() {
	CU_TestInfo create_suite[] = {
		{ "rf_relation_new_empty", test_rf_relation_new_empty },
		{ "rf_relation_new_full", test_rf_relation_new_full },
		{ "rf_relation_new", test_rf_relation_new },
		{ "rf_relation_clone", test_rf_relation_clone },
		{ "rf_relation_new_id", test_rf_relation_new_id },
		{ "rf_table_idx", test_rf_table_idx },
		{ "rf_relation_calc", test_rf_relation_calc },
		{ "rf_relation_new_top", test_rf_relation_new_top },
		{ "rf_relation_new_bottom", test_rf_relation_new_bottom },
		{ "rf_relation_new_union", test_rf_relation_new_union },
		{ "rf_relation_new_intersection", test_rf_relation_new_intersection },
		{ "rf_relation_new_complement", test_rf_relation_new_complement },
		{ "rf_relation_new_concatenation", test_rf_relation_new_concatenation },
		{ "rf_relation_new_converse", test_rf_relation_new_converse },
		{ "rf_relation_new_subsetleq", test_rf_relation_new_subsetleq },
		CU_TEST_INFO_NULL
	};

	CU_TestInfo search_suite[] = {
		{ "rf_relation_find_maximal_elements", test_rf_relation_find_maximal_elements },
		{ "rf_relation_find_minimal_elements", test_rf_relation_find_minimal_elements },
		{ "rf_relation_find_maximum", test_rf_relation_find_maximum },
		{ "rf_relation_find_minimum", test_rf_relation_find_minimum },
		{ "rf_relation_find_maximum_within_subset", test_rf_relation_find_maximum_within_subset },
		{ "rf_relation_find_upperbound", test_rf_relation_find_upperbound },
		{ "rf_relation_find_supremum", test_rf_relation_find_supremum },
		{ "rf_relation_find_lowerbound", test_rf_relation_find_lowerbound },
		{ "rf_relation_find_infimum", test_rf_relation_find_infimum },
		{ "rf_relation_find_transitive_gaps", test_rf_relation_find_transitive_gaps },
		{ "rf_relation_guess_transitive_core", test_rf_relation_guess_transitive_core },
		{ "rf_relation_find_transitive_hard_core", test_rf_relation_find_transitive_hard_core },

		{ "rf_relation_get_image", test_rf_relation_get_image },
		{ "rf_relation_get_preimage", test_rf_relation_get_preimage },
		CU_TEST_INFO_NULL
	};

	CU_TestInfo test_suite[] = {
		//homogeneous
		{ "rf_relation_is_homogeneous", test_rf_relation_is_homogeneous },
		{ "rf_relation_is_antisymmetric", test_rf_relation_is_antisymmetric },
		{ "rf_relation_is_asymmetric", test_rf_relation_is_asymmetric },
		{ "rf_relation_is_difcuntional", test_rf_relation_is_difunctional },
		{ "rf_relation_is_equivalent", test_rf_relation_is_equivalent },
		{ "rf_relation_is_symmetric", test_rf_relation_is_symmetric },
		{ "rf_relation_is_irreflexive", test_rf_relation_is_irreflexive },
		{ "rf_relation_is_reflexive", test_rf_relation_is_reflexive },
		{ "rf_relation_is_partial_order", test_rf_relation_is_partial_order },
		{ "rf_relation_is_transitive", test_rf_relation_is_transitive },
		{ "rf_relation_is_lattice", test_rf_relation_is_lattice },

		//heterogeneous
		{ "rf_relation_is_lefttotal", test_rf_relation_is_lefttotal },
		{ "rf_relation_is_functional", test_rf_relation_is_functional },
		{ "rf_relation_is_function", test_rf_relation_is_function },
		{ "rf_relation_is_surjective", test_rf_relation_is_surjective },
		{ "rf_relation_is_injective", test_rf_relation_is_injective },
		{ "rf_relation_is_bijective", test_rf_relation_is_bijective },
		CU_TEST_INFO_NULL
	};

	CU_TestInfo modification_suite[] = {
		{ "rf_relation_make_antisymmetric", test_rf_relation_make_antisymmetric },
		{ "rf_relation_make_asymmetric", test_rf_relation_make_asymmetric },
		{ "rf_relation_make_difunctional", test_rf_relation_make_difunctional },
		{ "rf_relation_make_equivalent", test_rf_relation_make_equivalent },
		{ "rf_relation_make_irreflexive", test_rf_relation_make_irreflexive },
		{ "rf_relation_make_partial_order", test_rf_relation_make_partial_order },
		{ "rf_relation_make_preorder", test_rf_relation_make_preorder },
		{ "rf_relation_make_reflexive", test_rf_relation_make_reflexive },
		{ "rf_relation_make_symmetric", test_rf_relation_make_symmetric },
		{ "rf_relation_make_transitive", test_rf_relation_make_transitive },
		CU_TEST_INFO_NULL
	};

	CU_SuiteInfo suites[] = {
		{ "rf_Relation: create-procedures", init_suite, clean_suite, create_suite },
		{ "rf_Relation: search-procedures", init_suite, clean_suite, search_suite },
		{ "rf_Relation: test-procedures", init_suite, clean_suite, test_suite },
		{ "rf_Relation: modificaiton-procedures", init_suite, clean_suite, modification_suite },
		CU_SUITE_INFO_NULL
	};

	return CU_register_suites(suites);
}
