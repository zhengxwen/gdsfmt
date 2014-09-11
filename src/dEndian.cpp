// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dEndian.cpp: Cross-platform functions with independent endianness
//
// Copyright (C) 2007 - 2014	Xiuwen Zheng
//
// This file is part of CoreArray.
//
// CoreArray is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License Version 3 as
// published by the Free Software Foundation.
//
// CoreArray is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with CoreArray.
// If not, see <http://www.gnu.org/licenses/>.

#include <dEndian.h>


namespace CoreArray
{
// =====================================================================
// Endian Macro and Function
// =====================================================================

#ifndef COREARRAY_ENDIAN_LITTLE

// native format to little endian, array

void NT_TO_LE_ARRAY(C_Int16 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = NT_TO_LE(*p);
}

void NT_TO_LE_ARRAY(C_UInt16 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = NT_TO_LE(*p);
}

void NT_TO_LE_ARRAY(C_Int32 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = NT_TO_LE(*p);
}

void NT_TO_LE_ARRAY(C_UInt32 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = NT_TO_LE(*p);
}

void NT_TO_LE_ARRAY(C_Int64 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = NT_TO_LE(*p);
}

void NT_TO_LE_ARRAY(C_UInt64 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = NT_TO_LE(*p);
}

void NT_TO_LE_ARRAY(C_Float32 *p, size_t n)
{
	C_UInt32 *pp = (C_UInt32*)p;
	for (; n > 0; n--, pp++) *pp = NT_TO_LE(*pp);
}

void NT_TO_LE_ARRAY(C_Float64 *p, size_t n)
{
	C_UInt64 *pp = (C_UInt64*)p;
	for (; n > 0; n--, pp++) *pp = NT_TO_LE(*pp);
}

void NT_TO_LE_ARRAY2(C_Int16 *d, const C_Int16 *s, size_t n)
{
	for (; n > 0; n--) *d++ = NT_TO_LE(*s++);
}

void NT_TO_LE_ARRAY2(C_UInt16 *d, const C_UInt16 *s, size_t n)
{
	for (; n > 0; n--) *d++ = NT_TO_LE(*s++);
}

void NT_TO_LE_ARRAY2(C_Int32 *d, const C_Int32 *s, size_t n)
{
	for (; n > 0; n--) *d++ = NT_TO_LE(*s++);
}

void NT_TO_LE_ARRAY2(C_UInt32 *d, const C_UInt32 *s, size_t n)
{
	for (; n > 0; n--) *d++ = NT_TO_LE(*s++);
}

void NT_TO_LE_ARRAY2(C_Int64 *d, const C_Int64 *s, size_t n)
{
	for (; n > 0; n--) *d++ = NT_TO_LE(*s++);
}

void NT_TO_LE_ARRAY2(C_UInt64 *d, const C_UInt64 *s, size_t n)
{
	for (; n > 0; n--) *d++ = NT_TO_LE(*s++);
}

void NT_TO_LE_ARRAY2(C_Float32 *d, const C_Float32 *s, size_t n)
{
	C_UInt32 *dd = (C_UInt32*)d;
	C_UInt32 *ss = (C_UInt32*)s;
	for (; n > 0; n--) *dd++ = NT_TO_LE(*ss++);
}

void NT_TO_LE_ARRAY2(C_Float64 *d, const C_Float64 *s, size_t n)
{
	C_UInt64 *dd = (C_UInt64*)d;
	C_UInt64 *ss = (C_UInt64*)s;
	for (; n > 0; n--) *dd++ = NT_TO_LE(*ss++);
}


// native format to little endian, array

void LE_TO_NT_ARRAY(C_Int16 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = LE_TO_NT(*p);
}

void LE_TO_NT_ARRAY(C_UInt16 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = LE_TO_NT(*p);
}

void LE_TO_NT_ARRAY(C_Int32 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = LE_TO_NT(*p);
}

void LE_TO_NT_ARRAY(C_UInt32 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = LE_TO_NT(*p);
}

void LE_TO_NT_ARRAY(C_Int64 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = LE_TO_NT(*p);
}

void LE_TO_NT_ARRAY(C_UInt64 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = LE_TO_NT(*p);
}

void LE_TO_NT_ARRAY(C_Float32 *p, size_t n)
{
	C_UInt32 *pp = (C_UInt32*)p;
	for (; n > 0; n--, pp++) *pp = LE_TO_NT(*pp);
}

void LE_TO_NT_ARRAY(C_Float64 *p, size_t n)
{
	C_UInt64 *pp = (C_UInt64*)p;
	for (; n > 0; n--, pp++) *pp = LE_TO_NT(*pp);
}

void LE_TO_NT_ARRAY2(C_Int16 *d, const C_Int16 *s, size_t n)
{
	for (; n > 0; n--) *d++ = LE_TO_NT(*s++);
}

void LE_TO_NT_ARRAY2(C_UInt16 *d, const C_UInt16 *s, size_t n)
{
	for (; n > 0; n--) *d++ = LE_TO_NT(*s++);
}

void LE_TO_NT_ARRAY2(C_Int32 *d, const C_Int32 *s, size_t n)
{
	for (; n > 0; n--) *d++ = LE_TO_NT(*s++);
}

void LE_TO_NT_ARRAY2(C_UInt32 *d, const C_UInt32 *s, size_t n)
{
	for (; n > 0; n--) *d++ = LE_TO_NT(*s++);
}

void LE_TO_NT_ARRAY2(C_Int64 *d, const C_Int64 *s, size_t n)
{
	for (; n > 0; n--) *d++ = LE_TO_NT(*s++);
}

void LE_TO_NT_ARRAY2(C_UInt64 *d, const C_UInt64 *s, size_t n)
{
	for (; n > 0; n--) *d++ = LE_TO_NT(*s++);
}

void LE_TO_NT_ARRAY2(C_Float32 *d, const C_Float32 *s, size_t n)
{
	C_UInt32 *dd = (C_UInt32*)d;
	C_UInt32 *ss = (C_UInt32*)s;
	for (; n > 0; n--) *dd++ = LE_TO_NT(*ss++);
}

void LE_TO_NT_ARRAY2(C_Float64 *d, const C_Float64 *s, size_t n)
{
	C_UInt64 *dd = (C_UInt64*)d;
	C_UInt64 *ss = (C_UInt64*)s;
	for (; n > 0; n--) *dd++ = LE_TO_NT(*ss++);
}


#endif
}
