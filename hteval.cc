/* 
 *	HT Editor
 *	hteval.cc
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

#include <string.h>

#include "htatom.h"
#include "htctrl.h"
#include "htendian.h"
#include "hthist.h"
#include "htiobox.h"
#include "htstring.h"
#include "snprintf.h"

extern "C" {
#include "evalx.h"
}

static int sprint_base2(char *x, dword value, bool leading_zeros)
{
	char *ix=x;
	bool draw = leading_zeros;
	for (int i=0; i<32; i++) {
		bool v = value & (1<<(32-i-1));
		if (v) draw = true;
		if (draw) *x++ = v ? '1' : '0';
	}
	return x-ix;
}

static int sprint_base2_0(char *x, dword value, int zeros)
{
	char *ix=x;
	char vi=0;
	dword m=0x80000000;
	while (zeros<32) {m>>=1; zeros++;}
	do {
		if (value & m) {
			while (vi--) *(x++)='0';
			vi = 0;
			*x='1';
			x++;
		} else {
			vi++;
		}
		m>>=1;
	} while (m);
	if (!value) *(x++)='0';
	*x=0;
	return x-ix;
}

/*static int sprint_basen(char *buffer, int base, qword q)
{
	static char *chars="0123456789abcdef";
	if ((base<2) || (base>16)) return 0;
	int n = 0;
	char *b = buffer;
	while (q != to_qword(0)) {
		int c = QWORD_GET_INT(q % to_qword(base));
		*buffer++ = chars[c];
		n++;
		q /= to_qword(base);
	}
	for (int i=0; i < n/2; i++) {
		char t = b[i];
		b[i] = b[n-i-1];
		b[n-i-1] = t;
	}
	b[n] = 0;
	return n;
}*/

/*static int sprintf_basen(char *buffer, const char *format, int base, qword q)
{
	int n = 0;
	while (*format) {
		if (*format == '%') {
			int i = sprint_basen(buffer, base, q);
			buffer += i;
			n += i;
		} else {
			*buffer++ = *format;
			n++;
		}
		format++;
	}
	buffer[n] = 0;
	return n;
}*/


static void nicify(char *dest, const char *src, int d)
{
	*dest = *src;
	int l=strlen(src);
	if (!l) return;
	dest++;
	src++;
	while (l--) {
		if ((l%d==0) && (l>1)) {
			*dest='\'';
			dest++;
		}
		*(dest++) = *(src++);
	}
	*dest=0;
}

