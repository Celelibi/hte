
/*  A Bison parser, made from eval.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	EVAL_INT	257
#define	EVAL_STR	258
#define	EVAL_FLOAT	259
#define	EVAL_IDENT	260
#define	EVAL_LAND	261
#define	EVAL_LXOR	262
#define	EVAL_LOR	263
#define	EVAL_EQ	264
#define	EVAL_NE	265
#define	EVAL_STR_EQ	266
#define	EVAL_STR_NE	267
#define	EVAL_LT	268
#define	EVAL_LE	269
#define	EVAL_GT	270
#define	EVAL_GE	271
#define	EVAL_STR_LT	272
#define	EVAL_STR_LE	273
#define	EVAL_STR_GT	274
#define	EVAL_STR_GE	275
#define	EVAL_SHL	276
#define	EVAL_SHR	277
#define	NEG	278
#define	EVAL_POW	279

#line 4 "eval.y"


/*#define YYDEBUG 1*/

#define YYPARSE_PARAM resultptr

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "lex.h"
#include "evaltype.h"
#include "evalx.h"

#ifdef EVAL_DEBUG

int debug_dump_ident;

#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/*
 *
 */

#define MAX_FUNCNAME_LEN	16
#define MAX_SYMBOLNAME_LEN	32
#define MAX_ERRSTR_LEN		64

static eval_func_handler_t eval_func_handler;
static eval_symbol_handler_t eval_symbol_handler;
static void *eval_context;

/* FIXME: !!! */
int f2i(double f)
{
	char b[100];
	sprintf(b, "%.0f", f);
	return atoi(b);
}

char *binstr2cstr(char *s, int len)
{
	char *x=(char*)malloc(len+1);
	memmove(x, s, len);
	x[len]=0;
	return x;
}
	
int bin2str(char *result, void *S, int len)
{
	unsigned char *s = (unsigned char*)S;
	while (len--) {
		if (*s==0) *result=' '; else *result=*s;
		result++;
		s++;
	}
	*result=0;
	return len;
}

/*
 *	ERROR HANDLING
 */

static int eval_error;
static int eval_error_pos;
static char eval_errstr[MAX_ERRSTR_LEN];

void clear_eval_error()
{
	eval_error=0;
}

int get_eval_error(char **str, int *pos)
{
	if (eval_error) {
		if (str) *str=eval_errstr;
		if (pos) *pos=eval_error_pos;
		return eval_error;
	}
	if (str) *str="?";
	if (pos) *pos=0;
	return 0;
}

void set_eval_error(char *format,...)
{
	va_list vargs;
	
	va_start(vargs, format);
	vsprintf(eval_errstr, format, vargs);
	va_end(vargs);
	eval_error_pos=lex_current_buffer_pos();
	eval_error=1;
}

void set_eval_error_ex(int pos, char *format, ...)
{
	va_list vargs;
	
	va_start(vargs, format);
	vsprintf(eval_errstr, format, vargs);
	va_end(vargs);
	eval_error_pos=pos;
	eval_error=1;
}

/*
 *
 */

#ifdef EVAL_DEBUG

void integer_dump(int_t *i)
{
	printf("%d", i->value);
}

void float_dump(float_t *f)
{
	printf("%f", f->value);
}

void string_dump(str_t *s)
{
	int i;
	for (i=0; i<s->len; i++) {
		if ((unsigned)s->value[i]<32) {
			printf("\\x%x", s->value[i]);
		} else {
			printf("%c", s->value[i]);
		}
	}
}

#endif

void string_destroy(str_t *s)
{
	if (s->value) free(s->value);
}

/*
 *	SCALARLIST
 */

void scalarlist_set(scalarlist_t *l, scalar_t *s)
{
	l->count=1;
	l->scalars=(scalar_t*)malloc(sizeof (scalar_t) * l->count);
	l->scalars[0]=*s;
}

void scalarlist_concat(scalarlist_t *l, scalarlist_t *a, scalarlist_t *b)
{
	l->count=a->count+b->count;
	l->scalars=(scalar_t*)malloc(sizeof (scalar_t) * l->count);
	memmove(l->scalars, a->scalars, sizeof (scalar_t) * a->count);
	memmove(l->scalars+a->count, b->scalars, sizeof (scalar_t) * b->count);
}

void scalarlist_destroy(scalarlist_t *l)
{
	int i;
	if (l && l->scalars) {
		for (i=0; i < l->count; i++) {
			scalar_destroy(&l->scalars[i]);
		}
		free(l->scalars);
	}		
}

void scalarlist_destroy_gentle(scalarlist_t *l)
{
	if (l && l->scalars) free(l->scalars);
}

#ifdef EVAL_DEBUG

void scalarlist_dump(scalarlist_t *l)
{
	int i;
	for (i=0; i<l->count; i++) {
		scalar_dump(&l->scalars[i]);
		if (i!=l->count-1) {
			printf(", ");
		}
	}
}

#endif

/*
 *	SCALAR
 */

void scalar_setint(scalar_t *s, int_t *i)
{
	s->type=SCALAR_INT;
	s->scalar.integer=*i;
}

void scalar_setstr(scalar_t *s, str_t *t)
{
	s->type=SCALAR_STR;
	s->scalar.str=*t;
}

#ifdef EVAL_DEBUG

void scalar_dump(scalar_t *s)
{
	switch (s->type) {
		case SCALAR_STR: {
			string_dump(&s->scalar.str);
			break;
		}
		case SCALAR_INT: {
			integer_dump(&s->scalar.integer);
			break;
		}
		case SCALAR_FLOAT: {
			float_dump(&s->scalar.floatnum);
			break;
		}
		default:
			break;
	}
}

#endif

void scalar_create_int(scalar_t *s, int_t *t)
{
	s->type=SCALAR_INT;
	s->scalar.integer=*t;
}

void scalar_create_int_c(scalar_t *s, int i)
{
	s->type=SCALAR_INT;
	s->scalar.integer.value=i;
	s->scalar.integer.type=TYPE_UNKNOWN;
}

