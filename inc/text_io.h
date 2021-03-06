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

#ifndef RF_TEXTIO_H
#define RF_TEXTIO_H

#include "set.h"
#include "relation.h"

/*
 * Strings allocated by these functions must be passed to rf_string_free
 */
rf_Set *        rf_set_from_string(const char *);
char *          rf_set_to_string(rf_Set *);
rf_Relation *   rf_relation_from_string(const char *);
char *          rf_relation_to_string(rf_Relation *);

void            rf_string_free(const char *);

#endif
