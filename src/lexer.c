/*
 * RelaFixLib, functions for the parser
 *
 * Copyright (C) Peter Berger, 2011
 */


#include "parse.h"
#include "tools.h"
#include "array.h"
#include "table.h"
#include "parser.tab.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>


/*!
\brief Safes a keyword and its id in the parser
*/
typedef struct
{
    char	*word; /*!< \brief The keyword */
    int		id; /*!< \brief The id that represents the keyword in the parser */
} RF_LEXER_KEYWORD;

static RF_BOOL is_first_run = RF_TRUE;

/*int rf_lexer_id = RF_LEXER_ID;*/

int rf_lexer_get_keyword(char *word)
{
	static RF_LEXER_KEYWORD keyword[] =
	{
		{"DOMAIN", T_DOMAIN},
		{"RELATION", T_RELATION},
		{"OPERATION", T_OPERATION},
		{"DOMAINS", T_DOMAINS},
		{"RELATIONS", T_RELATIONS},
		{"OPERATIONS", T_OPERATIONS},
		{"NEGATIONS", T_NEGATIONS},
		{"FUNCTIONS", T_FUNCTIONS},
		{"LIST", T_LIST},
		{"DELETE", T_DELETE},
		{"AS", T_AS},
		{"TABLE", T_TABLE},
		{"LOAD", T_LOAD},
		{"NEGATION", T_NEGATION},
		{"FORMULA", T_FORMULA},
		{"PATH", T_PATH},
		{"X", 'X'},
		{"Y", 'Y'},
		{0, 0}
	};
	int i;

	if(!word)
		return 0;

	for(i = 0; keyword[i].id != 0; i++)
		if(strcmp(word, keyword[i].word) == 0)
			return keyword[i].id;

	return 0;
}

int yylex(void)
{
	int c, i;
	char *file = 0;
	static char *id_buffer = 0;
	static int length = 0;
	RF_BOOL skip_test = RF_FALSE;
	
	if(length == 0)
	{
		length = 40;
		id_buffer = malloc(length + 1);
	}
	
	if(is_first_run == RF_TRUE)
	{
		is_first_run = RF_FALSE;
		rf_parser_register_system_functions();
		
		/* load system.rfc */
		file = rf_string_copy("system.rfc");
		rf_lexer_need_load_file(file, (RF_LOCATION *)&yylloc);
	}

	while(1)
	{
		while((c = rf_lexer_need_char()) == ' ' || c == '\t')
		{
			yylloc.last_column++;
			
			if(rf_parser_record == RF_TRUE)
				rf_parser_record_append(c);
		}

		if(c == EOF)
			return 0;


		/* check for new line. new line is  '\r' or '\r\n' or '\n\r' or '\n' */
		if(c == '\r')
		{
			c = rf_lexer_need_char();
			if(c != '\n')
				rf_lexer_need_ungetc(c);

			yylloc.last_line++;
			yylloc.first_column = yylloc.last_column = 1;
			continue;
		}
		else if(c == '\n')
		{
			c = rf_lexer_need_char();
			if(c != '\r')
				rf_lexer_need_ungetc(c);

			yylloc.last_line++;
			yylloc.first_column = yylloc.last_column = 1;
			continue;
		}

		/* skip comment till end of line, else go on */
		if(c == '#')
		{
			while(1)
			{
				c = rf_lexer_need_char();
				
				if(c == '\r' || c == '\n' || c == EOF)
					break;
			}

			rf_lexer_need_ungetc(c);
			continue;
		}
		else
		{
			yylloc.first_line = yylloc.last_line;
			yylloc.first_column = yylloc.last_column;
			break;
		}
	}

	if(c == '-')
	{
		c = rf_lexer_need_char();
		if(c == '>')
		{
			yylloc.last_column += 2;
			return T_ARROW_R;
		}
		else
		{
			rf_lexer_need_ungetc(c);
			c = '-';
		}
	}
	else if(c == '"')
	{
		i = 0;
		skip_test = RF_FALSE;
		c = rf_lexer_need_char();
		while((c != EOF && c != '"') || skip_test == RF_TRUE)
		{
			skip_test = RF_FALSE;
			if(i == length)
			{
				length *= 2;
				id_buffer = realloc(id_buffer, length + 1);
			}

			id_buffer[i] = c;
			i++;

			c = rf_lexer_need_char();
			
			if(c == '\\')
			{
				c = rf_lexer_need_char();
				if(c == EOF)
					break;
				
				skip_test = RF_TRUE;
				yylloc.last_column += 1;
			}
		}

		yylloc.last_column += i;
		if(c == '"')
			yylloc.last_column += 1;
		else
			rf_lexer_need_ungetc(c);
		
		id_buffer[i] = '\0';
		yylval.id = rf_string_copy(id_buffer);
		return STRING;
	}

	if(rf_lexer_mode == RF_LEXER_NORMAL)
	{
		if(c == '_' || isdigit(c) || isalpha(c))
		{
			int type;
			RF_BOOL has_dot = RF_FALSE;

			i = 0;
			do
			{
				if(rf_parser_record == RF_TRUE)
					rf_parser_record_append(c);
				
				if(c == '.')
					has_dot = RF_TRUE;

				if(i == length)
				{
					length *= 2;
					id_buffer = realloc(id_buffer, length + 1);
				}

				id_buffer[i] = c;
				i++;

				c = rf_lexer_need_char();
			} while(isalnum(c) || c == '_' || isdigit(c));

			yylloc.last_column += i;
			rf_lexer_need_ungetc(c);
			id_buffer[i] = '\0';

			if((type = rf_lexer_get_keyword(id_buffer)))
			{
				/* stop recording the input if keyword was found */
				switch(type)
				{
					case T_DOMAIN:
					case T_RELATION:
					case T_OPERATION:
					case T_LOAD:
					case T_NEGATION:
						rf_parser_record = RF_FALSE;
				}
			}
			else
			{
				type = ID;
				yylval.id = rf_string_copy(id_buffer);
			}

			return type;
		}
	}
	
	else if(rf_lexer_mode == RF_LEXER_FILE)
	{
		if(c == '_' || c == '.' || c == '/' || c == '\\' || isdigit(c) || isalpha(c))
		{
			int type;
			int i;

			i = 0;
			do
			{
				if(i == length)
				{
					length *= 2;
					id_buffer = realloc(id_buffer, length + 1);
				}

				id_buffer[i] = c;
				i++;

				c = rf_lexer_need_char();
			} while(c == '_' || c == '.' || c == '/' || c == '\\' || isdigit(c) || isalpha(c));

			yylloc.last_column += i;
			rf_lexer_need_ungetc(c);
			id_buffer[i] = '\0';


			if((type = rf_lexer_get_keyword(id_buffer)));
			else
			{
				type = FILENAME;
				yylval.id = rf_string_copy(id_buffer);
			}

			return type;
		}
	}

	else if(rf_lexer_mode == RF_LEXER_CROSS)
	{
		yylloc.last_column++;
		return c;
	}

	if(rf_parser_record == RF_TRUE)
		rf_parser_record_append(c);
	yylloc.last_column++;
	return c;
}

