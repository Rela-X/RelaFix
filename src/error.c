/*
 * RelaFix Error
 *
 * Copyright (C) Wilke Schwiedop 2013
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
