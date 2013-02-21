/*
 * RelaFix       Function
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 * Declares structure and functions to handle systemfunctions of the relafix language
 */

#ifndef RF_FUNCTION_H
#define RF_FUNCTION_H

#include <stdbool.h>

/*! Represents a systemfunction of the relafix language.

 The language of Relafix has some functions that can be used in the expressions. These
 functions will be represented with this structure in the relafix library.
 */
typedef struct rf_function
{
	char  *name;			/*!< Name of the function */
	int   argument_count;	/*!< The number of arguments the equivalent Relafix language function will take */
	void  *function;		/*!< Points to the C function */
	char  *description;	/*!< Description of the function. Will not be freed by rf_function_destroy()! */
} RF_FUNCTION;

/*!
 @relates RF_FUNCTION
 @param name Name of the new function. Pointer to heap. The name gets invalid for the user! If 0
        the new function wont have a name.
 @param fpointer Pointer to a function. Should be the function that represents the equivalent Relafix language function.
 @param argc The number of arguments the function takes in the Relafix language.
 @return New function. Must be destroyed with rf_function_destroy() if not needed anymore.
 @return 0 on error.
 */
RF_FUNCTION * rf_function_create(char *name, void *function, int argc);

/*! Frees all included data except the description!

 @relates RF_FUNCTION
 @param function The function to be destroyed.
 */
void rf_function_destroy(RF_FUNCTION *function);

/*! Return the number of arguments the function takes in the Relafix language.

 @relates RF_FUNCTION
 @param function The function whoms argument count should be retrieved.
 @return The number of arguments the function takes in the Relafix language.
 @return is < 0 on error.
 */
int rf_function_get_argument_count(RF_FUNCTION *function);

/*!
 @relates RF_FUNCTION
 @param function The function whoms description should be retrieved.
 @return The description. The description will also be used by the struct! But the description
         will not be deleted by rf_function_destroy().
 @return 0 on error or if no description exists.
 */
char * rf_function_get_description(RF_FUNCTION *function);

/*!
 @relates RF_FUNCTION
 @param function The function whoms func should be retrieved.
 @return The function that represents the equivalent Relafix language function.
 @return 0 on error
 */
void * rf_function_get_func(RF_FUNCTION *function);

/*!
 @relates RF_FUNCTION
 @param function The function whoes name should be retrieved.
 @return The name of the function. Should not be changed or freed by the user.
 @return 0 if no name exists or on error
 */
char * rf_function_get_name(RF_FUNCTION *function);

/*!
 @relates RF_FUNCTION
 @param function The function whose name should be compared.
 @param name The name in question.
 @return true if name is the name of the function.
 @return false if not or error.
 */
bool rf_function_has_name(RF_FUNCTION *function, char *name);

/*!
 @relates RF_FUNCTION
 @param function The function whose description is to be set.
 @param description Will be used by the RF_FUNCTION, but not freed with rf_function_destroy(). If 0 then
        the function will not have a description.
 */
void rf_function_set_description(RF_FUNCTION *function, char *description);

#endif
