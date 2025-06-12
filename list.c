/**
 * @file list.c
 * @author Sebastien Galvagno
 * @brief Doubly linked list
 * @version 0.1
 * @date 2022-04-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef __UNITTEST_LIST__
# include <assert.h>
#endif

#include "list.h"

/**
 * @brief to generate a list node
 * 
 * @param data 
 * @return list* 
 */
list* newNodeList(void* data){
    list* l = (list*)malloc(sizeof(list));
    if ( l == NULL ) return NULL;

    l->prev = l->next = NULL;
    l->data = data;
    
    return l;
}

void freeList(list* node){
    list *next, *n = node;
    while ( n ){
        next = n->next;
        free(n);
        n = next;
    }
}


/**
 * @brief generate and insert a new node in the list
 * 
 * @param node the first node of the list
 * @param data the data to insert in the list
 * @return list* 
 */
list* insertlist(list* node, void* data){
    list* l = newNodeList(data);
    if ( node == NULL) return l;
    if ( l == NULL ) return NULL;

    // node is the first list node
    if (node->prev == NULL){
        node->prev = l;
        l->next = node ;
    } else { // insert at the beginning of the list
        node->prev->next = l;
        l->prev = node->prev;
        node->prev = l;
    }
    return l;
}

/**
 * @brief move a node to the right position. Permit to sort the list at each update of the list
 * 
 * @param start the first node of the liste
 * @param node the node to move
 * @param compare the function pointer to compare the node
 * @return list* the first node of the list
 */
list * moveNode(list* start, list* node, int(*compare)(list*,list*)){
    list* current = start;
    if ( node->next ){
        int c = compare(node, node->next);
        if (c <= 0) { 
            return start; 
        }

        current = node->next;
        list* next = node->next->next;
        while ( next && compare(node, next) > 0 ){
            current = next;
            next = next->next;
        }

        // following node
        // prev -> node -> current -> next
        // prev -> current -> node -> next
        if ( node->next == current ){
            list* next = current->next;
            list* prev = node->prev;

            if ( prev ) prev->next = current;
            current->prev = prev;
            current->next = node;
            node->prev = current;
            node->next = next;
            if ( next ) next->prev = node;
            if ( node == start ) { return current; }
        } else {
            // 1st point: remove node from its position
            if ( node == start) {
                start = node->next;
                start->prev = NULL;
            } else {
                if (node->prev) node->prev->next = node->next;
                node->next->prev = node->prev;
            }
            // 2nd point: insert node after current
            node->prev = current;
            node->next = current->next;
            current->next = node;
            if (node->next) node->next->prev = node;
        }
    }
    return start;
}

/**
 * @brief Print the list
 * 
 * @param node the first node of the list
 * @param fn a function to print the data
 */
void printList(list* node, void(*fn)(void*) ){
    if (node == NULL) return;
    fn(node->data);
    printList(node->next, fn);
}



#ifdef __UNITTEST_LIST__

void affiche(void* data){
    printf("%u\n",(int)data);
}


int comparePacket(list* node1, list* node2){
    int compare = (int)node1->data - (int)node2->data;
    printf("compare %u et %u = %d\n", (int)node1->data, (int)node2->data, compare );
    return compare ;
}


void test_moveforward(){
    // 4,1,2,3,5
    list* start = insertlist(NULL,(void*)5);
    start = insertlist(start,(void*)3);
    start = insertlist(start,(void*)2);
    start = insertlist(start,(void*)1);
    list* four = start = insertlist(start,(void*)4);
    
    printList(start, &affiche);
    start = moveNode(start, four, &comparePacket);
    printList(start, &affiche);
    
    assert((int)start->data == 1);
    assert((int)start->next->data == 2);
    assert((int)start->next->next->data == 3);
    assert((int)start->next->next->next->data == 4);
    assert((int)start->next->next->next->next->data == 5);
}

