/*
 *	HT Editor
 *	pe_analy.cc
 *
 *	Copyright (C) 1999-2002 Sebastian Biallas (sb@web-productions.de)
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

#include "analy.h"
#include "analy_alpha.h"
#include "analy_il.h"
#include "analy_names.h"
#include "analy_register.h"
#include "analy_x86.h"
#include "global.h"
#include "pe_analy.h"

#include "htctrl.h"
#include "htdebug.h"
#include "htiobox.h"
#include "htpe.h"
#include "htstring.h"
#include "pestruct.h"
#include "x86asm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *
 */
void	PEAnalyser::init(ht_pe_shared_data *Pe_shared, ht_streamfile *File)
{
	pe_shared = Pe_shared;
	file = File;

	validarea = new Area();
	validarea->init();

	Analyser::init();
}


/*
 *
 */
int	PEAnalyser::load(ht_object_stream *f)
{
	/*
	ht_pe_shared_data 	*pe_shared;
	ht_stream 		*file;
	area				*validarea;
	*/
	GET_OBJECT(f, validarea);
	return Analyser::load(f);
}

/*
 *
 */
void	PEAnalyser::done()
{
	validarea->done();
	delete validarea;
	Analyser::done();
}

/*
 *
 */
void PEAnalyser::beginAnalysis()
{
	char	buffer[1024];

	setLocationTreeOptimizeThreshold(100);
	setSymbolTreeOptimizeThreshold(100);
	/*
	 *	entrypoint
	 */
	Address *entry=createAddress32(pe_shared->pe32.header.entrypoint_address+pe_shared->pe32.header_nt.image_base);

	pushAddress(entry, entry);
	
	/*
	 * give all sections a descriptive comment:
	 */

	/*struct PE_SECTION_HEADER {
		byte name[PE_SIZEOF_SHORT_NAME] __attribute__ ((packed));
		dword data_vsize __attribute__ ((packed));
		dword data_address __attribute__ ((packed));
		dword data_size __attribute__	((packed));
		dword data_offset __attribute__ ((packed));
		dword relocation_offset __attribute__ ((packed));
		dword linenumber_offset __attribute__ ((packed));
		word relocation_count __attribute__ ((packed));
		word linenumber_count __attribute__ ((packed));
		dword characteristics __attribute__ ((packed));
	};*/
	COFF_SECTION_HEADER *s=pe_shared->sections.sections;
	char blub[100];
	for (UINT i=0; i<pe_shared->sections.section_count; i++) {
		Address *secaddr = createAddress32(s->data_address+pe_shared->pe32.header_nt.image_base);
		sprintf(blub, ";  section %d <%s>", i+1, getSegmentNameByAddress(secaddr));
		addComment(secaddr, 0, "");
		addComment(secaddr, 0, ";******************************************************************");
		addComment(secaddr, 0, blub);
		sprintf(blub, ";  virtual address  %08x  virtual size   %08x", s->data_address, s->data_vsize);
		addComment(secaddr, 0, blub);
		sprintf(blub, ";  file offset      %08x  file size      %08x", s->data_offset, s->data_size);
		addComment(secaddr, 0, blub);
		addComment(secaddr, 0, ";******************************************************************");

		// mark end of sections
		sprintf(blub, ";  end of section <%s>", getSegmentNameByAddress(secaddr));
		Address *secend_addr = (Address *)secaddr->duplicate();
		secend_addr->add(MAX(s->data_size, s->data_vsize));
		newLocation(secend_addr)->flags |= AF_FUNCTION_END;
		addComment(secend_addr, 0, "");
		addComment(secend_addr, 0, ";******************************************************************");
		addComment(secend_addr, 0, blub);
		addComment(secend_addr, 0, ";******************************************************************");

		validarea->add(secaddr, secend_addr);
		Address *seciniaddr = (Address *)secaddr->duplicate();
		seciniaddr->add(MIN(s->data_size, s->data_vsize));
		if (validAddress(secaddr, scinitialized) && validAddress(seciniaddr, scinitialized)) {
			initialized->add(secaddr, seciniaddr);
		}
		s++;
		delete secaddr;
		delete secend_addr;
		delete seciniaddr;
	}

	// exports

	int export_count=pe_shared->exports.funcs->count();
	int *entropy = random_permutation(export_count);
	for (int i=0; i<export_count; i++) {
		ht_pe_export_function *f=(ht_pe_export_function *)pe_shared->exports.funcs->get(*(entropy+i));
		Address *faddr = createAddress32(f->address);
		if (validAddress(faddr, scvalid)) {
			char *label;
			if (f->byname) {
				sprintf(buffer, "; exported function %s, ordinal %04x", f->name, f->ordinal);
			} else {
				sprintf(buffer, "; unnamed exported function, ordinal %04x", f->ordinal);
			}
			label = export_func_name((f->byname) ? f->name : NULL, f->ordinal);
			addComment(faddr, 0, "");
			addComment(faddr, 0, ";********************************************************");
			addComment(faddr, 0, buffer);
			addComment(faddr, 0, ";********************************************************");
			pushAddress(faddr, faddr);
			assignSymbol(faddr, label, label_func);
			free(label);
		}
		delete faddr;
	}
	if (entropy) free(entropy);

	int import_count=pe_shared->imports.funcs->count();
	entropy = random_permutation(import_count);
	for (int i=0; i<import_count; i++) {
		ht_pe_import_function *f=(ht_pe_import_function *)pe_shared->imports.funcs->get(*(entropy+i));
		ht_pe_import_library *d=(ht_pe_import_library *)pe_shared->imports.libs->get(f->libidx);
		char *label;
		label = import_func_name(d->name, (f->byname) ? f->name.name : NULL, f->ordinal);
		Address *faddr = createAddress32(f->address);
		addComment(faddr, 0, "");
		if (!assignSymbol(faddr, label, label_func)) {
			// multiple import of a function (duplicate labelname)
			// -> mangle name a bit more
			addComment(faddr, 0, "; duplicate import");               
			sprintf(buffer, "%s_%x", label, f->address);
			assignSymbol(faddr, buffer, label_func);
		}
		data->setIntAddressType(faddr, dst_idword, 4);
		free(label);
		delete faddr;
	}
	if (entropy) free(entropy);

	int dimport_count=pe_shared->dimports.funcs->count();
	entropy = random_permutation(dimport_count);
	for (int i=0; i<dimport_count; i++) {
		// FIXME: delay imports need work (push addr)
		ht_pe_import_function *f=(ht_pe_import_function *)pe_shared->dimports.funcs->get(*(entropy+i));
		ht_pe_import_library *d=(ht_pe_import_library *)pe_shared->dimports.libs->get(f->libidx);
		if (f->byname) {
			sprintf(buffer, "; delay import function loader for %s, ordinal %04x", f->name.name, f->ordinal);
		} else {
			sprintf(buffer, "; delay import function loader for ordinal %04x", f->ordinal);
		}
		char *label;
		label = import_func_name(d->name, f->byname ? f->name.name : NULL, f->ordinal);
		Address *faddr = createAddress32(f->address);
		addComment(faddr, 0, "");
		addComment(faddr, 0, ";********************************************************");
		addComment(faddr, 0, buffer);
		addComment(faddr, 0, ";********************************************************");
		assignSymbol(faddr, label, label_func);
		free(label);
		delete faddr;
	}
	if (entropy) free(entropy);

	addComment(entry, 0, "");
	addComment(entry, 0, ";****************************");
	if (pe_shared->coffheader.characteristics & COFF_DLL) {
		addComment(entry, 0, ";  dll entry point");
	} else {
		addComment(entry, 0, ";  program entry point");
	}
	addComment(entry, 0, ";****************************");
	assignSymbol(entry, "entrypoint", label_func);

	setLocationTreeOptimizeThreshold(1000);
	setSymbolTreeOptimizeThreshold(1000);
	delete entry;

	Analyser::beginAnalysis();
}