void scalar_create_str(scalar_t *s, str_t *t)
{
	s->type=SCALAR_STR;
	s->scalar.str.value=(char*)malloc(t->len ? t->len : 1);
	memmove(s->scalar.str.value, t->value, t->len);
	s->scalar.str.len=t->len;
}

void scalar_create_str_c(scalar_t *s, char *cstr)
{
	str_t t;
	t.value=cstr;
	t.len=strlen(cstr);
	scalar_create_str(s, &t);
}

void scalar_create_float(scalar_t *s, float_t *t)
{
	s->type=SCALAR_FLOAT;
	s->scalar.floatnum=*t;
}

void scalar_create_float_c(scalar_t *s, double f)
{
	s->type=SCALAR_FLOAT;
	s->scalar.floatnum.value=f;
}

void scalar_context_str(scalar_t *s, str_t *t)
{
	switch (s->type) {
		case SCALAR_INT: {
			char buf[16];
			sprintf(buf, "%d", s->scalar.integer.value);
			t->value=(char*)strdup(buf);
			t->len=strlen(buf);
			break;
		}
		case SCALAR_STR: {
			t->value=(char*)malloc(s->scalar.str.len ? s->scalar.str.len : 1);
			t->len=s->scalar.str.len;
			memmove(t->value, s->scalar.str.value, t->len);
			break;
		}			
		case SCALAR_FLOAT: {
			char buf[32];
			sprintf(buf, "%f", s->scalar.floatnum.value);
			t->value=(char*)strdup(buf);
			t->len=strlen(buf);
			break;
		}
		default:
			break;
	}					
}

void scalar_context_int(scalar_t *s, int_t *t)
{
	switch (s->type) {
		case SCALAR_INT: {
			*t=s->scalar.integer;
			break;
		}
		case SCALAR_STR: {
			char *x=binstr2cstr(s->scalar.str.value, s->scalar.str.len);
			t->value=strtol(x, (char**)NULL, 10);
			t->type=TYPE_UNKNOWN;
			free(x);
			break;
		}			
		case SCALAR_FLOAT: {
			t->value=f2i(s->scalar.floatnum.value);
			t->type=TYPE_UNKNOWN;
			break;
		}
		default:
			break;
	}					
}

void scalar_context_float(scalar_t *s, float_t *t)
{
	switch (s->type) {
		case SCALAR_INT: {
			t->value=s->scalar.integer.value;
			break;
		}
		case SCALAR_STR:  {
			char *x=binstr2cstr(s->scalar.str.value, s->scalar.str.len);
			t->value=strtod(x, (char**)NULL);
			free(x);
			break;
		}			
		case SCALAR_FLOAT: {
			*t=s->scalar.floatnum;
			break;
		}
		default:
			break;
	}					
}

void string_concat(str_t *s, str_t *a, str_t *b)
{
	s->value=(char*)malloc(a->len+b->len ? a->len+b->len : 1);
	memmove(s->value, a->value, a->len);
	memmove(s->value+a->len, b->value, b->len);
	s->len=a->len+b->len;
	
	free(a->value);
	a->len=0;
	free(b->value);
	b->len=0;
}

void scalar_concat(scalar_t *s, scalar_t *a, scalar_t *b)
{
	str_t as, bs, rs;
	scalar_context_str(a, &as);
	scalar_context_str(b, &bs);
	string_concat(&rs, &as, &bs);
	s->type=SCALAR_STR;
	s->scalar.str=rs;
}

void scalar_destroy(scalar_t *s)
{
	switch (s->type) {
		case SCALAR_STR:
			string_destroy(&s->scalar.str);
			break;
		default:
			break;
	}
}

int string_compare(str_t *a, str_t *b)
{
	if (a->len==b->len) {
		return memcmp(a->value, b->value, a->len);
	}
	return a->len-b->len;
}

int scalar_strop(scalar_t *xr, scalar_t *xa, scalar_t *xb, int op)
{
	str_t as, bs;
	int r;
	int c;
	scalar_context_str(xa, &as);
	scalar_context_str(xb, &bs);
	
	c=string_compare(&as, &bs);
	switch (op) {
		case EVAL_STR_EQ: r=(c==0); break;
		case EVAL_STR_NE: r=(c!=0); break;
		case EVAL_STR_GT: r=(c>0); break;
		case EVAL_STR_GE: r=(c>=0); break;
		case EVAL_STR_LT: r=(c<0); break;
		case EVAL_STR_LE: r=(c>=0); break;
		default: 
			return 0;
	}
	xr->type=SCALAR_INT;
	xr->scalar.integer.value=r;
	xr->scalar.integer.type=TYPE_UNKNOWN;
	return 1;
}

int ipow(int a, int b)
{
	int i, r=1;
	for (i=0; i<b; i++) {
		r*=a;
	}
	return r;
}

int scalar_float_op(scalar_t *xr, scalar_t *xa, scalar_t *xb, int op)
{
	float_t ai, bi;
	float a, b, r;
	scalar_context_float(xa, &ai);
	scalar_context_float(xb, &bi);
	
	a=ai.value;
	b=bi.value;
	switch (op) {
		case '*': r=a*b; break;
		case '/': {
		    if (!b) {
			    set_eval_error("division by zero");
			    return 0;
		    }
		    r=a/b;
		    break;
		}			    
		case '+': r=a+b; break;
		case '-': r=a-b; break;
		case EVAL_POW: r=pow(a,b); break;
		case EVAL_EQ: r=(a==b); break;
		case EVAL_NE: r=(a!=b); break;
		case EVAL_GT: r=(a>b); break;
		case EVAL_GE: r=(a>=b); break;
		case EVAL_LT: r=(a<b); break;
		case EVAL_LE: r=(a<=b); break;
		case EVAL_LAND: r=(a) && (b); break;
		case EVAL_LXOR: r=(a && !b) || (!a && b); break;
		case EVAL_LOR: r=(a||b); break;
		default: 
			set_eval_error("invalid operator");
			return 0;
	}
	xr->type=SCALAR_FLOAT;
	xr->scalar.floatnum.value=r;
	return 1;
}

