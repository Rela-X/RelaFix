/*
 * Copyright (C) 2011,2013 Peter Berger, Wilke Schwiedop
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
#include <string.h>
#include <assert.h>

#include "error.h"

rf_Error *
rf_error_new() {
	rf_Error *e = malloc(sizeof(*e));
	return e;
}

void
rf_error_set(rf_Error *e, rf_ErrorCode c, char *msg) {
	assert(e != NULL);
	assert(msg != NULL);

	e->code = c;
	e->msg = strdup(msg);
}

void
rf_error_set_aux(rf_Error *e, rf_ErrorCode c, char *msg, void *aux, void (* aux_free)(void *)) {
	assert(aux != NULL);
	assert(aux_free != NULL);

	rf_error_set(e, c, msg);
	e->aux = aux;
	e->aux_free = aux_free;
}

void
rf_error_reset(rf_Error *e) {
	assert(e != NULL);

	e->code = RF_E_OK;
	if(e->msg)
		free(e->msg);
	if(e->aux)
		e->aux_free(e->aux);
}

void
rf_error_free(rf_Error *e) {
	assert(e != NULL);

	rf_error_reset(e);
	free(e);
}
