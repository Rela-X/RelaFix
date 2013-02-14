/*
 * RelaFix parser description for bison
 *
 * Copyright (C) Peter Berger, 2011
 */



/* IMPORTANT: All IDs that come from the lexer must be freed here!!! */





/* preamble */
%{
	#include "array.h"
	#include "domain.h"
	#include "relation.h"
	#include "operation.h"
	#include "function.h"
	#include "negation.h"
	#include "table.h"
	#include "list.h"
	#include "formula.h"
	#include "parse.h"
	#include "tools.h"
	#include <stdlib.h>
	#include <stdio.h>
	int yylex(void);
	void yyerror(char const *);
%}




/* parser description */
%error-verbose
%locations

%union
{
	RF_DOMAIN				*domain;
	RF_RELATION				*relation;
	RF_OPERATION			*operation;
	RF_FORMULA				*formula;
	char					*id;
	RF_TABLE				*table;
	RF_PARSER_TABLE_DATA	*table_data;
	RF_LIST					*list;
	RF_ARRAY				*array;
}

%token END 0 "end of file"
%token T_DOMAIN "DOMAIN"
%token T_RELATION "RELATION"
%token T_OPERATION "OPERATION"
%token T_ARROW_R "->"
%token T_TABLE "TABLE"
%token T_AS "AS"
%token T_LOAD "LOAD"
%token T_NEGATION "NEGATION"
%token T_FORMULA "FORMULA"
%token T_PATH "PATH"
%token T_LIST "LIST"
%token T_DELETE "DELETE"
%token T_DOMAINS "DOMAINS"
%token T_RELATIONS "RELATIONS"
%token T_OPERATIONS "OPERATIONS"
%token T_NEGATIONS "NEGATIONS"
%token T_FUNCTIONS "FUNCTIONS"
%token <id> ID STRING FILENAME

%type <domain> domain free_domain
%type <table_data> table relationdefx operationdefx formula
%type <list> domainx tablea tableb neglist
%type <array> farguments
%type <formula> fexpr fexprx

%destructor {if($$) free($$);} ID STRING FILENAME
%destructor {if($$) rf_domain_destroy($$);} domain free_domain
%destructor {if($$) rf_list_destroy($$, (void (*)(void *))rf_element_destroy);} domainx
%destructor {if($$) rf_list_destroy($$, free);} tablea neglist
%destructor {if($$) rf_list_destroy($$, (void (*)(void *))rf_parser_destroy_table_row);} tableb
%destructor {if($$) rf_array_destroy($$, (void (*)(void *))rf_formula_destroy);} farguments
%destructor {if($$) rf_formula_destroy($$);} fexpr fexprx
%destructor {if($$) rf_parser_destroy_table_data($$);} table formula relationdefx operationdefx

%right "AS"
%left ','

%%
input:
		| input statement	{
								rf_parser_record = RF_TRUE;
								rf_parser_record_clear();
								rf_parser_with_variable = RF_FALSE;
							}
		;

statement:	'\n'
			| domaindef
			| relationdef
			| operationdef
			| negationdef
			| list
			| delete
			| "PATH" {rf_lexer_mode = RF_LEXER_FILE;} FILENAME {rf_lexer_mode = RF_LEXER_NORMAL;} ';'	{
																											rf_parser_set_path($3);
																										}
			| "LOAD" {rf_lexer_mode = RF_LEXER_FILE;} FILENAME {rf_lexer_mode = RF_LEXER_NORMAL;} ';'	{
																											if(rf_lexer_need_load_file(rf_string_copy($3), (RF_LOCATION *)&yylloc) != 0)
																											{
																												if(rf_lexer_need_load_file(rf_string_combine(2, rf_parser_get_path(), $3), (RF_LOCATION *)&yylloc) != 0)
																													rf_parser_error(@3.last_line, @3.last_column, "Can not find file '", 2, $3, "'");
																											}

																											free($3);
																										}
			| show {rf_parser_record = RF_FALSE;}
			;

domaindef:	"DOMAIN" ID "AS" fexpr ';'	{
													if(!$4)
													{
														rf_parser_error(@4.first_line, @4.first_column, "no expression", 0);

														if($2)
															free($2);
													}
													else if(rf_parser_get_domain($2))
													{
														rf_parser_error(@2.first_line, @2.first_column, "found DOMAIN '", 2, $2, "' , expected free ID");
														rf_formula_destroy($4);
														free($2);
													}
													else if(rf_parser_get_relation($2))
													{
														rf_parser_error(@2.first_line, @2.first_column, "found RELATION '", 2, $2, "' , expected free ID");
														rf_formula_destroy($4);
														free($2);
													}
													else if(rf_parser_get_operation($2))
													{
														rf_parser_error(@2.first_line, @2.first_column, "found OPERATION '", 2, $2, "' , expected free ID");
														rf_formula_destroy($4);
														free($2);
													}
													else if(rf_parser_get_negation($2))
													{
														rf_parser_error(@2.first_line, @2.first_column, "found NEGATION '", 2, $2, "' , expected free ID");
														rf_formula_destroy($4);
														free($2);
													}
													else if(rf_parser_get_function($2))
													{
														rf_parser_error(@2.first_line, @2.first_column, "found FUNCTION '", 2, $2, "' , expected free ID");
														rf_formula_destroy($4);
														free($2);
													}
													else
													{
														RF_FORMULA_RESULT *result = 0;
														if(rf_formula_calc($4, 0, 0, &result) != 0)
														{
															rf_parser_error(@4.first_line, @4.first_column, result->error, 0);
															free($2);
														}
														else
														{
															if(result->type == RF_FO_ID || result->type == RF_FO_ELEMENT)
															{
																RF_DOMAIN *domain = rf_parser_get_domain(result->element);

																if(!domain)
																{
																	rf_parser_error(@4.first_line, @4.first_column, "'", 2, result->element, "' is not a DOMAIN.");
																	free($2);
																}
																else
																{
																	domain = rf_domain_copy(domain);
																	rf_domain_set_name(domain, $2);
																	rf_parser_add_domain(domain);
																}
															}
															else if(result->type == RF_FO_DOMAIN)
															{
																RF_DOMAIN *domain = rf_domain_copy(result->domain);
																rf_domain_set_name(domain, $2);
																rf_parser_add_domain(domain);
															}
															else
															{
																rf_parser_error(@4.first_line, @4.first_column, "The result of the expression is not a DOMAIN", 0);
																free($2);
															}
														}

														rf_formula_destroy_result(result);
														rf_formula_destroy($4);
													}
												}
			;