int scalar_int_op(scalar_t *xr, scalar_t *xa, scalar_t *xb, int op)
{
	int_t ai, bi;
	int a, b, r;
	scalar_context_int(xa, &ai);
	scalar_context_int(xb, &bi);
	
	a=ai.value;
	b=bi.value;
	switch (op) {
		case '*': r=a*b; break;
		case '/': {
		    if (!b) {
			    set_eval_error("division by zero");
			    return 0;
		    }
		    r=a/b;
		    break;
		}			    
		case '%': {
		    if (!b) {
			    set_eval_error("division by zero");
			    return 0;
		    }
		    r=a%b;
		    break;
		}			    
		case '+': r=a+b; break;
		case '-': r=a-b; break;
		case '&': r=a&b; break;
		case '|': r=a|b; break;
		case '^': r=a^b; break;
		case EVAL_POW: r=ipow(a,b); break;
		case EVAL_SHL: r=a<<b; break;
		case EVAL_SHR: r=a>>b; break;
		case EVAL_EQ: r=(a==b); break;
		case EVAL_NE: r=(a!=b); break;
		case EVAL_GT: r=(a>b); break;
		case EVAL_GE: r=(a>=b); break;
		case EVAL_LT: r=(a<b); break;
		case EVAL_LE: r=(a<=b); break;
		case EVAL_LAND: r=(a) && (b); break;
		case EVAL_LXOR: r=(a && !b) || (!a && b); break;
		case EVAL_LOR: r=(a||b); break;
		default: 
			set_eval_error("invalid operator");
			return 0;
	}
	xr->type=SCALAR_INT;
	xr->scalar.integer.value=r;
	xr->scalar.integer.type=TYPE_UNKNOWN;
	return 1;
}

int scalar_op(scalar_t *xr, scalar_t *xa, scalar_t *xb, int op)
{
	int r;
	if ((xa->type==SCALAR_FLOAT) || (xb->type==SCALAR_FLOAT)) {
		r=scalar_float_op(xr, xa, xb, op);
	} else {
		r=scalar_int_op(xr, xa, xb, op);
	}
	scalar_destroy(xa);
	scalar_destroy(xb);
	return r;
}
	
void scalar_negset(scalar_t *xr, scalar_t *xa)
{
	if (xa->type==SCALAR_FLOAT) {
		float_t a;
		a=xa->scalar.floatnum;
	
		xr->type=SCALAR_FLOAT;
		xr->scalar.floatnum.value=-a.value;
	} else {
		int_t a;
		scalar_context_int(xa, &a);
	
		xr->type=SCALAR_INT;
		xr->scalar.integer.value=-a.value;
		xr->scalar.integer.type=TYPE_UNKNOWN;
	}
	scalar_destroy(xa);
}

void scalar_miniif(scalar_t *xr, scalar_t *xa, scalar_t *xb, scalar_t *xc)
{
	int_t a;
	scalar_context_int(xa, &a);
	if (a.value) {
		*xr=*xb;
	} else {
		*xr=*xc;
	}
	scalar_destroy(xa);
}

/*
 *	BUILTIN FUNCTIONS
 */

int func_char(scalar_t *r, int_t *i)
{
	str_t s;
	char c=i->value;
	s.value=&c;
	s.len=1;
	scalar_create_str(r, &s);
	return 1;
}

int func_float(scalar_t *r, float_t *p)
{
	scalar_create_float(r, p);
	return 1;
}

int func_fmax(scalar_t *r, float_t *p1, float_t *p2)
{
	r->type=SCALAR_FLOAT;
	r->scalar.floatnum.value=(p1->value>p2->value) ? p1->value : p2->value;
	return 1;
}

int func_fmin(scalar_t *r, float_t *p1, float_t *p2)
{
	r->type=SCALAR_FLOAT;
	r->scalar.floatnum.value=(p1->value<p2->value) ? p1->value : p2->value;
	return 1;
}

int func_int(scalar_t *r, int_t *p)
{
	scalar_create_int(r, p);
	return 1;
}

int func_ord(scalar_t *r, str_t *s)
{
	if (s->len>=1) {
		scalar_create_int_c(r, s->value[0]);
		return 1;
	}
	set_eval_error("string must at least contain one character");
	return 0;		
}

int func_max(scalar_t *r, int_t *p1, int_t *p2)
{
	scalar_create_int(r, (p1->value>p2->value) ? p1 : p2);
	return 1;
}

int func_min(scalar_t *r, int_t *p1, int_t *p2)
{
	scalar_create_int(r, (p1->value<p2->value) ? p1 : p2);
	return 1;
}

int func_random(scalar_t *r, int_t *p1)
{
	scalar_create_int_c(r, (p1->value) ? (rand() % p1->value):0);
	return 1;
}

int func_rnd(scalar_t *r)
{
	scalar_create_int_c(r, rand() % 10);
	return 1;
}

int func_round(scalar_t *r, float_t *p)
{
	r->type=SCALAR_INT;
	r->scalar.integer.value=f2i(p->value+0.5);
	r->scalar.integer.type=TYPE_UNKNOWN;
	return 1;
}

int func_strchr(scalar_t *r, str_t *p1, str_t *p2)
{
	if (p2->len) {
		if (p1->len) {
			char *pos = (char *)memchr(p1->value, *p2->value, p1->len);
			if (pos) {
				scalar_create_int_c(r, pos-p1->value);
			} else {
				scalar_create_int_c(r, -1);
			}
		} else {
			scalar_create_int_c(r, -1);
		}
		return 1;
	} else {
		return 0;
	}
}

int func_strcmp(scalar_t *r, str_t *p1, str_t *p2)
{
	int r2=memcmp(p1->value, p2->value, MIN(p1->len, p2->len));
	if (r2) {
		scalar_create_int_c(r, r2);
	} else {
		if (p1->len > p2->len) {
			scalar_create_int_c(r, 1);
		} else if (p1->len < p2->len) {
			scalar_create_int_c(r, -1);
		} else {
			scalar_create_int_c(r, 0);
		}
	}
	return 1;     
}

int func_string(scalar_t *r, str_t *p)
{
	scalar_create_str(r, p);
	return 1;
}

int func_strlen(scalar_t *r, str_t *p1)
{
	scalar_create_int_c(r, p1->len);
	return 1;
}

