/*
 * Copyright (C) 2008 Michael Brown <mbrown@fensystems.co.uk>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <assert.h>
#include <gpxe/umalloc.h>
#include <gpxe/efi/efi.h>

/** @file
 *
 * gPXE user memory allocation API for EFI
 *
 */

/** Equivalent of NOWHERE for user pointers */
#define UNOWHERE ( ~UNULL )

/**
 * Reallocate external memory
 *
 * @v old_ptr		Memory previously allocated by umalloc(), or UNULL
 * @v new_size		Requested size
 * @ret new_ptr		Allocated memory, or UNULL
 *
 * Calling realloc() with a new size of zero is a valid way to free a
 * memory block.
 */
static userptr_t efi_urealloc ( userptr_t old_ptr, size_t new_size ) {
	EFI_BOOT_SERVICES *bs = efi_systab->BootServices;
	EFI_PHYSICAL_ADDRESS phys_addr;
	unsigned int new_pages, old_pages;
	userptr_t new_ptr = UNOWHERE;
	size_t old_size;
	EFI_STATUS efirc;

	/* Allocate new memory if necessary.  If allocation fails,
	 * return without touching the old block.
	 */
	if ( new_size ) {
		new_pages = ( EFI_SIZE_TO_PAGES ( new_size ) + 1 );
		if ( ( efirc = bs->AllocatePages ( AllocateAnyPages,
						   EfiBootServicesData,
						   new_pages,
						   &phys_addr ) ) != 0 ) {
			DBG ( "EFI could not allocate %d pages: %s\n",
			      new_pages, efi_strerror ( efirc ) );
			return UNULL;
		}
		assert ( phys_addr != 0 );
		new_ptr = phys_to_user ( phys_addr + EFI_PAGE_SIZE );
		copy_to_user ( new_ptr, -EFI_PAGE_SIZE,
			       &new_size, sizeof ( new_size ) );
		DBG ( "EFI allocated %d pages at %llx\n",
		      new_pages, phys_addr );
	}

	/* Copy across relevant part of the old data region (if any),
	 * then free it.  Note that at this point either (a) new_ptr
	 * is valid, or (b) new_size is 0; either way, the memcpy() is
	 * valid.
	 */
	if ( old_ptr && ( old_ptr != UNOWHERE ) ) {
		copy_from_user ( &old_size, old_ptr, -EFI_PAGE_SIZE,
				 sizeof ( old_size ) );
		memcpy_user ( new_ptr, 0, old_ptr, 0,
			      ( (old_size < new_size) ? old_size : new_size ));
		old_pages = ( EFI_SIZE_TO_PAGES ( old_size ) + 1 );
		phys_addr = user_to_phys ( old_ptr, -EFI_PAGE_SIZE );
		if ( ( efirc = bs->FreePages ( phys_addr, old_pages ) ) != 0 ){
			DBG ( "EFI could not free %d pages at %llx: %s\n",
			      old_pages, phys_addr, efi_strerror ( efirc ) );
			/* Not fatal; we have leaked memory but successfully
			 * allocated (if asked to do so).
			 */
		}
		DBG ( "EFI freed %d pages at %llx\n", old_pages, phys_addr );
	}

	return new_ptr;
}

PROVIDE_UMALLOC ( efi, urealloc, efi_urealloc );
