/*
 *	HT Editor
 *	stddata.cc
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

#include <stdlib.h>

#include "analy_register.h"

#include "htatom.h"
#include "stddata.h"
#include "stream.h"
#include "tools.h"

/*
 *   Area
 */
void Area::init()
{
	a = NULL;
}

void areaload(ht_object_stream *st, area_s *&p, int level, int &left)
{
	if (left<=0) {
		p = NULL;
		return;
	}
	p = (area_s *) smalloc0(sizeof(area_s));
	if ((level<=1) || (left<=1)) {
		st->getObject(p->start, "start");
		st->getObject(p->end, "end");
		p->left = p->right = NULL;
		left--;
	} else {
		areaload(st, p->left, level / 2, left);
		st->getObject(p->start, "start");
		st->getObject(p->end, "end");
		left--;
		areaload(st, p->right, level / 2 -1, left);
	}
}

int	Area::load(ht_object_stream *st)
{
	int count;
	GET_INT_DEC(st, count);
	areaload(st, a, count, count);
	return st->get_error();
}

void Area::done()
{
	freeRecursive(a);
}

OBJECT_ID	Area::object_id()
{
	return ATOM_AREA;
}

area_s *areaget(area_s *p, Object *V)
{
	if (p) {
		if (V->compareTo(p->start) < 0) return areaget(p->left, V);
		if (V->compareTo(p->end) >= 0) return areaget(p->right, V);
		/*if ((V >= (p->start)) && (V < (p->end)))*/ return p;
	} else return NULL;
}

area_s *Area::getArea(Object *at)
{
	return areaget(a, at);
}

void areaadd(area_s *&p, Object *Start, Object *End)
{
	if (p) {
		if ((Start->compareTo(p->start) >= 0) && (Start->compareTo(p->end)<=0)) {
			if (p->end->compareTo(End) < 0) {
				delete p->end;
				p->end = End->duplicate();
			}
			if ((End->compareTo(p->start) >= 0) && (End->compareTo(p->end)<=0)) {
				if (p->start->compareTo(Start) > 0) {
					delete p->start;
					p->start = Start->duplicate();
				}
			}
			return;
		}
		if ((End->compareTo(p->start) >= 0) && (End->compareTo(p->end) <= 0)) {
			if (p->start->compareTo(Start) > 0) {
				delete p->start;
				p->start = Start->duplicate();
			}
			return;
		}
		if (Start->compareTo(p->end) > 0) areaadd(p->right, Start, End);
					  else  areaadd(p->left, Start, End);
	} else {
		// new p
		area_s *tmp = (area_s *) smalloc(sizeof(area_s));
		p = tmp;
		p->start = Start->duplicate();
		p->end = End->duplicate();
		p->left = NULL;
		p->right = NULL;
	}
}

void Area::add(Object *Start, Object *End)
{
	areaadd(a, Start, End);
}

bool areacontains(area_s *p, Object *V)
{
	if (p) {
		if (V->compareTo(p->start) < 0) return areacontains(p->left, V);
		if (V->compareTo(p->end) >= 0) return areacontains(p->right, V);
		/*if ((V >= (p->start)) && (V < (p->end)))*/ return true;
	} else return false;
}

bool Area::contains(Object *v)
{
	if (v->instanceOf(ATOM_ADDRESS_INVALID)) return false;
	return areacontains(a, v);
}

void areafindnext(area_s *p, Object *from, Object **res)
{
	if (!from || from->compareTo(p->start) < 0) {
		*res = p->start;
		if (p->left) areafindnext(p->left, from, res);
	} else if (from->compareTo(p->end) >= 0) {
		if (p->right) areafindnext(p->right, from, res);
	} else *res = from;
}

Object *Area::findNext(Object *from)
{
	Object *res = NULL;
	if (a) areafindnext(a, from, &res);
	return res;
}

void areafindprev(area_s *p, Object *from, Object **res)
{
//FIXME ??:
	if (p) {
		if (from->compareTo(p->start) < 0) {
			if (p->left) areafindprev(p->left, from, res);
		} else if (from->compareTo(p->end) >= 0) {
			*res = p->start;
			areafindprev(p->right, from, res);
		} else *res = from;
	} else *res = NULL;
}

Object *Area::findPrev(Object *from)
{
	Object *res;
	areafindprev(a, from, &res);
	return res;
}

void Area::freeRecursive(area_s *p)
{
	if (p) {
		freeRecursive(p->left);
		freeRecursive(p->right);
		delete p->start;
		delete p->end;
		free(p);
	}
}

void areacount(area_s *p, int &c, Object **startend)
{
	if (p) {
		areacount(p->left, c, startend);
		if (!*startend || p->start->compareTo(*startend) != 0) c++;
		*startend = p->end;
		areacount(p->right, c, startend);
	}
}

void areastore(ht_object_stream *f, area_s *p, Object **startend)
{
	if (p) {
		areastore(f, p->left, startend);
		if (!*startend) {
			f->putObject(p->start, "start");
		} else {
			if ((*startend)->compareTo(p->start) != 0) {
				f->putObject(*startend, "end");
				f->putObject(p->start, "start");
			}
		}
		*startend = p->end;
		areastore(f, p->right, startend);
	}
}

void Area::store(ht_object_stream *f)
{
	int count = 0;
	Object *start = NULL;
	areacount(a, count, &start);
	PUT_INT_DEC(f, count);
	start = NULL;
	areastore(f, a, &start);
	if (start!=NULL) f->putObject(start, "end");
}

#ifdef DEBUG_FIXNEW
void areadump(int sp, area_s *A)
{
	if (A) {
		for (int i=0; i<sp; i++) printf(" ");
		printf("%08x %08x\n", A->start, A->end);
		++sp;++sp;
		areadump(sp, A->left);
		areadump(sp, A->right);
	}
}

void Area::dump()
{
	areadump(1, a);
}
#endif

/*
 *  BUILDER etc.
 */

BUILDER(ATOM_AREA, Area)

bool init_stddata()
{
	REGISTER(ATOM_AREA, Area)
	return true;
}

void done_stddata()
{
	UNREGISTER(ATOM_AREA, Area)
}