int func_strncmp(scalar_t *r, str_t *p1, str_t *p2, int_t *p3)
{
	return 1;
}

int func_strrchr(scalar_t *r, str_t *p1, str_t *p2)
{
	return 1;
}

int func_strstr(scalar_t *r, str_t *p1, str_t *p2)
{

	return 1;
}

int func_substr(scalar_t *r, str_t *p1, int_t *p2, int_t *p3)
{
	if (p2->value >= 0 && p3->value > 0) {
		if (p2->value < p1->len) {
			str_t s;
			s.len = MIN(p3->value, p1->len-p2->value);
			s.value = &p1->value[p2->value];
			scalar_create_str(r, &s);
		} else {
			scalar_create_str_c(r, "");
		}
	} else {
		scalar_create_str_c(r, "");
	}
	return 1;
}

int func_trunc(scalar_t *r, float_t *p)
{
	r->type=SCALAR_INT;
	r->scalar.integer.value=f2i(p->value);
	r->scalar.integer.type=TYPE_UNKNOWN;
	return 1;
}

#define EVALFUNC_FMATH1(name) int func_##name(scalar_t *r, float_t *p)\
{\
	r->type=SCALAR_FLOAT;\
	r->scalar.floatnum.value=name(p->value);\
	return 1;\
}

#define EVALFUNC_FMATH1i(name) int func_##name(scalar_t *r, float_t *p)\
{\
	r->type=SCALAR_INT;\
	r->scalar.integer.value=f2i(name(p->value));\
	r->scalar.integer.type=TYPE_UNKNOWN;\
	return 1;\
}

#define EVALFUNC_FMATH2(name) int func_##name(scalar_t *r, float_t *p1, float_t *p2)\
{\
	r->type=SCALAR_FLOAT;\
	r->scalar.floatnum.value=name(p1->value, p2->value);\
	return 1;\
}

EVALFUNC_FMATH2(pow)

EVALFUNC_FMATH1(sqrt)

EVALFUNC_FMATH1(exp)
EVALFUNC_FMATH1(log)

EVALFUNC_FMATH1i(ceil)
EVALFUNC_FMATH1i(floor)

EVALFUNC_FMATH1(sin)
EVALFUNC_FMATH1(cos)
EVALFUNC_FMATH1(tan)

EVALFUNC_FMATH1(asin)
EVALFUNC_FMATH1(acos)
EVALFUNC_FMATH1(atan)

EVALFUNC_FMATH1(sinh)
EVALFUNC_FMATH1(cosh)
EVALFUNC_FMATH1(tanh)

#ifdef HAVE_ASINH
EVALFUNC_FMATH1(asinh)
#endif

#ifdef HAVE_ACOSH
EVALFUNC_FMATH1(acosh)
#endif

#ifdef HAVE_ATANH
EVALFUNC_FMATH1(atanh)
#endif

void sprintf_puts(char **b, char *blimit, char *buf)
{
	while ((*b<blimit) && (*buf)) {
		**b=*(buf++);
		(*b)++;
	}
}

int sprintf_percent(char **fmt, int *fmtl, char **b, char *blimit, scalar_t *s)
{
	char cfmt[32];
	char buf[512];
	int ci=1;
	cfmt[0]='%';
	while ((*fmtl) && (ci<32-1)) {
		cfmt[ci]=(*fmt)[0];
		cfmt[ci+1]=0;
		switch ((*fmt)[0]) {
			case 'd':
			case 'i':
			case 'o':
			case 'u':
			case 'x':
			case 'X':
			case 'c': {
				int_t i;
				scalar_context_int(s, &i);
				
				sprintf(buf, cfmt, i.value);
				sprintf_puts(b, blimit, buf);
				
				return 1;
			}
			case 's': {
				char *q=cfmt+1;
				str_t t;
/*				int l;*/
				scalar_context_str(s, &t);
				
				while (*q!='s') {
					if ((*q>='0') && (*q<='9')) {
						int sl=strtol(q, NULL, 10);
						if (sl>sizeof buf-1) sl=sizeof buf-1;
						sprintf(q, "%ds", sl);
						break;
					} else {
						switch (*q) {
							case '+':
							case '-':
							case '#':
							case ' ':
								break;
							default:
							/* FIXME: invalid format */
								break;
						}
					}
					q++;
				}
				
				if (t.len>sizeof buf-1) t.len=sizeof buf-1;
				t.value[t.len]=0;
				
				sprintf(buf, cfmt, t.value);
				
/*				l=t.len;
				if (l > (sizeof buf)-1) l=(sizeof buf)-1;

				memmove(buf, t.value, l);
				buf[l]=0;*/
				sprintf_puts(b, blimit, buf);
				
				string_destroy(&t);
				return 1;
			}
			case 'e':
			case 'E':
			case 'f':
			case 'F':
			case 'g':
			case 'G': {
				float_t f;
				scalar_context_float(s, &f);
				
				sprintf(buf, cfmt, f.value);
				sprintf_puts(b, blimit, buf);
				
				return 1;
			}
			case '%':
				sprintf_puts(b, blimit, "%");
				return 1;
		}
		(*fmt)++;
		(*fmtl)--;
		ci++;
	}
	return 0;
}

int func_sprintf(scalar_t *r, str_t *format, scalarlist_t *scalars)
{
	char buf[512];		/* FIXME: possible buffer overflow */
	char *b=buf;
	char *fmt;
	int fmtl;
	scalar_t *s=scalars->scalars;
	
	fmt=format->value;
	fmtl=format->len;
	
	while (fmtl) {
		if (fmt[0]=='%') {
			fmt++;
			fmtl--;
			if (!fmtl) break;
			if (fmt[0]!='%') {
				if (s-scalars->scalars >= scalars->count) {
					DEBUG_DUMP("too few parameters");
					return 0;
				}					
				if (!sprintf_percent(&fmt, &fmtl, &b, buf+sizeof buf, s)) return 0;
				s++;
			} else {
				*b++=fmt[0];
				if (b-buf>=512) break;
			}
		} else {
			*b++=fmt[0];
			if (b-buf>=512) break;
		}
		fmt++;
		fmtl--;
	}
	*b=0;
	r->type=SCALAR_STR;
	r->scalar.str.value=(char*)strdup(buf);
	r->scalar.str.len=strlen(r->scalar.str.value);
	return 1;
}

