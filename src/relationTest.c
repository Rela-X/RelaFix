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
#include <stdio.h>
#include <time.h>
#include "CUnit/Basic.h"
#include "error.c"
#include "set.c"
#include "relation.c"


/**
 * @TODO:
 * bool            rf_relation_calc(rf_Relation *relation, rf_SetElement *element1, rf_SetElement *element2, rf_Error *error);
 *
 */

rf_Set *set;
rf_Set *set2;

char a[] = "a";
char b[] = "b";
char c[] = "c";
char d[] = "d";
char e[] = "e";
char f[] = "f";
char g[] = "g";
char h[] = "h";
char i[] = "i";
char j[] = "j";

int init_suite1(void)
{
	rf_SetElement *elems[3];
	rf_SetElement *elems2[1];
	
	elems[0] = rf_set_element_new_string(a);
	elems[1] = rf_set_element_new_string(b);
	elems[2] = rf_set_element_new_string(c);
	elems2[0] = rf_set_element_new_string(d);
	
	set = rf_set_new(3, elems);
	set2 = rf_set_new(1, elems2);
   
	return 0;
}


int clean_suite1(void)
{
	return 0;
}

//Here follow all test cases

void test_rf_relation_new_empty(){
	rf_Relation *empty_relation = rf_relation_new_empty(set, set);
	
	CU_ASSERT_PTR_NOT_NULL(empty_relation);  
	
	CU_ASSERT_PTR_EQUAL(set, empty_relation->domains[0]);
	CU_ASSERT_PTR_EQUAL(set, empty_relation->domains[1]);
	
	int n = (set->cardinality) * (set->cardinality);
	for(int i=0; i<n;i++){
	  CU_ASSERT_FALSE(empty_relation->table[i]);
	}
}

