#ifndef	_GPXE_IN_H
#define	_GPXE_IN_H

#include <stdint.h>
#include <gpxe/socket.h>

/* Protocol numbers */

#define IP_ICMP		1
#define IP_IGMP		2
#define IP_TCP		6
#define IP_UDP		17
#define IP_ICMP6	58

/* IP address constants */

#define INADDR_NONE 0xffffffff

#define INADDR_BROADCAST 0xffffffff

#define	IN_CLASSA(addr)		( ( (addr) & 0x80000000 ) == 0x00000000 )
#define	IN_CLASSA_NET		0xff000000
#define	IN_CLASSB(addr)		( ( (addr) & 0xc0000000 ) == 0x80000000 )
#define	IN_CLASSB_NET		0xffff0000
#define	IN_CLASSC(addr)		( ( (addr) & 0xe0000000 ) == 0xc0000000 )
#define	IN_CLASSC_NET		0xffffff00
#define IN_MULTICAST(addr)	( ( (addr) & 0xf0000000 ) == 0xe0000000 )

/**
 * IP address structure
 */
struct in_addr {
	uint32_t	s_addr;
};

typedef struct in_addr in_addr;

/**
 * IP6 address structure
 */
struct in6_addr {
        union {
                uint8_t u6_addr8[16];
                uint16_t u6_addr16[8];
                uint32_t u6_addr32[4];
        } in6_u;
#define s6_addr         in6_u.u6_addr8
#define s6_addr16       in6_u.u6_addr16
#define s6_addr32       in6_u.u6_addr32
};

/**
 * IPv4 socket address
 */
struct sockaddr_in {
	/** Socket address family (part of struct @c sockaddr)
	 *
	 * Always set to @c AF_INET for IPv4 addresses
	 */
	sa_family_t sin_family;
	/** TCP/IP port (part of struct @c sockaddr_tcpip) */
	uint16_t sin_port;
	/** IPv4 address */
	struct in_addr sin_addr;
	/** Padding
	 *
	 * This ensures that a struct @c sockaddr_tcpip is large
	 * enough to hold a socket address for any TCP/IP address
	 * family.
	 */
	char pad[ sizeof ( struct sockaddr ) - sizeof ( sa_family_t )
					     - sizeof ( uint16_t )
					     - sizeof ( struct in_addr ) ];
} __attribute__ (( may_alias ));

/**
 * IPv6 socket address
 */
struct sockaddr_in6 {
	/** Socket address family (part of struct @c sockaddr)
	 *
	 * Always set to @c AF_INET6 for IPv6 addresses
	 */
	sa_family_t sin_family;
	/** TCP/IP port (part of struct @c sockaddr_tcpip) */
	uint16_t 	sin_port;
        uint32_t        sin6_flowinfo;  /* Flow number */
        struct in6_addr sin6_addr;      /* 128-bit destination address */
        uint32_t        sin6_scope_id;  /* Scope ID */
} __attribute__ (( may_alias ));

extern int inet_aton ( const char *cp, struct in_addr *inp );
extern char * inet_ntoa ( struct in_addr in );

/* Adding the following for IP6 support
 *

extern int inet6_aton ( const char *cp, struct in6_addr *inp );
extern char * inet6_ntoa ( struct in_addr in );

 */

#endif	/* _GPXE_IN_H */
