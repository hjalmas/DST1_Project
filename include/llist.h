/*
 * llist.h
 *
 *  Created on: Nov 17, 2016
 *      Author: root
 */

#ifndef INCLUDE_LLIST_H_
#define INCLUDE_LLIST_H_

/**
 * ---------------------------------------------------------------------------------------
 * 										INCLUDES
 * ---------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "diag/Trace.h"
#include "cmsis_device.h"
#include "sam3x_conf.h"

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

typedef struct NODE {
	void* item;
	struct NODE* next;
} node_t;

typedef struct LLIST {
	uint32_t size;
	node_t* next;
} llist_t;

/**
 * ---------------------------------------------------------------------------------------
 * 										PROTOTYPES
 * ---------------------------------------------------------------------------------------
 */

/**
 * Creates a linked list.
 */
llist_t* llist_create(void);

/**
 * Adds an item to the list.
 * Returns true if the item was successfully added.
 */
bool llist_add(llist_t* list, void* item);

/**
 * Deletes the specified item from the list.
 */
bool llist_delete(llist_t* list, uint32_t idx);

/**
 * Prints the contents of this list to standard output.
 */
void llist_print(llist_t* list);

#endif /* INCLUDE_LLIST_H_ */
