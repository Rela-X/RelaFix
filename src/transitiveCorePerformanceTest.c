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

const int MAX_TESTSIZE = 13;


void generateTestSet(int n, rf_SetElement *elements[n]){
	for(int i=0;i<n;i++){
		char *myString = malloc(sizeof(char));
	    myString[0] = 'a' + i;
	    myString[1] = '\0';
	    elements[i] = rf_set_element_new_string(myString);
	}
}


void test_rf_relation_guess_transitive_core(){
    printf("\n");

    for(int testsize=5; testsize<=MAX_TESTSIZE;testsize++){
      rf_SetElement *elems[testsize];
      generateTestSet(testsize, elems);
      rf_Set *bigSet = rf_set_new(testsize, elems);
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

    for(int testsize=5; testsize<=MAX_TESTSIZE;testsize++){
    	rf_SetElement *elems[testsize];
    	generateTestSet(testsize, elems);
    	rf_Set *bigSet = rf_set_new(testsize, elems);
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
	CU_pSuite findGet = CU_add_suite("find/get-Methods", NULL, NULL);

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
