
/**
 * @file radix.h
 * @author Sebastien Galvagno
 * @brief Radix tree
 * @version 0.1
 * @date 2022-04-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __SG__CHIMERE_RADIX_H__
#define __SG__CHIMERE_RADIX_H__


#define RADIXBASE 16

typedef struct node {
    char * key;
    struct node* children[RADIXBASE];
    void* data;
} node ;

/**
 * @brief insert a key in a radix tree - generating a new node or return the existing
 * 
 * @param root the root tree
 * @param key the key to insert in the radix
 * @return node* the leaf that correspond to the key
 */
node* insert(node * root, char * key);

// print the radix tree
void printRadix(node* root);

#endif
;
