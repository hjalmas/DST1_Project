/*
 * llist.c
 *
 *  Created on: Nov 17, 2016
 *      Author: root
 */

/**
 * ---------------------------------------------------------------------------------------
 * 										INCLUDES
 * ---------------------------------------------------------------------------------------
 */
#include <llist.h>
/**
 * ---------------------------------------------------------------------------------------
 * 										DEFINES
 * ---------------------------------------------------------------------------------------
 */

/**
 * ---------------------------------------------------------------------------------------
 * 										VARIABLES
 * ---------------------------------------------------------------------------------------
 */

/**
 * ---------------------------------------------------------------------------------------
 * 										PROTOTYPES
 * ---------------------------------------------------------------------------------------
 */

/**
 * ---------------------------------------------------------------------------------------
 * 										DECLARATIONS
 * ---------------------------------------------------------------------------------------
 */

/**
 * Creates a linked list.
 */
llist_t* llist_create(void) {
	llist_t* result;
	result = malloc(sizeof(llist_t));
	result->next = malloc(sizeof(node_t));
	result->next->next = NULL;
	result->size = 0;
	return result;
}

/**
 * Adds an item to the list.
 * Returns true if the item was successfully added.
 */
bool llist_add(llist_t* list, void* item) {
	if(list != NULL) {
		node_t* tmp = list->next;

		while(tmp->next != NULL) {
			tmp = tmp->next;
		}

		tmp->item = item;

		tmp->next = malloc(sizeof(node_t));
		if(tmp->next == NULL) {
			return false;
		}

		tmp->next->next = NULL;
		list->size++;

		return true;
	}

	return false;
}

/**
 * Deletes the specified item from the list.
 */
bool llist_delete(llist_t* list, uint32_t idx) {

	if(list != NULL) {
		node_t* tmp = (node_t*) list;
		uint32_t i = 0;
		for(; i<idx && tmp->next != NULL; i++, tmp = tmp->next);

		if(i == idx) {
			node_t* tmp2 = tmp->next->next;
			free(tmp->next);
			tmp->next = tmp2;
			list->size--;
			return true;
		}
	}
	return false;
}

/**
 * Prints the contents of this list to standard output.
 */
void llist_print(llist_t* list);
