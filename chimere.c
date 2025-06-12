

/**
 * @file chimere.c
 * @author Sebastien Galvagno
 * @brief Chimere challenge
 * @version 0.1
 * @date 2022-04-22
 * 
 * @copyright Copyright (c) 2022
 * 
 * Build command
 * ./script.sh && date; ./chimere < mock.txt; date
 * ./script.sh && date && ./chimere < mock_10000.txt 2> radix_10000.txt > result_10000.txt && date
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>

#include "SG_Types.h"
#include "packet.h"
#include "radix.h"
#include "list.h"

typedef int bool;
enum { false, true };

typedef enum { noError = 0, atonError, atoiError } error_t;

error_t tryStrtol(UInt32* result, const char *nptr, char **endptr, int base){
    errno = noError;
    *result = strtol(nptr, endptr, base);
    return errno;
}

/**
 * @brief to decode the input stream
 * 
 * @param buffer the string to decode
 * @return fromtopacket* the data
 */
fromtopacket* decode(char *buffer){
    fromtopacket* packet = NULL;
    char *saveptr;
    char * from = strtok_r(buffer, ",", &saveptr);
    char * to = strtok_r(NULL, ",", &saveptr);
    char * seq = strtok_r(NULL, ",", &saveptr);

    char * ipFrom = strtok_r(from, ":", &saveptr);
    char * portFrom = strtok_r(NULL, ":", &saveptr);

    char * ipTo = strtok_r(to, ":" , &saveptr);
    char * portTo = strtok_r(NULL, ":", &saveptr);
    
    struct in_addr addrFrom;
    struct in_addr addrTo;
    UInt32 iPortfrom, iPortto,iSeq;
    if ( from && to && seq && ipFrom && portFrom && ipTo && portTo
        && inet_aton(ipFrom, &addrFrom) != 0 
        && inet_aton(ipTo, &addrTo) != 0
        && tryStrtol(&iPortfrom, portFrom, NULL, 10) == noError
        && tryStrtol(&iPortto, portTo, NULL, 10) == noError
        && tryStrtol(&iSeq, seq, NULL, 10) == noError
    ){
        packet = malloc(sizeof(fromtopacket));
        if ( packet ){
            memset(packet, 0, sizeof(fromtopacket));
            packet->from = addrFrom.s_addr ;
            packet->to = addrTo.s_addr ;
            packet->portFrom = (UInt16) iPortfrom ;
            packet->portTo = (UInt16) iPortto ;
            packet->firstPacket = iSeq ;
        }
    }
    return packet;
}


/**
 * @brief the function use by the generic list to print the data
 * 
 * @param data here data is the packet structure
 */
void affiche(void* data){
    printPacketSummary((fromtopacket*)data);
}

/**
 * @brief the function to compare 2 items of the list - used to sort the list
 * 
 * @param node1 
 * @param node2 
 * @return int 
            <0  node1  < node2
            ==0 node1 == node2
            >0  node1  > node2
 */
int comparePacket(list* node1, list* node2){

    fromtopacket* packet1 = (fromtopacket*)node1->data;
    fromtopacket* packet2 = (fromtopacket*)node2->data;

    int nbPacket1 = packet1->lastPacket ? packet1->lastPacket - packet1->firstPacket : 0;
    int nbPacket2 = packet2->lastPacket ? packet2->lastPacket - packet2->firstPacket : 0;

    return nbPacket1 - nbPacket2 ;
}


int main(int argc, char **argv){
  
  char buffer[SIZEFROMTOMASK+1];

    FILE* fp = NULL;
    if ( argc == 2 ) {
        fp = fopen(argv[1], "r");
    }
    if ( fp == NULL ){
        fp = stdin;
    }

    node* radixRoot = NULL;
    list* listFlux = NULL;
    list* last = NULL;

    while ( fgets(buffer, sizeof(buffer), fp) != NULL){
        if ( *buffer == '\n' ) continue;

        fromtopacket* packet = decode(buffer);
        if ( packet ){
            char* zflux = fluxString(packet);

            if ( zflux ){
                node* n = insert(radixRoot, zflux);
                if ( radixRoot == NULL) {
                    radixRoot = n;
                }

                if ( n->data == NULL ){
                    list* newflux = insertlist(listFlux, n);                    
                    if ( newflux ){
                        newflux->data = (void*) packet;

                        n->data = (void*)newflux;
                            listFlux = newflux;
                    }
                }

                else {
                    list* nodelist = (list*) n->data;
                    fromtopacket* p = (fromtopacket*)nodelist->data;

                    if (p->lastPacket < packet->firstPacket){
                        p->lastPacket = packet->firstPacket;
                    } else {
                        printf("Packet - Bad sequence number\n");
                        printPacketStr(p);
                        printPacketStr(packet);
                        printf("--------------------\n");
                        return 1;
                    }

                    listFlux = moveNode(listFlux, nodelist, &comparePacket);
                }

                free(zflux);
            }
        }
    }

#ifdef __SHOW_RADIX__
    printRadix(radixRoot);
    printf("----------------------\n");
#endif
    printList(listFlux, &affiche);
    return 0;
}
