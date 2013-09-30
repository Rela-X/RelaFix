#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

extern CU_pSuite add_suite_set(void);
extern CU_pSuite add_suite_set_element(void);

extern CU_pSuite add_suite_relation_create(void);
extern CU_pSuite add_suite_relation_search(void);
extern CU_pSuite add_suite_relation_test(void);
extern CU_pSuite add_suite_relation_modification(void);

int
main() {
	/* initialize the CUnit test registry */
	if(CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suites to the registry */
	// test_set.c
	if(add_suite_set() == NULL) goto cleanup;
	if(add_suite_set_element() == NULL) goto cleanup;
	// test_relation.c
	if(add_suite_relation_create() == NULL) goto cleanup;
	if(add_suite_relation_search() == NULL) goto cleanup;
	if(add_suite_relation_test() == NULL) goto cleanup;
	if(add_suite_relation_modification() == NULL) goto cleanup;
	//

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();

cleanup:
	CU_cleanup_registry();

	return CU_get_error();
}
