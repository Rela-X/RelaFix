/*
 * RelaFix       Table
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 A 2 dimensional array.

 Declares functions to work on struct RF_TABLE. The table abstracts a simple array. It is used
 in RF_OPERATION and RF_RELATION to hide the implementation of the array. The table can set to
 binary and string mode (::rf_enum_tt). In binary mode it uses every bit allocated. In string
 mode it stores pointers to strings.
 */

#ifndef RF_TABLE_H
#define RF_TABLE_H

/*!
 modes of RF_TABLE
 */
enum rf_enum_tt {RF_TT_BINARY, RF_TT_STRING};

/*!
 A 2 dimensional array.

 The table abstracts a simple array. It is used
 in RF_OPERATION and RF_RELATION to hide the implementation of the array. The table can set to
 binary and string mode (::rf_enum_tt). In binary mode it uses every bit allocated. In string
 mode it stores pointers to strings.
 */
typedef struct rf_table
{
	enum rf_enum_tt type;	/*!< Describes the data stored in data. Value must be from enum ::rf_enum_tt. */
	unsigned int    width;
	unsigned int    height;

	/*!
	 The data stored in the table. Depends on the type (::rf_enum_tt).
	 - RF_TT_BINARY: data is of type char *. An array of char. Every bit is used!
	 - RF_TT_STRING: data is of type char **. An array of strings.
	 */
	void            *data;
} RF_TABLE;


/*!
 Table data is be copied. But not the data the pointers stored in the table point to.
 @relates RF_TABLE
 @param[in] table The table that should be copied.
 @return The copy of the table.
 @return 0 on error.
 */
RF_TABLE *   rf_table_copy(RF_TABLE *table);

/*!
Creates a new empty table of given size and type.

@relates RF_TABLE
@param width The width of the table
@param height The height of the table
@param type The type of the table. Must be from ::rf_enum_tt.
@return The new table
@return 0 on error.
*/
RF_TABLE *   rf_table_create(unsigned int width, unsigned int height, int type);

/*!
Frees the table.

@relates RF_TABLE
@param[in] table The table that should be freed.
*/
void         rf_table_destroy(RF_TABLE *table);

/*!
Returns the height of the table.

@relates RF_TABLE
@param[in] table The table whoes height is in question.
@return The height
@return 0 on error.
*/
unsigned int rf_table_get_height(RF_TABLE *table);

/*!
Returns the bit at the given position.

@relates RF_TABLE
@param[in] table The table that contains the bits.
@param x Horizontal position
@param y Vertical position
@return The bit at the position (1 or 0)
@return <0 on error
*/
int          rf_table_get_bit(RF_TABLE *table, unsigned int x, unsigned int y);

/*!
Returns the string at the given position.

@relates RF_TABLE
@param[in] table The table that contains the strings.
@param x Horizontal position
@param y Vertical position
@param[out] string_out A null-pointer. Will contain a string after the call. The owner is the table!
@return 0 on success. The string is written to string_out.
@return >0 on error
*/
int          rf_table_get_string(RF_TABLE *table, unsigned int x, unsigned int y, char **string_out);

/*!
Returns the type of the table (::rf_enum_tt)

@relates RF_TABLE
@param[in] table The tables whoes type is in question.
@return the type that is from ::rf_enum_tt.
@return -1 on error.
*/
int          rf_table_get_type(RF_TABLE *table);

/*!
Returns the width of the table.

@relates RF_TABLE
@param[in] table The table whoes width is in question.
@return The width
@return 0 on error.
*/
unsigned int rf_table_get_width(RF_TABLE *table);

/*!
Sets the bit at the given position.

@relates RF_TABLE
@param[in] table The table that contains the bits.
@param x Horizontal position
@param y Vertical position
@param bit the new bit to be written (0 or 1)
@return 0 on success
@return >0 on error
*/
int          rf_table_set_bit(RF_TABLE *table, unsigned int x, unsigned int y, int bit);

/*!
Sets the string at the given position.

@relates RF_TABLE
@param[in] table The table that contains the strings.
@param x Horizontal position
@param y Vertical position
@param string the new string to be written. The table takes the ownership of the string. The string gets invalid, when the table gets invalid.
@return 0 on success
@return >0 on error
*/
int          rf_table_set_string(RF_TABLE *table, unsigned int x, unsigned int y, char *string);


#endif