void eval_dialog()
{
	bounds b, c;
	app->getbounds(&c);
	b.w=50;
	b.h=16;
	b.x=(c.w-b.w)/2;
	b.y=(c.h-b.h)/2;
	ht_dialog *d=new ht_dialog();
	c=b;
	char *hint="type integer, float or string expression to evaluate";
	
	d->init(&b, "evaluate", FS_TITLE | FS_MOVE | FS_RESIZE);

	ht_list *ehist=(ht_list*)find_atom(HISTATOM_EVAL_EXPR);

	/* input line */
	BOUNDS_ASSIGN(b, 1, 1, c.w-4, 1);
	ht_strinputfield *s=new ht_strinputfield();
	s->init(&b, 255, ehist);
	d->insert(s);
	/* result text */
	BOUNDS_ASSIGN(b, 1, 3, c.w-4, c.h-5);
	ht_statictext *t=new ht_statictext();
	t->init(&b, hint, align_left);
	t->growmode = MK_GM(GMH_LEFT, GMV_FIT);
	d->insert(t);

	while (d->run(false) != button_cancel) {
		ht_strinputfield_data str;
		eval_scalar r;
		char b[1024];
		s->databuf_get(&str, sizeof str);
		if (str.textlen) {
			bin2str(b, str.text, str.textlen);
		
			insert_history_entry(ehist, b, 0);

			if (eval(&r, b, NULL, NULL, NULL)) {
				switch (r.type) {
					case SCALAR_INT: {
						char *x = b;
						char buf1[1024];
						char buf2[1024];
						// FIXME
						dword lo = QWORD_GET_LO(r.scalar.integer.value);
						dword hi = QWORD_GET_HI(r.scalar.integer.value);
						x += sprintf(x, "64bit integer:\n");
						ht_snprintf(buf1, sizeof buf1, "%qx", &r.scalar.integer.value);
						nicify(buf2, buf1, 4);
						x += ht_snprintf(x, 64, "hex   %s\n", buf2);
						ht_snprintf(buf1, sizeof buf1, "%qu", &r.scalar.integer.value);
						nicify(buf2, buf1, 3);
						x += ht_snprintf(x, 64, "dec   %s\n", buf2);
						if (to_sint64(r.scalar.integer.value) < to_sint64(0)) {
							ht_snprintf(buf1, sizeof buf1, "%qd", &r.scalar.integer.value);
							nicify(buf2, buf1+1, 3);                                                                                                                                                      // I hope nobody ever sees this
							x += ht_snprintf(x, 64, "sdec  -%s\n", buf2);
						}                              
						ht_snprintf(buf1, sizeof buf1, "%qo", &r.scalar.integer.value);
						nicify(buf2, buf1, 3);
						x += ht_snprintf(x, 64, "oct   %s\n", buf2);
						x += sprintf(x, "binlo ");
						x += sprint_base2(x, lo, true);
						*(x++) = '\n';
						if (hi) {
							x += sprintf(x, "binhi ");
							x += sprint_base2(x, hi, true);
							*(x++) = '\n';
						}
						char bb[4];
						int i = lo;
						/* big-endian string */
						x += sprintf(x, "%s", "string \"");
						create_foreign_int(bb, i, 4, big_endian);
						bin2str(x, bb, 4);
						x += 4;
						x += sprintf(x, "%s", "\" 32bit big-endian (e.g. network)\n");
						/* little-endian string */
						x += sprintf(x, "string \"");
						create_foreign_int(bb, i, 4, little_endian);
						bin2str(x, bb, 4);
						x += 4;
						x += sprintf(x, "%s", "\" 32bit little-endian (e.g. x86)\n");
						/* finish */
						*x = 0;
						break;
					}
					case SCALAR_STR: {
						char *x=b;
						x+=sprintf(x, "string:\n");
						/* c-escaped */
						x+=sprintf(x, "c-escaped \"");
						x+=escape_special(x, 0xff, r.scalar.str.value, r.scalar.str.len, NULL, true);
						*(x++)='"';
						*(x++)='\n';
						/* raw */
						x+=sprintf(x, "raw       '");
						int ll = MIN((UINT)r.scalar.str.len, 0xff);
						bin2str(x, r.scalar.str.value, ll);
						x+=ll;
						*(x++)='\'';
						*(x++)='\n';
						*x=0;
						break;
					}
					case SCALAR_FLOAT: {
						char *x=b;
						x+=sprintf(b, "val   %.20f\nnorm  %.20e", r.scalar.floatnum.value, r.scalar.floatnum.value);
						// FIXME: endianess/hardware format
						float ff = ((float)r.scalar.floatnum.value);
						dword f = *(dword*)&ff;
						x += sprintf(x, "\n-- IEEE-754, 32 bit --");
						x += sprintf(x, "\nhex   %08x\nbin   ", f);
						x += sprint_base2(x, f, true);
						x += sprintf(x, "\nsplit %c1.", (f>>31) ? '-' : '+');
						x += sprint_base2_0(x, f&((1<<23)-1), 23);
						x += sprintf(x, "b * 2^%d", ((f>>23)&255)-127);
						break;
					}
					default:
						strcpy(b, "?");
				}
				scalar_destroy(&r);
			} else {
				char *str="?";
				int pos=0;
				get_eval_error(&str, &pos);
				s->isetcursor(pos);
				sprintf(b, "error at pos %d: %s", pos+1, str);
			}

			t->settext(b);
		} else {
			t->settext(hint);
		}
	}

	d->done();
	delete d;
}

