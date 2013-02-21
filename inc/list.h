/*
 * RelaFixLib    List
 *
 * Copyright (C) Peter Berger, 2011
 */

/*! \file
 \brief A dynamic double linked list.

 Declares functions to work on struct RF_LIST. The list is build by nodes RF_LIST_ITEM and can
 be traversed by the use of iterators (RF_LIST_ITERATOR). Use the list if you need often to add
 and delete items from it.
 */

#ifndef RF_LIST_H
#define RF_LIST_H

#include <stdbool.h>

/*!
\brief The item that builds the linked list.
*/
typedef struct rf_list_item
{
	struct rf_list_item     *prev; /*!< \brief The previous item. 0 if there is no previous item. */
	struct rf_list_item     *next; /*!< \brief The next item. 0 if there is no next item. */
	void                    *data; /*!< \brief A pointer to the data saved at this node. */
} RF_LIST_ITEM;

/*!
\brief A dynamic double linked list.

Declares functions to work on struct RF_LIST. The list is build by nodes RF_LIST_ITEM and can
be traversed by the use of iterators (RF_LIST_ITERATOR). Use the list if you need often to add
and delete items from it.
*/
typedef struct rf_list
{
	int             count; /*!< \brief The number of elements in the list. */
	RF_LIST_ITEM    *first; /*!< \brief The first element in the list. */
	RF_LIST_ITEM    *last; /*!< \brief The last element in the list. */
} RF_LIST;

/*!
\brief The iterator that can be used to traverse a list.
*/
typedef struct rf_list_iterator
{
	RF_LIST         *list; /*!< \brief The list the iterator works on. */
	RF_LIST_ITEM    *prev; /*!< \brief The next item. 0 if there is no next item. */
	RF_LIST_ITEM    *mid; /*!< \brief The item that the iterator points on. 0 if at the begining or end.*/
	RF_LIST_ITEM    *next; /*!< \brief The next item. 0 if there is no next item. */
} RF_LIST_ITERATOR;


void               rf_list_append(RF_LIST *list, void *data);
RF_LIST_ITERATOR * rf_list_copy_iterator(RF_LIST_ITERATOR *iterator);
RF_LIST *          rf_list_create();
void *             rf_list_delete_item(RF_LIST_ITERATOR *iterator, void (*)(void *data));
void               rf_list_delete_iterator(RF_LIST_ITERATOR *iterator);
void               rf_list_destroy(RF_LIST *list, void (*)(void *data));
RF_LIST_ITERATOR * rf_list_get_begin(RF_LIST *list);
void *             rf_list_get_by_position(RF_LIST *list, int position);
int                rf_list_get_count(RF_LIST *list);
RF_LIST_ITERATOR * rf_list_get_end(RF_LIST *list);
bool               rf_list_has_next(RF_LIST_ITERATOR *iterator);
bool               rf_list_has_prev(RF_LIST_ITERATOR *iterator);
void               rf_list_merge(RF_LIST *list_1, RF_LIST *list_2);
void *             rf_list_next(RF_LIST_ITERATOR *iterator);
void *             rf_list_prev(RF_LIST_ITERATOR *iterator);
int                rf_list_set_by_position(RF_LIST *list, int position, void *data);

#endif
