/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2006 Erwan Velu - All Rights Reserved
 *
 *   Permission is hereby granted, free of charge, to any person
 *   obtaining a copy of this software and associated documentation
 *   files (the "Software"), to deal in the Software without
 *   restriction, including without limitation the rights to use,
 *   copy, modify, merge, publish, distribute, sublicense, and/or
 *   sell copies of the Software, and to permit persons to whom
 *   the Software is furnished to do so, subject to the following
 *   conditions:
 *
 *   The above copyright notice and this permission notice shall
 *   be included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 *
 * -----------------------------------------------------------------------
*/

/*
 * dmitest.c
 *
 * DMI demo program using libcom32
 */

#include <string.h>
#include <stdio.h>
#include <console.h>
#include "dmi/dmi.h"
#include "syslinux/boot.h"

char display_line;

int main(void)
{
    s_dmi dmi;
    openconsole(&dev_stdcon_r, &dev_stdcon_w);

    if (dmi_iterate(&dmi) == -ENODMITABLE) {
	printf("No DMI Structure found\n");
	return -1;
    }
    parse_dmitable(&dmi);

    moreprintf("productname: %s\n", dmi.system.product_name);
    if (strncmp(dmi.system.product_name, "ProLiant ", 8) == 0) {
      syslinux_run_kernel_image("pxelinux.cfg/proliant", "", 0, IMAGE_TYPE_CONFIG);
    } else {
      syslinux_run_kernel_image("pxelinux.cfg/menu", "", 0, IMAGE_TYPE_CONFIG);
    }

    return 0;
}
