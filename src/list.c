/*
 * RelaFixLib    List
 *
 * Copyright (C) Peter Berger, 2011
 */

#include <stdlib.h>

#include "list.h"

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

RF_LIST * rf_list_create()
{
	RF_LIST *list = malloc(sizeof(RF_LIST));
	if(!list)
		return 0;

	list->count = 0;
	list->first = list->last = 0;

	return list;
}

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

void rf_list_delete_iterator(RF_LIST_ITERATOR *iterator)
{
	free(iterator);
}

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

bool rf_list_has_next(RF_LIST_ITERATOR *iterator)
{
	if(!iterator)
		return false;

	if(iterator->next)
		return true;
	else
		return false;
}

bool rf_list_has_prev(RF_LIST_ITERATOR *iterator)
{
	if(!iterator)
		return false;

	if(iterator->prev)
		return true;
	else
		return false;
}

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

int rf_list_get_count(RF_LIST *list)
{
	if(!list)
		return -1;

	return list->count;
}

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
