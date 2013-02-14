/*
 * RelaFix       Table
 *
 * Copyright (C) Peter Berger, 2011
 */

#include "table.h"
#include <stdlib.h>
#include <string.h>


/*enum {__SINGLE_BLOCK, __MULTI_BLOCK};*/




/*!
 Table data is be copied. But not the data the pointers stored in the table point to.
 @relates RF_TABLE
 @param[in] table The table that should be copied.
 @return The copy of the table.
 @return 0 on error.
 */
RF_TABLE * rf_table_copy(RF_TABLE *table)
{
	RF_TABLE *copy;
	int one_more, size;
	
	if(!table)
		return 0;
	
	/* get memory for new table */
	copy = malloc(sizeof(RF_TABLE));
	if(!copy)
		return 0;
	
	/* copy the properties */
	copy->type = table->type;
	copy->width = table->width;
	copy->height = table->height;
	
	/* copy the stored data based on the type */
	if(table->type == RF_TT_BINARY) /* copy every bit */
	{
		one_more = (table->width * table->height) % sizeof(char);
		size = ((table->width * table->height) / sizeof(char) + (one_more ? 1 : 0)) * sizeof(char);
		copy->data = malloc(size);
		memcpy(copy->data, table->data, size);
	}
	else if(table->type == RF_TT_STRING) /* copy only pointers to the strings */
	{
		size = table->width * table->height * sizeof(char *);
		copy->data = malloc(size);
		memcpy(copy->data, table->data, size);
	}
	else /* invalid type */
	{
		free(copy);
		return 0;
	}
	
	return copy;
}


/*!
Creates a new empty table of given size and type.

@relates RF_TABLE
@param width The width of the table
@param height The height of the table
@param type The type of the table. Must be from ::rf_enum_tt.
@return The new table
@return 0 on error.
*/
RF_TABLE * rf_table_create(unsigned int width, unsigned int height, int type)
{
	RF_TABLE *table;
	int one_more;

	if(width == 0 || height == 0)
		return 0;
	
	if(type != RF_TT_BINARY && type != RF_TT_STRING)
		return 0;

	/* get the memory for the struct */
	table = malloc(sizeof(RF_TABLE));
	if(!table)
		return 0;

	table->type = type;
	table->width = width;
	table->height = height;
	
	/* if needed in future we can save the space with RLE compression algorithms ! */
	/* get the memory for the table */
	if(type == RF_TT_BINARY)
	{
		one_more = (width * height) % sizeof(char);
		table->data = calloc((width * height) / sizeof(char) + (one_more ? 1 : 0), sizeof(char));
	}
	else if(type == RF_TT_STRING)
	{
		table->data = calloc(width * height, sizeof(char *));
	}
	else
	{
		free(table);
		return 0;
	}

	if(!table->data)
	{
		free(table);
		return 0;
	}

	return table;
}

/*!
Frees the table.

@relates RF_TABLE
@param[in] table The table that should be freed.
*/
void rf_table_destroy(RF_TABLE *table)
{
	if(!table)
		return;

	if(table->data)
		free(table->data);

	free(table);
}

/*!
Returns the height of the table.

@relates RF_TABLE
@param[in] table The table whoes height is in question.
@return The height
@return 0 on error.
*/
unsigned int rf_table_get_height(RF_TABLE *table)
{
	if(!table)
		return 0;

	return table->height;
}

/*!
Returns the bit at the given position.

@relates RF_TABLE
@param[in] table The table that contains the bits.
@param x Horizontal position
@param y Vertical position
@return The bit at the position (1 or 0)
@return <0 on error
*/
int rf_table_get_bit(RF_TABLE *table, unsigned int x, unsigned int y)
{
	unsigned int pos, block_pos, bit_pos;
	char *field = 0;
	char mask;

	if(!table)
		return -2;
	if(y < 0 || x < 0 || y >= table->height || x >= table->width)
		return -3;
	if(!table->data)
		return -4;
	if(table->type != RF_TT_BINARY)
		return -5;
	
	pos = (y * table->width) + x;
	block_pos = pos / sizeof(char);
	bit_pos = pos - (block_pos * sizeof(char));
	field = table->data;
	mask = 0x01;
	mask <<= bit_pos;
	if(field[block_pos] & mask)
		return 1;
	else
		return 0;
}

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
int rf_table_get_string(RF_TABLE *table, unsigned int x, unsigned int y, char **string_out)
{
	unsigned int pos;
	char **strings = 0;

	if(!table)
		return 1;
	if(y < 0 || x < 0 || y >= table->height || x >= table->width)
		return 2;
	if(!string_out)
		return 3;
	if(!table->data)
		return 4;
	if(table->type != RF_TT_STRING)
		return 5;
	
	pos = (y * table->width) + x;
	strings = table->data;
	*string_out = strings[pos];
	
	return 0;
}

/*!
Returns the type of the table (::rf_enum_tt)

@relates RF_TABLE
@param[in] table The tables whoes type is in question.
@return the type that is from ::rf_enum_tt.
@return -1 on error.
*/
int rf_table_get_type(RF_TABLE *table)
{
	if(!table)
		return -1;
	
	return table->type;
}

/*!
Returns the width of the table.

@relates RF_TABLE
@param[in] table The table whoes width is in question.
@return The width
@return 0 on error.
*/
unsigned int rf_table_get_width(RF_TABLE *table)
{
	if(!table)
		return 0;

	return table->width;
}


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
int rf_table_set_bit(RF_TABLE *table, unsigned int x, unsigned int y, int bit)
{
	unsigned int pos, block_pos, bit_pos;
	char *field = 0;
	char mask;

	if(!table)
		return 1;
	if(y < 0 || x < 0 || y >= table->height || x >= table->width)
		return 2;
	if(!table->data)
		return 4;
	if(table->type != RF_TT_BINARY)
		return 5;


	pos = (y * table->width) + x;
	block_pos = pos / sizeof(char);
	bit_pos = pos - (block_pos * sizeof(char));
	field = table->data;
	mask = 0x01;
	mask <<= bit_pos;
	if(bit)
		field[block_pos] |= mask;
	else
		field[block_pos] &= ~mask;

	return 0;
}

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
int rf_table_set_string(RF_TABLE *table, unsigned int x, unsigned int y, char *string)
{
	unsigned int pos;
	char **strings = 0;

	if(!table)
		return 1;
	if(y < 0 || x < 0 || y >= table->height || x >= table->width)
		return 2;
	if(!table->data)
		return 4;
	if(table->type != RF_TT_STRING)
		return 5;

	
	pos = (y * table->width) + x;
	strings = table->data;
	strings[pos] = string;
	
	return 0;
}
