/*
 * RelaFix       Table
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 \brief A 2 dimensional array.

 Declares functions to work on struct RF_TABLE. The table abstracts a simple array. It is used
 in RF_OPERATION and RF_RELATION to hide the implementation of the array. The table can set to
 binary and string mode (::rf_enum_tt). In binary mode it uses every bit allocated. In string
 mode it stores pointers to strings.
 */

#ifndef RF_TABLE_H
#define RF_TABLE_H

/*!
 @brief modes of RF_TABLE
 */
enum rf_enum_tt {RF_TT_BINARY, RF_TT_STRING};

/*!
 @brief A 2 dimensional array.

 The table abstracts a simple array. It is used
 in RF_OPERATION and RF_RELATION to hide the implementation of the array. The table can set to
 binary and string mode (::rf_enum_tt). In binary mode it uses every bit allocated. In string
 mode it stores pointers to strings.
 */
typedef struct rf_table
{
	enum rf_enum_tt type;	/*!< \brief Describes the data stored in data. Value must be from enum ::rf_enum_tt. */
	unsigned int    width;
	unsigned int    height;

	/*!
	 @brief The data stored in the table. Depends on the type (::rf_enum_tt).
	 - RF_TT_BINARY: data is of type char *. An array of char. Every bit is used!
	 - RF_TT_STRING: data is of type char **. An array of strings.
	 */
	void            *data;
} RF_TABLE;


RF_TABLE *   rf_table_copy(RF_TABLE *table);
RF_TABLE *   rf_table_create(unsigned int width, unsigned int height, int type);
void         rf_table_destroy(RF_TABLE *table);
unsigned int rf_table_get_height(RF_TABLE *table);
int          rf_table_get_bit(RF_TABLE *table, unsigned int x, unsigned int y);
int          rf_table_get_string(RF_TABLE *table, unsigned int x, unsigned int y, char **string_out);
int          rf_table_get_type(RF_TABLE *table);
unsigned int rf_table_get_width(RF_TABLE *table);
int          rf_table_set_bit(RF_TABLE *table, unsigned int x, unsigned int y, int bit);
int          rf_table_set_string(RF_TABLE *table, unsigned int x, unsigned int y, char *string);

#endif