void test_moveforwardFirst(){
    // 2,1,3,4,5
    list* start = insertlist(NULL,(void*)5);
    start = insertlist(start,(void*)4);
    start = insertlist(start,(void*)3);
    start = insertlist(start,(void*)1);
    list* four = start = insertlist(start,(void*)2);
    
    printList(start, &affiche);
    start = moveNode(start, four, &comparePacket);
    printList(start, &affiche);
    
    assert((int)start->data == 1);
    assert((int)start->next->data == 2);
    assert((int)start->next->next->data == 3);
    assert((int)start->next->next->next->data == 4);
    assert((int)start->next->next->next->next->data == 5);
}

void test_moveforwardLast(){
    // 5,1,2,3,4
    list* start = insertlist(NULL,(void*)4);
    start = insertlist(start,(void*)3);
    start = insertlist(start,(void*)2);
    start = insertlist(start,(void*)1);
    list* four = start = insertlist(start,(void*)5);
    
    printList(start, &affiche);
    start = moveNode(start, four, &comparePacket);
    printList(start, &affiche);
    
    assert((int)start->data == 1);
    assert((int)start->next->data == 2);
    assert((int)start->next->next->data == 3);
    assert((int)start->next->next->next->data == 4);
    assert((int)start->next->next->next->next->data == 5);
}

void test_move1node(){
    printf("-------------test_move1node\n");
    list* start = insertlist(NULL,(void*)4);
    start = moveNode(start, start, &comparePacket);
    printList(start, &affiche);
    assert((int)start->data == 4);
    assert(start->prev == NULL);
    assert(start->next == NULL);
}

void test_samevalue(){
    printf("-------------test_samevalue\n");

    list* start = insertlist(NULL,(void*)0);
    start = insertlist(start,(void*)0);
    start = insertlist(start,(void*)0);
    list* four = start = insertlist(start,(void*)4);

    printList(start, &affiche);
    printf("----------\n");
    start = moveNode(start, start, &comparePacket);

    printList(start, &affiche);
    assert(start->prev == NULL);
    assert((int)start->data == 0);
    assert((int)start->next->data == 0);
    assert((int)start->next->next->data == 0);
    assert((int)start->next->next->next->data == 4);
    assert(start->next->next->next->next == NULL);

}



void test_move2node(){
    printf("-------------test_move2node\n");
    list *start, *one, *three;
    start = one = insertlist(NULL,(void*)1);
    start = three = insertlist(start,(void*)3);
    printList(start, &affiche);
    start = moveNode(start, start, &comparePacket);
    printList(start, &affiche);
    assert(start == one);
    assert((int)start->data == 1);
    assert(start->prev == NULL);
    assert(start->next != NULL);
    assert(start->next == three);
    assert((int)start->next->data == 3);
    assert(three->prev == one);
    assert(three->next == NULL);
}

void test_medium(){

    printf("-------------test_medium\n");

    list* start = insertlist(NULL,(void*)0);
    list* four = start = insertlist(start,(void*)4);
    start = insertlist(start,(void*)0);
    start = insertlist(start,(void*)0);

    printList(start, &affiche);
    printf("----------\n");
    start = moveNode(start, four, &comparePacket);

    printList(start, &affiche);
    assert(start->prev == NULL);
    assert((int)start->data == 0);
    assert((int)start->next->data == 0);
    assert((int)start->next->next->data == 0);
    assert((int)start->next->next->next->data == 4);
    assert(start->next->next->next->next == NULL);

}

int main (){

    test_moveforward();
    test_move1node();
    test_move2node();
    test_moveforwardFirst();
    test_moveforwardLast();
    test_samevalue();
    test_medium();
    
    return 0;
}

// gcc -o list list.c -g -D__UNITTEST_LIST__ -D__UNITTEST__ && ./list
// -D__UNITTEST_LIST__ -D__UNITTEST__ -Wno-pointer-to-int-cast

#endif