relationdef:	"RELATION" ID ':' ID "->" ID "AS" relationdefx ';'	{
																		RF_DOMAIN *domain_1, *domain_2;
																		RF_RELATION *relation;

																		if(!$8);
																		else if(rf_parser_get_relation($2))
																		{
																			rf_parser_error(@2.first_line, @2.first_column, "found RELATION '", 2, $2, "' , expected free ID");
																			free($2);
																		}
																		else if(rf_parser_get_domain($2))
																		{
																			rf_parser_error(@2.first_line, @2.first_column, "found DOMAIN '", 2, $2, "' , expected free ID");
																			free($2);
																		}
																		else if(rf_parser_get_operation($2))
																		{
																			rf_parser_error(@2.first_line, @2.first_column, "found OPERATION '", 2, $2, "' , expected free ID");
																			free($2);
																		}
																		else if(rf_parser_get_negation($2))
																		{
																			rf_parser_error(@2.first_line, @2.first_column, "found NEGATION '", 2, $2, "' , expected free ID");
																			free($2);
																		}
																		else if(rf_parser_get_function($2))
																		{
																			rf_parser_error(@2.first_line, @2.first_column, "found FUNCTION '", 2, $2, "' , expected free ID");
																			free($2);
																		}
																		else if(!(domain_1 = rf_parser_get_domain($4)))
																		{
																			rf_parser_error(@4.first_line, @4.first_column, "'", 2, $4, "' is not a DOMAIN");
																			free($2);
																		}
																		else if(!(domain_2 = rf_parser_get_domain($6)))
																		{
																			rf_parser_error(@6.first_line, @6.first_column, "'", 2, $6, "' is not a DOMAIN");
																			free($2);
																		}
																		else if($8)
																		{

																			if($8->type == RF_PTD_DATA)
																			{
																				int result;
																				RF_ERROR error;
																				relation = rf_relation_create($2, domain_1, domain_2);
																				result = rf_relation_build_table(relation, $8, &error);
																				if(result == 0)
																					rf_parser_add_relation(relation);
																				else
																				{
																					rf_parser_error(@8.first_line, @8.first_column, error.string, 0);
																					free(error.string);
																					rf_relation_destroy(relation);
																				}

																			}
																			else if($8->type == RF_PTD_FORMULA)
																			{
																				if($8->formula)
																				{
																					int result;
																					RF_ERROR error;
																					relation = rf_relation_create($2, domain_1, domain_2);
																					result = rf_relation_build_table_by_formula(relation, $8->formula, &error);
																					if(result == 0)
																						rf_parser_add_relation(relation);
																					else
																					{
																						rf_parser_error(error.location.first_line, error.location.first_column, error.string, 0);
																						if(error.string)
																							free(error.string);
																						rf_relation_destroy(relation);
																					}
																				}
																				else
																					free($2);
																			}
																			else if($8->type == RF_PTD_RELATION)
																			{
																				if($8->relation)
																				{
																					relation = $8->relation;
																					$8->relation = 0;

																					if(domain_1 != rf_relation_get_domain_1(relation))
																					{
																						rf_parser_error(@4.first_line, @4.first_column, "domain mismatch '", 4,
																							rf_domain_get_name(domain_1), "' != '",
																							rf_domain_get_name(rf_relation_get_domain_1(relation)), "'");
																						free($2);
																						rf_relation_destroy(relation);
																					}
																					else if(domain_2 != rf_relation_get_domain_2(relation))
																					{
																						rf_parser_error(@6.first_line, @6.first_column, "domain mismatch '", 4,
																							rf_domain_get_name(domain_2), "' != '",
																							rf_domain_get_name(rf_relation_get_domain_2(relation)), "'");
																						free($2);
																						rf_relation_destroy(relation);
																					}
																					else
																					{
																						rf_relation_set_name(relation, $2);
																						rf_parser_add_relation(relation);
																					}
																				}
																				else
																					free($2);
																			}
																			else
																			{
																				rf_parser_error(@8.first_line, @8.first_column, "program error - not implemented", 0);
																				free($2);
																			}
																		}

																		free($4);
																		free($6);
																		if($8)
																			rf_parser_destroy_table_data($8);
																	}
				;

relationdefx:	table	{$$ = $1;}
				| formula	{$$ = $1;}
				| fexpr	{
							RF_RELATION *relation = 0;
							RF_FORMULA_RESULT *result;
							int err = rf_formula_calc($1, 0, 0, &result);

							if(err)
							{
								if(result)
									rf_parser_error(result->location.first_line, result->location.first_column,
										result->error, 0);
								else
									rf_parser_error(@1.first_line, @1.first_column,
										"program error - no errormessage after failed formula_calc", 0);
							}
							else if(result->type == RF_FO_RELATION)
							{
								if(result->is_temporary && result->can_delete)
								{
									relation = result->relation;
									result->can_delete = RF_FALSE;
								}
								else
									relation = rf_relation_copy(result->relation);
							}
							else if(result->type == RF_FO_ELEMENT || result->type == RF_FO_ID)
							{
								relation = rf_parser_get_relation(result->element);
								relation = rf_relation_copy(relation);

								if(!relation)
								{
									rf_parser_error(@1.first_line, @1.first_column, "found ELEMENT '", 2, result->element, "', expected RELATION", 0);
								}
							}
							else
								rf_parser_error(@1.first_line, @1.first_column, "expression has wrong result, expected RELATION", 0);

							if(result)
								rf_formula_destroy_result(result);
							if($1)
								rf_formula_destroy($1);

							if(relation)
							{
								RF_PARSER_TABLE_DATA *table_data = calloc(1, sizeof(RF_PARSER_TABLE_DATA));
								if(!table_data)
								{
									rf_parser_error(@1.first_line, @1.first_column, "program error - no memory", 0);
									$$ = 0;
								}
								else
								{
									table_data->type = RF_PTD_RELATION;
									table_data->relation = relation;

									$$ = table_data;
								}
							}
							else
								$$ = 0;
						}
				;

operationdef:	"OPERATION" ID ':' ID {rf_lexer_mode = RF_LEXER_CROSS;} 'x' {rf_lexer_mode = RF_LEXER_NORMAL;}
					ID "->" ID "AS" operationdefx ';'	{
															RF_DOMAIN *domain_1, *domain_2, *domain_3;
															RF_OPERATION *operation;

															if(rf_parser_get_operation($2))
															{
																rf_parser_error(@2.first_line, @2.first_column, "found OPERATION '", 2, $2, "' , expected free ID");
																free($2);
															}
															else if(rf_parser_get_negation($2))
															{
																rf_parser_error(@2.first_line, @2.first_column, "found NEGATION '", 2, $2, "' , expected free ID");
																free($2);
															}
															else if(rf_parser_get_domain($2))
															{
																rf_parser_error(@2.first_line, @2.first_column, "found DOMAIN '", 2, $2, "' , expected free ID");
																free($2);
															}
															else if(rf_parser_get_relation($2))
															{
																rf_parser_error(@2.first_line, @2.first_column, "found RELATION '", 2, $2, "' , expected free ID");
																free($2);
															}
															else if(rf_parser_get_function($2))
															{
																rf_parser_error(@2.first_line, @2.first_column, "found FUNCTION '", 2, $2, "' , expected free ID");
																free($2);
															}
															else if(!(domain_1 = rf_parser_get_domain($4)))
															{
																rf_parser_error(@4.first_line, @4.first_column, "'", 2, $4, "' is not a DOMAIN");
																free($2);
															}
															else if(!(domain_2 = rf_parser_get_domain($8)))
															{
																rf_parser_error(@8.first_line, @8.first_column, "'", 2, $8, "' is not a DOMAIN");
																free($2);
															}
															else if(!(domain_3 = rf_parser_get_domain($10)))
															{
																rf_parser_error(@10.first_line, @10.first_column, "'", 2, $10, "' is not a DOMAIN");
																free($2);
															}
															else if($12)
															{

																if($12->type == RF_PTD_DATA)
																{
																	int result;
																	RF_ERROR error;
																	operation = rf_operation_create($2, domain_1, domain_2, domain_3);
																	result = rf_operation_build_table(operation, $12, &error);
																	if(result == 0)
																		rf_parser_add_operation(operation);
																	else
																	{
																		rf_parser_error(@12.first_line, @12.first_column, error.string, 0);
																		free(error.string);
																		rf_operation_destroy(operation);
																	}
																}
																else if($12->type == RF_PTD_FORMULA)
																{
																	if($12->formula)
																	{
																		int result;
																		RF_ERROR error;
																		operation = rf_operation_create($2, domain_1, domain_2, domain_3);
																		result = rf_operation_build_table_by_formula(operation, $12->formula, &error);
																		if(result == 0)
																			rf_parser_add_operation(operation);
																		else
																		{
																			rf_parser_error(error.location.first_line, error.location.first_column, error.string, 0);
																			if(error.string)
																				free(error.string);
																			rf_operation_destroy(operation);
																		}
																	}
																	else
																		free($2);
																}
																else if($12->type == RF_PTD_OPERATION)
																{
																	if($12->operation)
																	{
																		operation = $12->operation;
																		$12->operation = 0;

																		if(domain_1 != rf_operation_get_domain_1(operation))
																		{
																			rf_parser_error(@4.first_line, @4.first_column, "domain mismatch '", 4,
																				rf_domain_get_name(domain_1), "' != '",
																				rf_domain_get_name(rf_operation_get_domain_1(operation)), "'");
																			free($2);
																			rf_operation_destroy(operation);
																		}
																		else if(domain_2 != rf_operation_get_domain_2(operation))
																		{
																			rf_parser_error(@8.first_line, @8.first_column, "domain mismatch '", 4,
																				rf_domain_get_name(domain_2), "' != '",
																				rf_domain_get_name(rf_operation_get_domain_2(operation)), "'");
																			free($2);
																			rf_operation_destroy(operation);
																		}
																		else if(domain_3 != rf_operation_get_domain_3(operation))
																		{
																			rf_parser_error(@10.first_line, @10.first_column, "domain mismatch '", 4,
																				rf_domain_get_name(domain_3), "' != '",
																				rf_domain_get_name(rf_operation_get_domain_3(operation)), "'");
																			free($2);
																			rf_operation_destroy(operation);
																		}
																		else
																		{
																			rf_operation_set_name(operation, $2);
																			rf_parser_add_operation(operation);
																		}
																	}
																	else
																		free($2);
																}
																else
																{
																	rf_parser_add_operation($12->operation);
																	$12->operation = 0;
																	free($2);
																}
															}

															if($4)
																free($4);
															if($8)
																free($8);
															if($10)
																free($10);
															if($12)
																rf_parser_destroy_table_data($12);
														}
				;

