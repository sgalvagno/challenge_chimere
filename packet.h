/**
 * @file packet.h
 * @author Sebastien Galvagno
 * @brief 
 * @version 0.1
 * @date 2022-04-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __SG__CHIMERE_PACKET_H__
#define __SG__CHIMERE_PACKET_H__

#include "SG_Types.h"

#define IPV4MASK "XXX.XXX.XXX.XXX"
#define IPV4WITHPORTMASK IPV4MASK ":" "65536"
#define FROMTOMASK IPV4WITHPORTMASK "," IPV4WITHPORTMASK ","  
#define INT32MASK "4294967296"
#define SIZEFROMTOMASK strlen(FROMTOMASK)+strlen(INT32MASK)

#define IPV4MASKMIN "X.X.X.X"
#define IPV4WITHPORTMASKMIN IPV4MASKMIN ":" "1"
#define FROMTOMASKMIN IPV4WITHPORTMASKMIN "," IPV4WITHPORTMASKMIN "," 
//   min seq_tcp = 0
#define SIZEFROMTOMASKMIN strlen(FROMTOMASKMIN)+1

typedef UInt32 tcp_seq;

typedef struct {
  UInt32 from;
  UInt32 to;
  UInt16 portFrom;
  UInt16 portTo;
  tcp_seq firstPacket;
  tcp_seq lastPacket; 
} fromtopacket;

/**
 * @brief print the flux summary to show result
 * 
 * @param packet 
 */
void printPacketSummary(fromtopacket* packet);

/**
 * @brief print a packet structure
 * 
 * @param packet 
 */
void printPacketStr(fromtopacket* packet);

/**
 * @brief stringify a packet structure
 * 
 * @param packet 
 */
char * PacketStr(fromtopacket* packet);

/**
 * @brief generate an hexadecimal string of the packet structure
 * 
 * @param packet 
 * @return char* the hexadecimal string
 */
char * fluxString(fromtopacket* packet);

#endif
;