/*
 *	FUNCTIONS
 */

int func_eval(scalar_t *r, str_t *p)
{
	char *q=(char*)malloc(p->len+1);
	int x;
	memmove(q, p->value, p->len);
	q[p->len]=0;
	x=eval(r, q, eval_func_handler, eval_symbol_handler, eval_context);
	free(q);
/*     if (get_eval_error(NULL, NULL)) {
		eval_error_pos+=lex_current_buffer_pos();
	}*/
	return x;
}

int func_error(scalar_t *r, str_t *s)
{
	char c[1024];
	bin2str(c, s->value, MIN(s->len, sizeof c));
	set_eval_error(c);
	return 0;
}

evalfunc_t builtin_evalfuncs[]=	{
/* eval */
	{ "eval", &func_eval, {SCALAR_STR}, "evaluate string" },
/* type juggling */
	{ "int", &func_int, {SCALAR_INT}, "converts to integer" },
	{ "string", &func_string, {SCALAR_STR}, "converts to string" },
	{ "float", &func_float, {SCALAR_FLOAT}, "converts to float" },
/*
	{ "is_int", &func_is_int, {SCALAR_INT}, "returns non-zero if param is an integer" },
	{ "is_string", &func_is_string, {SCALAR_STR}, "returns non-zero if param is a string" },
	{ "is_float", &func_is_float, {SCALAR_FLOAT}, "returns non-zero if param is a float" },
*/
/* general */
	{ "error", &func_error, {SCALAR_STR}, "abort with error" },
/* string functions */
	{ "char", &func_char, {SCALAR_INT}, "return the ascii character (1-char string) specified by p1" },
	{ "ord", &func_ord, {SCALAR_STR}, "return the ordinal value of p1" },
	{ "sprintf", &func_sprintf, {SCALAR_STR, SCALAR_VARARGS}, "returns formatted string" },
	{ "strchr", &func_strchr, {SCALAR_STR, SCALAR_STR}, "returns position of first occurrence of character param2 in param1" },
	{ "strcmp", &func_strcmp, {SCALAR_STR, SCALAR_STR}, "returns zero for equality, positive number for str1 > str2 and negative number for str1 < str2" },
	{ "strlen", &func_strlen, {SCALAR_STR}, "returns length of string" },
	{ "strncmp", &func_strncmp, {SCALAR_STR, SCALAR_STR, SCALAR_INT}, "like strcmp, but considers a maximum of param3 characters" },
	{ "strstr", &func_strchr, {SCALAR_STR, SCALAR_STR}, "returns position of first occurrence of string param2 in param1" },
	{ "substr", &func_substr, {SCALAR_STR, SCALAR_INT, SCALAR_INT}, "returns substring from param1, start param2, length param3" },
/*	{ "stricmp", &func_stricmp, {SCALAR_STR, SCALAR_STR}, "like strcmp but case-insensitive" },
	{ "strnicmp", &func_strnicmp, {SCALAR_STR, SCALAR_STR}, "" }, */
/* math */	
	{ "pow", &func_pow, {SCALAR_FLOAT, SCALAR_FLOAT}, 0 },
	{ "sqrt", &func_sqrt, {SCALAR_FLOAT}, 0 },
	
	{ "fmin", &func_fmin, {SCALAR_FLOAT, SCALAR_FLOAT}, 0 },
	{ "fmax", &func_fmax, {SCALAR_FLOAT, SCALAR_FLOAT}, 0 },
	{ "min", &func_min, {SCALAR_INT, SCALAR_INT}, 0 },
	{ "max", &func_max, {SCALAR_INT, SCALAR_INT}, 0 },
	
	{ "random", &func_random, {SCALAR_INT}, "returns a random integer between 0 and param1-1" },
	{ "rnd", &func_rnd, {}, "returns a random number between 0 and 1" },

	{ "exp", &func_exp, {SCALAR_FLOAT}, 0 },
	{ "log", &func_log, {SCALAR_FLOAT}, 0 },
	
	{ "ceil", &func_ceil, {SCALAR_FLOAT}, 0 },
	{ "floor", &func_floor, {SCALAR_FLOAT}, 0 },
	{ "round", &func_round, {SCALAR_FLOAT}, 0 },
	{ "trunc", &func_trunc, {SCALAR_FLOAT}, 0 },
	
	{ "sin", &func_sin, {SCALAR_FLOAT}, 0 },
	{ "cos", &func_cos, {SCALAR_FLOAT}, 0 },
	{ "tan", &func_tan, {SCALAR_FLOAT}, 0 },
	
	{ "asin", &func_asin, {SCALAR_FLOAT}, 0 },
	{ "acos", &func_acos, {SCALAR_FLOAT}, 0 },
	{ "atan", &func_atan, {SCALAR_FLOAT}, 0 },
	
	{ "sinh", &func_sinh, {SCALAR_FLOAT}, 0 },
	{ "cosh", &func_cosh, {SCALAR_FLOAT}, 0 },
	{ "tanh", &func_tanh, {SCALAR_FLOAT}, 0 },
	
#ifdef HAVE_ASINH
	{ "asinh", &func_asinh, {SCALAR_FLOAT}, 0 },
#endif

#ifdef HAVE_ACOSH
	{ "acosh", &func_acosh, {SCALAR_FLOAT}, 0 },
#endif

#ifdef HAVE_ATANH
	{ "atanh", &func_atanh, {SCALAR_FLOAT}, 0 },
#endif
	
	{ NULL, NULL }
};

protomatch_t match_evalfunc_proto(char *name, scalarlist_t *params, evalfunc_t *proto)
{
	int j;
	int protoparams=0;
	
	if (strcmp(name, proto->name)!=0) return PROTOMATCH_NAME_FAIL;
	
	for (j=0; j<MAX_EVALFUNC_PARAMS; j++) {
		if (proto->ptype[j]==SCALAR_NULL) break;
		if (proto->ptype[j]==SCALAR_VARARGS) {
			if (params->count > protoparams) protoparams=params->count;
			break;
		}
		protoparams++;
	}
	return (protoparams==params->count) ? PROTOMATCH_OK : PROTOMATCH_PARAM_FAIL;
}

