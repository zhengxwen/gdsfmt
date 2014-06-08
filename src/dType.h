// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dType.h: Elementary types
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

/**
 *	\file     dType.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Elementary types
 *	\details
**/


#ifndef _HEADER_COREARRAY_TYPE_
#define _HEADER_COREARRAY_TYPE_

#include <CoreDEF.h>

#ifndef __STDC_LIMIT_MACROS
#  define __STDC_LIMIT_MACROS
#endif

#ifdef COREARRAY_UNIX
#  include <unistd.h>
#endif

#ifdef COREARRAY_MSC
#  include <msvc/stdint.h>
#else
#  include <stdint.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

	// ******************************************************************** //

	/// Memory data type id
	enum C_SVType
	{
		svCustom = 0,   ///< Unknown or customized type
		svCustomInt,    ///< Customized signed integer
		svCustomUInt,   ///< Customized unsigned integer
		svCustomFloat,  ///< Customized float number
		svCustomStr,    ///< Customized string type
		svInt8,         ///< Signed integer of 8 bits
		svUInt8,        ///< Unsigned integer of 8 bits
		svInt16,        ///< Signed integer of 16 bits
		svUInt16,       ///< Unsigned integer of 16 bits
		svInt32,        ///< Signed integer of 32 bits
		svUInt32,       ///< Unsigned integer of 32 bits
		svInt64,        ///< Signed integer of 64 bits
		svUInt64,       ///< Unsigned integer of 64 bits
		svFloat32,      ///< Float number of single precision (32 bits)
		svFloat64,      ///< Float number of double precision (64 bits)
		svStrUTF8,      ///< UTF-8 string
		svStrUTF16      ///< UTF-16 string
	};


	/// Whether x (C_SVType) is an integer or not
	#define COREARRAY_SV_INTEGER(x) \
		((svInt8<=(x) && (x)<=svUInt64) || (x)==svCustomInt || (x)==svCustomUInt)

	/// Whether x (C_SVType) is a signed integer or not
	#define COREARRAY_SV_SINT(x) \
		((x)==svInt8 || (x)==svInt16 || (x)==svInt32 || (x)==svInt64 || (x)==svCustomInt)

	/// Whether x (C_SVType) is an unsigned integer or not
	#define COREARRAY_SV_UINT(x) \
		((x)==svUInt8 || (x)==svUInt16 || (x)==svUInt32 || (x)==svUInt64 || (x)==svCustomUInt)

	/// Whether x (C_SVType) is a float number or not
	#define COREARRAY_SV_FLOAT(x) \
		((x)==svFloat32 || (x)==svFloat64 || (x)==svCustomFloat)

	/// Whether x (C_SVType) is a float number or not
	#define COREARRAY_SV_NUMERIC(x) \
		(COREARRAY_SV_INTEGER(x) || COREARRAY_SV_FLOAT(x))

	/// Whether x (C_SVType) is a string or not
	#define COREARRAY_SV_STRING(x) \
		((x)==svStrUTF8 || (x)==svStrUTF16 || (x)==svCustomStr)


	// ******************************************************************** //

	// Integers

	/// Signed integer of 8 bits
	typedef int8_t        C_Int8;
	typedef C_Int8*       C_PInt8;		

	/// Unsigned integer of 8 bits
	typedef uint8_t       C_UInt8;
	typedef C_UInt8*      C_PUInt8;

	/// Signed integer of 16 bits
	typedef int16_t       C_Int16;
	typedef C_Int16*      C_PInt16;

	/// Unsigned integer of 16 bits
	typedef uint16_t      C_UInt16;
	typedef C_UInt16*     C_PUInt16;

	/// Signed integer of 32 bits
	typedef int32_t       C_Int32;
	typedef C_Int32*      C_PInt32;

	/// Unsigned integer of 32 bits
	typedef uint32_t      C_UInt32;
	typedef C_UInt32*     C_PUInt32;

	/// Signed integer of 64 bits
	typedef int64_t       C_Int64;
	typedef C_Int64*      C_PInt64;

	/// Unsigned integer of 64 bits
	typedef uint64_t      C_UInt64;
	typedef C_UInt64*     C_PUInt64;


	/// CoreArray Boolean
	typedef C_Int8        C_BOOL;
	typedef C_BOOL*       C_PBOOL;


	#if defined(COREARRAY_MSC) && !defined(ssize_t)
	typedef ptrdiff_t     ssize_t;
	#endif


	// ******************************************************************** //

	// Float

	/// Float number of single precision (32 bits)
	typedef float         C_Float32;
	/// Float number of double precision (64 bits)
	typedef double        C_Float64;
	/// Float number of long precision
	typedef long double   C_LongFloat;


#ifdef __cplusplus
}
#endif

#endif /* _HEADER_COREARRAY_TYPE_ */
