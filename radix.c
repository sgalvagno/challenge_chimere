/**
 * @file radix.c
 * @author Sebastien Galvagno
 * @brief Radix tree
 * @version 0.1
 * @date 2022-04-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __UNITTEST_RADIX__
#include <assert.h>
#endif

#include "radix.h"
#include "packet.h"
#include "list.h"



 /**
  * @brief  a structure to store the prefix and its 2 suffixes
  * 
  * prefix: prefix between two keys
  * suffix1: is the suffix from the previous key, generate by spliting an existing key
  * suffix2: is the suffix from the new key - the key we want to insert in the tree
  */
typedef struct {
    char * prefix;
    char * suffix1;
    char * suffix2;
} split;

/**
 * @brief A function to generate a split struct
 * 
 * @param prefix 
 * @param suffix1 
 * @param suffix2 
 * @return split* 
 */
split* newSplit( const char* prefix, const char* suffix1, const char* suffix2){
    split * s = (split*)malloc(sizeof(split));
    if (s == NULL) return NULL;
    s->prefix = s->suffix1 = s->suffix2 = NULL;
    if ( prefix) s->prefix = strdup(prefix);
    if ( suffix1 ) s->suffix1 = strdup(suffix1);
    if ( suffix2 ) s->suffix2 = strdup(suffix2);
    return s;
}


void freeSplit(split* s){
    if (s == NULL) return;
    if (s->prefix)  free(s->prefix);
    if (s->suffix1) free(s->suffix1);
    if (s->suffix2) free(s->suffix2);
    free(s);
}


/**
 * @brief the function to define the radix between the 2 strings
 * 
 * @param key1 
 * @param key2 
 * @return split*  - the prefix and its 2 suffixes structure
 */
split* keycmp( const char * key1, const char * key2){
#ifdef __UNITTEST_RADIX__
    printf("keycmp(%s, %s)\n", key1, key2);
#endif
    const char* pkey1;
    const char* pkey2;
    unsigned char c1, c2;
    int size1 = strlen(key1);
    int size2 = strlen(key2);
    int invert = 0;

    if ( size1 <= size2 ){
        pkey1 = (const char*)key1;
        pkey2 = (const char*)key2;
    } else {
        invert = 1;
        pkey1 = (const char*)key2;
        pkey2 = (const char*)key1;
    }

    do {
        c1 = (unsigned char) *pkey1;
        c2 = (unsigned char) *pkey2;
        if ( c1 == '\0' ){
            if ( c2 == '\0' ){
                return newSplit(key1, NULL, NULL);
            } else {
                int size = !invert ? key1 - pkey1 : key2 - pkey1;
                char * a = !invert ? strndup(key1, size) : strndup(key2, size);
                return !invert ? newSplit(a, NULL, pkey2) : newSplit(a, pkey2, NULL) ;
            }
        }
        pkey1++;
        pkey2++;
    } while ( c1 == c2 );

    pkey1--;
    pkey2--;
    int size = ( !invert ? pkey1 - key1 : pkey1 - key2 );
    char* a = !invert ? strndup(key1, size) : strndup(key2, size);
    if ( a ){
        split* key = !invert ? newSplit(a, pkey1, pkey2) : newSplit(a ,pkey2, pkey1) ;
        free(a);
        return key;
    }
    return NULL;
}


#ifdef __UNITTEST_RADIX__
void printSplit(const split* s){
    printf("split: %s | %s | %s\n", s->prefix, s->suffix1, s->suffix2);
}
#endif


#ifdef __UNITTEST__
void assertChar(const char* s1, const char* s2){
    if ( s1 == NULL && s2 == NULL ) return;
    assert( s1 != NULL );
    assert( s2 != NULL );
    int compare = strcmp(s1, s2);
    if ( compare ){
        printf("%s != %s\n", s1, s2);
    }
    assert( compare == 0 );
}

void test_keycmp(const char* s1, const char* s2, const split* s){
    split* r = keycmp(s1, s2);
    assert( r != NULL );
    if ( r == NULL ) return;
    assertChar(r->prefix , s->prefix );
    assertChar(r->suffix1, s->suffix1);
    assertChar(r->suffix2, s->suffix2);
    printSplit(r);
    printf("-----------------------\n");
}
#endif

/**
 * @brief a function to convert a base 16 character to its int value
 * 
 * @param c 
 * @return int 
 */
int convert(char c){
    if ( c >= '0' && c <= '9' ){
        return c - '0';   
    }
    if ( c >= 'A' && c <='F' ){
        return 10 + c-'A';
    }
    if ( c >= 'a' && c <='f' ){
        return 10+ c-'a';
    }   
    return -1;
}



/**
 * @brief to generate a new node for the radix tree
 * 
 * @param key 
 * @return node* 
 */
