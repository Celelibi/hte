/* 
 *	HT Editor
 *	htelfrel.cc
 *
 *	Copyright (C) 1999, 2000, 2001 Stefan Weyergraf (stefan@weyergraf.de)
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

#include "elfstruc.h"
#include "htatom.h"
#include "htelf.h"
#include "htelfrel.h"
#include "httag.h"
#include "formats.h"
#include "snprintf.h"

#include <stdlib.h>

int_hash elf_r_386_type[] =
{
	{ELF_R_386_NONE,		"ELF_R_386_NONE"},
	{ELF_R_386_32,			"ELF_R_386_32"},
	{ELF_R_386_PC32,		"ELF_R_386_PC32"},
	{ELF_R_386_GOT32,		"ELF_R_386_GOT32"},
	{ELF_R_386_PLT32,		"ELF_R_386_PLT32"},
	{ELF_R_386_COPY,		"ELF_R_386_COPY"},
	{ELF_R_386_GLOB_DAT,	"ELF_R_386_GLOB_DAT"},
	{ELF_R_386_JMP_SLOT,	"ELF_R_386_JMP_SLOT"},
	{ELF_R_386_RELATIVE,	"ELF_R_386_RELATIVE"},
	{ELF_R_386_GOTOFF,		"ELF_R_386_GOTOFF"},
	{ELF_R_386_GOTPC,		"ELF_R_386_GOTPC"},
	{0, 0}
};

ht_view *htelfreloctable_init(bounds *b, ht_streamfile *file, ht_format_group *group)
{
	ht_elf_shared_data *elf_shared=(ht_elf_shared_data *)group->get_shared_data();

	if ((elf_shared->ident.e_ident[ELF_EI_CLASS]!=ELFCLASS32) ||
	(elf_shared->ident.e_ident[ELF_EI_DATA]!=ELFDATA2LSB) ||
	(elf_shared->header32.e_machine!=ELF_EM_386)) return 0;
	
	UINT skip = elf_shared->reloctables;
	UINT reloctab_shidx = ELF_SHN_UNDEF;
	UINT reloctab_sh_type = ELF_SHT_NULL;
	for (UINT i=1; i<elf_shared->sheaders.count; i++) {
		if ((elf_shared->sheaders.sheaders32[i].sh_type==ELF_SHT_REL) || (elf_shared->sheaders.sheaders32[i].sh_type==ELF_SHT_RELA)) {
			if (!skip--) {
				reloctab_sh_type=elf_shared->sheaders.sheaders32[i].sh_type;
				reloctab_shidx=i;
				break;
			}
		}
	}
	if (reloctab_shidx==ELF_SHN_UNDEF) return 0;

	FILEOFS h=elf_shared->sheaders.sheaders32[reloctab_shidx].sh_offset;
	
/* section index of associated symbol table */
	int si_symbol=elf_shared->sheaders.sheaders32[reloctab_shidx].sh_link;
	
/* section index of section to be relocated */
	int si_dest=elf_shared->sheaders.sheaders32[reloctab_shidx].sh_info;

	file->seek(elf_shared->sheaders.sheaders32[elf_shared->header32.e_shstrndx].sh_offset+elf_shared->sheaders.sheaders32[reloctab_shidx].sh_name);
	char *reloctab_name=fgetstrz(file);
	char desc[128];
	sprintf(desc, DESC_ELF_RELOCTAB, reloctab_name, reloctab_shidx);
	free(reloctab_name);
	
	ht_uformat_viewer *v=new ht_uformat_viewer();
	v->init(b, desc, VC_EDIT, file, group);
	
	ht_mask_sub *m=new ht_mask_sub();
	m->init(file, 0);
	
	register_atom(ATOM_ELF_R_386_TYPE, elf_r_386_type);

	char t[256];	/* FIXME: possible buffer overflow ! */
	ht_snprintf(t, sizeof t, "* ELF relocation table at offset %08x, relocates section %d, symtab %d", h, si_dest, si_symbol);

	m->add_mask(t);

	bool elf_bigendian = (elf_shared->ident.e_ident[ELF_EI_DATA] == ELFDATA2MSB);

	switch (reloctab_sh_type) {
		case ELF_SHT_REL: {
			m->add_mask("destofs  symidx type");
			UINT relnum=elf_shared->sheaders.sheaders32[reloctab_shidx].sh_size / sizeof (ELF_REL32);
			for (UINT i=0; i<relnum; i++) {
				char *tt=t;
				/* dest offset */
				tt=tag_make_edit_dword(tt, h+i*sizeof (ELF_REL32), elf_bigendian ? tag_endian_big : tag_endian_little);
				*tt++=' ';
				/* symbol (table idx) */
				tt=tag_make_edit_word(tt, h+i*sizeof (ELF_REL32)+4+1, elf_bigendian ? tag_endian_big : tag_endian_little);
				*tt++=' ';
				*tt++=' ';
				*tt++=' ';
				/* type */
				tt=tag_make_edit_byte(tt, h+i*sizeof (ELF_REL32)+4);
				*tt++='(';
				/* type */
				tt=tag_make_desc_byte(tt, h+i*sizeof (ELF_REL32)+4, ATOM_ELF_R_386_TYPE);
				*tt++=')';
				*tt=0;
				m->add_mask(t);
			}
			break;
		}
		case ELF_SHT_RELA: {
			m->add_mask("destofs  symidx addend   type");
			UINT relnum=elf_shared->sheaders.sheaders32[reloctab_shidx].sh_size / sizeof (ELF_RELA32);
			for (UINT i=0; i<relnum; i++) {
				char *tt=t;
				/* dest offset */
				tt=tag_make_edit_dword(tt, h+i*sizeof (ELF_RELA32), elf_bigendian ? tag_endian_big : tag_endian_little);
				*tt++=' ';
				/* symbol (table idx) */
				tt=tag_make_edit_word(tt, h+i*sizeof (ELF_RELA32)+4+1, elf_bigendian ? tag_endian_big : tag_endian_little);
				*tt++=' ';
				*tt++=' ';
				*tt++=' ';
				/* addend */
				tt=tag_make_edit_dword(tt, h+i*sizeof (ELF_RELA32)+4+4, elf_bigendian ? tag_endian_big : tag_endian_little);
				*tt++=' ';
				/* type */
				tt=tag_make_edit_byte(tt, h+i*sizeof (ELF_RELA32)+4);
				*tt++='(';
				/* type */
				tt=tag_make_desc_byte(tt, h+i*sizeof (ELF_RELA32)+4, ATOM_ELF_R_386_TYPE);
				*tt++=')';
				*tt=0;
				m->add_mask(t);
			}
			break;
		}
	}
	
	v->insertsub(m);
	return v;
}

format_viewer_if htelfreloctable_if = {
	htelfreloctable_init,
	0
};
