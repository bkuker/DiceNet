// packet.h: Defines the header of a packet
//
//////////////////////////////////////////////////////////////////////
#if !defined(__PACKET__)
#define __PACKET__

typedef unsigned char byte;

struct PACKET{
	byte size;
	byte type;
};

#define PACKET_MAX_LEN 255

#endif