node* newNode(const char* key){
    node* n = (node*)malloc(sizeof(node));
    if ( n == NULL) return NULL;
    n->key = strdup(key);
    n->data = NULL;
    memset(n->children, 0, RADIXBASE);
    return n;
}


/**
 * @brief insert a key in a radix tree - generating a new node or return the existing
 * 
 * @param root the root tree (or the node )
 * @param key the key to insert in the radix
 * @return node* the leaf that correspond to the key
 */
node* insert(node * n, char * key){
    if ( n == NULL ){
        return newNode(key);
    }

    split* s = keycmp(n->key, key);
#ifdef __UNITTEST_RADIX__
    printSplit(s);
#endif

    n->key = s->prefix;
     
    if ( s->suffix1 ){
        int i = convert(s->suffix1[0]);
        if ( n->children[i] == NULL ){
            node* new = newNode(s->suffix1);
            if ( new ){
                for(int j=0; j<RADIXBASE; j++){
                    new->children[j] = n->children[j];
                    n->children[j] = NULL;
                }
                n->children[i] = new;
                new->data = n->data; // the child is getting the data
                n->data = NULL; // n become a gateway (no data)
                if ( s->suffix2 == NULL ) return new;
            }
        } else {
            node* next = n->children[i];
            node* leaf = insert(n->children[i], s->suffix1);
            n->children[i] = next;
            if ( s->suffix2 == NULL ) return leaf;
        }
    }

    if ( s->suffix2 ){  
        int i = convert(s->suffix2[0]);
        if ( n->children[i] == NULL ){
            n->children[i] = newNode(s->suffix2);
            return n->children[i];
        } else {
            node* next = n->children[i];
            node* leaf = insert(n->children[i], s->suffix2);
            n->children[i] = next;
            return leaf;
        }
    }

    return n;
}


char * space(int nb){
    char * sp = (char*)malloc(nb+1);
    if (sp==NULL)return NULL;
    memset(sp,'\t',nb);
    sp[nb]=0;
    return sp;
}

void printRadixExt(node* n, int index, int tab){
    char * sp = space(tab);
    if (sp==NULL) return;

    char * flux = NULL;
    if ( n->data && ((list*)n->data)->data){
        flux = PacketStr((fromtopacket*)((list*)n->data)->data);
    }

    fprintf(stderr, "%skey[%0X]: %s%s%s\n", sp, index, n->key ? n->key : "NULL" , flux ? " - Flux: " : "", flux?flux:"");
    free(flux);
    if ( n->data == NULL ) {  }
    //free(sp);
    for (int i=0; i<RADIXBASE; i++) {
        if ( n->children[i] != NULL ){ 
            printRadixExt(n->children[i], i, tab+1);
        }
    }
}

/**
 * @brief to print the radix tree from a radix node
 * 
 * @param root a radix node
 */
void printRadix(node* root){
    printRadixExt(root,0,0);
}

#ifdef __UNITTEST_RADIX__
void test_Split(){
    split* s = NULL;
    test_keycmp("123", "123", s = newSplit("123", NULL, NULL) );
    freeSplit(s);

    test_keycmp("133", "123", s = newSplit("1", "33", "23") );
    freeSplit(s);

    test_keycmp("123", "12", s = newSplit("12", "3", NULL) );
    freeSplit(s);

    test_keycmp("12", "123", s = newSplit("12", NULL, "3") );
    freeSplit(s);

    test_keycmp("120", "123", s = newSplit("12", "0", "3") );
    freeSplit(s);

    test_keycmp("223", "123", s = newSplit("", "223", "123") );
    freeSplit(s);

    test_keycmp("A012C4D8", "A012D4D8", s = newSplit("A012", "C4D8", "D4D8") );
    freeSplit(s);
}

