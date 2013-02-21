/*
 * RelaFix       Table
 *
 * Copyright (C) Peter Berger, 2011
 */

#include <stdlib.h>
#include <string.h>

#include "table.h"


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

void rf_table_destroy(RF_TABLE *table)
{
	if(!table)
		return;

	if(table->data)
		free(table->data);

	free(table);
}

unsigned int rf_table_get_height(RF_TABLE *table)
{
	if(!table)
		return 0;

	return table->height;
}

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

int rf_table_get_type(RF_TABLE *table)
{
	if(!table)
		return -1;

	return table->type;
}

unsigned int rf_table_get_width(RF_TABLE *table)
{
	if(!table)
		return 0;

	return table->width;
}

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
