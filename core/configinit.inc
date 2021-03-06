;; -----------------------------------------------------------------------
;;
;;   Copyright 1994-2008 H. Peter Anvin - All Rights Reserved
;;
;;   This program is free software; you can redistribute it and/or modify
;;   it under the terms of the GNU General Public License as published by
;;   the Free Software Foundation, Inc., 53 Temple Place Ste 330,
;;   Boston MA 02111-1307, USA; either version 2 of the License, or
;;   (at your option) any later version; incorporated herein by reference.
;;
;; -----------------------------------------------------------------------

;;
;; configinit.inc
;;
;; Initialize the configuration section
;;

		section .text

reset_config:
		call highmemsize

		; Initialize the .config section
		xor eax,eax
		mov si,__config_lma
		mov di,__config_start
		mov cx,__config_dwords
		rep movsd

%ifndef DEPEND
%if NULLFILE != 0
		mov al,NULLFILE
		mov di,FKeyName
		mov cx,MAX_FKEYS*(1 << FILENAME_MAX_LG2)
		rep stosb
%endif
%endif

		mov si,linuxauto_cmd		; Default command: "linux auto"
		mov di,default_cmd
                mov cx,linuxauto_len
		rep movsb

		mov di,KbdMap			; Default keymap 1:1
		xor al,al
		inc ch				; CX <- 256
mkkeymap:	stosb
		inc al
		loop mkkeymap

		mov eax,[HighMemSize]
		mov [VKernelEnd],eax

		ret

		section .data
linuxauto_cmd	db 'linux auto',0
linuxauto_len   equ $-linuxauto_cmd
