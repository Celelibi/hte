/* 
 *	HT Editor
 *	httag.h
 *
 *	Copyright (C) 1999-2002 Stefan Weyergraf (stefan@weyergraf.de)
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

#ifndef __HTTAG_H__
#define __HTTAG_H__

#include "global.h"
#include "htio.h"
#include "stream.h"

/* SELECTION-TAG */
#define HT_TAG_SEL					0x01
#define HT_TAG_SEL_LEN(n)			(sizeof(ht_tag_sel)+n)
#define HT_TAG_SEL_VLEN(n)			(n)

#define HT_STATICTAG_SEL				0x81
#define HT_STATICTAG_SEL_CH			"\e\x81"

struct ht_tag_sel {
	byte escape HTPACKED;
	byte magic HTPACKED;
	ID id128_1 HTPACKED;
	ID id128_2 HTPACKED;
	ID id128_3 HTPACKED;
	ID id128_4 HTPACKED;
	byte strlen HTPACKED;
};

/* FLAGS-TAG */
#define HT_TAG_FLAGS				0x02
#define HT_TAG_FLAGS_LEN           	sizeof(ht_tag_flags)
#define HT_TAG_FLAGS_VLEN			1

#define HT_STATICTAG_FLAGS			0x82
#define HT_STATICTAG_FLAGS_CH			"\e\x82"

struct ht_tag_flags {
	byte escape HTPACKED;
	byte magic HTPACKED;
	FILEOFS offset HTPACKED;
	ID id HTPACKED;
};

struct ht_tag_flags_s {
	char bitidx HTPACKED;
	char *desc HTPACKED;
};

/* GROUP-TAG */
#define HT_TAG_GROUP				0x03
#define HT_TAG_GROUP_LEN				sizeof(ht_tag_group)

#define HT_STATICTAG_GROUP			0x83
#define HT_STATICTAG_GROUP_CH			"\e\x83"

struct ht_tag_group {
	byte escape HTPACKED;
	byte magic HTPACKED;
};

/* COLOR-TAG */
#define HT_TAG_COLOR				0x04
#define HT_TAG_COLOR_LEN				sizeof(ht_tag_color)

#define HT_STATICTAG_COLOR			0x84
#define HT_STATICTAG_COLOR_CH			"\e\x84"

struct ht_tag_color {
	byte escape HTPACKED;
	byte magic HTPACKED;
	dword color HTPACKED;
};

/* EDIT-BYTE-TAG */
#define HT_TAG_EDIT_BYTE				0x10
#define HT_TAG_EDIT_BYTE_LEN			sizeof(ht_tag_edit_byte)
#define HT_TAG_EDIT_BYTE_VLEN			(2)
#define HT_TAG_EDIT_BYTE_SIZE			(1)

#define HT_STATICTAG_EDIT_BYTE		0x90
#define HT_STATICTAG_EDIT_BYTE_CH		"\e\x90"

struct ht_tag_edit_byte {
	byte escape HTPACKED;
	byte magic HTPACKED;
	FILEOFS offset HTPACKED;
};

/* EDIT WORD */

struct ht_tag_edit_word_generic {
	byte escape HTPACKED;
	byte magic HTPACKED;
	FILEOFS offset HTPACKED;
};

/* EDIT DWORD */

struct ht_tag_edit_dword_generic {
	byte escape HTPACKED;
	byte magic HTPACKED;
	FILEOFS offset HTPACKED;
};

/* EDIT QWORD */

struct ht_tag_edit_qword_generic {
	byte escape HTPACKED;
	byte magic HTPACKED;
	FILEOFS offset HTPACKED;
};

/* EDIT-WORD-TAG (little-endian) */
#define HT_TAG_EDIT_WORD_LE			0x11
#define HT_TAG_EDIT_WORD_LE_LEN		sizeof(ht_tag_edit_word_generic)
#define HT_TAG_EDIT_WORD_LE_VLEN		(4)
#define HT_TAG_EDIT_WORD_LE_SIZE		(2)

#define HT_STATICTAG_EDIT_WORD_LE		0x91
#define HT_STATICTAG_EDIT_WORD_LE_CH	"\e\x91"