int exec_evalfunc(scalar_t *r, scalarlist_t *params, evalfunc_t *proto)
{
	int j;
	int retv;
	scalar_t sc[MAX_EVALFUNC_PARAMS];
	void *pptrs[MAX_EVALFUNC_PARAMS];
	int protoparams=0;
	scalarlist_t *sclist=0;
	char *errmsg;
	int errpos;

	for (j=0; j<MAX_EVALFUNC_PARAMS; j++) {
		sc[j].type=SCALAR_NULL;
		pptrs[j]=NULL;
	}					
	
	DEBUG_DUMP("%s:", proto->name);
	
	for (j=0; j<MAX_EVALFUNC_PARAMS; j++) {
		int term=0;
		if (proto->ptype[j]==SCALAR_NULL) break;
		switch (proto->ptype[j]) {
			case SCALAR_INT:
				protoparams++;
				if (params->count<protoparams) return 0;
				sc[j].type=SCALAR_INT;
				scalar_context_int(&params->scalars[j], &sc[j].scalar.integer);
				pptrs[j]=&sc[j].scalar.integer;

				DEBUG_DUMP_SCALAR(&sc[j], "param %d: int=", j);
				break;
			case SCALAR_STR:
				protoparams++;
				if (params->count<protoparams) return 0;
				sc[j].type=SCALAR_STR;
				scalar_context_str(&params->scalars[j], &sc[j].scalar.str);
				pptrs[j]=&sc[j].scalar.str;
				
				DEBUG_DUMP_SCALAR(&sc[j], "param %d: str=", j);
				break;
			case SCALAR_FLOAT:
				protoparams++;
				if (params->count<protoparams) return 0;
				sc[j].type=SCALAR_FLOAT;
				scalar_context_float(&params->scalars[j], &sc[j].scalar.floatnum);
				pptrs[j]=&sc[j].scalar.floatnum;

				DEBUG_DUMP_SCALAR(&sc[j], "param %d: float=", j);
				break;
			case SCALAR_VARARGS: {
				sclist=(scalarlist_t*)malloc(sizeof (scalarlist_t));
				sclist->count=params->count-j;
				if (sclist->count) {
					sclist->scalars=(scalar_t*)malloc(sizeof (scalar_t) * sclist->count);
					memmove(sclist->scalars, &params->scalars[j], sizeof (scalar_t) * sclist->count);
				} else {
					sclist->scalars=NULL;
				}					
				pptrs[j]=sclist;
				protoparams = params->count;
				term=1;
				
				DEBUG_DUMP_SCALARLIST(params, "param %d: varargs=", j);
				break;
			}								
			default:
				set_eval_error("internal error (%s:%d)", __FILE__, __LINE__);
				return 0;
		}
		if (term) break;
	}
	if (params->count == protoparams) {
		DEBUG_DUMP_INDENT_IN;
		retv=((int(*)(scalar_t*,void*,void*,void*,void*,void*,void*,void*,void*))proto->func)(r, pptrs[0], pptrs[1], pptrs[2], pptrs[3], pptrs[4], pptrs[5], pptrs[6], pptrs[7]);
		DEBUG_DUMP_INDENT_OUT;
	} else {
		retv=0;
	}		
	if (retv) {
		DEBUG_DUMP_SCALAR(r, "returns ");
	} else {
		DEBUG_DUMP("fails...");
	}

	if (sclist) {	
		scalarlist_destroy_gentle(sclist);
		free(sclist);
	}		
	
	for (j=0; j<MAX_EVALFUNC_PARAMS; j++) {
		if (sc[j].type!=SCALAR_NULL) {
			scalar_destroy(&sc[j]);
		}
	}
	
	if (!get_eval_error(NULL, NULL) && !retv) {
		set_eval_error("?");
	}
	
	if (get_eval_error(&errmsg, &errpos)) {
		char ee[MAX_ERRSTR_LEN+1];
		ee[MAX_ERRSTR_LEN]=0;
		strncpy(ee, proto->name, sizeof ee);
		strncat(ee, "(): ", sizeof ee);
		strncat(ee, errmsg, sizeof ee);
		set_eval_error_ex(errpos, "%s", ee);
	}
	return retv;
}

int evalsymbol(scalar_t *r, char *sname)
{
	int s=0;
	if (eval_symbol_handler) s=eval_symbol_handler(r, sname);
	if (!get_eval_error(NULL, NULL) && !s) {
		char sname_short[MAX_SYMBOLNAME_LEN+1];
		sname_short[MAX_SYMBOLNAME_LEN]=0;
		strncpy(sname_short, sname, MAX_SYMBOLNAME_LEN);
		set_eval_error("unknown symbol: %s", sname_short);
	}
	return s;
}

int std_eval_func_handler(scalar_t *r, char *fname, scalarlist_t *params, evalfunc_t *protos)
{
	char fname_short[MAX_FUNCNAME_LEN+1];
	
	fname_short[MAX_FUNCNAME_LEN]=0;
	strncpy(fname_short, fname, MAX_FUNCNAME_LEN);
	
	while (protos->name) {
		switch (match_evalfunc_proto(fname_short, params, protos)) {
			case PROTOMATCH_OK:
				return exec_evalfunc(r, params, protos);
			case PROTOMATCH_PARAM_FAIL:
				set_eval_error("invalid params to function %s", fname_short);
				return 0;
			default: {}
		}
		protos++;
	}
	return 0;
}

int evalfunc(scalar_t *r, char *fname, scalarlist_t *params)
{
	char fname_short[MAX_FUNCNAME_LEN+1];
	
	int s;
	if (eval_func_handler) {
		s=eval_func_handler(r, fname, params);
		if (get_eval_error(NULL, NULL)) return 0;
		if (s) return s;
	}
	
	s=std_eval_func_handler(r, fname, params, builtin_evalfuncs);
	if (get_eval_error(NULL, NULL)) return 0;
	if (s) return s;
	
	fname_short[MAX_FUNCNAME_LEN]=0;
	strncpy(fname_short, fname, MAX_FUNCNAME_LEN);
	
	set_eval_error("unknown function %s", fname_short);
	return 0;
}

