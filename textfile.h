/* 
 *	HT Editor
 *	textfile.h
 *
 *	Copyright (C) 2001, 2002 Stefan Weyergraf (stefan@weyergraf.de)
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

#ifndef __TEXTFILE_H__
#define __TEXTFILE_H__

#include "htdata.h"
#include "stream.h"
#include "syntax.h"

/*
 *	CLASS ht_textfile
 */
 
class ht_textfile: public ht_layer_streamfile {
public:
/* new */
	virtual	bool convert_ofs2line(FILEOFS o, UINT *line, UINT *pofs)=0;
	virtual	bool convert_line2ofs(UINT line, UINT pofs, FILEOFS *o)=0;
	virtual	void delete_lines(UINT line, UINT count)=0;
	virtual	void delete_chars(UINT line, UINT ofs, UINT count)=0;
	virtual	bool get_char(UINT line, char *ch, UINT pos)=0;
	virtual	bool getline(UINT line, UINT pofs, void *buf, UINT buflen, UINT *retlen, lexer_state *state)=0;
	virtual	UINT getlinelength(UINT line)=0;
	virtual	void insert_lines(UINT before, UINT count, void **line_ends = NULL, int *lineendlens = NULL)=0;
	virtual	void insert_chars(UINT line, UINT ofs, void *chars, UINT len)=0;
	virtual	bool has_line(UINT line)=0;
	virtual	UINT linecount()=0;
	virtual	void set_layered_assume(ht_streamfile *streamfile, bool changes_applied)=0;
	virtual	void set_lexer(ht_syntax_lexer *lexer)=0;
};

/*
 *	CLASS ht_layer_textfile
 */
 
class ht_layer_textfile: public ht_textfile {
public:
			void init(ht_textfile *textfile, bool own_textfile);
/* overwritten */
	virtual	bool convert_ofs2line(FILEOFS o, UINT *line, UINT *pofs);
	virtual	bool convert_line2ofs(UINT line, UINT pofs, FILEOFS *o);
	virtual	void delete_lines(UINT line, UINT count);
	virtual	void delete_chars(UINT line, UINT ofs, UINT count);
	virtual	bool get_char(UINT line, char *ch, UINT pos);
	virtual	bool getline(UINT line, UINT pofs, void *buf, UINT buflen, UINT *retlen, lexer_state *state);
	virtual	UINT getlinelength(UINT line);
	virtual	void insert_lines(UINT before, UINT count, void **line_ends, int *lineendlens);
	virtual	void insert_chars(UINT line, UINT ofs, void *chars, UINT len);
	virtual	bool has_line(UINT line);
	virtual	UINT linecount();
	virtual	void set_layered_assume(ht_streamfile *streamfile, bool changes_applied);
	virtual	void set_lexer(ht_syntax_lexer *lexer);
};

/*
 *	CLASS ht_ltextfile_line
 */

class ht_ltextfile_line: public ht_data {
public:
	virtual ~ht_ltextfile_line();

	lexer_state instate;
	struct {
		FILEOFS ofs;
		UINT len;
	} on_disk;
	bool is_in_memory;
	struct {
		char *data;
		UINT len;
	} in_memory;
	FILEOFS nofs;
	byte lineendlen;
	byte lineend[2];
};

/*
 *	CLASS ht_ltextfile
 */
 
class ht_ltextfile: public ht_textfile {
protected:
	FILEOFS ofs;
	ht_clist *lines;
	ht_clist *orig_lines;
	ht_syntax_lexer *lexer;
	UINT first_parse_dirty_line;
	UINT first_nofs_dirty_line;
	bool dirty;

			void cache_invd();
			void cache_flush();
			void dirty_nofs(UINT line);
			void dirty_parse(UINT line);
			UINT find_linelen_forwd(byte *buf, UINT maxbuflen, FILEOFS ofs, int *le_len);
	virtual	ht_ltextfile_line *fetch_line(UINT line);
			ht_ltextfile_line *fetch_line_nofs_ok(UINT line);
			ht_ltextfile_line *fetch_line_into_memory(UINT line);
			UINT getlinelength_i(ht_ltextfile_line *e);
			bool is_dirty_nofs(UINT line);
			bool is_dirty_parse(UINT line);
			byte *match_lineend_forwd(byte *buf, UINT buflen, int *le_len);
			lexer_state next_instate(UINT line);
			FILEOFS next_nofs(ht_ltextfile_line *l);
			void split_line(UINT a, UINT pos, void *line_end, int line_end_len);
			void update_nofs(UINT line);
			void update_parse(UINT line);
			void reread();
public:
			void	init(ht_streamfile *streamfile, bool own_streamfile, ht_syntax_lexer *lexer);
	virtual	void done();
/* overwritten (streamfile) */
	virtual	void	copy_to(ht_stream *stream);
	virtual	int	extend(UINT newsize);
	virtual	UINT	get_size();
	virtual	void	pstat(pstat_t *s);
	virtual	UINT	read(void *buf, UINT size);
	virtual	void set_layered(ht_streamfile *streamfile);
	virtual	int	seek(FILEOFS offset);
	virtual	FILEOFS tell();
	virtual	int	truncate(UINT newsize);
	virtual	int	vcntl(UINT cmd, va_list vargs);
	virtual	UINT	write(const void *buf, UINT size);
/* overwritten (textfile) */
	virtual	bool convert_ofs2line(FILEOFS o, UINT *line, UINT *pofs);
	virtual	bool convert_line2ofs(UINT line, UINT pofs, FILEOFS *o);
	virtual	void delete_lines(UINT line, UINT count);
	virtual	void delete_chars(UINT line, UINT ofs, UINT count);
	virtual	bool get_char(UINT line, char *ch, UINT pos);
	virtual	bool getline(UINT line, UINT pofs, void *buf, UINT buflen, UINT *retlen, lexer_state *state);
	virtual	UINT getlinelength(UINT line);
	virtual	void insert_lines(UINT before, UINT count, void **line_ends, int *lineendlens);
	virtual	void insert_chars(UINT line, UINT ofs, void *chars, UINT len);
	virtual	bool has_line(UINT line);
	virtual	UINT linecount();
	virtual	void set_layered_assume(ht_streamfile *streamfile, bool changes_applied);
	virtual	void set_lexer(ht_syntax_lexer *lexer);
};

#endif /* __TEXTFILE_H__ */