/* EDIT-DWORD-TAG (little-endian) */
#define HT_TAG_EDIT_DWORD_LE			0x12
#define HT_TAG_EDIT_DWORD_LE_LEN		sizeof(ht_tag_edit_dword_generic)
#define HT_TAG_EDIT_DWORD_LE_VLEN		(8)
#define HT_TAG_EDIT_DWORD_LE_SIZE		(4)

#define HT_STATICTAG_EDIT_DWORD_LE		0x92
#define HT_STATICTAG_EDIT_DWORD_LE_CH	"\e\x92"

/* EDIT-QWORD-TAG (little-endian) */
#define HT_TAG_EDIT_QWORD_LE			0x13
#define HT_TAG_EDIT_QWORD_LE_LEN		sizeof(ht_tag_edit_qword_generic)
#define HT_TAG_EDIT_QWORD_LE_VLEN		(16)
#define HT_TAG_EDIT_QWORD_LE_SIZE		(8)

#define HT_STATICTAG_EDIT_QWORD_LE		0x93
#define HT_STATICTAG_EDIT_QWORD_LE_CH	"\e\x93"

/* EDIT-WORD-TAG (big-endian) */
#define HT_TAG_EDIT_WORD_BE			0x14
#define HT_TAG_EDIT_WORD_BE_LEN		sizeof(ht_tag_edit_word_generic)
#define HT_TAG_EDIT_WORD_BE_VLEN		(4)
#define HT_TAG_EDIT_WORD_BE_SIZE		(2)

#define HT_STATICTAG_EDIT_WORD_BE		0x94
#define HT_STATICTAG_EDIT_WORD_BE_CH	"\e\x94"

/* EDIT-DWORD-TAG (big-endian) */
#define HT_TAG_EDIT_DWORD_BE			0x15
#define HT_TAG_EDIT_DWORD_BE_LEN		sizeof(ht_tag_edit_dword_generic)
#define HT_TAG_EDIT_DWORD_BE_VLEN		(8)
#define HT_TAG_EDIT_DWORD_BE_SIZE		(4)

#define HT_STATICTAG_EDIT_DWORD_BE		0x95
#define HT_STATICTAG_EDIT_DWORD_BE_CH	"\e\x95"

/* EDIT-QWORD-TAG (big-endian) */
#define HT_TAG_EDIT_QWORD_BE			0x16
#define HT_TAG_EDIT_QWORD_BE_LEN		sizeof(ht_tag_edit_qword_generic)
#define HT_TAG_EDIT_QWORD_BE_VLEN		(16)
#define HT_TAG_EDIT_QWORD_BE_SIZE		(8)

#define HT_STATICTAG_EDIT_QWORD_BE		0x96
#define HT_STATICTAG_EDIT_QWORD_BE_CH	"\e\x96"

/* EDIT-WORD-TAG (var-endian) */
#define HT_TAG_EDIT_WORD_VE			0x17
#define HT_TAG_EDIT_WORD_VE_LEN		sizeof(ht_tag_edit_word_generic)
#define HT_TAG_EDIT_WORD_VE_VLEN		(4)
#define HT_TAG_EDIT_WORD_VE_SIZE		(2)

#define HT_STATICTAG_EDIT_WORD_VE		0x97
#define HT_STATICTAG_EDIT_WORD_VE_CH	"\e\x97"

/* EDIT-DWORD-TAG (var-endian) */
#define HT_TAG_EDIT_DWORD_VE			0x18
#define HT_TAG_EDIT_DWORD_VE_LEN		sizeof(ht_tag_edit_dword_generic)
#define HT_TAG_EDIT_DWORD_VE_VLEN		(8)
#define HT_TAG_EDIT_DWORD_VE_SIZE		(4)

#define HT_STATICTAG_EDIT_DWORD_VE		0x98
#define HT_STATICTAG_EDIT_DWORD_VE_CH	"\e\x98"

/* EDIT-QWORD-TAG (var-endian) */
#define HT_TAG_EDIT_QWORD_VE			0x19
#define HT_TAG_EDIT_QWORD_VE_LEN		sizeof(ht_tag_edit_qword_generic)
#define HT_TAG_EDIT_QWORD_VE_VLEN		(16)
#define HT_TAG_EDIT_QWORD_VE_SIZE		(8)

