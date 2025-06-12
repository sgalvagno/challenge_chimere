/**
 * @file packet.c
 * @author Sebastien Galvagno
 * @brief 
 * @version 0.1
 * @date 2022-04-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "packet.h"

/**
 * @brief print the flux summary to show result
 * 
 * @param packet 
 */
void printPacketSummary(fromtopacket* packet){
        struct in_addr addr;

        addr.s_addr = packet->from; 
        char* ip = inet_ntoa(addr);
        char ipFrom[strlen(IPV4MASK)+1];
        strncpy(ipFrom, ip, sizeof(ipFrom));

        addr.s_addr   = packet->to;
        ip   = inet_ntoa(addr);
        char ipTo[strlen(IPV4MASK)+1];
        strncpy(ipTo, ip, sizeof(ipTo));

        int size = packet->lastPacket ? packet->lastPacket - packet->firstPacket : 0;

        printf("Flux %s:%u,%s:%u / Taille : %u\n" , ipFrom, packet->portFrom, ipTo, packet->portTo, size);
}

/**
 * @brief stringify a packet structure
 * 
 * @param packet 
 */
 char * PacketStr(fromtopacket* packet){
        struct in_addr addr;

        addr.s_addr = packet->from; 
        char* ip = inet_ntoa(addr);
        char ipFrom[strlen(IPV4MASK)+1];
        strncpy(ipFrom, ip, sizeof(ipFrom));

        addr.s_addr   = packet->to;
        ip   = inet_ntoa(addr);
        char ipTo[strlen(IPV4MASK)+1];
        strncpy(ipTo, ip, sizeof(ipTo));

        int size = packet->lastPacket - packet->firstPacket;

        char txt[1000];
        //printf("Flux %s:%u,%s:%u / Taille : %u\n" , ipFrom, packet->portFrom, ipTo, packet->portTo, size);
        snprintf(txt, 1000, "%s:%u %s:%u - %u - %u", ipFrom, packet->portFrom, ipTo, packet->portTo, packet->firstPacket, packet->lastPacket);
        return strdup(txt);
}

/**
 * @brief print a packet structure
 * 
 * @param packet 
 */
void printPacketStr(fromtopacket* packet){
    char * str = PacketStr(packet);
    if (str){
        printf("%s\n",str);
        free(str);
    }
}


// IPv4 4 bytes + 2 bytes then 8 + 4 characters
#define FLUXHEXASIZE 2*(8+4)
/**
 * @brief generate an hexadecimal string of the packet structure
 * 
 * @param packet 
 * @return char* the hexadecimal string
 */
char * fluxString(fromtopacket* packet){

    char str[FLUXHEXASIZE+1];
    memset(str,0,FLUXHEXASIZE+1);

    snprintf(str, FLUXHEXASIZE+1, "%X%X%X%X", packet->from, packet->to, packet->portFrom, packet->portTo);

    return strdup(str);
}


#ifdef __UNITTEST_PACKET__

fromtopacket * initPacket(UInt32 from, UInt32 to, UInt16 portFrom, UInt16 portTo){

    fromtopacket* packet = malloc(sizeof(fromtopacket));
        if ( packet ){
            memset(packet, 0, sizeof(fromtopacket));
            packet->from = from; // addrFrom.s_addr ;
            packet->to = to; // addrTo.s_addr ;
            packet->portFrom = portFrom; //(UInt16) iPortfrom ;
            packet->portTo = portTo; //(UInt16) iPortto ;
            packet->firstPacket = 0; //iSeq ;
        }
        return packet;
}

 int main(){

    fromtopacket* packet = initPacket(0x12AB34CD, 0x56EF7890, 0xDCBA, 0x4321);
    char * fluxstr =  "12AB34CD56EF7890DCBA4321";

    char* flux = fluxString(packet);
    free(packet);

    printf("orig: %s\n",fluxstr);
    printf("calc: %s\n",flux);

    if ( strcmp(fluxstr,flux) != 0 ){
        printf("Error not equal!\n");
    } else {
        printf("String are equal\n")
    }

    free(flux);
 }
 #endif
 
