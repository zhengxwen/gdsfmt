// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dType.h: Fundamental types
//
// Copyright (C) 2007-2018    Xiuwen Zheng
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
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2007 - 2018
 *	\brief    Fundamental types
 *	\details
**/


#ifndef _HEADER_COREARRAY_TYPE_
#define _HEADER_COREARRAY_TYPE_

#include "CoreDEF.h"

#ifndef __STDC_LIMIT_MACROS
#   define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>

#ifdef COREARRAY_POSIX
#   // need 'ssize_t'
#   include <unistd.h>
#endif

#ifdef __cplusplus
#   // to define UTF8String, UTF16String and UTF32String
#   include <string>
#endif


#ifdef __cplusplus
extern "C" {
#endif

	// =======================================================================
	// Define type ID
	// =======================================================================

	/// Type ID for data used in memory
	enum C_SVType
	{
		svCustom      = 0,    ///< unknown or customized type

		svCustomInt   = 1,    ///< customized signed integer
		svCustomUInt  = 2,    ///< customized unsigned integer
		svCustomFloat = 3,    ///< customized float number
		svCustomStr   = 4,    ///< customized string type

		svInt8        = 5,    ///< 8-bit signed integer
		svUInt8       = 6,    ///< 8-bit unsigned integer
		svInt16       = 7,    ///< 16-bit signed integer
		svUInt16      = 8,    ///< 16-bit unsigned integer
		svInt32       = 9,    ///< 32-bit signed integer
		svUInt32      = 10,   ///< 32-bit unsigned integer
		svInt64       = 11,   ///< 64-bit signed integer
		svUInt64      = 12,   ///< 64-bit unsigned integer

		svFloat32     = 13,   ///< 32-bit floating point number
		svFloat64     = 14,   ///< 64-bit floating point number

		svStrUTF8     = 15,   ///< UTF-8 string
		svStrUTF16    = 16    ///< UTF-16 string
	};

	/// Whether x (C_SVType) is valid
	#define COREARRAY_SV_VALID(x) \
		((svInt8<=(x) && (x)<=svStrUTF16))

	/// Whether x (C_SVType) is integer
	#define COREARRAY_SV_INTEGER(x) \
		((svInt8<=(x) && (x)<=svUInt64) || (x)==svCustomInt || (x)==svCustomUInt)

	/// Whether x (C_SVType) is signed integer
	#define COREARRAY_SV_SINT(x) \
		((x)==svInt8 || (x)==svInt16 || (x)==svInt32 || (x)==svInt64 || (x)==svCustomInt)

	/// Whether x (C_SVType) is unsigned integer
	#define COREARRAY_SV_UINT(x) \
		((x)==svUInt8 || (x)==svUInt16 || (x)==svUInt32 || (x)==svUInt64 || (x)==svCustomUInt)

	/// Whether x (C_SVType) is floating number
	#define COREARRAY_SV_FLOAT(x) \
		((x)==svFloat32 || (x)==svFloat64 || (x)==svCustomFloat)

	/// Whether x (C_SVType) is a number
	#define COREARRAY_SV_NUMERIC(x) \
		(COREARRAY_SV_INTEGER(x) || COREARRAY_SV_FLOAT(x))

	/// Whether x (C_SVType) is string
	#define COREARRAY_SV_STRING(x) \
		((x)==svStrUTF8 || (x)==svStrUTF16 || (x)==svCustomStr)



	// =======================================================================
	// Integers
	// =======================================================================

	/// 8-bit signed integer
	typedef int8_t        C_Int8;
	/// Pointer to 8-bit signed integer
	typedef C_Int8*       C_PInt8;		

	/// 8-bit unsigned integer
	typedef uint8_t       C_UInt8;
	/// Pointer to 8-bit unsigned integer
	typedef C_UInt8*      C_PUInt8;

	/// 16-bit signed integer
	typedef int16_t       C_Int16;
	/// Pointer to 16-bit signed integer
	typedef C_Int16*      C_PInt16;

	/// 16-bit unsigned integer
	typedef uint16_t      C_UInt16;
	/// Pointer to 16-bit unsigned integer
	typedef C_UInt16*     C_PUInt16;

	/// 32-bit signed integer
	typedef int32_t       C_Int32;
	/// Pointer to 32-bit signed integer
	typedef C_Int32*      C_PInt32;

	/// 32-bit unsigned integer
	typedef uint32_t      C_UInt32;
	/// Pointer to 32-bit unsigned integer
	typedef C_UInt32*     C_PUInt32;

	/// 64-bit signed integer
	typedef int64_t       C_Int64;
	/// Pointer to 64-bit signed integer
	typedef C_Int64*      C_PInt64;

	/// 64-bit unsigned integer
	typedef uint64_t      C_UInt64;
	/// Pointer to 64-bit unsigned integer
	typedef C_UInt64*     C_PUInt64;

	/// CoreArray Boolean
	typedef C_Int8        C_BOOL;
	/// Pointer to CoreArray Boolean
	typedef C_BOOL*       C_PBOOL;



	// =======================================================================
	// Floating point number
	// =======================================================================

	/// 32-bit floating point number
	typedef float         C_Float32;
	/// Pointer to 32-bit floating point number
	typedef C_Float32*    C_PFloat32;

	/// 64-bit floating point number
	typedef double        C_Float64;
	/// Pointer to 64-bit floating point number
	typedef C_Float64*    C_PFloat64;

	/// floating point number of long precision
	typedef long double   C_LongFloat;
	/// Pointer to floating point number of long precision
	typedef C_LongFloat*  C_PLongFloat;



	// =======================================================================
	// Character
	// =======================================================================

	/// UTF-8 character
	typedef C_UInt8     C_UTF8;
	/// UTF-16 character
	typedef C_UInt16    C_UTF16;
	/// UTF-32 character
	typedef C_UInt32    C_UTF32;

#ifdef __cplusplus
}
#endif


