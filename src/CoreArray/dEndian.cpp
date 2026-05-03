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
// Copyright (C) 2007-2026    Xiuwen Zheng
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

#include "dEndian.h"


#if !defined(COREARRAY_ENDIAN_LITTLE) && !defined(COREARRAY_ENDIAN_BIG)
#  error "Endianness is not defined (neither COREARRAY_ENDIAN_LITTLE nor COREARRAY_ENDIAN_BIG)."
#endif


namespace CoreArray
{
// =====================================================================
// Endian Macro and Function
// =====================================================================

#ifndef COREARRAY_ENDIAN_LITTLE

// Helpers to byte-swap floats. Use std::memcpy for type-punning between
// float and the corresponding unsigned integer representation. Modern
// compilers fold small fixed-size std::memcpy into a single load/store,
// and it is the only standard-compliant way to reinterpret the bit
// pattern of a float without violating strict aliasing.

static COREARRAY_FORCEINLINE C_Float32 bswap_f32(C_Float32 v)
{
	C_UInt32 u;
	std::memcpy(&u, &v, sizeof(u));
	u = NT_TO_LE(u);
	std::memcpy(&v, &u, sizeof(u));
	return v;
}

static COREARRAY_FORCEINLINE C_Float64 bswap_f64(C_Float64 v)
{
	C_UInt64 u;
	std::memcpy(&u, &v, sizeof(u));
	u = NT_TO_LE(u);
	std::memcpy(&v, &u, sizeof(u));
	return v;
}

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
	for (; n > 0; n--, p++) *p = bswap_f32(*p);
}

void NT_TO_LE_ARRAY(C_Float64 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = bswap_f64(*p);
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
	for (; n > 0; n--) *d++ = bswap_f32(*s++);
}

void NT_TO_LE_ARRAY2(C_Float64 *d, const C_Float64 *s, size_t n)
{
	for (; n > 0; n--) *d++ = bswap_f64(*s++);
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
	// NT_TO_LE and LE_TO_NT are identical byte-swaps for the non-LE branch
	for (; n > 0; n--, p++) *p = bswap_f32(*p);
}

void LE_TO_NT_ARRAY(C_Float64 *p, size_t n)
{
	for (; n > 0; n--, p++) *p = bswap_f64(*p);
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
	for (; n > 0; n--) *d++ = bswap_f32(*s++);
}

void LE_TO_NT_ARRAY2(C_Float64 *d, const C_Float64 *s, size_t n)
{
	for (; n > 0; n--) *d++ = bswap_f64(*s++);
}


#endif
}
