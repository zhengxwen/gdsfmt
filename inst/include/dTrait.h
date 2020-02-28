// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dTrait.h: Trait classes for elementary types
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
 *	\file     dTrait.h
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2007 - 2018
 *	\brief    Trait classes for elementary types
 *	\details
**/


#ifndef _HEADER_COREARRAY_TRAIT_
#define _HEADER_COREARRAY_TRAIT_

#include "dType.h"

#include <cfloat>
#include <limits>
#include <string>


namespace CoreArray
{
	// =======================================================================

	// Trait flags
	#define COREARRAY_TR_INT_FLAG        0x0100
	#define COREARRAY_TR_REAL_FLAG       0x0200
	#define COREARRAY_TR_NUMERIC_FLAG    (COREARRAY_TR_INT_FLAG | COREARRAY_TR_REAL_FLAG)
	#define COREARRAY_TR_STRING_FLAG     0x0400

	#define COREARRAY_TR_UNKNOWN                 -1
	#define COREARRAY_TR_CUSTOM                   0

	#define COREARRAY_TR_INTEGER                 (COREARRAY_TR_INT_FLAG | 0)
	#define COREARRAY_TR_BIT_INTEGER             (COREARRAY_TR_INT_FLAG | 1)
	#define COREARRAY_TR_VARIABLE_LEN_INTEGER    (COREARRAY_TR_INT_FLAG | 2)
	#define COREARRAY_TR_SPARSE_INTEGER          (COREARRAY_TR_INT_FLAG | 3)

	#define COREARRAY_TR_FLOAT                   (COREARRAY_TR_REAL_FLAG | 0)
	#define COREARRAY_TR_PACKED_REAL             (COREARRAY_TR_REAL_FLAG | 1)
	#define COREARRAY_TR_SPARSE_REAL             (COREARRAY_TR_REAL_FLAG | 2)

	#define COREARRAY_TR_STRING                  (COREARRAY_TR_STRING_FLAG | 0)
	#define COREARRAY_TR_FIXED_LEN_STRING        (COREARRAY_TR_STRING_FLAG | 1)
	#define COREARRAY_TR_VARIABLE_LEN_STRING     (COREARRAY_TR_STRING_FLAG | 2)



	// =======================================================================
	// Traits of integer
	// =======================================================================

	/// Trait information
	/** The structure provides 'TType', 'ElmType' and others.
	 *  \tparam TYPE    fundamental  or user-defined type
	**/
	template<typename TYPE> struct COREARRAY_DLL_DEFAULT TdTraits
	{
    	typedef TYPE TType;
		typedef TYPE ElmType;
		static const int trVal = COREARRAY_TR_UNKNOWN;
		static const unsigned BitOf = sizeof(TYPE)*8u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svCustom;
	};


	/// Trait information for C_Int8
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<C_Int8>
	{
		typedef C_Int8 TType;
		typedef C_Int8 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 8u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svInt8;

		static const char *TraitName() { return "Int8"; }
		static const char *StreamName() { return "dInt8"; }

		COREARRAY_INLINE static short Min() { return INT8_MIN; }
		COREARRAY_INLINE static short Max() { return INT8_MAX; }
	};


	/// Trait information for C_UInt8
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<C_UInt8>
	{
		typedef C_UInt8 TType;
		typedef C_UInt8 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 8u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svUInt8;

		static const char *TraitName() { return "UInt8"; }
		static const char *StreamName() { return "dUInt8"; }

		COREARRAY_INLINE static unsigned short Min() { return 0; }
		COREARRAY_INLINE static unsigned short Max() { return UINT8_MAX; }
	};


	/// Trait information for C_Int16
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<C_Int16>
	{
		typedef C_Int16 TType;
		typedef C_Int16 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 16u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svInt16;

		static const char *TraitName() { return "Int16"; }
		static const char *StreamName() { return "dInt16"; }

		COREARRAY_INLINE static C_Int16 Min() { return INT16_MIN; }
		COREARRAY_INLINE static C_Int16 Max() { return INT16_MAX; }
	};


	/// Trait information for C_UInt16
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<C_UInt16>
	{
		typedef C_UInt16 TType;
		typedef C_UInt16 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 16u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svUInt16;

		static const char *TraitName() { return "UInt16"; }
		static const char *StreamName() { return "dUInt16"; }

		COREARRAY_INLINE static C_UInt16 Min() { return 0; }
		COREARRAY_INLINE static C_UInt16 Max() { return UINT16_MAX; }
	};


