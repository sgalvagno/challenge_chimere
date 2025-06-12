/**
 * @file list.h
 * @author Sebastien Galvagno
 * @brief Doubly linked list
 * @version 0.1
 * @date 2022-04-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __SG__CHIMERE_LIST_H__
#define __SG__CHIMERE_LIST_H__

typedef struct list {
    struct list* prev;
    struct list* next;
    void * data;
} list;

/**
 * @brief generate and insert a new node in the list
 * 
 * @param node the first node of the list
 * @param data the data to insert in the list
 * @return list* 
 */
list* insertlist(list* node, void* data);

/**
 * @brief move a node to the right position. Permit to sort the list at each update of the list
 * 
 * @param start the first node of the liste
 * @param node the node to move
 * @param compare the function pointer to compare the node
 * @return list* the first node of the list
 */
list * moveNode(list* start, list* node, int(*compare)(list*,list*));

/**
 * @brief Print the list
 * 
 * @param node the first node of the list
 * @param fn a function to print the data
 */
void printList(list* node, void(*fn)(void*) );

#endif
;