void test_rf_relation_new_full(){
	rf_Relation *full_relation = rf_relation_new_full(set, set);
	
	CU_ASSERT_PTR_NOT_NULL(full_relation);  
	
	CU_ASSERT_PTR_EQUAL(set, full_relation->domains[0]);
	CU_ASSERT_PTR_EQUAL(set, full_relation->domains[1]);
	bool            rf_relation_is_equivalent(const rf_Relation *relation);
	int n = (set->cardinality) * (set->cardinality);
	for(int i=0; i<n;i++){
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
	
	for(int i=0; i<n;i++){
	  if (i % (set->cardinality+1) == 0){
	    CU_ASSERT_TRUE(relation->table[i]);
	  }
	  else{ 
	    CU_ASSERT_FALSE(relation->table[i]);
	  }    
	}
}

void test_rf_relation_copy(){
	size_t n = set->cardinality * set->cardinality;
        bool *table = calloc(n, sizeof(table));
	
	table[0] = table[4] = table[8] = true;  
	
	rf_Relation *source = rf_relation_new(set, set, table);
	rf_Relation *result = rf_relation_copy(source);
	
	
	CU_ASSERT_PTR_NOT_EQUAL(source, result);
	CU_ASSERT_PTR_NOT_EQUAL(set, result->domains[0]);
	CU_ASSERT_PTR_NOT_EQUAL(set, result->domains[1]);
	CU_ASSERT_PTR_NOT_EQUAL(table, result->table);
	
	CU_ASSERT_EQUAL(set->cardinality, result->domains[0]->cardinality);
	
	for(int i=0; i<n;i++){
	  if (i % (set->cardinality+1) == 0){
	    CU_ASSERT_TRUE(result->table[i]);
	  }
	  else{ 
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
	for(int i=0; i<n;i++){
	  if (i % (set->cardinality+1) == 0){
	    CU_ASSERT_TRUE(relation->table[i]);
	  }
	  else{ 
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
   rf_Error *error = rf_error_new();
   rf_Relation *idR = rf_relation_new_id(set);
   rf_Relation *r1 = rf_relation_new_empty(set, set);
   rf_Relation *result;
   
   rf_Relation *idRSet2 = rf_relation_new_id(set2);
   
   //TESTS
   
   //Union between id-Relation and an empty relation
   result = rf_relation_new_union(idR, r1, error);
   int n = set->cardinality * set->cardinality;
   for(int i=0;i<n;i++){
	    CU_ASSERT_EQUAL(idR->table[i], result->table[i]);
   }
   
   //Union id-Relation with itself
   result = rf_relation_new_union(idR, idR, error);
   for(int i=0;i<n;i++){
	    CU_ASSERT_EQUAL(idR->table[i], result->table[i]);
   }
   
   //Union between id-R and its complement
   result = rf_relation_new_union(idR, rf_relation_new_complement(idR, error), error);

   for(int i=0;i<n;i++){
	    CU_ASSERT_TRUE(result->table[i]);
   }
   
   //Union between two relations that have different domains
   result = rf_relation_new_union(idR, idRSet2, error);
   CU_ASSERT_TRUE(result == NULL);
  
}

void test_rf_relation_new_intersection(){
   
   //inits
   rf_Error *error = rf_error_new();
   rf_Relation *idR = rf_relation_new_id(set);
   rf_Relation *r1 = rf_relation_new_empty(set, set);
   rf_Relation *r2 = rf_relation_new_empty(set, set);
   r2->table[rf_table_idx(r1,1,1)] = 1;
   rf_Relation *result;
   
   rf_Relation *idRSet2 = rf_relation_new_id(set2);
   
   
   //TESTS
   
   //Intersection between id-Relation and an empty relation
   result = rf_relation_new_intersection(idR, r1, error);
   int n = set->cardinality * set->cardinality;
   for(int i=0;i<n;i++){
	    CU_ASSERT_FALSE(result->table[i]);
   }
   
   //Intersction id-Relation with itself
   result = rf_relation_new_intersection(idR, idR, error);
   for(int i=0;i<n;i++){
	    CU_ASSERT_EQUAL(idR->table[i], result->table[i]);
   }
   
   //Intersction id-Relation with itself
   result = rf_relation_new_intersection(idR, idR, error);
   for(int i=0;i<n;i++){
	    CU_ASSERT_EQUAL(idR->table[i], result->table[i]);
   }
   
   //Intersection id-Relation with a relation having one intersction element
   result = rf_relation_new_intersection(idR, r2, error);
   for(int i=0;i<n;i++){
	    if (i == rf_table_idx(result, 1,1)){
	      CU_ASSERT_TRUE(result->table[i])
	    } 
	    else{
	      CU_ASSERT_FALSE(result->table[i]);
	    }
   }
   
   //Intersection between two relations that have different domains
   result = rf_relation_new_intersection(idR, idRSet2, error);
   CU_ASSERT_TRUE(result == NULL);
  
}

void test_rf_relation_new_complement(){
  
   rf_Error *error = NULL;
   rf_Relation *idR = rf_relation_new_id(set);
   
   //The error function is never used in this method...consider refactoring 
   rf_Relation *result = rf_relation_new_complement(idR, error);
   
   int n = set->cardinality * set->cardinality;
   for(int i=0;i<n;i++){
	    CU_ASSERT_NOT_EQUAL(idR->table[i], result->table[i]);
   }
  
}

void test_rf_relation_new_concatenation(){
  //often also called composition, which is I think the more correct terminus
  
  rf_Error *error = rf_error_new();
  rf_Relation *r1 = rf_relation_new_empty(set, set);
  rf_Relation *r2 = rf_relation_new_empty(set, set);
  
  r1->table[rf_table_idx(r1,0,1)] = r1->table[rf_table_idx(r1,1,1)] = r1->table[rf_table_idx(r1,2,1)] = true;
  r2->table[rf_table_idx(r2,1,0)] = r2->table[rf_table_idx(r2,1,1)] = r2->table[rf_table_idx(r2,1,2)] = true;
  
  rf_Relation *result = rf_relation_new_concatenation(r1, r2, error);
  
  //should be the full relation in this example
  int n = set->cardinality * set->cardinality;
  
  for(int i=0;i<n;i++){
	  CU_ASSERT_TRUE(result->table[i]);
  }
  
  //TODO: Need more testcases: Other concacts with no full relation result and for heterogeneous relations, too
  
}

void test_rf_relation_new_converse(){
  rf_Error *error = rf_error_new();
  rf_Relation *r1 = rf_relation_new_empty(set, set);
  r1->table[rf_table_idx(r1,0,2)] = true;
  r1->table[rf_table_idx(r1,1,1)] = true;
  
  rf_Relation *result = rf_relation_new_converse(r1, error);
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
      elems[0] = rf_set_element_new_string(a);
      elems[1] = rf_set_element_new_string(b);
      elems[2] = rf_set_element_new_string(c);
      elems[3] = rf_set_element_new_string(d);
      
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
    
    //this should be false but returns true cause symetric is not implemented correctly.
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
	//if those three are correct this method works correct, no need for 
	//a test here-
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
	
	CU_ASSERT_TRUE(rf_set_element_equal(rf_relation_find_maximum(id, NULL), id->domains[0]->elements[0]));
	
	id->table[rf_table_idx(id, 1,2)] = true;
	id->table[rf_table_idx(id, 1,0)] = true;
	CU_ASSERT_EQUAL(rf_relation_find_maximum(id, NULL), NULL);
	id->table[rf_table_idx(id, 0,1)] = false;
	CU_ASSERT_TRUE(rf_set_element_equal(rf_relation_find_maximum(id, NULL), id->domains[0]->elements[1]));
}

void test_rf_relation_find_minimum(){
  
	//if there is more then one min (oder max), this algo returns the first occurence
	//should it maybe be a set of all min / max values?)
	rf_Relation *id = rf_relation_new_id(set);
	id->table[rf_table_idx(id, 0,1)] = true;
	
	CU_ASSERT_TRUE(rf_set_element_equal(rf_relation_find_minimum(id, NULL),id->domains[0]->elements[1]));
	
	
	id->table[rf_table_idx(id, 1,2)] = true;
	id->table[rf_table_idx(id, 1,0)] = true;
	CU_ASSERT_EQUAL(rf_relation_find_minimum(id, NULL), NULL);
	id->table[rf_table_idx(id, 0,1)] = false;
	CU_ASSERT_EQUAL(rf_relation_find_minimum(id, NULL), NULL);
	//printf("output: %s \n", rf_relation_find_minimum(id, NULL)->value.string);
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
}

void test_rf_relation_find_minimal_elements(){
  //lower priority, tested also in maximum by function call in find method
  
}

void test_rf_relation_find_supremum(){
       /*
       * 
       *   a b c d
	 a 1     1
	 b 1 1   1
	 c     1
	 d       1

	M = (a,b,c,d)
	S = (b,c,d)
	S2= (b,d)
       * 
       */
      
      rf_SetElement *elems[4];
      rf_SetElement *elems3[2];
      
      elems[0] = rf_set_element_new_string(a);
      elems[1] = rf_set_element_new_string(b);
      elems[2] = rf_set_element_new_string(c);
      elems[3] = rf_set_element_new_string(d);
      
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
}

void test_rf_relation_find_infimum(){
      rf_SetElement *elems[4];
      rf_SetElement *elems2[2];
      rf_SetElement *elems3[2];
      rf_SetElement *elems4[3];
      
      elems[0] = rf_set_element_new_string(a);
      elems[1] = rf_set_element_new_string(b);
      elems[2] = rf_set_element_new_string(c);
      elems[3] = rf_set_element_new_string(d);
      elems[4] = rf_set_element_new_string(e);
     
      elems2[0] = rf_set_element_new_string(a);
      elems2[1] = rf_set_element_new_string(b);
      
      elems3[0] = rf_set_element_new_string(c);
      elems3[1] = rf_set_element_new_string(d);
      
      elems4[0] = rf_set_element_new_string(a);
      elems4[1] = rf_set_element_new_string(b);
      elems4[2] = rf_set_element_new_string(c);
      
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
}

void test_rf_relation_find_upperbound(){
    
      /*
       *   a b c d
	 a 1     1
	 b 1 1   1
	 c     1
	 d       1

	M = (a,b,c,d)
	S = (b,c,d)
	S2= (b,d)
	
       */
     
      rf_SetElement *elems[4];
      rf_SetElement *elems2[3];
      rf_SetElement *elems3[2];
      
      elems[0] = rf_set_element_new_string(a);
      elems[1] = rf_set_element_new_string(b);
      elems[2] = rf_set_element_new_string(c);
      elems[3] = rf_set_element_new_string(d);
      
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
      elems4[0] = rf_set_element_new_string(a);
      elems4[1] = rf_set_element_new_string(b);
      elems4[2] = rf_set_element_new_string(c);
      elems4[3] = rf_set_element_new_string(d);
      elems4[4] = rf_set_element_new_string(e);
      
      rf_SetElement *elems5[2];
      elems5[0] = rf_set_element_new_string(a);
      elems5[1] = rf_set_element_new_string(b);
       
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
      
      elems[0] = rf_set_element_new_string(a);
      elems[1] = rf_set_element_new_string(b);
      elems[2] = rf_set_element_new_string(c);
      elems[3] = rf_set_element_new_string(d);
      elems[4] = rf_set_element_new_string(e);
      
      rf_SetElement *elems2[2];
      elems2[0] = rf_set_element_new_string(a);
      elems2[1] = rf_set_element_new_string(b);
      
      rf_SetElement *elems3[3];
      elems3[0] = rf_set_element_new_string(a);
      elems3[1] = rf_set_element_new_string(b);
      elems3[2] = rf_set_element_new_string(c);
      
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
      elems[0] = rf_set_element_new_string(a);
      elems[1] = rf_set_element_new_string(b);
      elems[2] = rf_set_element_new_string(c);
      elems[3] = rf_set_element_new_string(d);
      
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
      elems[0] = rf_set_element_new_string(a);
      elems[1] = rf_set_element_new_string(b);
      elems[2] = rf_set_element_new_string(c);
      elems[3] = rf_set_element_new_string(d);

      rf_Set *mySet = rf_set_new(4, elems);
  
      rf_Relation *rel = rf_relation_new_empty(mySet, mySet);
      rel->table[rf_table_idx(rel,0,2)] = true;
      rel->table[rf_table_idx(rel,0,3)] = true;
      rel->table[rf_table_idx(rel,1,0)] = true;
      rel->table[rf_table_idx(rel,2,1)] = true;
      rel->table[rf_table_idx(rel,3,2)] = true;
      
      int expected[16] = {0,0,2,1,3,0,0,0,0,3,0,0,0,0,1,0}; 
      
      int *occurrences = malloc(sizeof(int)*rel->domains[0]->cardinality*rel->domains[0]->cardinality);
      rf_Set *gaps = rf_set_new(0, malloc(0));
      
      int numOfGaps = rf_relation_find_transitive_gaps(rel, occurrences, gaps, NULL);
      
      CU_ASSERT_FALSE(gaps == NULL);
      CU_ASSERT_EQUAL(numOfGaps, 5);
      
      int sum = 0;
      for(int i=0;i<16;i++){
	sum += occurrences[i];
	CU_ASSERT_EQUAL(occurrences[i], expected[i]);	
      }
      CU_ASSERT_EQUAL(numOfGaps*2, sum);
}

void test_rf_relation_guess_transitive_core(){
      printf("\n");
      rf_SetElement *elems[4]; 
      elems[0] = rf_set_element_new_string(a);
      elems[1] = rf_set_element_new_string(b);
      elems[2] = rf_set_element_new_string(c);
      elems[3] = rf_set_element_new_string(d);
      
      rf_Set *mySet = rf_set_new(4, elems);
      
      rf_Relation *rel = rf_relation_new_empty(mySet, mySet);
      
      rel->table[rf_table_idx(rel,0,2)] = true;
      rel->table[rf_table_idx(rel,0,3)] = true;
      rel->table[rf_table_idx(rel,1,0)] = true;
      rel->table[rf_table_idx(rel,2,1)] = true;
      rel->table[rf_table_idx(rel,3,2)] = true;

      bool success = rf_relation_guess_transitive_core(rel, NULL);
      
      CU_ASSERT_FALSE(success);
      
      int dim = 10;
      rf_SetElement *elems2[dim]; 
      elems2[0] = rf_set_element_new_string(a);
      elems2[1] = rf_set_element_new_string(b);
      elems2[2] = rf_set_element_new_string(c);
      elems2[3] = rf_set_element_new_string(d);
      elems2[4] = rf_set_element_new_string(e);
      elems2[5] = rf_set_element_new_string(f);
      elems2[6] = rf_set_element_new_string(g);
      elems2[7] = rf_set_element_new_string(h);
      elems2[8] = rf_set_element_new_string(i);
      elems2[9] = rf_set_element_new_string(j);
      
      for(int testsize=5; testsize<8;testsize++){
	rf_Set *bigSet = rf_set_new(testsize, elems2);
	rf_Relation *bigRel = rf_relation_new_empty(bigSet, bigSet);
      
	for(int i=0;i<testsize-1;i++){
	  bigRel->table[rf_table_idx(bigRel, i,testsize-1)] = true;
	  bigRel->table[rf_table_idx(bigRel, testsize-1,i)] = true;
	}
	clock_t prgstart, prgende;
	prgstart=clock();
      
	rf_relation_guess_transitive_core(bigRel, NULL);
      
	prgende=clock();//CPU-Zeit am Ende des Programmes
	printf("Laufzeit für Problemgröße %i: %.2f Sekunden",testsize, (float)(prgende-prgstart) / CLOCKS_PER_SEC);
	
	for(int i = 0; i<testsize*testsize;i++){
	 // if (i%testsize == 0) printf("\n");
	  //printf("%i", bigRel->table[i]);
	}
	printf("\n");
	
      }

}

void test_rf_relation_find_transitive_hard_core(){
      printf("\n");
      
      rf_SetElement *elems[4]; 
      elems[0] = rf_set_element_new_string(a);
      elems[1] = rf_set_element_new_string(b);
      elems[2] = rf_set_element_new_string(c);
      elems[3] = rf_set_element_new_string(d);
      
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
	if (result->table[i])
		relationsCount++;
      }
      CU_ASSERT_EQUAL(3, relationsCount);
      
      
      //BIG EXAMPLE -WORST CASE SCENARIO
      
      int dim = 7;
      rf_SetElement *elems2[dim]; 
      elems2[0] = rf_set_element_new_string(a);
      elems2[1] = rf_set_element_new_string(b);
      elems2[2] = rf_set_element_new_string(c);
      elems2[3] = rf_set_element_new_string(d);
      elems2[4] = rf_set_element_new_string(e);
      elems2[5] = rf_set_element_new_string(f);
      elems2[6] = rf_set_element_new_string(g);
      elems2[7] = rf_set_element_new_string(h);
      elems2[8] = rf_set_element_new_string(i);
      elems2[9] = rf_set_element_new_string(j);
      
      for(int testsize=5; testsize<8;testsize++){
	rf_Set *bigSet = rf_set_new(testsize, elems2);
	rf_Relation *bigRel = rf_relation_new_empty(bigSet, bigSet);
      
	for(int i=0;i<testsize-1;i++){
	  bigRel->table[rf_table_idx(bigRel, i,testsize-1)] = true;
	  bigRel->table[rf_table_idx(bigRel, testsize-1,i)] = true;
	}
	clock_t prgstart, prgende;
	prgstart=clock();
      
	result = rf_relation_find_transitive_hard_core(bigRel, NULL);
      
	prgende=clock();//CPU-Zeit am Ende des Programmes
	printf("Laufzeit für Problemgröße %i: %.2f Sekunden",testsize, (float)(prgende-prgstart) / CLOCKS_PER_SEC);
	
	for(int i = 0; i<testsize*testsize;i++){
	 // if (i%testsize == 0) printf("\n");
	 // printf("%i", result->table[i]);
	}
	printf("\n");
	relationsCount = 0;
	CU_ASSERT_TRUE(rf_relation_is_transitive(result));
	for(int i = 0; i<testsize*testsize;i++){
	  if (result->table[i])
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
      elems[0] = rf_set_element_new_string(a);
      elems[1] = rf_set_element_new_string(b);
      elems[2] = rf_set_element_new_string(c);
      elems[3] = rf_set_element_new_string(d);
      
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
      //low priority
  
}

void test_rf_relation_make_irreflexive(){
      rf_Relation *test = rf_relation_new_id(set);
      
      CU_ASSERT_FALSE(rf_relation_is_irreflexive(test));
      
      bool result = rf_relation_make_irreflexive(test, NULL);
      CU_ASSERT_TRUE(result);
      CU_ASSERT_TRUE(rf_relation_is_irreflexive(test));
}

void test_rf_relation_make_partial_order(){
      //low priority
  
}

void test_rf_relation_make_preorder(){
      //low priority
  
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



void add_CreateMethods() {
	CU_pSuite createSuite = CU_add_suite("Create-Methods", init_suite1, clean_suite1);
  
        CU_add_test(createSuite, "rf_relation_new_empty", test_rf_relation_new_empty);
        CU_add_test(createSuite, "rf_relation_new_full", test_rf_relation_new_full);
        CU_add_test(createSuite, "rf_relation_new", test_rf_relation_new);
	CU_add_test(createSuite, "rf_relation_copy", test_rf_relation_copy);
	CU_add_test(createSuite, "rf_relation_new_id", test_rf_relation_new_id);
	CU_add_test(createSuite, "rf_table_idx", test_rf_table_idx);
	CU_add_test(createSuite, "rf_relation_new_top", test_rf_relation_new_top);
	CU_add_test(createSuite, "rf_relation_new_bottom", test_rf_relation_new_bottom);
	CU_add_test(createSuite, "rf_relation_new_union", test_rf_relation_new_union);
	CU_add_test(createSuite, "rf_relation_new_intersection", test_rf_relation_new_intersection);
	CU_add_test(createSuite, "rf_relation_new_complement", test_rf_relation_new_complement);
	CU_add_test(createSuite, "rf_relation_new_concatenation", test_rf_relation_new_concatenation);
	CU_add_test(createSuite, "rf_relation_new_converse", test_rf_relation_new_converse);
	CU_add_test(createSuite, "rf_relation_new_subsetleq", test_rf_relation_new_subsetleq);	
}

void add_findGet() {
	CU_pSuite findGet = CU_add_suite("find/get-Methods", init_suite1, clean_suite1);
	
	CU_add_test(findGet, "rf_relation_find_maximum", test_rf_relation_find_maximum);
	CU_add_test(findGet, "rf_relation_find_minimum", test_rf_relation_find_minimum);
	CU_add_test(findGet, "rf_relation_find_maximum_within_subset", test_rf_relation_find_maximum_within_subset);
	CU_add_test(findGet, "rf_relation_find_upperbound", test_rf_relation_find_upperbound);
	CU_add_test(findGet, "rf_relation_find_supremum", test_rf_relation_find_supremum);
	CU_add_test(findGet, "rf_relation_find_lowerbound", test_rf_relation_find_lowerbound);
	CU_add_test(findGet, "rf_relation_find_infimum", test_rf_relation_find_infimum);
	CU_add_test(findGet, "rf_relation_find_transitive_gaps", test_rf_relation_find_transitive_gaps);
	CU_add_test(findGet, "rf_relation_guess_transitive_core", test_rf_relation_guess_transitive_core);
	CU_add_test(findGet, "rf_relation_find_transitive_hard_core", test_rf_relation_find_transitive_hard_core);
	
	CU_add_test(findGet, "rf_relation_get_image", test_rf_relation_get_image);
	CU_add_test(findGet, "rf_relation_get_preimage", test_rf_relation_get_preimage);
	
}

void add_isMethods() {
	CU_pSuite isMethods = CU_add_suite("is-Methods", init_suite1, clean_suite1);
	
	//homogeneous
	CU_add_test(isMethods, "rf_relation_is_homogeneous", test_rf_relation_is_homogeneous);
	CU_add_test(isMethods, "rf_relation_is_antisymmetric", test_rf_relation_is_antisymmetric);
	CU_add_test(isMethods, "rf_relation_is_asymmetric", test_rf_relation_is_asymmetric);
	CU_add_test(isMethods, "rf_relation_is_difcuntional", test_rf_relation_is_difunctional);
	CU_add_test(isMethods, "rf_relation_is_equivalent", test_rf_relation_is_equivalent);
	CU_add_test(isMethods, "rf_relation_is_symmetric", test_rf_relation_is_symmetric);
	CU_add_test(isMethods, "rf_relation_is_irreflexive", test_rf_relation_is_irreflexive);
	CU_add_test(isMethods, "rf_relation_is_reflexive", test_rf_relation_is_reflexive);
	CU_add_test(isMethods, "rf_relation_is_partial_order", test_rf_relation_is_partial_order);
	CU_add_test(isMethods, "rf_relation_is_transitive", test_rf_relation_is_transitive);
	CU_add_test(isMethods, "rf_relation_is_lattice", test_rf_relation_is_lattice);
	
	//heterogeneous
	CU_add_test(isMethods, "rf_relation_is_lefttotal", test_rf_relation_is_lefttotal);
	CU_add_test(isMethods, "rf_relation_is_functional", test_rf_relation_is_functional);
	CU_add_test(isMethods, "rf_relation_is_function", test_rf_relation_is_function);
	CU_add_test(isMethods, "rf_relation_is_surjective", test_rf_relation_is_surjective);
	CU_add_test(isMethods, "rf_relation_is_injective", test_rf_relation_is_injective);
	CU_add_test(isMethods, "rf_relation_is_bijective", test_rf_relation_is_bijective);
}

void add_makeMethods() {
	CU_pSuite makeMethods = CU_add_suite("make-Methods", init_suite1, clean_suite1);
	
	CU_add_test(makeMethods, "rf_relation_make_antisymmetric", test_rf_relation_make_antisymmetric); 
	CU_add_test(makeMethods, "rf_relation_make_asymmetric", test_rf_relation_make_asymmetric);
	CU_add_test(makeMethods, "rf_relation_make_difunctional", test_rf_relation_make_difunctional);
	CU_add_test(makeMethods, "rf_relation_make_equivalent", test_rf_relation_make_equivalent);
	CU_add_test(makeMethods, "rf_relation_make_irreflexive", test_rf_relation_make_irreflexive); 
	CU_add_test(makeMethods, "rf_relation_make_partial_order", test_rf_relation_make_partial_order); 
	CU_add_test(makeMethods, "rf_relation_make_preorder", test_rf_relation_make_preorder);
	CU_add_test(makeMethods, "rf_relation_make_reflexive", test_rf_relation_make_reflexive);
	CU_add_test(makeMethods, "rf_relation_make_symmetric", test_rf_relation_make_symmetric);
	CU_add_test(makeMethods, "rf_relation_make_transitive", test_rf_relation_make_transitive);
}

int main(){
	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
	   return CU_get_error();

	add_CreateMethods();
	add_findGet();
	add_isMethods();
	add_makeMethods();

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