/*
 *
 */
OBJECT_ID	PEAnalyser::object_id()
{
	return ATOM_PE_ANALYSER;
}

/*
 *
 */
UINT PEAnalyser::bufPtr(Address *Addr, byte *buf, int size)
{
	FILEOFS ofs = addressToFileofs(Addr);
/*	if (ofs == INVALID_FILE_OFS) {
		int as=0;
	}*/
	assert(ofs != INVALID_FILE_OFS);
	file->seek(ofs);
	return file->read(buf, size);
}

bool PEAnalyser::convertAddressToRVA(Address *addr, RVA *r)
{
	OBJECT_ID oid = addr->object_id();
	if (oid==ATOM_ADDRESS_FLAT_32) {
		*r = ((AddressFlat32*)addr)->addr - pe_shared->pe32.header_nt.image_base;
		return true;
	} else if (oid == ATOM_ADDRESS_X86_FLAT_32) {
		*r = ((AddressX86Flat32*)addr)->addr - pe_shared->pe32.header_nt.image_base;
		return true;
	} else if (oid == ATOM_ADDRESS_ALPHA_FLAT_32) {
		*r = ((AddressAlphaFlat32*)addr)->addr - pe_shared->pe32.header_nt.image_base;
		return true;
     }
	return false;
}

/*
 *
 */
