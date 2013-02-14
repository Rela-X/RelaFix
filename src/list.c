/*
 * RelaFixLib    List
 *
 * Copyright (C) Peter Berger, 2011
 */

#include "list.h"
#include <stdlib.h>

/* Full definition of the structs */



/*!
Add an new item to the end of the list.

@relates RF_LIST
@param[in] list The list at which the item should be appended.
@param[in] data A pointer to the data that will be appended.
*/
void rf_list_append(RF_LIST *list, void *data)
{
	RF_LIST_ITEM *item;

	if(!data || !list)
		return;

	/* get memeory for new item */
	item = malloc(sizeof(RF_LIST_ITEM));
	if(!item) /* error */
		return;

	/* init data */
	item->data = data;
	item->prev = item->next = 0;

	/* append item */
	if(!list->last)
	{
		list->first = list->last = item;
	}
	else
	{
		list->last->next = item;
		item->prev = list->last;
		list->last = item;
	}

	list->count++;
}

/*!
Copys the iterator.

@relates RF_LIST
@param[in] iterator The iterator to be copied.
@return The copied iterator.
@return 0 on error.
*/
RF_LIST_ITERATOR * rf_list_copy_iterator(RF_LIST_ITERATOR *iterator)
{
	RF_LIST_ITERATOR *iterator_new = 0;
	
	if(!iterator)
		return 0;
	
	iterator_new = malloc(sizeof(RF_LIST_ITERATOR));
	if(!iterator_new)
		return 0;
	
	*iterator_new = *iterator;
	return iterator_new;
}

/*!
Creates a new list.

@relates RF_LIST
@return A new empty list.
@return 0 on error.
*/
RF_LIST * rf_list_create()
{
	RF_LIST *list = malloc(sizeof(RF_LIST));
	if(!list)
		return 0;

	list->count = 0;
	list->first = list->last = 0;

	return list;
}

/*!
Deletes the item the iterator is pointing to.

@relates RF_LIST
@param[in] iterator The iterator pointing to the item that should be deleted.
@param[in] destroy A pointer to a function that is called to destroy the data in the item. If 0 the function is not called.
@return The data in the item.
@return 0 on error.
*/
void * rf_list_delete_item(RF_LIST_ITERATOR *iterator, void (*destroy)(void *data))
{
	void * tmpData;

	if(!iterator)
		return 0;
	if(!iterator->mid || !iterator->list)
		return 0;

	tmpData = iterator->mid->data;

	if(iterator->prev)
		iterator->prev->next = iterator->next;
	else
		iterator->list->first = iterator->next;

	if(iterator->next)
		iterator->next->prev = iterator->prev;
	else
		iterator->list->last = iterator->prev;

	free(iterator->mid);
	iterator->mid = 0;
	iterator->list->count--;

	if(destroy)
	{
		destroy(tmpData);
		return 0;
	}
	else
		return tmpData;
}

/*!
Frees the iterator.

The item the iterator points to is not effected.
@relates RF_LIST
@param[in] iterator The iterator to be deleted.
*/
void rf_list_delete_iterator(RF_LIST_ITERATOR *iterator)
{
	free(iterator);
}

/*!
Frees the list and all its items.

@relates RF_LIST
@param[in] list The list to be destroyed.
@param[in] destroy A function that is called for every items data. If 0 no function is called.
*/
void rf_list_destroy(RF_LIST *list, void (*destroy)(void *data))
{
	RF_LIST_ITERATOR *iterator;

	if(!list)
		return;

	iterator = rf_list_get_begin(list);

	while(rf_list_has_next(iterator))
	{
		rf_list_next(iterator);
		rf_list_delete_item(iterator, destroy);
	}

	rf_list_delete_iterator(iterator);
	free(list);
}

/*!
Get an iterator that points to the beginning of the list.

@relates RF_LIST
@param[in] list The list whoes beginning should be deliverd.
@return An iterator pointing to the beginning.
@return 0 on error.
*/
RF_LIST_ITERATOR * rf_list_get_begin(RF_LIST *list)
{
	RF_LIST_ITERATOR *iterator = malloc(sizeof(RF_LIST_ITERATOR));

	if(!iterator || !list)
		return 0;

	iterator->list = list;
	iterator->prev = 0;
	iterator->mid = 0;
	iterator->next = list->first;

	return iterator;
}