void radix_test(){
    node* root = newNode("A012C4D8");
    assertChar(root->key, "A012C4D8");

    node* A012C4D8 = insert(root, "A012C4D8");
    printf("%p == %p\n", root, A012C4D8 );
    assert(root == A012C4D8);
    printf("-------------------\n");
    printRadix(root);

    node* A012D4D8 = insert(root,"A012D4D8");

    assertChar(root->key, "A012");
    assertChar(root->children[0xC]->key, "C4D8");
    assertChar(root->children[0xD]->key, "D4D8");

    printf("-------------------\n");

    node* A012D408 = insert(root, "A012D408");
    printf("-------------------\n");
    printRadix(root);

    node* A012D408bis = insert(root, "A012D408");
    printf("%p == %p\n", A012D408, A012D408bis );
    assert(A012D408 == A012D408bis);
    printf("-------------------\n");
    printRadix(root);


    assertChar(root->key, "A012");
    assertChar(root->children[0xC]->key, "C4D8");
    assertChar(root->children[0xD]->key, "D4");
    assertChar(root->children[0xD]->children[0x0]->key, "08");
    assertChar(root->children[0xD]->children[0xD]->key, "D8");

    printf("-------------------\n");
    node* A012D401 = insert(root, "A012D401");
    node* A012D407 = insert(root, "A012D407");
    printf("-------------------\n");
    printRadix(root);

    assertChar(root->key, "A012");
    assertChar(root->children[0xC]->key, "C4D8");
    assertChar(root->children[0xD]->key, "D4");
    assertChar(root->children[0xD]->children[0x0]->key, "0");    
    assertChar(root->children[0xD]->children[0x0]->children[0x1]->key, "1");
    assertChar(root->children[0xD]->children[0x0]->children[0x8]->key, "8");
    assertChar(root->children[0xD]->children[0x0]->children[0x7]->key, "7");
    assertChar(root->children[0xD]->children[0xD]->key, "D8");

    printf("-------------------\n");
    node* A012C4D7 = insert(root, "A012C4D7");
    printf("-------------------\n");

    assertChar(root->key, "A012");
    assertChar(root->children[0xC]->key, "C4D");
    assertChar(root->children[0xC]->children[0x7]->key, "7");
    assertChar(root->children[0xC]->children[0x8]->key, "8");
    assertChar(root->children[0xD]->key, "D4");
    assertChar(root->children[0xD]->children[0x0]->key, "0");
    assertChar(root->children[0xD]->children[0x0]->children[0x1]->key, "1");
    assertChar(root->children[0xD]->children[0x0]->children[0x8]->key, "8");
    assertChar(root->children[0xD]->children[0x0]->children[0x7]->key, "7");
    assertChar(root->children[0xD]->children[0xD]->key, "D8");

    printf("-------------------\n");
    printRadix(root);

    printf("test end\n");
}


void test_radix(){
    printf("-----First--------------\n");
    node* root = insert(NULL, "A012C4D8");

    assert(root != NULL);
    root->data = (void*)"1st";

    printf("-----Second--------------\n");
    /*  A012
            C4D8
            D4D8
    */
    node* node = insert(root, "A012D4D8");
    node->data = "2nd";

    assert(root->data == NULL);
    assertChar((char*)root->key, "A012");
    assertChar((char*)root->children[0xC]->key, "C4D8");
    assert( strcmp((char*)root->children[0xC]->data, "1st") == 0);
    assert( strcmp((char*)root->children[0xD]->key, "D4D8") == 0);
    assert( strcmp((char*)root->children[0xD]->data, "2nd") == 0);

  printf("-----Third--------------\n");
    /*  A0
            12
                C4D8 -> 1st
                D4D8 -> 2nd
            22D4D8 -> 3rd
    */
    node = insert(root, "A022D4D8");
    node->data = "3rd";
    assert(root->data == NULL);
    assertChar(root->key,"A0");
    assert(root->children[0x2] != NULL);
    assertChar(root->children[0x2]->key, "22D4D8");
    assertChar(root->children[0x2]->data,"3rd");
    assert(root->children[0x1]->data == NULL);
    assert( strcmp(root->children[0x1]->key, "12") == 0 );
    assert(root->children[0x1]->children[0xC] != NULL);
    assertChar((char*)root->children[0x1]->children[0xC]->data, "1st");
    assertChar((char*)root->children[0x1]->children[0xC]->key, "C4D8");
    assertChar((char*)root->children[0x1]->children[0xD]->data, "2nd");
    assertChar((char*)root->children[0x1]->children[0xD]->key, "D4D8");

    printf("-----Fourth--------------\n");
    /*  A
            0
                12
                    C4D8 -> 1st
                    D4D8 -> 2nd
                22D4D8 -> 3rd
            122D4D8 -> 4th
    */
    node = insert(root, "A122D4D8");
    node->data = "4th";

    assertChar(root->key, "A");
    assert(root->data == NULL);
    assert(root->children[0x0] != NULL);
    assertChar((char*)root->children[0x0]->key, "0");
    assert(root->children[0x0]->data == NULL);
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xC]->data, "1st");
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xC]->key, "C4D8");
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xD]->data, "2nd");
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xD]->key, "D4D8");

    assertChar((char*)root->children[0x0]->children[0x2]->key, "22D4D8");
    assertChar((char*)root->children[0x0]->children[0x2]->data, "3rd");

    assert(root->children[0x1] != NULL);
    assertChar((char*)root->children[0x1]->key, "122D4D8");
    assertChar((char*)root->children[0x1]->data, "4th");

  printf("-----Fifth--------------\n");
    /*  A
            0
                12
                    C4
                        13 -> 5th
                        D8 -> 1st
                    D4D8 -> 2nd
                22D4D8 -> 3rd
            122D4D8 -> 4th
    */
    node = insert(root, "A012C413");
    node->data = "5th";

    assertChar(root->key, "A");
    assert(root->data == NULL);
    assert(root->children[0x0] != NULL);
    assertChar((char*)root->children[0x0]->key, "0");
    assert(           root->children[0x0]->data == NULL);

    assert(           root->children[0x0]->children[0x1]->children[0xC]->data == NULL);
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xC]->key, "C4");

    assertChar((char*)root->children[0x0]->children[0x1]->children[0xC]->children[0x1]->key, "13");
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xC]->children[0x1]->data, "5th");
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xC]->children[0xD]->key, "D8");
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xC]->children[0xD]->data, "1st");

    assertChar((char*)root->children[0x0]->children[0x1]->children[0xD]->data, "2nd");
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xD]->key, "D4D8");

    assertChar((char*)root->children[0x0]->children[0x2]->key, "22D4D8");
    assertChar((char*)root->children[0x0]->children[0x2]->data, "3rd");

    assert(root->children[0x1] != NULL);
    assertChar((char*)root->children[0x1]->key, "122D4D8");
    assertChar((char*)root->children[0x1]->data, "4th");

    printf("-----Sixth--------------\n");
    /*  A
            0
                12
                    C4
                        13 -> 5th
                        D8 -> 1st
                    D4C8 -> 2nd
                22D4C8 -> 3rd
            122D
                4D8 -> 4th
                BE3 -> 6th
    */
    node = insert(root, "A122DBE3");
    node->data = "6th";

    assertChar(root->key, "A");
    assert(root->data == NULL);
    assert(root->children[0x0] != NULL);
    assertChar((char*)root->children[0x0]->key, "0");
    assert(           root->children[0x0]->data == NULL);

    assert(           root->children[0x0]->children[0x1]->children[0xC]->data == NULL);
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xC]->key, "C4");

    assertChar((char*)root->children[0x0]->children[0x1]->children[0xC]->children[0x1]->key, "13");
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xC]->children[0x1]->data, "5th");
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xC]->children[0xD]->key, "D8");
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xC]->children[0xD]->data, "1st");

    assertChar((char*)root->children[0x0]->children[0x1]->children[0xD]->data, "2nd");
    assertChar((char*)root->children[0x0]->children[0x1]->children[0xD]->key, "D4D8");

    assertChar((char*)root->children[0x0]->children[0x2]->key, "22D4D8");
    assertChar((char*)root->children[0x0]->children[0x2]->data, "3rd");

    assert(           root->children[0x1] != NULL);
    assertChar((char*)root->children[0x1]->key, "122D");

    assertChar((char*)root->children[0x1]->children[0x4]->key, "4D8");
    assertChar((char*)root->children[0x1]->children[0x4]->data, "4th");
    assertChar((char*)root->children[0x1]->children[0xB]->key, "BE3");
    assertChar((char*)root->children[0x1]->children[0xB]->data, "6th");
}