operationdefx:	table	{$$ = $1;}
				| formula	{$$ = $1;}
				| fexpr	{
							RF_OPERATION *operation = 0;
							RF_FORMULA_RESULT *result;
							int err = rf_formula_calc($1, 0, 0, &result);

							if(err)
							{
								if(result)
									rf_parser_error(result->location.first_line, result->location.first_column,
										result->error, 0);
								else
									rf_parser_error(@1.first_line, @1.first_column,
										"program error - no errormessage after failed formula_calc", 0);
							}
							else if(result->type == RF_FO_OPERATION)
							{
								if(result->is_temporary && result->can_delete)
								{
									operation = result->operation;
									result->can_delete = RF_FALSE;
								}
								else
									operation = rf_operation_copy(result->operation);
							}
							else if(result->type == RF_FO_ELEMENT || result->type == RF_FO_ID)
							{
								operation = rf_parser_get_operation(result->element);
								operation = rf_operation_copy(operation);

								if(!operation)
								{
									rf_parser_error(@1.first_line, @1.first_column, "found ELEMENT '", 2, result->element, "', expected OPERATION", 0);
								}
							}
							else
								rf_parser_error(@1.first_line, @1.first_column, "expression has wrong result, expected OPERATION", 0);

							if(result)
								rf_formula_destroy_result(result);
							if($1)
								rf_formula_destroy($1);

							if(operation)
							{
								RF_PARSER_TABLE_DATA *table_data = calloc(1, sizeof(RF_PARSER_TABLE_DATA));
								if(!table_data)
								{
									rf_parser_error(@1.first_line, @1.first_column, "program error - no memory", 0);
									$$ = 0;
								}
								else
								{
									table_data->type = RF_PTD_OPERATION;
									table_data->operation = operation;

									$$ = table_data;
								}
							}
							else
								$$ = 0;
						}
				;

negationdef:	"NEGATION" ID ':' ID "AS" neglist ';'	{
															RF_NEGATION *negation = 0;
															RF_DOMAIN *domain = 0;

															if(rf_parser_get_negation($2))
															{
																rf_parser_error(@2.first_line, @2.first_column, "found NEGATION '", 2, $2, "' , expected free ID");
																if($6)
																	rf_list_destroy($6, free);
																if($2)
																	free($2);
																if($4)
																	free($4);
															}
															else if(rf_parser_get_operation($2))
															{
																rf_parser_error(@2.first_line, @2.first_column, "found OPERATION '", 2, $2, "' , expected free ID");
																if($6)
																	rf_list_destroy($6, free);
																if($2)
																	free($2);
																if($4)
																	free($4);
															}
															else if(rf_parser_get_function($2))
															{
																rf_parser_error(@2.first_line, @2.first_column, "found FUNCTION '", 2, $2, "' , expected free ID");
																if($6)
																	rf_list_destroy($6, free);
																if($2)
																	free($2);
																if($4)
																	free($4);
															}
															else if(rf_parser_get_domain($2))
															{
																rf_parser_error(@2.first_line, @2.first_column, "found DOMAIN '", 2, $2, "' , expected free ID");
																if($6)
																	rf_list_destroy($6, free);
																if($2)
																	free($2);
																if($4)
																	free($4);
															}
															else if(rf_parser_get_relation($2))
															{
																rf_parser_error(@2.first_line, @2.first_column, "found RELATION '", 2, $2, "' , expected free ID");
																if($6)
																	rf_list_destroy($6, free);
																if($2)
																	free($2);
																if($4)
																	free($4);
															}
															else if(!(domain = rf_parser_get_domain($4)))
															{
																rf_parser_error(@4.first_line, @4.first_column, "found ID '", 2, $4, "' , expected DOMAIN");
																if($6)
																	rf_list_destroy($6, free);
																if($2)
																	free($2);
																if($4)
																	free($4);
															}
															else
															{
																RF_ERROR error;
																negation = rf_negation_create($2, domain);
																if(negation)
																{
																	int result = rf_negation_set_items(negation, $6, &error);
																	if(result)
																	{
																		rf_parser_error(@6.first_line, @6.first_column, error.string, 0);
																		if(error.string)
																			free(error.string);
																	}
																	else
																		rf_parser_add_negation(negation);
																}
																else
																{
																	rf_list_destroy($6, free);
																	rf_parser_error(@1.first_line, @1.first_column, "no memory", 0);
																}

																free($4);
															}
														}
				;

list:	"LIST" "DOMAINS" ';'	{rf_parser_list_domains();}
		| "LIST" "RELATIONS" ';'	{rf_parser_list_relations();}
		| "LIST" "OPERATIONS" ';'	{rf_parser_list_operations();}
		| "LIST" "NEGATIONS" ';'	{rf_parser_list_negations();}
		| "LIST" "FUNCTIONS" ';'	{rf_parser_list_functions();}
		;

