/* 
 *	HT Editor
 *	htexcept.h
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

#ifndef __HTEXCEPT_H__
#define __HTEXCEPT_H__

#include <exception>

/*
 *	CLASS ht_exception
 */

class ht_exception {
public:
			ht_exception();
/* new */
	virtual const char *what();
};

/*
 *	CLASS ht_io_exception
 */

#define HT_IO_EXCEPTION_MAX_ERRSTR		256

class ht_io_exception: public ht_exception {
protected:
	char estr[HT_IO_EXCEPTION_MAX_ERRSTR];
public:
			    ht_io_exception(char *errstr,...);
/* new */
	virtual const char *what();
};

#endif /* __HTEXCEPT_H__ */
