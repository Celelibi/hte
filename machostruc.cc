/* 
 *	HT Editor
 *	machostruc.cc
 *
 *	Copyright (C) 1999-2002 Stefan Weyergraf
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "machostruc.h"
#include "global.h"
#include "htendian.h"

byte MACHO_HEADER_struct[]= {
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	0
};

byte MACHO_COMMAND_struct[]= {
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	0
};

byte MACHO_SEGMENT_COMMAND_struct[]= {
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	0
};

byte MACHO_SECTION_struct[]= {
// sectname
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
// segname
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
//
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	0
};

byte MACHO_THREAD_COMMAND_struct[] = {
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	// .state not included !
	0
};

byte MACHO_PPC_THREAD_STATE_struct[] = {
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	0
};

byte MACHO_I386_THREAD_STATE_struct[] = {
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	0
};

byte MACHO_SYMTAB_COMMAND_struct[] = {
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	0
};

byte MACHO_SYMTAB_NLIST_struct[] = {
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_BYTE | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_WORD | STRUCT_ENDIAN_HOST,
	STRUCT_ENDIAN_DWORD | STRUCT_ENDIAN_HOST,
	0
};