#define HT_STATICTAG_EDIT_QWORD_VE		0x99
#define HT_STATICTAG_EDIT_QWORD_VE_CH	"\e\x99"

/* EDIT-TIME-TAG */
#define HT_TAG_EDIT_TIME				0x1a
#define HT_TAG_EDIT_TIME_LEN			sizeof(ht_tag_edit_time)
#define HT_TAG_EDIT_TIME_VLEN			(19)
#define HT_TAG_EDIT_TIME_SIZE			(4)

#define HT_STATICTAG_EDIT_TIME		0x9a
#define HT_STATICTAG_EDIT_TIME_CH		"\e\x9a"

struct ht_tag_edit_time {
	byte escape HTPACKED;
	byte magic HTPACKED;
	FILEOFS offset HTPACKED;
};

/* EDIT-CHAR-TAG */
#define HT_TAG_EDIT_CHAR				0x1b
#define HT_TAG_EDIT_CHAR_LEN			sizeof(ht_tag_edit_char)
#define HT_TAG_EDIT_CHAR_VLEN			(1)
#define HT_TAG_EDIT_CHAR_SIZE			(1)

#define HT_STATICTAG_EDIT_CHAR 		0x9b
#define HT_STATICTAG_EDIT_CHAR_CH		"\e\x9b"

struct ht_tag_edit_char {
	byte escape HTPACKED;
	byte magic HTPACKED;
	FILEOFS offset HTPACKED;
};

/* EDIT-BIT-TAG */
#define HT_TAG_EDIT_BIT				0x1c
#define HT_TAG_EDIT_BIT_LEN			sizeof(ht_tag_edit_bit)
#define HT_TAG_EDIT_BIT_VLEN			(1)
#define HT_TAG_EDIT_BIT_SIZE			(1)

#define HT_STATICTAG_EDIT_BIT 		0x9c
#define HT_STATICTAG_EDIT_BIT_CH		"\e\x9c"

struct ht_tag_edit_bit {
	byte escape HTPACKED;
	byte magic HTPACKED;
	FILEOFS offset HTPACKED;
	byte bitidx HTPACKED;
};

/* EDIT-SELVIS-TAG */
#define HT_TAG_EDIT_SELVIS			0x1d
#define HT_TAG_EDIT_SELVIS_LEN		sizeof(ht_tag_edit_selvis)
#define HT_TAG_EDIT_SELVIS_VLEN		(1)

#define HT_STATICTAG_EDIT_SELVIS		0x9d
#define HT_STATICTAG_EDIT_SELVIS_CH	"\e\x9d"

struct ht_tag_edit_selvis {
	byte escape HTPACKED;
	byte magic HTPACKED;
	FILEOFS offset HTPACKED;
	char ch HTPACKED;
};

/* DESC-BYTE-TAG */
#define HT_TAG_DESC_BYTE				0x20
#define HT_TAG_DESC_BYTE_LEN			sizeof(ht_tag_desc_byte)

#define HT_STATICTAG_DESC_BYTE		0xa0
#define HT_STATICTAG_DESC_BYTE_CH		"\e\xa0"

struct ht_tag_desc_byte {
	byte escape HTPACKED;
	byte magic HTPACKED;
	ID id HTPACKED;
	FILEOFS offset HTPACKED;
};

/* DESC WORD */
struct ht_tag_desc_word_generic {
	byte escape HTPACKED;
	byte magic HTPACKED;
	ID id HTPACKED;
	FILEOFS offset HTPACKED;
};

/* DESC DWORD */
struct ht_tag_desc_dword_generic {
	byte escape HTPACKED;
	byte magic HTPACKED;
	ID id HTPACKED;
	FILEOFS offset HTPACKED;
};

/* DESC QWORD */
struct ht_tag_desc_qword_generic {
	byte escape HTPACKED;
	byte magic HTPACKED;
	ID id HTPACKED;
	FILEOFS offset HTPACKED;
};

