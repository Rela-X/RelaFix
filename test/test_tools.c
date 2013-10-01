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

#include "tools.h"

void
test_rf_bitcount() {
	unsigned int six = 6;
	int result = rf_bitcount(six);
	CU_ASSERT_EQUAL(result, 2);

	unsigned int twentyseven = 27;
	result = rf_bitcount(twentyseven);
	CU_ASSERT_EQUAL(result, 4);
}


CU_ErrorCode
register_suites_tools() {
	CU_TestInfo tools_suite[] = {
		{ "rf_bitcount", test_rf_bitcount },
		CU_TEST_INFO_NULL,
	};

	CU_SuiteInfo suites[] = {
		{ "Tools", NULL, NULL, tools_suite },
		CU_SUITE_INFO_NULL,
	};

	return CU_register_suites(suites);
}