delete:	"DELETE" ID ';'	{
							RF_DOMAIN *domain = 0;
							RF_RELATION *relation = 0;
							RF_OPERATION *operation = 0;
							RF_NEGATION *negation = 0;

							if(!$2)
							{
								rf_parser_error(@2.first_line, @2.first_column, "program error - empty ID", 0);
							}
							else if(domain = rf_parser_get_domain($2))
							{
								rf_parser_delete_domain(domain);
								free($2);
							}
							else if(relation = rf_parser_get_relation($2))
							{
								rf_parser_delete_relation(relation);
								free($2);
							}
							else if(operation = rf_parser_get_operation($2))
							{
								rf_parser_delete_operation(operation);
								free($2);
							}
							else if(negation = rf_parser_get_negation($2))
							{
								rf_parser_delete_negation(negation);
								free($2);
							}
							else
							{
								rf_parser_error(@2.first_line, @2.first_column, "Cant delete ID '", 2, $2, "', expected DOMAIN or RELATION or OPERATION or NEGATION.");
								free($2);
							}
						}
		;

neglist:	ID '/' ID	{
							RF_LIST *list = rf_list_create();
							if(!list)
							{
								rf_parser_error(@1.first_line, @1.first_column, "no memory", 0);
								free($1);
								free($3);
								$$ = 0;
							}
							else
							{
								rf_list_append(list, $1);
								rf_list_append(list, $3);

								$$ = list;
							}

						}
			| neglist ',' neglist	{
										if($1 == 0 || $3 == 0)
										{
											if($1)
												rf_list_destroy($1, free);
											if($3)
												rf_list_destroy($3, free);

											$$ = 0;
										}
										else
										{
											rf_list_merge($1, $3);
											rf_list_destroy($3, 0);

											$$ = $1;
										}
									}
			;

show:	fexpr ';'	{
						RF_FORMULA_RESULT *result = 0;

						if(!$1);
						else if(rf_formula_calc($1, 0, 0, &result) != 0)
						{
							if(result == 0)
								rf_parser_error(@1.first_line, @1.first_column, "error in formula", 0);
							else
								rf_parser_error(result->location.first_line,
									result->location.first_column, result->error, 0);
						}
						else if(result->type == RF_FO_DOMAIN)
						{
							rf_parser_show_domain(result->domain);
						}
						else if(result->type == RF_FO_RELATION)
						{
							rf_parser_show_relation(result->relation);
						}
						else if(result->type == RF_FO_NEGATION)
						{
							rf_parser_show_negation(result->negation);
						}
						else if(result->type == RF_FO_OPERATION)
						{
							rf_parser_show_operation(result->operation);
						}
						else if(result->type == RF_FO_ELEMENT)
						{
							rf_parser_show_string(rf_parser_record_read());
							rf_parser_show_string("  =  ");
							rf_parser_show_element(result->element);
							rf_parser_show_string("\n");
						}
						else if(result->type == RF_FO_ID)
						{
							RF_RELATION *relation = rf_parser_get_relation(result->element);
							RF_OPERATION *operation = rf_parser_get_operation(result->element);
							RF_NEGATION *negation = rf_parser_get_negation(result->element);
							RF_DOMAIN *domain = rf_parser_get_domain(result->element);
							RF_FUNCTION *function = rf_parser_get_function(result->element);

							if(relation)
								rf_parser_show_relation(relation);
							else if(operation)
								rf_parser_show_operation(operation);
							else if(negation)
								rf_parser_show_negation(negation);
							else if(domain)
								rf_parser_show_domain(domain);
							else if(function)
								rf_parser_show_function(function);
							else
								rf_parser_error(@1.first_line, @1.first_column, "found free ID '", 2, result->element, "', expected"
									" RELATION or OPERATION or NEGATION or DOMAIN or FUNCTION or EXPRESSION");
						}
						else
						{
							rf_parser_error(@1.first_line, @1.first_column, "program error - not implemented", 0);
						}

						if(result)
							rf_formula_destroy_result(result);
						if($1)
							rf_formula_destroy($1);
					}
		| STRING ';'	{
							if(!$1)
								rf_parser_error(@1.first_line, @1.first_column, "program error: no string", 0);
							else
							{
								rf_parser_show_string($1);
								rf_parser_show_string("\n\n");
								free($1);
							}
						}
		;

free_domain:	'{' domainx '}'	{
									if($2)
									{
										RF_DOMAIN *domain = rf_domain_create(rf_string_copy("no name"));
										RF_ERROR error;

										if(rf_domain_check_elements($2, &error) == 1)
										{
											rf_parser_error(@2.first_line, @2.first_column, error.string, 0);
											free(error.string);
											rf_domain_destroy(domain);
											rf_list_destroy($2, (void (*)(void *))rf_element_destroy);
											$$ = 0;
										}
										else
										{
											rf_domain_set_list(domain, $2);

											$$ = domain;
										}
									}
									else
										$$ = 0;
								}

domain:	ID ':' '{' domainx '}'	{
									if(!$1)
									{
										rf_parser_error(@1.first_line, @1.first_column, "program error - no id", 0);

										if($4)
											rf_list_destroy($4, (void (*)(void *))rf_element_destroy);

										$$ = 0;
									}
									else if($4)
									{
										RF_DOMAIN *domain = rf_domain_create($1);
										RF_ERROR error;

										if(rf_domain_check_elements($4, &error) == 1)
										{
											rf_parser_error(@4.first_line, @4.first_column, error.string, 0);
											free(error.string);
											rf_domain_destroy(domain);

											$$ = 0;
										}
										else
										{
											rf_domain_set_list(domain, $4);

											$$ = domain;
										}
									}
									else
									{
										free($1);

										$$ = 0;
									}
								}
		| ID ':' '{' '}'	{
								if(!$1)
								{
									rf_parser_error(@1.first_line, @1.first_column, "program error - no id", 0);
									$$ = 0;
								}
								else
								{
									RF_DOMAIN *domain = rf_domain_create($1);
									$$ = domain;
								}
							}
		;

domainx:	ID	{
					RF_LIST *elemente = rf_list_create();
					RF_ELEMENT *element = rf_element_create(RF_ET_NAME, $1);
					rf_list_append(elemente, element);

					$$ = elemente;
				}
			| ':' ID	{
							RF_DOMAIN *domain = rf_parser_get_domain($2);
							RF_LIST *elemente = 0;
							RF_ELEMENT *element = 0;

							if(!domain)
							{
								rf_parser_error(@1.first_line, @1.first_column, "'", 2, $2, "' is not a DOMAIN");
								if($2)
									free($2);
								$$ = 0;
							}
							else
							{
								elemente = rf_list_create();
								element = rf_element_create(RF_ET_GLOBAL_DOMAIN, domain);
								rf_list_append(elemente, element);

								$$ = elemente;
							}
						}
			| domain	{
							if($1)
							{
								RF_ELEMENT *element = rf_element_create(RF_ET_DOMAIN, $1);
								RF_LIST *elemente = rf_list_create();
								rf_list_append(elemente, element);

								$$ = elemente;
							}
							else
								$$ = 0;
						}
			| domainx ',' domainx	{
										if($1 && $3)
										{
											rf_list_merge($1, $3);
											rf_list_destroy($3, 0);

											$$ = $1;
										}
										else if($1)
										{
											rf_list_destroy($1, (void (*)(void *))rf_element_destroy);
											$$ = 0;
										}
										else if($3)
										{
											rf_list_destroy($3, (void (*)(void *))rf_element_destroy);
											$$ = 0;
										}
									}
			;

table:	"TABLE" '(' tablea ',' tableb ')' 	{
												RF_PARSER_TABLE_DATA *table = malloc(sizeof(RF_PARSER_TABLE_DATA));
												table->type = RF_PTD_DATA;
												table->rows = $5;
												table->columns = $3;

												$$ = table;
											}
		;

