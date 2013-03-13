/* 
 * Copyright (C) 2008,2013 Peter Berger, Wilke Schwiedop
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

/*! \file
 * Error handling structures
 */

#ifndef RF_ERROR_H
#define RF_ERROR_H

enum _rf_errorcode {
	RF_E_OK,
	RF_E_GENERIC,
	RF_E_NO_MEMORY,
	RF_E_NULL_ARG,
	RF_E_SET_DUP_ELEMENT,
	RF_E_SET_NOT_MEMBER,
	RF_E_SET_NOT_SUBSET,
	RF_E_REL_NO_MIN,
	RF_E_REL_NO_MAX,
	RF_E_REL_NOT_HOMOGENEOUS,
	RF_E_REL_NOT_ORDERED,
};

typedef struct _rf_error        rf_Error;
typedef enum _rf_errorcode      rf_ErrorCode;

struct _rf_error {
	rf_ErrorCode    code;           /*!< Error Code. */
	char            *msg;           /*!< Error Message. */
	void            *aux;           /*!< Auxilluary data structure. */
	void            (*aux_free)(void *);
};

//rf_Error * rf_error_new();
void       rf_error_set(rf_Error *e, rf_ErrorCode c, char *msg);
void       rf_error_set_aux(rf_Error *e, rf_ErrorCode c, char *msg, void *aux, void (*aux_free)(void *));
void       rf_error_reset(rf_Error *e);
//void       rf_error_free(rf_Error *e);

#endif