void test_radix_root(){

    printf("-----First--------------\n");
    node* root = insert(NULL, "A012C4D8");
    assert(root != NULL);
    root->data = (void*)"1st";

    printf("-----Second--------------\n");
    /*  
            0012C4D8 -> 2nd
            A012C4D8 -> 1st
    */
    node* node = insert(root, "0012C4D8");
    node->data = "2nd";

    assert(root->data == NULL);
    assertChar((char*)root->key, "");
    assertChar((char*)root->children[0x0]->key, "0012C4D8");
    assertChar((char*)root->children[0x0]->data, "2nd");
    assertChar((char*)root->children[0xA]->key, "A012C4D8");
    assertChar((char*)root->children[0xA]->data, "1st");

  printf("-----Third--------------\n");
    /*  
            0012C4D8 -> 2nd
            A0
                12C4D8 -> 1st
                22D4D8 -> 3rd
    */
    node = insert(root, "A022D4D8");
    node->data = "3rd";

    assert(root->data == NULL);
    assertChar((char*)root->key, "");
    assertChar((char*)root->children[0x0]->key, "0012C4D8");
    assertChar((char*)root->children[0x0]->data, "2nd");
    assertChar((char*)root->children[0xA]->key, "A0");
    assert(           root->children[0xA]->data == NULL);
    assertChar((char*)root->children[0xA]->children[0x1]->key, "12C4D8");
    assertChar((char*)root->children[0xA]->children[0x1]->data, "1st");
    assertChar((char*)root->children[0xA]->children[0x2]->key, "22D4D8");
    assertChar((char*)root->children[0xA]->children[0x2]->data, "3rd");
}

int main(){
    test_Split();
    radix_test();

    test_radix();
    test_radix_root();

    return 0;
}

// gcc -o radix packet.o list.o radix.c -g -D__UNITTEST_RADIX__ -D__UNITTEST__ && ./radix

#endif