tablea:	ID	{
				RF_LIST *list = rf_list_create();
				rf_list_append(list, $1);
				$$ = list;
			}
		| tablea ID	{
						rf_list_append($1, $2);
						$$ = $1;
					}
		;

tableb:	ID ':' tablea	{
							RF_LIST *row_list = rf_list_create();
							RF_PARSER_TABLE_ROW *row = rf_parser_create_table_row($1);
							RF_LIST *list = rf_parser_table_row_get_list(row);
							rf_list_merge(list, $3);
							rf_list_destroy($3, 0);
							rf_list_append(row_list, row);

							$$ = row_list;
						}
		| ID ':' tablea ',' tableb	{
										RF_LIST *row_list = rf_list_create();
										RF_PARSER_TABLE_ROW *row = rf_parser_create_table_row($1);
										RF_LIST *list = rf_parser_table_row_get_list(row);
										rf_list_merge(list, $3);
										rf_list_destroy($3, 0);
										rf_list_append(row_list, row);

										rf_list_merge(row_list, $5);
										rf_list_destroy($5, 0);

										$$ = row_list;
									}
		;

formula:	"FORMULA" {rf_parser_with_variable = RF_TRUE;} fexpr	{
																		RF_PARSER_TABLE_DATA *table = malloc(sizeof(RF_PARSER_TABLE_DATA));
																		table->type = RF_PTD_FORMULA;
																		table->formula = $3;

																		$$ = table;
																	}
			;

