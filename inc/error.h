/*
 * RelaFixLib    Error
 *
 * Copyright (C) Peter Berger, Wilke Schwiedop 2011-2013
 */

/*! \file
 * \brief	Error handling structures
 */

#ifndef RF_ERROR_H
#define RF_ERROR_H

/*! \brief	Provides start and end position of textarea
 *
 * This structure is used in the library to remember the position of the
 * parsed token. If some error occures the library will use this structure in RF_ERROR
 * so that the position of the error can be transfered to the user.
 */
typedef struct rf_location
{
	int first_line;		/*!< \brief line where the textarea starts */
	int first_column;	/*!< \brief column where the textarea starts */
	int last_line;		/*!< \brief line where the textarea ends */
	int last_column;	/*!< \brief column where the thextarea ends */
} RF_LOCATION;


/*! \brief	Used to return message and location of an error
 *
 * A pointer to an instance of this structure should be given to a function that can
 * fail for many different reasons. If that function fails, it will fill out the structure.
 * The errormessage that is pointed to by 'string' must be on the heap and will be freed
 * outside of that function!
 */
typedef struct rf_error
{
	char		*string;	/*!< \brief Points to an errormessage on the heap. */
	RF_LOCATION	location;	/*!< \brief Textposition where the error occured */
} RF_ERROR;

#endif