/* DESC-WORD-TAG (little-endian) */
#define HT_TAG_DESC_WORD_LE			0x21
#define HT_TAG_DESC_WORD_LE_LEN		sizeof(ht_tag_desc_word_generic)

#define HT_STATICTAG_DESC_WORD_LE		0xa1
#define HT_STATICTAG_DESC_WORD_LE_CH	"\e\xa1"

/* DESC-DWORD-TAG (little-endian) */
#define HT_TAG_DESC_DWORD_LE			0x22
#define HT_TAG_DESC_DWORD_LE_LEN		sizeof(ht_tag_desc_dword_generic)

#define HT_STATICTAG_DESC_DWORD_LE		0xa2
#define HT_STATICTAG_DESC_DWORD_LE_CH	"\e\xa2"

/* DESC-QWORD-TAG (little-endian) */
#define HT_TAG_DESC_QWORD_LE			0x23
#define HT_TAG_DESC_QWORD_LE_LEN		sizeof(ht_tag_desc_qword_generic)

#define HT_STATICTAG_DESC_QWORD_LE		0xa3
#define HT_STATICTAG_DESC_QWORD_LE_CH	"\e\xa3"

/* DESC-WORD-TAG (big-endian) */
#define HT_TAG_DESC_WORD_BE			0x24
#define HT_TAG_DESC_WORD_BE_LEN		sizeof(ht_tag_desc_word_generic)

#define HT_STATICTAG_DESC_WORD_BE		0xa4
#define HT_STATICTAG_DESC_WORD_BE_CH	"\e\xa4"

/* DESC-DWORD-TAG (big-endian) */
#define HT_TAG_DESC_DWORD_BE			0x25
#define HT_TAG_DESC_DWORD_BE_LEN		sizeof(ht_tag_desc_dword_generic)

#define HT_STATICTAG_DESC_DWORD_BE		0xa5
#define HT_STATICTAG_DESC_DWORD_BE_CH	"\e\xa5"

/* DESC-QWORD-TAG (big-endian) */
#define HT_TAG_DESC_QWORD_BE			0x26
#define HT_TAG_DESC_QWORD_BE_LEN		sizeof(ht_tag_desc_qword_generic)

#define HT_STATICTAG_DESC_QWORD_BE		0xa6
#define HT_STATICTAG_DESC_QWORD_BE_CH	"\e\xa6"

/* DESC-WORD-TAG (var-endian) */
#define HT_TAG_DESC_WORD_VE			0x27
#define HT_TAG_DESC_WORD_VE_LEN		sizeof(ht_tag_desc_word_generic)

#define HT_STATICTAG_DESC_WORD_VE		0xa7
#define HT_STATICTAG_DESC_WORD_VE_CH	"\e\xa7"

/* DESC-DWORD-TAG (var-endian) */
#define HT_TAG_DESC_DWORD_VE			0x28
#define HT_TAG_DESC_DWORD_VE_LEN		sizeof(ht_tag_desc_dword_generic)

#define HT_STATICTAG_DESC_DWORD_VE		0xa8
#define HT_STATICTAG_DESC_DWORD_VE_CH	"\e\xa8"

/* DESC-QWORD-TAG (var-endian) */
#define HT_TAG_DESC_QWORD_VE			0x29
#define HT_TAG_DESC_QWORD_VE_LEN		sizeof(ht_tag_desc_qword_generic)

#define HT_STATICTAG_DESC_QWORD_VE		0xa9
#define HT_STATICTAG_DESC_QWORD_VE_CH	"\e\xa9"

enum tag_endian { tag_endian_big, tag_endian_little, tag_endian_var };

/*
 *	tag palette
 */

#define palkey_tags_default					"default"

#define palidx_tags_edit_tag_cursor_select		0
#define palidx_tags_edit_tag_cursor_edit		1
#define palidx_tags_edit_tag_cursor_unfocused	2
#define palidx_tags_edit_tag_selected			3
#define palidx_tags_edit_tag_modified			4
#define palidx_tags_edit_tag					5
#define palidx_tags_sel_tag_cursor_focused		6
#define palidx_tags_sel_tag_cursor_unfocused		7
#define palidx_tags_sel_tag					8

