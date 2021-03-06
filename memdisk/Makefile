## -----------------------------------------------------------------------
##
##   Copyright 2001-2009 H. Peter Anvin - All Rights Reserved
##   Copyright 2009 Intel Corporation; author: H. Peter Anvin
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as published by
##   the Free Software Foundation, Inc., 53 Temple Place Ste 330,
##   Boston MA 02111-1307, USA; either version 2 of the License, or
##   (at your option) any later version; incorporated herein by reference.
##
## -----------------------------------------------------------------------

topdir = ..
include $(topdir)/MCONFIG.embedded
-include $(topdir)/version.mk

INCLUDES = -I$(topdir)/com32/include
CFLAGS  += -DDATE='"$(DATE)"'
LDFLAGS  = $(GCCOPT) -g
NASM     = nasm
NASMOPT  = -O9999
NFLAGS   = -dDATE='"$(DATE)"'
NINCLUDE =

SRCS	 = $(wildcard *.asm *.c *.h)

# The DATE is set on the make command line when building binaries for
# official release.  Otherwise, substitute a hex string that is pretty much
# guaranteed to be unique to be unique from build to build.
ifndef HEXDATE
HEXDATE := $(shell $(PERL) ../now.pl $(SRCS))
endif
ifndef DATE
DATE    := $(shell sh ../gen-id.sh $(VERSION) $(HEXDATE))
endif

# Important: init.o16 must be first!!
OBJS16   = init.o16 init32.o
OBJS32   = start32.o setup.o msetup.o e820func.o conio.o memcpy.o memset.o \
           unzip.o memdisk_chs.o memdisk_edd.o

CSRC     = setup.c msetup.c e820func.c conio.c unzip.c
SSRC     = start32.S memcpy.S memset.S
NASMSRC  = memdisk_chs.asm memdisk_edd.asm memdisk16.asm

all: memdisk # e820test

# tidy, clean removes everything except the final binary
tidy dist:
	rm -f *.o *.s *.tmp *.o16 *.s16 *.bin *.lst *.elf e820test .*.d

clean: tidy

# spotless also removes the product binary
spotless: clean
	rm -f memdisk .depend

memdisk16.o: memdisk16.asm

# Cancel rule
%.o: %.asm

memdisk16.o: memdisk16.asm
	( $(NASM) -M -DDEPEND $(NFLAGS) $(NINCLUDE) -o $@ $< ; echo '' ) > .$@.d ; true
	$(NASM) -f elf $(NASMOPT) $(NFLAGS) $(NINCLUDE) -o $@ -l $*.lst $<

.PRECIOUS: %.bin
%.bin: %.asm
	( $(NASM) -M -DDEPEND $(NFLAGS) $(NINCLUDE) -o $@ $< ; echo '' ) > .$@.d ; true
	$(NASM) -f bin $(NASMOPT) $(NFLAGS) $(NINCLUDE) -o $@ -l $*.lst $<

memdisk_%.o: memdisk_%.bin
	$(LD) -r -b binary -o $@ $<

memdisk16.elf: $(OBJS16)
	$(LD) -Ttext 0 -o $@ $^

memdisk32.elf: memdisk.ld $(OBJS32)
	$(LD) -o $@ -T $^

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

memdisk: memdisk16.bin memdisk32.bin postprocess.pl
	$(PERL) postprocess.pl $@ memdisk16.bin memdisk32.bin

e820test: e820test.c e820func.c msetup.c
	$(CC) -m32 -g -W -Wall -DTEST -o $@ $^

# This file contains the version number, so add a dependency for it
setup.s: ../version

# Include dependencies file
-include .*.d
