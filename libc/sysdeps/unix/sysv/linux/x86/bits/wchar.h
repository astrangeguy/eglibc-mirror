/* wchar_t type related definitions.  i386/x86-64 version.
   Copyright (C) 2000-2013 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _BITS_WCHAR_H
#define _BITS_WCHAR_H	1

#include <bits/wordsize.h>

#if __WORDSIZE == 64
# define __WCHAR_MIN	(-2147483647 - 1)
# define __WCHAR_MAX	(2147483647)
#else
# define __WCHAR_MIN	(-2147483647l - 1l)
# define __WCHAR_MAX	(2147483647l)
#endif

#endif	/* bits/wchar.h */