fexpr:	fexprx	{$$ = $1;}
		| fexprx ID fexprx	{
								RF_OPERATION *operation = rf_parser_get_operation($2);
								RF_RELATION *relation = rf_parser_get_relation($2);
								RF_FORMULA *formula = 0;

								if(!$1 || !$3)
								{
									if($1)
										rf_formula_destroy($1);
									if($3)
										rf_formula_destroy($3);

									free($2);

									$$ = 0;
								}
								else if(operation)
								{
									formula = rf_formula_create();
									rf_formula_set_operation(formula, operation);
									rf_formula_set_location(formula, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
									rf_formula_append_argument(formula, $1);
									rf_formula_append_argument(formula, $3);

									free($2);

									$$ = formula;
								}
								else if(relation)
								{
									formula = rf_formula_create();
									rf_formula_set_relation(formula, relation);
									rf_formula_set_location(formula, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
									rf_formula_append_argument(formula, $1);
									rf_formula_append_argument(formula, $3);

									free($2);

									$$ = formula;
								}
								else
								{
									rf_parser_error(@2.first_line, @2.first_column, "'", 2, $2, "' is not a OPERATION or RELATION");
									free($2);
									if($1)
										rf_formula_destroy($1);
									if($3)
										rf_formula_destroy($3);

									$$ = 0;
								}
							}
		;

fexprx:	ID	{
				RF_FORMULA *formula = rf_formula_create();
				rf_formula_set_element(formula, $1);
				rf_formula_set_location(formula, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
				$$ = formula;
			}
		| 'X'	{
					if(rf_parser_with_variable == RF_TRUE)
					{
						RF_FORMULA *formula = rf_formula_create();
						rf_formula_set_variable(formula, 'X');
						rf_formula_set_location(formula, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
						$$ = formula;
					}
					else
					{
						rf_parser_error(@1.first_line, @1.first_column, "Variable 'X' is only in FORMULA allowed", 0);
						$$ = 0;
					}
				}
		| 'Y'	{
					if(rf_parser_with_variable == RF_TRUE)
					{
						RF_FORMULA *formula = rf_formula_create();
						rf_formula_set_variable(formula, 'Y');
						rf_formula_set_location(formula, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
						$$ = formula;
					}
					else
					{
						rf_parser_error(@1.first_line, @1.first_column, "Variable 'Y' is only in FORMULA allowed", 0);
						$$ = 0;
					}
				}
		| free_domain	{
							RF_FORMULA *formula = 0;

							if(!$1)
							{
								$$ = 0;
							}
							else
							{
								formula = rf_formula_create();
								rf_formula_set_domain(formula, $1);

								$$ = formula;
							}
						}
		| '(' fexpr ')'	{$$ = $2;}
		| ID '(' farguments ')'	{
									RF_FUNCTION *function = rf_parser_get_function($1);
									RF_NEGATION *negation = rf_parser_get_negation($1);
									RF_FORMULA *formula = 0;

									if(!$1 || !$3 || (!function && !negation))
									{
										if($1 && (!function && !negation))
											rf_parser_error(@1.first_line, @1.first_column, "'", 2, $1, "' is not a FUNCTION or NEGATION");
										if(!$1)
											rf_parser_error(@1.first_line, @1.first_column, "program error - no ID", 0);
										if(!$3)
											rf_parser_error(@3.first_line, @3.first_column, "no arguments for '", 2, $1, "'");

										if($1)
											free($1);
										if($3)
											rf_array_destroy($3, (void (*)(void *))rf_formula_destroy);

										$$ = 0;
									}
									else if(function)
									{
										if(rf_function_get_argument_count(function) != rf_array_size($3))
										{
											rf_parser_error(@3.first_line, @3.first_column, "Wrong argument count for function '", 2, $1, "'");
											free($1);
											rf_array_destroy($3, (void (*)(void *))rf_formula_destroy);

											$$ = 0;
										}
										else
										{
											formula = rf_formula_create();
											rf_formula_set_function(formula, function);
											rf_formula_set_location(formula, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
											rf_formula_set_arguments(formula, $3);

											free($1);

											$$ = formula;
										}
									}
									else if(negation)
									{
										if(rf_array_size($3) != 1)
										{
											rf_parser_error(@3.first_line, @3.first_column, "Wrong argument count for negation '", 2, $1, "'");
											free($1);
											rf_array_destroy($3, (void (*)(void *))rf_formula_destroy);

											$$ = 0;
										}
										else
										{
											formula = rf_formula_create();
											rf_formula_set_negation(formula, negation);
											rf_formula_set_location(formula, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
											rf_formula_set_arguments(formula, $3);

											free($1);

											$$ = formula;
										}
									}
								}
		;

farguments:	fexpr	{
						RF_ARRAY *array = 0;

						if(!$1)
							$$ = 0;
						else
						{
							array = rf_array_create();
							rf_array_append(array, $1);

							$$ = array;
						}
					}
			| farguments ',' farguments	{
											if(!$1 || !$3)
											{
												if($1)
													rf_array_destroy($1, (void (*)(void *))rf_formula_destroy);
												if($3)
													rf_array_destroy($3, (void (*)(void *))rf_formula_destroy);

												$$ = 0;
											}
											else
											{
												rf_array_merge($1, $3);
												rf_array_destroy($3, (void (*)(void *))rf_formula_destroy);

												$$ = $1;
											}
										}
			;

%%


/* c code */

/*!
 Fills the table of RF_RELATION based on the data that was retrieved from parsing the user input.

 The function will check if the conditions of the parsed data will match with the relation.
 Checks are:
 - wrong horizontal element count
 - wrong vertikal element count
 - elements are in domain
 - elements are missing

 If one of these checks fail an error message is returned in RF_ERROR.
 @relates RF_RELATION
 @param[in] relation The relation whoes table should be set.
 @param[in] table_data Contains the parsed table information.
 @param[out] error If the function fails an error description is written in error. See RF_ERROR. 0 is not allowed.
 @return 0 on success
 @return !0 on fail. A description is written to error.
 */
int rf_relation_build_table(RF_RELATION *relation, RF_PARSER_TABLE_DATA *table_data, RF_ERROR *error)
{
	RF_LIST *list_h, *list_v;
	RF_LIST_ITERATOR *names_h, *names_v, *iterator_row, *iterator_column = 0;
	RF_PARSER_TABLE_ROW *table_row;
	int row, column, pos;
	char *name_h, *name_v, *tmp_name, *tmp_value;

	if(!table_data || !relation)
	{
		error->string = rf_string_copy("program error - invalid input to relation_build_table");
		return 1;
	}

	if(table_data->type == RF_PTD_DATA)
	{
		/* check if table has the right width and height */
		if(rf_table_get_height(relation->table) != rf_list_get_count(table_data->rows))
		{
			error->string = rf_string_copy("wrong table height");
			return 1;
		}
		if(rf_table_get_width(relation->table) != rf_list_get_count(table_data->columns))
		{
			error->string = rf_string_copy("wrong table width");
			return 1;
		}



		/* Walk over the domain names to fill the table. For every row fill columns
		 * The inputdata is searched for the matching element names for every position in table
		 */
		list_h = rf_domain_get_element_names(relation->domain_2);
		list_v = rf_domain_get_element_names(relation->domain_1);
		names_h = 0;
		names_v = rf_list_get_begin(list_v);
		iterator_row = 0;
		row = column = 0;
		/* row from domain*/
		while(rf_list_has_next(names_v))
		{
			if(names_h)
				rf_list_delete_iterator(names_h);
			names_h = rf_list_get_begin(list_h);
			iterator_row = rf_list_get_begin(table_data->rows);
			name_v = rf_list_next(names_v);

			/* find matching row in parsed tabledata */
			while(1)
			{
				if(!rf_list_has_next(iterator_row))
				{
					rf_list_delete_iterator(iterator_row);
					rf_list_delete_iterator(names_h);
					rf_list_delete_iterator(names_v);
					rf_list_destroy(list_v, 0);
					rf_list_destroy(list_h, 0);

					error->string = rf_string_combine(3, "Row '", name_v, "' is missing in table");
					return 1;
				}

				table_row = rf_list_next(iterator_row);
				if(!table_row)
					continue;
				if(!table_row->element)
					continue;

				/* check if count of parsed row elements fit the domain element count */
				if(rf_list_get_count(table_row->values) < rf_list_get_count(list_h))
				{
					rf_list_delete_iterator(iterator_row);
					rf_list_delete_iterator(names_h);
					rf_list_delete_iterator(names_v);
					rf_list_destroy(list_v, 0);
					rf_list_destroy(list_h, 0);

					error->string = rf_string_combine(3, "Elements are missing in row '", name_v, "'");
					return 1;
				}
				else if(rf_list_get_count(table_row->values) > rf_list_get_count(list_h))
				{
					rf_list_delete_iterator(iterator_row);
					rf_list_delete_iterator(names_h);
					rf_list_delete_iterator(names_v);
					rf_list_destroy(list_v, 0);
					rf_list_destroy(list_h, 0);

					error->string = rf_string_combine(3, "Too many elements in row '", name_v, "'");
					return 1;
				}

				/* if row of parsed tabledata matches the current domain row break search */
				if(strcmp(table_row->element, name_v) == 0)
					break;
			}

			rf_list_delete_iterator(iterator_row);
			iterator_row = 0;


			/* column from domain */
			column = 0;
			while(rf_list_has_next(names_h))
			{
				name_h = rf_list_next(names_h);
				if(iterator_column)
					rf_list_delete_iterator(iterator_column);

				/* search the matching column position in parsed data */
				iterator_column = rf_list_get_begin(table_data->columns);
				for(pos = 0; 1; pos++)
				{
					if(!rf_list_has_next(iterator_column))
					{
						rf_list_delete_iterator(iterator_column);
						rf_list_delete_iterator(names_h);
						rf_list_delete_iterator(names_v);
						rf_list_destroy(list_v, 0);
						rf_list_destroy(list_h, 0);

						error->string = rf_string_combine(3, "Column '", name_h, "' is missing in table");
						return 1;
					}

					tmp_name = rf_list_next(iterator_column);
					if(!tmp_name)
						continue;

					/* if found matching column, break */
					if(strcmp(tmp_name, name_h) == 0)
						break;
				}

				rf_list_delete_iterator(iterator_column);
				iterator_column = 0;

				/* read element from parsed input row */
				tmp_value = rf_list_get_by_position(table_row->values, pos);
				if(!tmp_value)
				{
					rf_list_delete_iterator(names_h);
					rf_list_delete_iterator(names_v);
					rf_list_destroy(list_v, 0);
					rf_list_destroy(list_h, 0);

					error->string = rf_string_combine(3, "Row '", name_v, "' is missing in table");
					return 1;
				}

				/* if the element is different from 0 and 1 then its not of domain boolean */
				if(strcmp(tmp_value, "0") && strcmp(tmp_value, "1"))
				{
					rf_list_delete_iterator(names_h);
					rf_list_delete_iterator(names_v);
					rf_list_destroy(list_v, 0);
					rf_list_destroy(list_h, 0);

					error->string = rf_string_combine(7, "Element '", name_v, " x ", name_h,
						" -> ", tmp_value, "' is not a element of domain 'boolean'");
					return 1;
				}

				/* write the element into the table of the relation*/
				if(strcmp(tmp_value, "0") == 0)
				{
					if(rf_table_set_bit(relation->table, column, row, 0) != 0)
					{
						rf_list_delete_iterator(names_h);
						rf_list_delete_iterator(names_v);
						rf_list_destroy(list_v, 0);
						rf_list_destroy(list_h, 0);

						error->string = rf_string_copy("program error - while writing bit to table");
						return 1;
					}
				}
				else
				{
					if(rf_table_set_bit(relation->table, column, row, 1) != 0)
					{
						rf_list_delete_iterator(names_h);
						rf_list_delete_iterator(names_v);
						rf_list_destroy(list_v, 0);
						rf_list_destroy(list_h, 0);

						error->string = rf_string_copy("program error - while writing bit to table");
						return 1;
					}
				}

				column++;
			}

			row++;
		}

		rf_list_destroy(list_h, 0);
		rf_list_destroy(list_v, 0);
		rf_list_delete_iterator(names_h);
		rf_list_delete_iterator(names_v);
	}
	else
	{
		error->string = rf_string_copy("program error - wrong type in tabledata");
		return 1;
	}

	return 0;
}


/*!
 Fills the table of RF_RELATION based on the formula that was retrieved from parsing the user input.

 A formula in the Relafix language is an expression that allows the placeholders 'X' and 'Y'. The expression
 was recorded and stored in struct RF_FORMULA. A call to rf_formula_calc() with 'X' and 'Y' will calculate
 the solution for the stored expression. To fill the table rf_formula_calc() will be called for
 every combination of 'X' and 'Y' where 'X' is element from domain_1 and 'Y' is from domain_2. The solution
 must be of domain boolean and will be saved in the table. Domains are given by the relation. The function will
 return a description of the following errors:
 - A solution has wrong type
 - A solution is not element of domain
 - rf_formula_calc() did fail.

 The function will not modify or destroy the formula.
 @relates RF_RELATION
 @param[in] relation The relation whoes table should be set.
 @param[in] formula The formula containing the expression used for calculating the solutions.
 @param[out] error error If the function fails an error description is written in error. See RF_ERROR. 0 is not allowed.
 @return 0 on success
 @return !0 on fail. A description is written to error.
 */
int rf_relation_build_table_by_formula(RF_RELATION *relation, RF_FORMULA *formula, RF_ERROR *error)
{
	RF_LIST *list_1, *list_2;
	RF_LIST_ITERATOR *names_1, *names_2;
	char *name_1, *name_2;
	RF_FORMULA_RESULT *result;
	int x, y, err;

	if(!relation)
	{
		error->string = rf_string_combine(1, "program error - no relation in table by formula");
		return 1;
	}
	if(!formula)
	{
		error->string = rf_string_combine(1, "program error - no formula in table by formula");
		return 1;
	}

	/* get lists filled with the names of a domains elements */
	list_1 = rf_domain_get_element_names(relation->domain_1);
	list_2 = rf_domain_get_element_names(relation->domain_2);
	names_1 = rf_list_get_begin(list_1);


	/*
	 For every combination of X and Y calculate the given formula and put the result
	 into the table. X represents is column and Y is line.
	 */
	for(y = 0; rf_list_has_next(names_1); y++)
	{
		/* get next element of domain 1 */
		name_1 = rf_list_next(names_1);

		names_2 = rf_list_get_begin(list_2);
		for(x = 0; rf_list_has_next(names_2); x++)
		{
			/* get next element of domain 2 */
			name_2 = rf_list_next(names_2);

			/* calculate the formula. On error return */
			err = rf_formula_calc(formula, name_1, name_2, &result);
			if(err)
			{
				rf_list_delete_iterator(names_1);
				rf_list_delete_iterator(names_2);
				rf_list_destroy(list_1, 0);
				rf_list_destroy(list_2, 0);

				/* pass the error description and position to the caller */
				error->string = result->error;
				error->location = result->location;
				result->can_delete = RF_FALSE;		/* this way the error string wont get freed */
				rf_formula_destroy_result(result);
				return 1;
			}

			/* check if the result is a element. If not return an error */
			if(result->type != RF_FO_ID && result->type != RF_FO_ELEMENT)
			{
				rf_formula_destroy_result(result);
				rf_list_delete_iterator(names_1);
				rf_list_delete_iterator(names_2);
				rf_list_destroy(list_1, 0);
				rf_list_destroy(list_2, 0);

				error->string = rf_string_copy("The result has the wrong type.");
				error->location = result->location;
				rf_formula_destroy_result(result);
				return 1;
			}

			/* check if the resulting element is of domain boolean. If not return an error. */
			if(strcmp(result->element, "0") == 0)
				rf_table_set_bit(relation->table, x, y, 0);
			else if(strcmp(result->element, "1") == 0)
				rf_table_set_bit(relation->table, x, y, 1);
			else
			{
				rf_formula_destroy_result(result);
				rf_list_delete_iterator(names_1);
				rf_list_delete_iterator(names_2);
				rf_list_destroy(list_1, 0);
				rf_list_destroy(list_2, 0);

				error->string = rf_string_combine(4, "'", result->element, "' is not a element"
					" of domain 'boolean'");
				error->location = result->location;
				rf_formula_destroy_result(result);
				return 1;
			}

			rf_formula_destroy_result(result);
		}
		rf_list_delete_iterator(names_2);
	}
	rf_list_delete_iterator(names_1);

	rf_list_destroy(list_1, 0);
	rf_list_destroy(list_2, 0);
	return 0;
}


/*!
 Fills the table of RF_OPERATION based on the data that was retrieved from parsing the user input.

 The function will check if the conditions of the parsed data will match with the operation.
 Checks are:
 - wrong horizontal element count
 - wrong vertikal element count
 - elements are in domain
 - elements are missing

 If one of these checks fail an error message is returned in RF_ERROR.
 @relates RF_OPERATION
 @param[in] operation The operation whoes table should be set.
 @param[in] table_data Contains the parsed table information.
 @param[out] error If the function fails an error description is written in error. See RF_ERROR. 0 is not allowed.
 @return 0 on success
 @return !0 on fail. A description is written to error.
 */
int rf_operation_build_table(RF_OPERATION *operation, RF_PARSER_TABLE_DATA *table_data, RF_ERROR *error)
{
	RF_LIST *list_h, *list_v, *names_d3_list = 0;
	RF_LIST_ITERATOR *names_h, *names_v, *iterator_row, *iterator_column = 0, *names_d3 = 0;
	RF_PARSER_TABLE_ROW *table_row;
	RF_BOOL is_element_of_d3;
	int row, column, pos;
	char *name_h, *name_v, *tmp_name, *tmp_value;

	if(!table_data || !operation)
	{
		error->string = rf_string_copy("program error - invalid input to operation_build_table");
		return 1;
	}

	if(table_data->type == RF_PTD_DATA)
	{
		/* check if table has the right width and height */
		if(rf_table_get_height(operation->table) != rf_list_get_count(table_data->rows))
		{
			error->string = rf_string_copy("wrong table height");
			return 1;
		}
		if(rf_table_get_width(operation->table) != rf_list_get_count(table_data->columns))
		{
			error->string = rf_string_copy("wrong table width");
			return 1;
		}



		/* Walk over the domain names to fill the table. For every row fill columns
		 * The inputdata is searched for the matching element names for every position in table
		 */
		list_h = rf_domain_get_element_names(operation->domain_2);
		list_v = rf_domain_get_element_names(operation->domain_1);
		names_h = 0;
		names_v = rf_list_get_begin(list_v);
		iterator_row = 0;
		row = column = 0;
		/* row from domain*/
		while(rf_list_has_next(names_v))
		{
			if(names_h)
				rf_list_delete_iterator(names_h);
			names_h = rf_list_get_begin(list_h);
			iterator_row = rf_list_get_begin(table_data->rows);
			name_v = rf_list_next(names_v);

			/* find matching row in parsed tabledata */
			while(1)
			{
				if(!rf_list_has_next(iterator_row))
				{
					rf_list_delete_iterator(iterator_row);
					rf_list_delete_iterator(names_h);
					rf_list_delete_iterator(names_v);
					rf_list_destroy(list_v, 0);
					rf_list_destroy(list_h, 0);

					error->string = rf_string_combine(3, "Row '", name_v, "' is missing in table");
					return 1;
				}

				table_row = rf_list_next(iterator_row);
				if(!table_row)
					continue;
				if(!table_row->element)
					continue;

				/* check if count of parsed row elements fit the domain element count */
				if(rf_list_get_count(table_row->values) < rf_list_get_count(list_h))
				{
					rf_list_delete_iterator(iterator_row);
					rf_list_delete_iterator(names_h);
					rf_list_delete_iterator(names_v);
					rf_list_destroy(list_v, 0);
					rf_list_destroy(list_h, 0);

					error->string = rf_string_combine(3, "Elements are missing in row '", name_v, "'");
					return 1;
				}
				if(rf_list_get_count(table_row->values) > rf_list_get_count(list_h))
				{
					rf_list_delete_iterator(iterator_row);
					rf_list_delete_iterator(names_h);
					rf_list_delete_iterator(names_v);
					rf_list_destroy(list_v, 0);
					rf_list_destroy(list_h, 0);

					error->string = rf_string_combine(3, "Too many elements in row '", name_v, "'");
					return 1;
				}

				/* if row of parsed tabledata matches the current domain row break search */
				if(strcmp(table_row->element, name_v) == 0)
					break;
			}

			rf_list_delete_iterator(iterator_row);
			iterator_row = 0;


			/* column from domain */
			column = 0;
			while(rf_list_has_next(names_h))
			{
				name_h = rf_list_next(names_h);
				if(iterator_column)
					rf_list_delete_iterator(iterator_column);

				/* search the matching column position in parsed data */
				iterator_column = rf_list_get_begin(table_data->columns);
				for(pos = 0; 1; pos++)
				{
					if(!rf_list_has_next(iterator_column))
					{
						rf_list_delete_iterator(iterator_column);
						rf_list_delete_iterator(names_h);
						rf_list_delete_iterator(names_v);
						rf_list_destroy(list_v, 0);
						rf_list_destroy(list_h, 0);

						error->string = rf_string_combine(3, "Column '", name_h, "' is missing in table");
						return 1;
					}

					tmp_name = rf_list_next(iterator_column);
					if(!tmp_name)
						continue;

					/* if found matching column, break */
					if(strcmp(tmp_name, name_h) == 0)
						break;
				}

				rf_list_delete_iterator(iterator_column);
				iterator_column = 0;

				/* read element from parsed input row */
				tmp_value = rf_list_get_by_position(table_row->values, pos);
				if(!tmp_value)
				{
					rf_list_delete_iterator(names_h);
					rf_list_delete_iterator(names_v);
					rf_list_destroy(list_v, 0);
					rf_list_destroy(list_h, 0);

					error->string = rf_string_combine(3, "Row '", name_v, "' is missing in table");
					return 1;
				}


				/* check if element is from resultdomain */
				is_element_of_d3 = RF_FALSE;
				names_d3_list = rf_domain_get_element_names(operation->domain_3);
				names_d3 = rf_list_get_begin(names_d3_list);
				while(rf_list_has_next(names_d3))
				{
					tmp_name = 0;
					tmp_name = rf_list_next(names_d3);
					if(strcmp(tmp_name, tmp_value) == 0)
					{
						is_element_of_d3 = RF_TRUE;
						break;
					}
				}
				rf_list_delete_iterator(names_d3);
				rf_list_destroy(names_d3_list, 0);

				if(is_element_of_d3 == RF_FALSE)
				{
					rf_list_delete_iterator(names_h);
					rf_list_delete_iterator(names_v);
					rf_list_destroy(list_v, 0);
					rf_list_destroy(list_h, 0);

					error->string = rf_string_combine(9, "Element '", name_v, " x ", name_h,
						" -> ", tmp_value, "' is not a element of domain '",
						rf_domain_get_name(operation->domain_3), "'");
					return 1;
				}


				/* write the element into the table of the relation*/
				if(rf_table_set_string(operation->table, column, row, tmp_name) != 0)
				{
					rf_list_delete_iterator(names_h);
					rf_list_delete_iterator(names_v);
					rf_list_destroy(list_v, 0);
					rf_list_destroy(list_h, 0);

					error->string = rf_string_copy("program error - while writing string to table");
					return 1;
				}


				column++;
			}

			row++;
		}

		rf_list_destroy(list_h, 0);
		rf_list_destroy(list_v, 0);
		rf_list_delete_iterator(names_h);
		rf_list_delete_iterator(names_v);
	}
	else
	{
		error->string = rf_string_copy("program error - wrong type in tabledata");
		return 1;
	}

	return 0;
}


/*!
 Fills the table of RF_OPERATION based on the formula that was retrieved from parsing the user input.

 A formula in the Relafix language is an expression that allows the placeholders 'X' and 'Y'. The expression
 was recorded and stored in struct RF_FORMULA. A call to rf_formula_calc() with 'X' and 'Y' will calculate
 the solution for the stored expression. To fill the table rf_formula_calc() will be called for
 every combination of 'X' and 'Y' where 'X' is element from domain_1 and 'Y' is from domain_2. The solution
 must be of domain_3 and will be saved in the table. Domains are given by the operation. The function will
 return a description of the following errors:
 - A solution has wrong type
 - A solution is not element of domain
 - rf_formula_calc() did fail.

 The function will not modify or destroy the formula.
 @relates RF_OPERATION
 @param[in] operation The operation whoes table should be set.
 @param[in] formula The formula containing the expression used for calculating the solutions.
 @param[out] error error If the function fails an error description is written in error. See RF_ERROR. 0 is not allowed.
 @return 0 on success
 @return !0 on fail. A description is written to error.
 */
int rf_operation_build_table_by_formula(RF_OPERATION *operation, RF_FORMULA *formula, RF_ERROR *error)
{
	RF_LIST *list_1, *list_2;
	RF_LIST_ITERATOR *names_1, *names_2;
	char *name_1, *name_2;
	RF_FORMULA_RESULT *result;
	RF_ELEMENT *element;
	int x, y, err;

	if(!operation)
	{
		error->string = rf_string_combine(1, "program error - no operation in table by formula");
		return 1;
	}
	if(!formula)
	{
		error->string = rf_string_combine(1, "program error - no formula in table by formula");
		return 1;
	}


	/* get lists filled with the names of a domains elements */
	list_1 = rf_domain_get_element_names(operation->domain_1);
	list_2 = rf_domain_get_element_names(operation->domain_2);
	names_1 = rf_list_get_begin(list_1);


	/*
	 For every combination of X and Y calculate the given formula and put the result
	 into the table. X represents is column and Y is line.
	 */
	for(y = 0; rf_list_has_next(names_1); y++)
	{
		/* get next element of domain 1 */
		name_1 = rf_list_next(names_1);

		names_2 = rf_list_get_begin(list_2);
		for(x = 0; rf_list_has_next(names_2); x++)
		{
			/* get next element of domain 2 */
			name_2 = rf_list_next(names_2);

			/* calculate the formula. On error return */
			err = rf_formula_calc(formula, name_1, name_2, &result);
			if(err)
			{
				rf_list_delete_iterator(names_1);
				rf_list_delete_iterator(names_2);
				rf_list_destroy(list_1, 0);
				rf_list_destroy(list_2, 0);

				/* pass the error description and position to the caller */
				error->string = result->error;
				error->location = result->location;
				result->can_delete = RF_FALSE;		/* this way the error string wont get freed */
				rf_formula_destroy_result(result);
				return 1;
			}

			/* check if the result is a element. If not return an error */
			if(result->type != RF_FO_ID && result->type != RF_FO_ELEMENT)
			{
				rf_formula_destroy_result(result);
				rf_list_delete_iterator(names_1);
				rf_list_delete_iterator(names_2);
				rf_list_destroy(list_1, 0);
				rf_list_destroy(list_2, 0);

				error->string = rf_string_copy("The result has the wrong type.");
				error->location = result->location;
				rf_formula_destroy_result(result);
				return 1;
			}

			/* check if the resulting element belongs to domain 3. If not return an error */
			if(!(element = rf_domain_get_element(operation->domain_3, result->element)))
			{
				rf_formula_destroy_result(result);
				rf_list_delete_iterator(names_1);
				rf_list_delete_iterator(names_2);
				rf_list_destroy(list_1, 0);
				rf_list_destroy(list_2, 0);

				error->string = rf_string_combine(5, "'", result->element, "' is not a element"
					" of domain '", rf_domain_get_name(operation->domain_3), "'");
				error->location = result->location;
				rf_formula_destroy_result(result);
				return 1;
			}

			/* Add the result to the table and destroy the result */
			rf_table_set_string(operation->table, x, y, rf_element_get_name(element));
			rf_formula_destroy_result(result);
		}
		rf_list_delete_iterator(names_2);
	}
	rf_list_delete_iterator(names_1);

	rf_list_destroy(list_1, 0);
	rf_list_destroy(list_2, 0);
	return 0;
}


/* see lexer.c */
