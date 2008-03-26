#ifndef _MEMSIZES_H
#define _MEMSIZES_H

#include <basemem.h>

/**
 * Get size of base memory from BIOS free base memory counter
 *
 * @ret basemem		Base memory size, in kB
 */
static inline unsigned int basememsize ( void ) {
	return get_fbms();
}

extern unsigned int extmemsize ( void );

#endif /* _MEMSIZES_H */
