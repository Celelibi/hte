/* 
 *	HT Editor
 *	analy_alpha.h
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

#ifndef ANALY_ALPHA_H
#define ANALY_ALPHA_H

#include "analy.h"
#include "alphadis.h"

class analy_alpha_disassembler: public analy_disassembler {
public:
			void			init(analyser *A);
			int 			load(ht_object_stream *f);
	virtual   void	     	done();
	virtual	OBJECT_ID		object_id();

	virtual	ADDR			branch_addr(OPCODE *opcode, tbranchtype branchtype, bool examine);
	virtual	void			examine_opcode(OPCODE *opcode);
	virtual	void			init_disasm();
	virtual	tbranchtype 	is_branch(OPCODE *opcode);
	virtual	void			store(ht_object_stream *f);
};

#endif