Address *PEAnalyser::createAddress32(dword addr)
{
	switch (pe_shared->coffheader.machine) {
		case COFF_MACHINE_I386:
		case COFF_MACHINE_I486:
		case COFF_MACHINE_I586:
			return new AddressX86Flat32(addr);
		case COFF_MACHINE_ALPHA:
			return new AddressAlphaFlat32(addr);
	}
	// fallback to standard-addrs
	return new AddressFlat32(addr);
}

/*
 *
 */
Address *PEAnalyser::createAddress64(dword high_addr, dword low_addr)
{
	assert(0);
	return NULL;
}

Address *PEAnalyser::createAddress()
{
	switch (pe_shared->coffheader.machine) {
		case COFF_MACHINE_I386:
		case COFF_MACHINE_I486:
		case COFF_MACHINE_I586:
			return new AddressX86Flat32();
		case COFF_MACHINE_ALPHA:
			return new AddressAlphaFlat32();
	}
	return new AddressX86Flat32();
}

/*
 *
 */
Assembler *PEAnalyser::createAssembler()
{
	switch (pe_shared->coffheader.machine) {
		case COFF_MACHINE_I386:
		case COFF_MACHINE_I486:
		case COFF_MACHINE_I586:
			Assembler *a = new x86asm(X86_OPSIZE32, X86_ADDRSIZE32);
			a->init();
			return a;
	}
	return NULL;
}

/*
 *
 */
FILEOFS PEAnalyser::addressToFileofs(Address *Addr)
{
/*     char tbuf[1024];
	Addr->stringify(tbuf, 1024, 0);
	printf("ADDR=%s", tbuf);*/
	if (validAddress(Addr, scinitialized)) {
//     	printf(" v1\n");
		FILEOFS ofs;
		RVA r;
		if (!convertAddressToRVA(Addr, &r)) return INVALID_FILE_OFS;
		if (!pe_rva_to_ofs(&pe_shared->sections, r, &ofs)) return INVALID_FILE_OFS;
		return ofs;
	} else {
//     	printf(" IV1\n");
		return INVALID_FILE_OFS;
	}
}

/*
 *
 */
char *PEAnalyser::getSegmentNameByAddress(Address *Addr)
{
	static char sectionname[9];
	pe_section_headers *sections=&pe_shared->sections;
	int i;
	RVA r;
//	Addr-=pe_shared->pe32.header_nt.image_base;
	if (!convertAddressToRVA(Addr, &r)) return NULL;
	pe_rva_to_section(sections, r, &i);
	COFF_SECTION_HEADER *s=sections->sections+i;
	if (!pe_rva_is_valid(sections, r)) return NULL;
	memmove(sectionname, s->name, 8);
	sectionname[8]=0;
	return sectionname;
}

/*
 *
 */
char	*PEAnalyser::getName()
{
	return file->get_desc();
}

/*
 *
 */
char *PEAnalyser::getType()
{
	return "PE/Analyser";
}

/*
 *
 */
void PEAnalyser::initCodeAnalyser()
{
	Analyser::initCodeAnalyser();
}

static char *string_func(dword ofs, void *context)
{
	char str[1024];
	static char str2[1024];
	ht_pe_shared_data *pe = (ht_pe_shared_data*)context;
	if (ofs < pe->il->string_pool_size) {
		dword length;
		dword o = ILunpackDword(length, (byte*)&pe->il->string_pool[ofs], 10);
		wide_char_to_multi_byte(str, (byte*)&pe->il->string_pool[ofs+o], length/2+1);
		escape_special_str(str2, sizeof str2, str, "\"");
		return str2;
	} else {
		return NULL;
	}
}

static char *token_func(dword token, void *context)
{
	static char tokenstr[1024];
	ht_pe_shared_data *pe = (ht_pe_shared_data*)context;
	switch (token & IL_META_TOKEN_MASK) {
		case IL_META_TOKEN_TYPE_REF:
		case IL_META_TOKEN_TYPE_DEF: {
			sprintf(tokenstr, "typedef");
			break;
		}
		case IL_META_TOKEN_FIELD_DEF: {
			sprintf(tokenstr, "fielddef");
			break;
		}
		case IL_META_TOKEN_METHOD_DEF: {
			sprintf(tokenstr, "methoddef");
			break;
		}
		case IL_META_TOKEN_MEMBER_REF: {
			sprintf(tokenstr, "memberref");
			break;
		}
		case IL_META_TOKEN_TYPE_SPEC: {
			sprintf(tokenstr, "typespec");
			break;
		}
		default:
			return NULL;
	}
	return tokenstr;
}

