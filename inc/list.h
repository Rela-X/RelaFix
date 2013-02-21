/*
 * RelaFixLib    List
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 A dynamic double linked list.

 Declares functions to work on struct RF_LIST. The list is build by nodes RF_LIST_ITEM and can
 be traversed by the use of iterators (RF_LIST_ITERATOR). Use the list if you need often to add
 and delete items from it.
 */

#ifndef RF_LIST_H
#define RF_LIST_H

#include <stdbool.h>

/*!
The item that builds the linked list.
*/
typedef struct rf_list_item
{
	struct rf_list_item     *prev; /*!< The previous item. 0 if there is no previous item. */
	struct rf_list_item     *next; /*!< The next item. 0 if there is no next item. */
	void                    *data; /*!< A pointer to the data saved at this node. */
} RF_LIST_ITEM;

/*!
A dynamic double linked list.

Declares functions to work on struct RF_LIST. The list is build by nodes RF_LIST_ITEM and can
be traversed by the use of iterators (RF_LIST_ITERATOR). Use the list if you need often to add
and delete items from it.
*/
typedef struct rf_list
{
	int             count; /*!< The number of elements in the list. */
	RF_LIST_ITEM    *first; /*!< The first element in the list. */
	RF_LIST_ITEM    *last; /*!< The last element in the list. */
} RF_LIST;

/*!
The iterator that can be used to traverse a list.
*/
typedef struct rf_list_iterator
{
	RF_LIST         *list; /*!< The list the iterator works on. */
	RF_LIST_ITEM    *prev; /*!< The next item. 0 if there is no next item. */
	RF_LIST_ITEM    *mid; /*!< The item that the iterator points on. 0 if at the begining or end.*/
	RF_LIST_ITEM    *next; /*!< The next item. 0 if there is no next item. */
} RF_LIST_ITERATOR;


/*!
Add an new item to the end of the list.

@relates RF_LIST
@param[in] list The list at which the item should be appended.
@param[in] data A pointer to the data that will be appended.
*/
void               rf_list_append(RF_LIST *list, void *data);

/*!
Copys the iterator.

@relates RF_LIST
@param[in] iterator The iterator to be copied.
@return The copied iterator.
@return 0 on error.
*/
RF_LIST_ITERATOR * rf_list_copy_iterator(RF_LIST_ITERATOR *iterator);

/*!
Creates a new list.

@relates RF_LIST
@return A new empty list.
@return 0 on error.
*/
RF_LIST *          rf_list_create();

/*!
Deletes the item the iterator is pointing to.

@relates RF_LIST
@param[in] iterator The iterator pointing to the item that should be deleted.
@param[in] destroy A pointer to a function that is called to destroy the data in the item. If 0 the function is not called.
@return The data in the item.
@return 0 on error.
*/
void *             rf_list_delete_item(RF_LIST_ITERATOR *iterator, void (*)(void *data));

/*!
Frees the iterator.

The item the iterator points to is not effected.
@relates RF_LIST
@param[in] iterator The iterator to be deleted.
*/
void               rf_list_delete_iterator(RF_LIST_ITERATOR *iterator);

/*!
Frees the list and all its items.

@relates RF_LIST
@param[in] list The list to be destroyed.
@param[in] destroy A function that is called for every items data. If 0 no function is called.
*/
void               rf_list_destroy(RF_LIST *list, void (*)(void *data));

/*!
Get an iterator that points to the beginning of the list.

@relates RF_LIST
@param[in] list The list whoes beginning should be deliverd.
@return An iterator pointing to the beginning.
@return 0 on error.
*/
RF_LIST_ITERATOR * rf_list_get_begin(RF_LIST *list);

/*!
Get the data of an item by its position.

This is slow! Better use an array if you need to do a lot of jumping readins.
@relates RF_LIST
@param[in] list The list containing the data.
@param position The position of the item. The list starts with item 0.
@return The data at position.
@return 0 on error.
*/
void *             rf_list_get_by_position(RF_LIST *list, int position);

/*!
Return the number of items in the list.

@relates RF_LIST
@param[in] The list whoes count is in question.
@return The number of items in the list.
@return -1 on error.
*/
int                rf_list_get_count(RF_LIST *list);

/*!
Get an iterator that points to the end of the list.

@relates RF_LIST
@param[in] list The list whoes end should be deliverd.
@return An iterator pointing to the end.
@return 0 on error.
*/
RF_LIST_ITERATOR * rf_list_get_end(RF_LIST *list);

/*!
Tests if there is an item following the given iterator.

@relates RF_LIST
@param[in] The iterator pointing to an item.
@return true if there is a following item.
@return false if not or error.
*/
bool               rf_list_has_next(RF_LIST_ITERATOR *iterator);

/*!
Tests if there is an item before the given iterator.

@relates RF_LIST
@param[in] The iterator pointing to an item.
@return true if there is a previous item.
@return false if not or error.
*/
bool               rf_list_has_prev(RF_LIST_ITERATOR *iterator);

/*!
Merges the second list into the first one.

The second list will remain empty.
@relates RF_LIST
@param[in, out] The list that will contain all elements after the call.
@param[in, out] This list will be appended into the first one. After the call this list will be empty.
*/
void               rf_list_merge(RF_LIST *list_1, RF_LIST *list_2);

/*!
Moves the iterator to the next item and returns its data.

Test with rf_list_has_next() before using this function.
@relates RF_LIST
@param[in] iterator An iterator pointing to an item.
@return The data of the item the iterator is moved to.
@return 0 on error.
*/
void *             rf_list_next(RF_LIST_ITERATOR *iterator);

/*!
Moves the iterator to the previous item and returns its data.

Test with rf_list_has_prev() before using this function.
@relates RF_LIST
@param[in] iterator An iterator pointing to an item.
@return The data of the item the iterator is moved to.
@return 0 on error.
*/
void *             rf_list_prev(RF_LIST_ITERATOR *iterator);

/*!
Set the data of an item by its position.

This is slow! Better use an array if using a lot of random access.
@relates RF_LIST
@param[in] list The list that contains the items.
@param position The position of the item whoes data should be set. The list starts with item 0.
@param[in] data The data to be set.
@return 0 on success.
@return >0 on error.
*/
int                rf_list_set_by_position(RF_LIST *list, int position, void *data);

#endif