// ===========================================================================

#ifdef __cplusplus
namespace CoreArray
{
	// =======================================================================
	// String
	// =======================================================================

	/// Raw string
	typedef std::string                   RawString;
	/// UTF-8 string
	typedef std::string                   UTF8String;
	/// UTF-16 string
	typedef std::basic_string<C_UTF16>    UTF16String;
	/// UTF-32 string
	typedef std::basic_string<C_UTF32>    UTF32String;



	// =======================================================================
	// Customized integer
	// =======================================================================

	/// Customized integer type
	/**   This template creates a new and unique integer type with a signature,
	 *  so that users can define new operator behaviors for this new type,
	 *  e.g., operator >>(TdInteger &val)
	 *  \tparam TYPE         integer-type
	 *  \tparam SIGNATURE    signature for uniqueness
	**/
	template<typename TYPE, unsigned SIGNATURE>
		struct COREARRAY_DLL_DEFAULT TdInteger
	{
	public:
		/// constructor
		TdInteger() {}
		/// constructor with value
		TdInteger(TYPE val) { fVal = val; }

		COREARRAY_INLINE TdInteger<TYPE, SIGNATURE>& operator+= (TYPE val)
			{ fVal += val; return *this; }
		COREARRAY_INLINE TdInteger<TYPE, SIGNATURE>& operator-= (TYPE val)
			{ fVal -= val; return *this; }
		COREARRAY_INLINE TdInteger<TYPE, SIGNATURE>& operator++ ()
			{ fVal++; return *this; }
		COREARRAY_INLINE TdInteger<TYPE, SIGNATURE>& operator-- ()
			{ fVal--; return *this; }

		/// assignment
		COREARRAY_INLINE TdInteger<TYPE, SIGNATURE>& operator= (TYPE val)
			{ fVal = val; return *this; }

		COREARRAY_INLINE bool operator== (const TdInteger<TYPE, SIGNATURE> &val) const
			{ return fVal == val.fVal; }
		COREARRAY_INLINE bool operator!= (const TdInteger<TYPE, SIGNATURE> &val) const
			{ return fVal != val.fVal; }

		/// return the value
		COREARRAY_INLINE operator TYPE() const
			{ return fVal; }
		/// return a reference
		COREARRAY_INLINE TYPE& Get()
			{ return fVal; }
		/// return a const reference
		COREARRAY_INLINE const TYPE& Get() const
			{ return fVal; }

	private:
		TYPE fVal;
    };
}
#endif

#endif /* _HEADER_COREARRAY_TYPE_ */
