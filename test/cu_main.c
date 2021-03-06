#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

extern CU_ErrorCode register_suites_set(void);
extern CU_ErrorCode register_suites_relation(void);
extern CU_ErrorCode register_suites_tools(void);
extern CU_ErrorCode register_suites_text_io(void);

int
main() {
	/* initialize the CUnit test registry */
	if(CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suites to the registry */
	if(CUE_SUCCESS != register_suites_set()) goto cleanup;
//	if(CUE_SUCCESS != register_suites_relation()) goto cleanup;
	if(CUE_SUCCESS != register_suites_tools()) goto cleanup;
	if(CUE_SUCCESS != register_suites_text_io()) goto cleanup;

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();

cleanup:
	CU_cleanup_registry();

	return CU_get_error();
}
