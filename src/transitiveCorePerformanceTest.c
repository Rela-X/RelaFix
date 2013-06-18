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

rf_Set *set;
rf_Set *set2;

const int MAX_TESTSIZE = 10;

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
	return 0;
}


int clean_suite1(void)
{
	return 0;
}


void test_rf_relation_guess_transitive_core(){
      printf("\n");
      
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
      
      for(int testsize=5; testsize<=MAX_TESTSIZE;testsize++){
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
	  if (i%testsize == 0) 
	    printf("\n");
	  printf("%i", bigRel->table[i]);
	}
	printf("\n");
	
      }

}

void test_rf_relation_find_transitive_hard_core(){
      printf("\n");
      
      //BIG EXAMPLE -WORST CASE SCENARIO
      
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
      
      for(int testsize=5; testsize<=MAX_TESTSIZE;testsize++){
	rf_Set *bigSet = rf_set_new(testsize, elems2);
	rf_Relation *bigRel = rf_relation_new_empty(bigSet, bigSet);
      
	for(int i=0;i<testsize-1;i++){
	  bigRel->table[rf_table_idx(bigRel, i,testsize-1)] = true;
	  bigRel->table[rf_table_idx(bigRel, testsize-1,i)] = true;
	}
	clock_t prgstart, prgende;
	prgstart=clock();
      
	rf_Relation *result = rf_relation_find_transitive_hard_core(bigRel, NULL);
      
	prgende=clock();//CPU-Zeit am Ende des Programmes
	printf("Laufzeit für Problemgröße %i: %.2f Sekunden",testsize, (float)(prgende-prgstart) / CLOCKS_PER_SEC);
	
	for(int i = 0; i<testsize*testsize;i++){
	  if (i%testsize == 0) 
	    printf("\n");
	  printf("%i", result->table[i]);
	}
	printf("\n");
	int relationsCount = 0;
	CU_ASSERT_TRUE(rf_relation_is_transitive(result));
	for(int i = 0; i<testsize*testsize;i++){
	  if (result->table[i])
		  relationsCount++;
	}
	CU_ASSERT_EQUAL(testsize-1, relationsCount);
	
      }
      
}

void add_findGet() {
	CU_pSuite findGet = CU_add_suite("find/get-Methods", init_suite1, clean_suite1);
	
	CU_add_test(findGet, "rf_relation_guess_transitive_core", test_rf_relation_guess_transitive_core);
	CU_add_test(findGet, "rf_relation_find_transitive_hard_core", test_rf_relation_find_transitive_hard_core);
	
}



int main(){
	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
	   return CU_get_error();

	add_findGet();;

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