/*
 *
 */
void PEAnalyser::initUnasm()
{
	DPRINTF("pe_analy: ");
	if (pe_shared->il) {
		analy_disasm = new AnalyILDisassembler();
		((AnalyILDisassembler *)analy_disasm)->init(this, string_func, token_func, pe_shared);
	} else {
	switch (pe_shared->coffheader.machine) {
		case COFF_MACHINE_I386:	// Intel 386
		case COFF_MACHINE_I486:	// Intel 486
		case COFF_MACHINE_I586:	// Intel 586
			DPRINTF("initing analy_x86_disassembler\n");
			analy_disasm = new AnalyX86Disassembler();
			((AnalyX86Disassembler *)analy_disasm)->init(this, false, false);
			break;
		case COFF_MACHINE_R3000:	// MIPS little-endian, 0x160 big-endian
			DPRINTF("no apropriate disassembler for MIPS\n");
			warnbox("No disassembler for MIPS!");
			break;
		case COFF_MACHINE_R4000:	// MIPS little-endian
			DPRINTF("no apropriate disassembler for MIPS\n");
			warnbox("No disassembler for MIPS!");
			break;
		case COFF_MACHINE_R10000:	// MIPS little-endian
			DPRINTF("no apropriate disassembler for MIPS\n");
			warnbox("No disassembler for MIPS!");
			break;
		case COFF_MACHINE_ALPHA:	// Alpha_AXP
			DPRINTF("initing alpha_axp_disassembler\n");
			analy_disasm = new AnalyAlphaDisassembler();
			((AnalyAlphaDisassembler *)analy_disasm)->init(this);
			break;
		case COFF_MACHINE_POWERPC:	// IBM PowerPC Little-Endian
			DPRINTF("no apropriate disassembler for POWER PC\n");
			warnbox("No disassembler for POWER PC!");
			break;
		case COFF_MACHINE_UNKNOWN:
		default:
			DPRINTF("no apropriate disassembler for machine %04x\n", pe_shared->coffheader.machine);
			warnbox("No disassembler for unknown machine type %04x!", pe_shared->coffheader.machine);
	}
	}
}

/*
 *
 */
void PEAnalyser::log(const char *msg)
{
	/*
	 *	log() creates to much traffic so dont log
	 *   perhaps we reactivate this later
	 *
	 */
/*	LOG(msg);*/
}

/*
 *
 */
Address *PEAnalyser::nextValid(Address *Addr)
{
	return (Address *)validarea->findNext(Addr);
}

/*
 *
 */
void PEAnalyser::store(ht_object_stream *st)
{
	/*
	ht_pe_shared_data 	*pe_shared;
	ht_stream 		*file;
	area				*validarea;
	*/
	PUT_OBJECT(st, validarea);
	Analyser::store(st);
}

/*
 *
 */
int	PEAnalyser::queryConfig(int mode)
{
	switch (mode) {
		case Q_DO_ANALYSIS:
		case Q_ENGAGE_CODE_ANALYSER:
		case Q_ENGAGE_DATA_ANALYSER:
			return true;
		default:
			return 0;
	}
}

/*
 *
 */
Address *PEAnalyser::fileofsToAddress(FILEOFS fileofs)
{
	RVA r;
	if (pe_ofs_to_rva(&pe_shared->sections, fileofs, &r)) {
		return createAddress32(r+pe_shared->pe32.header_nt.image_base);
	} else {
		return new InvalidAddress();
	}
}

/*
 *
 */
bool PEAnalyser::validAddress(Address *Addr, tsectype action)
{
	pe_section_headers *sections=&pe_shared->sections;
	int sec;
	RVA r;
	if (!convertAddressToRVA(Addr, &r)) return false;
	if (!pe_rva_to_section(sections, r, &sec)) return false;
	COFF_SECTION_HEADER *s=sections->sections+sec;
	switch (action) {
		case scvalid:
			return true;
		case scread:
			return s->characteristics & COFF_SCN_MEM_READ;
		case scwrite:
			return s->characteristics & COFF_SCN_MEM_WRITE;
		case screadwrite:
			return s->characteristics & COFF_SCN_MEM_WRITE;
		case sccode:
			// FIXME: EXECUTE vs. CNT_CODE ?
			if (!pe_rva_is_physical(sections, r)) return false;
			return (s->characteristics & (COFF_SCN_MEM_EXECUTE | COFF_SCN_CNT_CODE));
		case scinitialized:
			if (!pe_rva_is_physical(sections, r)) return false;
			return !(s->characteristics & COFF_SCN_CNT_UNINITIALIZED_DATA);
	}
	return false;
}