	/// Trait information for C_Int32
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<C_Int32>
	{
		typedef C_Int32 TType;
		typedef C_Int32 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svInt32;

		static const char *TraitName() { return "Int32"; }
		static const char *StreamName() { return "dInt32"; }

		COREARRAY_INLINE static C_Int32 Min() { return INT32_MIN; }
		COREARRAY_INLINE static C_Int32 Max() { return INT32_MAX; }
	};


	/// Trait information for C_UInt32
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<C_UInt32>
	{
		typedef C_UInt32 TType;
		typedef C_UInt32 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svUInt32;

		static const char *TraitName() { return "UInt32"; }
		static const char *StreamName() { return "dUInt32"; }

		COREARRAY_INLINE static C_UInt32 Min() { return 0; }
		COREARRAY_INLINE static C_UInt32 Max() { return UINT32_MAX; }
	};


	/// Trait information for C_Int64
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<C_Int64>
	{
		typedef C_Int64 TType;
		typedef C_Int64 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 64u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svInt64;

		static const char *TraitName() { return "Int64"; }
		static const char *StreamName() { return "dInt64"; }

		COREARRAY_INLINE static C_Int64 Min() { return std::numeric_limits<C_Int64>::min(); }
		COREARRAY_INLINE static C_Int64 Max() { return std::numeric_limits<C_Int64>::max(); }
	};


	/// Trait information for C_UInt64
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<C_UInt64>
	{
		typedef C_UInt64 TType;
		typedef C_UInt64 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 64u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svUInt64;

		static const char *TraitName() { return "UInt64"; }
		static const char *StreamName() { return "dUInt64"; }

		COREARRAY_INLINE static C_UInt64 Min() { return 0; }
		COREARRAY_INLINE static C_UInt64 Max() { return std::numeric_limits<C_UInt64>::max(); }
	};



	// =======================================================================
  	// Traits of floating point number
	// =======================================================================

	/// Trait information for C_Float32
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<C_Float32>
	{
		typedef C_Float32 TType;
		typedef C_Float32 ElmType;
		static const int trVal = COREARRAY_TR_FLOAT;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svFloat32;

		static const char *TraitName() { return "Float32"; }
		static const char *StreamName() { return "dFloat32"; }

		COREARRAY_INLINE static C_Float32 Min() { return FLT_MIN; }
		COREARRAY_INLINE static C_Float32 Max() { return FLT_MAX; }
		COREARRAY_INLINE static C_Float32 Epsilon() { return FLT_EPSILON; }
		COREARRAY_INLINE static int Digits() { return FLT_MANT_DIG; }
	};


	/// Trait information for C_Float64
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<C_Float64>
	{
		typedef C_Float64 TType;
		typedef C_Float64 ElmType;
		static const int trVal = COREARRAY_TR_FLOAT;
		static const unsigned BitOf = 64u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svFloat64;

		static const char *TraitName() { return "Float64"; }
		static const char *StreamName() { return "dFloat64"; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }
		COREARRAY_INLINE static C_Float64 Epsilon() { return DBL_EPSILON; }
		COREARRAY_INLINE static int Digits() { return DBL_MANT_DIG; }
	};


	/// Trait information for C_LongFloat
	template<> struct TdTraits<C_LongFloat>
	{
		typedef C_LongFloat TType;
		typedef C_LongFloat ElmType;
		static const int trVal = COREARRAY_TR_FLOAT;
		static const unsigned BitOf = sizeof(C_LongFloat)*8u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dLongFloat"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_LongFloat Min() { return LDBL_MIN; }
		COREARRAY_INLINE static C_LongFloat Max() { return LDBL_MAX; }
		COREARRAY_INLINE static C_LongFloat Epsilon() { return LDBL_EPSILON; }
		COREARRAY_INLINE static int Digits() { return LDBL_MANT_DIG; }
	};



	// =======================================================================
  	// Traits of string
	// =======================================================================

	/// Trait information for UTF8String
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<UTF8String>
	{
		typedef UTF8String TType;
		typedef C_UTF8 ElmType;
		static const int trVal = COREARRAY_TR_STRING;
		static const unsigned BitOf = 8u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svStrUTF8;

		static const char *TraitName() { return "UTF8String"; }
	};

	/// Trait information for UTF16String
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<UTF16String>
	{
		typedef UTF16String TType;
		typedef C_UTF16 ElmType;
		static const int trVal = COREARRAY_TR_STRING;
		static const unsigned BitOf = 16u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svStrUTF16;

		static const char *TraitName() { return "UTF16String"; }
	};

	/// Trait information for UTF32String
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<UTF32String>
	{
		typedef UTF32String TType;
		typedef C_UTF32 ElmType;
		static const int trVal = COREARRAY_TR_STRING;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svCustomStr;

		static const char *TraitName() { return "UTF32String"; }
	};
}

#endif /* _HEADER_COREARRAY_TRAIT_ */