/*!
Get the data of an item by its position.

This is slow! Better use an array if you need to do a lot of jumping readins.
@relates RF_LIST
@param[in] list The list containing the data.
@param position The position of the item. The list starts with item 0.
@return The data at position.
@return 0 on error.
*/
void * rf_list_get_by_position(RF_LIST *list, int position)
{
	int i;
	RF_LIST_ITEM *tmp_item;

	if(!list)
		return 0;

	if(position < 0 || position >= list->count)
		return 0;

	for(i = 0, tmp_item = list->first; i < position; i++, tmp_item = tmp_item->next)
	{
		if(!tmp_item)
			return 0;
	}

	if(!tmp_item)
		return 0;

	return tmp_item->data;
}

/*!
Get an iterator that points to the end of the list.

@relates RF_LIST
@param[in] list The list whoes end should be deliverd.
@return An iterator pointing to the end.
@return 0 on error.
*/
RF_LIST_ITERATOR * rf_list_get_end(RF_LIST *list)
{
	RF_LIST_ITERATOR *iterator = malloc(sizeof(RF_LIST_ITERATOR));

	if(!iterator || !list)
		return 0;

	iterator->list = list;
	iterator->prev = list->last;
	iterator->mid = 0;
	iterator->next = 0;

	return iterator;
}

/*!
Tests if there is an item following the given iterator.

@relates RF_LIST
@param[in] The iterator pointing to an item.
@return RF_TRUE if there is a following item.
@return RF_FALSE if not or error.
*/
RF_BOOL rf_list_has_next(RF_LIST_ITERATOR *iterator)
{
	if(!iterator)
		return RF_FALSE;

	if(iterator->next)
		return RF_TRUE;
	else
		return RF_FALSE;
}

/*!
Tests if there is an item before the given iterator.

@relates RF_LIST
@param[in] The iterator pointing to an item.
@return RF_TRUE if there is a previous item.
@return RF_FALSE if not or error.
*/
RF_BOOL rf_list_has_prev(RF_LIST_ITERATOR *iterator)
{
	if(!iterator)
		return RF_FALSE;

	if(iterator->prev)
		return RF_TRUE;
	else
		return RF_FALSE;
}

/*!
Merges the second list into the first one.

The second list will remain empty.
@relates RF_LIST
@param[in, out] The list that will contain all elements after the call.
@param[in, out] This list will be appended into the first one. After the call this list will be empty.
*/
void rf_list_merge(RF_LIST *list_1, RF_LIST *list_2)
{
	if(!list_1 || !list_2)
		return;

	if(list_2->count == 0)
		return;

	if(list_1->count != 0)
	{
		list_1->last->next = list_2->first;
		list_2->first->prev = list_1->last;
	}
	else
	{
		list_1->first = list_2->first;
	}

	list_1->last = list_2->last;
	list_2->first = 0;
	list_2->last = 0;

	list_1->count += list_2->count;
	list_2->count = 0;
}

/*!
Moves the iterator to the next item and returns its data.

Test with rf_list_has_next() before using this function.
@relates RF_LIST
@param[in] iterator An iterator pointing to an item.
@return The data of the item the iterator is moved to.
@return 0 on error.
*/
void * rf_list_next(RF_LIST_ITERATOR *iterator)
{
	if(!iterator)
		return 0;
	if(!iterator->next)
		return 0;

	iterator->prev = iterator->mid;
	iterator->mid = iterator->next;
	iterator->next = iterator->mid->next;

	return iterator->mid->data;
}

/*!
Moves the iterator to the previous item and returns its data.

Test with rf_list_has_prev() before using this function.
@relates RF_LIST
@param[in] iterator An iterator pointing to an item.
@return The data of the item the iterator is moved to.
@return 0 on error.
*/
void * rf_list_prev(RF_LIST_ITERATOR *iterator)
{
	if(!iterator)
		return 0;
	if(!iterator->prev)
		return 0;

	iterator->next = iterator->mid;
	iterator->mid = iterator->prev;
	iterator->prev = iterator->mid->prev;

	return iterator->mid->data;
}

/*!
Return the number of items in the list.

@relates RF_LIST
@param[in] The list whoes count is in question.
@return The number of items in the list.
@return -1 on error.
*/
int rf_list_get_count(RF_LIST *list)
{
	if(!list)
		return -1;

	return list->count;
}

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
int rf_list_set_by_position(RF_LIST *list, int position, void *data)
{
	int i;
	RF_LIST_ITEM *tmp_item;
	
	if(!list)
		return 1;
	
	if(position < 0 || position >= list->count)
		return 2;

	for(i = 0, tmp_item = list->first; i < position; i++, tmp_item = tmp_item->next)
	{
		if(!tmp_item)
			return 3;
	}

	if(!tmp_item)
		return 3;

	tmp_item->data = data;
	return 0;
}