void *eval_get_context()
{
	return eval_context;
}

void eval_set_context(void *context)
{
	eval_context=context;
}

void eval_set_func_handler(eval_func_handler_t func_handler)
{
	eval_func_handler=func_handler;
}

void eval_set_symbol_handler(eval_symbol_handler_t symbol_handler)
{
	eval_symbol_handler=symbol_handler;
}

void yyerror (char *s)
{
	set_eval_error(s);
}


#line 1252 "eval.y"
typedef union {
	scalar_t scalar;
	char *ident;
	scalarlist_t scalars;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		79
#define	YYFLAG		-32768
#define	YYNTBASE	40

#define YYTRANSLATE(x) ((unsigned)(x) <= 279 ? yytranslate[x] : 45)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,    35,    16,     2,    38,
    39,    33,    32,     7,    31,    10,    34,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     9,     2,     2,
     2,     2,     8,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    15,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    14,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
    11,    12,    13,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    36,    37
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     6,     8,    12,    16,    20,    24,    28,
    32,    36,    40,    44,    48,    52,    56,    60,    64,    68,
    72,    76,    80,    84,    88,    92,    96,   100,   104,   108,
   112,   115,   119,   125,   129,   134,   136,   137,   139,   141,
   145
};

static const short yyrhs[] = {    41,
     0,     3,     0,     4,     0,     5,     0,    41,    33,    41,
     0,    41,    34,    41,     0,    41,    35,    41,     0,    41,
    32,    41,     0,    41,    31,    41,     0,    41,    16,    41,
     0,    41,    14,    41,     0,    41,    15,    41,     0,    41,
    37,    41,     0,    41,    29,    41,     0,    41,    30,    41,
     0,    41,    17,    41,     0,    41,    18,    41,     0,    41,
    23,    41,     0,    41,    24,    41,     0,    41,    21,    41,
     0,    41,    22,    41,     0,    41,    11,    41,     0,    41,
    12,    41,     0,    41,    13,    41,     0,    41,    19,    41,
     0,    41,    20,    41,     0,    41,    27,    41,     0,    41,
    28,    41,     0,    41,    25,    41,     0,    41,    26,    41,
     0,    31,    41,     0,    38,    41,    39,     0,    41,     8,
    41,     9,    41,     0,    41,    10,    41,     0,    44,    38,
    42,    39,     0,    44,     0,     0,    43,     0,    41,     0,
    43,     7,    43,     0,     6,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
  1288,  1291,  1292,  1293,  1294,  1295,  1296,  1297,  1298,  1299,
  1300,  1301,  1302,  1303,  1304,  1305,  1306,  1307,  1308,  1309,
  1310,  1311,  1312,  1313,  1314,  1315,  1316,  1317,  1318,  1319,
  1320,  1321,  1322,  1323,  1329,  1336,  1344,  1351,  1353,  1354,
  1362
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","EVAL_INT",
"EVAL_STR","EVAL_FLOAT","EVAL_IDENT","','","'?'","':'","'.'","EVAL_LAND","EVAL_LXOR",
"EVAL_LOR","'|'","'^'","'&'","EVAL_EQ","EVAL_NE","EVAL_STR_EQ","EVAL_STR_NE",
"EVAL_LT","EVAL_LE","EVAL_GT","EVAL_GE","EVAL_STR_LT","EVAL_STR_LE","EVAL_STR_GT",
"EVAL_STR_GE","EVAL_SHL","EVAL_SHR","'-'","'+'","'*'","'/'","'%'","NEG","EVAL_POW",
"'('","')'","input","scalar","scalarlist_or_null","scalarlist","identifier", NULL
};
#endif

static const short yyr1[] = {     0,
    40,    41,    41,    41,    41,    41,    41,    41,    41,    41,
    41,    41,    41,    41,    41,    41,    41,    41,    41,    41,
    41,    41,    41,    41,    41,    41,    41,    41,    41,    41,
    41,    41,    41,    41,    41,    41,    42,    42,    43,    43,
    44
};

static const short yyr2[] = {     0,
     1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     2,     3,     5,     3,     4,     1,     0,     1,     1,     3,
     1
};

static const short yydefact[] = {     0,
     2,     3,     4,    41,     0,     0,     1,    36,    31,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    37,    32,
     0,    34,    22,    23,    24,    11,    12,    10,    16,    17,
    25,    26,    20,    21,    18,    19,    29,    30,    27,    28,
    14,    15,     9,     8,     5,     6,     7,    13,    39,     0,
    38,     0,    35,     0,    33,    40,     0,     0,     0
};

static const short yydefgoto[] = {    77,
    69,    70,    71,     8
};

static const short yypact[] = {    36,
-32768,-32768,-32768,-32768,    36,    36,   125,   -37,   -35,    65,
    36,    36,    36,    36,    36,    36,    36,    36,    36,    36,
    36,    36,    36,    36,    36,    36,    36,    36,    36,    36,
    36,    36,    36,    36,    36,    36,    36,    36,    36,-32768,
    97,   152,    31,   177,   201,   224,   246,   267,   288,   288,
   288,   288,   305,   305,   305,   305,   305,   305,   305,   305,
   314,   314,   319,   319,   -35,   -35,   -35,-32768,   125,   -36,
    -3,    36,-32768,    36,   125,-32768,     7,     8,-32768
};

static const short yypgoto[] = {-32768,
     0,-32768,   -65,-32768
};


#define	YYLAST		356