#define STATICTAG_SEL(len8, str) HT_STATICTAG_SEL_CH "0000000000000000" "0000000000000000" len8 str
#define STATICTAG_REF(id64, len8, str) HT_STATICTAG_SEL_CH id64 "0000000000000000" len8 str
#define STATICTAG_FLAGS(ofs32, id32) HT_STATICTAG_FLAGS_CH ofs32 id32
#define STATICTAG_GROUP() HT_STATICTAG_GROUP_CH
#define STATICTAG_COLOR(color8) HT_STATICTAG_COLOR_CH color8

#define STATICTAG_EDIT_BYTE(ofs32) HT_STATICTAG_EDIT_BYTE_CH ofs32
#define STATICTAG_EDIT_WORD_LE(ofs32) HT_STATICTAG_EDIT_WORD_LE_CH ofs32
#define STATICTAG_EDIT_DWORD_LE(ofs32) HT_STATICTAG_EDIT_DWORD_LE_CH ofs32
#define STATICTAG_EDIT_QWORD_LE(ofs32) HT_STATICTAG_EDIT_QWORD_LE_CH ofs32
#define STATICTAG_EDIT_WORD_BE(ofs32) HT_STATICTAG_EDIT_WORD_BE_CH ofs32
#define STATICTAG_EDIT_DWORD_BE(ofs32) HT_STATICTAG_EDIT_DWORD_BE_CH ofs32
#define STATICTAG_EDIT_QWORD_BE(ofs32) HT_STATICTAG_EDIT_QWORD_BE_CH ofs32
#define STATICTAG_EDIT_WORD_VE(ofs32) HT_STATICTAG_EDIT_WORD_VE_CH ofs32
#define STATICTAG_EDIT_DWORD_VE(ofs32) HT_STATICTAG_EDIT_DWORD_VE_CH ofs32
#define STATICTAG_EDIT_QWORD_VE(ofs32) HT_STATICTAG_EDIT_QWORD_VE_CH ofs32
#define STATICTAG_EDIT_TIME(ofs32) HT_STATICTAG_EDIT_TIME_CH ofs32
#define STATICTAG_EDIT_CHAR(ofs32) HT_STATICTAG_EDIT_CHAR_CH ofs32
#define STATICTAG_EDIT_BIT(ofs32, bitidx8) HT_STATICTAG_EDIT_BIT_CH ofs32 bitidx8
#define STATICTAG_EDIT_SELVIS(ofs32, char8) HT_STATICTAG_EDIT_SELVIS_CH ofs32 char8

#define STATICTAG_DESC_BYTE(ofs32, id32) HT_STATICTAG_DESC_BYTE_CH ofs32 id32
#define STATICTAG_DESC_WORD_LE(ofs32, id32) HT_STATICTAG_DESC_WORD_LE_CH ofs32 id32
#define STATICTAG_DESC_DWORD_LE(ofs32, id32) HT_STATICTAG_DESC_DWORD_LE_CH ofs32 id32
#define STATICTAG_DESC_QWORD_LE(ofs32, id32) HT_STATICTAG_DESC_QWORD_LE_CH ofs32 id32
#define STATICTAG_DESC_WORD_BE(ofs32, id32) HT_STATICTAG_DESC_WORD_BE_CH ofs32 id32
#define STATICTAG_DESC_DWORD_BE(ofs32, id32) HT_STATICTAG_DESC_DWORD_BE_CH ofs32 id32
#define STATICTAG_DESC_QWORD_BE(ofs32, id32) HT_STATICTAG_DESC_QWORD_BE_CH ofs32 id32
#define STATICTAG_DESC_WORD_VE(ofs32, id32) HT_STATICTAG_DESC_WORD_VE_CH ofs32 id32
#define STATICTAG_DESC_DWORD_VE(ofs32, id32) HT_STATICTAG_DESC_DWORD_VE_CH ofs32 id32
#define STATICTAG_DESC_QWORD_VE(ofs32, id32) HT_STATICTAG_DESC_QWORD_VE_CH ofs32 id32

#define tag_class_no	0
#define tag_class_edit	1
#define tag_class_sel	2

typedef char TAGSTRING;