static const short yytable[] = {     7,
    39,    38,    73,    74,     9,    10,    78,    79,    76,     0,
    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
    51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
    61,    62,    63,    64,    65,    66,    67,    68,     1,     2,
     3,     4,    14,    15,    16,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,     5,    38,     0,     0,
     0,    75,    11,     6,    12,    13,    14,    15,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
     0,    38,     0,    40,    11,    72,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    11,    38,    12,    13,    14,    15,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
     0,    38,    13,    14,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,     0,    38,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,     0,    38,    16,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,     0,    38,    17,    18,
    19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    37,     0,
    38,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,     0,    38,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,     0,    38,-32768,-32768,-32768,-32768,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,     0,    38,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,    31,    32,    33,    34,    35,    36,    37,
     0,    38,-32768,-32768,    33,    34,    35,    36,    37,     0,
    38,    35,    36,    37,     0,    38
};

static const short yycheck[] = {     0,
    38,    37,    39,     7,     5,     6,     0,     0,    74,    -1,
    11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,     3,     4,
     5,     6,    12,    13,    14,    15,    16,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    31,    37,    -1,    -1,
    -1,    72,     8,    38,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    -1,    37,    -1,    39,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,     8,    37,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    -1,    37,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    33,    34,    35,    -1,    37,    13,
    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    -1,    37,    14,    15,    16,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    -1,    37,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
    37,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    -1,    37,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    -1,    37,    17,    18,    19,    20,    21,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    -1,    37,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    -1,    37,    29,    30,    31,    32,    33,    34,    35,    -1,
    37,    33,    34,    35,    -1,    37
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 1288 "eval.y"
{ *(scalar_t*)resultptr=yyvsp[0].scalar; ;
    break;}
case 2:
#line 1291 "eval.y"
{ yyval.scalar = yyvsp[0].scalar; ;
    break;}
case 3:
#line 1292 "eval.y"
{ yyval.scalar = yyvsp[0].scalar; ;
    break;}
case 4:
#line 1293 "eval.y"
{ yyval.scalar = yyvsp[0].scalar; ;
    break;}
case 5:
#line 1294 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, '*')) YYERROR; ;
    break;}
case 6:
#line 1295 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, '/')) YYERROR; ;
    break;}
case 7:
#line 1296 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, '%')) YYERROR; ;
    break;}
case 8:
#line 1297 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, '+')) YYERROR; ;
    break;}
case 9:
#line 1298 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, '-')) YYERROR; ;
    break;}
case 10:
#line 1299 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, '&')) YYERROR; ;
    break;}
case 11:
#line 1300 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, '|')) YYERROR; ;
    break;}
case 12:
#line 1301 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, '^')) YYERROR; ;
    break;}
case 13:
#line 1302 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_POW)) YYERROR; ;
    break;}
case 14:
#line 1303 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_SHL)) YYERROR; ;
    break;}
case 15:
#line 1304 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_SHR)) YYERROR; ;
    break;}
case 16:
#line 1305 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_EQ)) YYERROR; ;
    break;}
case 17:
#line 1306 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_NE)) YYERROR; ;
    break;}
case 18:
#line 1307 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_GT)) YYERROR; ;
    break;}
case 19:
#line 1308 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_GE)) YYERROR; ;
    break;}
case 20:
#line 1309 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_LT)) YYERROR; ;
    break;}
case 21:
#line 1310 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_LE)) YYERROR; ;
    break;}
case 22:
#line 1311 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_LAND)) YYERROR; ;
    break;}
case 23:
#line 1312 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_LXOR)) YYERROR; ;
    break;}
case 24:
#line 1313 "eval.y"
{ if (!scalar_op(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_LOR)) YYERROR; ;
    break;}
case 25:
#line 1314 "eval.y"
{ scalar_strop(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_STR_EQ); ;
    break;}
case 26:
#line 1315 "eval.y"
{ scalar_strop(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_STR_NE); ;
    break;}
case 27:
#line 1316 "eval.y"
{ scalar_strop(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_STR_GT); ;
    break;}
case 28:
#line 1317 "eval.y"
{ scalar_strop(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_STR_GE); ;
    break;}
case 29:
#line 1318 "eval.y"
{ scalar_strop(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_STR_LT); ;
    break;}
case 30:
#line 1319 "eval.y"
{ scalar_strop(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar, EVAL_STR_LE); ;
    break;}
case 31:
#line 1320 "eval.y"
{ scalar_negset(&yyval.scalar, &yyvsp[0].scalar); ;
    break;}
case 32:
#line 1321 "eval.y"
{ yyval.scalar = yyvsp[-1].scalar; ;
    break;}
case 33:
#line 1322 "eval.y"
{ scalar_miniif(&yyval.scalar, &yyvsp[-4].scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar); ;
    break;}
case 34:
#line 1324 "eval.y"
{
			scalar_concat(&yyval.scalar, &yyvsp[-2].scalar, &yyvsp[0].scalar);
			scalar_destroy(&yyvsp[-2].scalar);
			scalar_destroy(&yyvsp[0].scalar);
		;
    break;}
case 35:
#line 1330 "eval.y"
{
			int r=evalfunc(&yyval.scalar, yyvsp[-3].ident, &yyvsp[-1].scalars);
			scalarlist_destroy(&yyvsp[-1].scalars);
			free(yyvsp[-3].ident);
			if (!r) YYERROR;
		;
    break;}
case 36:
#line 1337 "eval.y"
{
			int r=evalsymbol(&yyval.scalar, yyvsp[0].ident);
			free(yyvsp[0].ident);
			if (!r) YYERROR;
		;
    break;}
case 37:
#line 1345 "eval.y"
{
			scalarlist_t s;
			s.count=0;
			s.scalars=NULL;
			yyval.scalars = s;
		;
    break;}
case 38:
#line 1351 "eval.y"
{ yyval.scalars = yyvsp[0].scalars; ;
    break;}
case 39:
#line 1353 "eval.y"
{ scalarlist_set(&yyval.scalars, &yyvsp[0].scalar); ;
    break;}
case 40:
#line 1355 "eval.y"
{
			scalarlist_concat(&yyval.scalars, &yyvsp[-2].scalars, &yyvsp[0].scalars);
			scalarlist_destroy_gentle(&yyvsp[-2].scalars);
			scalarlist_destroy_gentle(&yyvsp[0].scalars);
		;
    break;}
case 41:
#line 1362 "eval.y"
{ yyval.ident = yyvsp[0].ident; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 1365 "eval.y"