void statictag_to_tag(char *statictag_str, TAGSTRING *tag_str, dword relocation, bool std_bigendian);

TAGSTRING *tag_findnext(TAGSTRING *tagstring);

vcp tag_get_color(TAGSTRING *tagstring);
void tag_get_id(TAGSTRING *tagstring, dword *id128_1, dword *id128_2, dword *id128_3, dword *id128_4);
int tag_get_len(TAGSTRING *tagstring);
dword tag_get_offset(TAGSTRING *tagstring);
int tag_get_size(TAGSTRING *tagstring);
int tag_get_vlen(TAGSTRING *tagstring);
int tag_get_seltextlen(TAGSTRING *tagstring);
char* tag_get_seltext(TAGSTRING *tagstring);
int tag_get_micropos(TAGSTRING *tagstring, int i);
int tag_get_microsize(TAGSTRING *tagstring);
bool tag_get_desc_id(TAGSTRING *tagstring, dword *id);

void tag_set_offset(TAGSTRING *tagstring, dword offset);
void tag_set_value(TAGSTRING *tagstring, dword value);

int tag_is_editable(TAGSTRING *tagstring);

void tag_strcat(TAGSTRING *dest, TAGSTRING *src);
void tag_strcpy(TAGSTRING *dest, TAGSTRING *src);
TAGSTRING *tag_strdup(TAGSTRING *tagstring);
int tag_strlen(TAGSTRING *tagstring);
int tag_strvlen(TAGSTRING *tagstring);

int tag_count_selectable_tags_in_group(TAGSTRING *tagstring, int group);
int tag_count_selectable_tags(TAGSTRING *tagstring);
int tag_count_groups(TAGSTRING *tagstring);
TAGSTRING *tag_get_selectable_tag(TAGSTRING *tagstring, int n, int group);
TAGSTRING *tag_get_group(TAGSTRING *tagstring, int group);
int tag_get_class(TAGSTRING *tagstring);

TAGSTRING *tag_make_sel(TAGSTRING *buf, char *string);
TAGSTRING *tag_make_ref(TAGSTRING *buf, dword id128_1, dword id128_2, dword id128_3, dword id128_4, char *string);
TAGSTRING *tag_make_ref_len(TAGSTRING *buf, dword id128_1, dword id128_2, dword id128_3, dword id128_4, char *string, int strlen);
TAGSTRING *tag_make_flags(TAGSTRING *buf, dword id, dword offset);
TAGSTRING *tag_make_group(TAGSTRING *buf);
TAGSTRING *tag_make_color(TAGSTRING *buf, dword color);
TAGSTRING *tag_make_default_color(TAGSTRING *buf);
TAGSTRING *tag_make_edit_byte(TAGSTRING *buf, FILEOFS ofs32);
TAGSTRING *tag_make_edit_word(TAGSTRING *buf, FILEOFS ofs32, tag_endian e);
TAGSTRING *tag_make_edit_dword(TAGSTRING *buf, FILEOFS ofs32, tag_endian e);
TAGSTRING *tag_make_edit_qword(TAGSTRING *buf, FILEOFS ofs32, tag_endian e);
TAGSTRING *tag_make_edit_time(TAGSTRING *buf, FILEOFS ofs32);
TAGSTRING *tag_make_edit_char(TAGSTRING *buf, FILEOFS ofs32);
TAGSTRING *tag_make_edit_bit(TAGSTRING *buf, FILEOFS ofs32, int bitidx);
TAGSTRING *tag_make_edit_selvis(TAGSTRING *buf, FILEOFS offset, char ch);
TAGSTRING *tag_make_desc_byte(TAGSTRING *buf, FILEOFS ofs32, dword id32);
TAGSTRING *tag_make_desc_word(TAGSTRING *buf, FILEOFS ofs32, dword id32, tag_endian e);
TAGSTRING *tag_make_desc_dword(TAGSTRING *buf, FILEOFS ofs32, dword id32, tag_endian e);
TAGSTRING *tag_make_desc_qword(TAGSTRING *buf, FILEOFS ofs32, dword id32, tag_endian e);
char *tag_striptags(char *dest, TAGSTRING *src);

#endif /* !__HTTAG_H__ */
