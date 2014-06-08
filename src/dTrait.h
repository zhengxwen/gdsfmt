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
 *	\file     dTrait.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Trait classes for elementary types
 *	\details
**/


#ifndef _HEADER_COREARRAY_TRAIT_
#define _HEADER_COREARRAY_TRAIT_

#include <CoreDEF.h>
#include <dType.h>

#include <cfloat>
#include <limits>
#include <string>


namespace CoreArray
{
	// ******************************************************************** //
	// ******************************************************************** //

	// define little-endian atomic type

	/// little-endian int8_t
	#define COREARRAY_LE_INT8_TRAIT_ID            0
	/// little-endian uint8_t
	#define COREARRAY_LE_UINT8_TRAIT_ID           1
	/// little-endian int16_t
	#define COREARRAY_LE_INT16_TRAIT_ID           2
	/// little-endian uint16_t
	#define COREARRAY_LE_UINT16_TRAIT_ID          3
	/// little-endian int32_t
	#define COREARRAY_LE_INT32_TRAIT_ID           4
	/// little-endian uint32_t
	#define COREARRAY_LE_UINT32_TRAIT_ID          5
	/// little-endian int64_t
	#define COREARRAY_LE_INT64_TRAIT_ID           6
	/// little-endian uint64_t
	#define COREARRAY_LE_UINT64_TRAIT_ID          7

	/// IEEE 32-bit floating point
	#define COREARRAY_IEEE_LE_FLOAT32_TRAIT_ID    10
	/// IEEE 64-bit floating point
	#define COREARRAY_IEEE_LE_FLOAT64_TRAIT_ID    11


	// define big-endian atomic type

	/// big-endian int8_t
	#define COREARRAY_BE_INT8_TRAIT_ID            20
	/// big-endian uint8_t
	#define COREARRAY_BE_UINT8_TRAIT_ID           21
	/// big-endian int16_t
	#define COREARRAY_BE_INT16_TRAIT_ID           22
	/// big-endian uint16_t
	#define COREARRAY_BE_UINT16_TRAIT_ID          23
	/// big-endian int32_t
	#define COREARRAY_BE_INT32_TRAIT_ID           24
	/// big-endian uint32_t
	#define COREARRAY_BE_UINT32_TRAIT_ID          25
	/// big-endian int64_t
	#define COREARRAY_BE_INT64_TRAIT_ID           26
	/// big-endian uint64_t
	#define COREARRAY_BE_UINT64_TRAIT_ID          27

	/// IEEE 32-bit floating point
	#define COREARRAY_IEEE_BE_FLOAT32_TRAIT_ID    30
	/// IEEE 64-bit floating point
	#define COREARRAY_IEEE_BE_FLOAT64_TRAIT_ID    31



	// define native atomic type

	#if defined(COREARRAY_LITTLE_ENDIAN)

		/// native int8_t
		#define COREARRAY_NATIVE_INT8_TRAIT_ID      COREARRAY_LE_INT8_TRAIT_ID
		/// native uint8_t
		#define COREARRAY_NATIVE_UINT8_TRAIT_ID     COREARRAY_LE_UINT8_TRAIT_ID
		/// native int16_t
		#define COREARRAY_NATIVE_INT16_TRAIT_ID     COREARRAY_LE_INT16_TRAIT_ID
		/// native uint16_t
		#define COREARRAY_NATIVE_UINT16_TRAIT_ID    COREARRAY_LE_UINT16_TRAIT_ID
		/// native int32_t
		#define COREARRAY_NATIVE_INT32_TRAIT_ID     COREARRAY_LE_INT32_TRAIT_ID
		/// native uint32_t
		#define COREARRAY_NATIVE_UINT32_TRAIT_ID    COREARRAY_LE_UINT32_TRAIT_ID
		/// native int64_t
		#define COREARRAY_NATIVE_INT64_TRAIT_ID     COREARRAY_LE_INT64_TRAIT_ID
		/// native uint64_t
		#define COREARRAY_NATIVE_UINT64_TRAIT_ID    COREARRAY_LE_UINT64_TRAIT_ID

		/// native 32-bit floating point
		#define COREARRAY_IEEE_NATIVE_FLOAT32_TRAIT_ID    COREARRAY_IEEE_LE_FLOAT32_TRAIT_ID
		/// native 64-bit floating point
		#define COREARRAY_IEEE_NATIVE_FLOAT64_TRAIT_ID    COREARRAY_IEEE_LE_FLOAT64_TRAIT_ID

	#elif defined(COREARRAY_BIG_ENDIAN)

		/// native int8_t
		#define COREARRAY_NATIVE_INT8_TRAIT_ID      COREARRAY_BE_INT8_TRAIT_ID
		/// native uint8_t
		#define COREARRAY_NATIVE_UINT8_TRAIT_ID     COREARRAY_BE_UINT8_TRAIT_ID
		/// native int16_t
		#define COREARRAY_NATIVE_INT16_TRAIT_ID     COREARRAY_BE_INT16_TRAIT_ID
		/// native uint16_t
		#define COREARRAY_NATIVE_UINT16_TRAIT_ID    COREARRAY_BE_UINT16_TRAIT_ID
		/// native int32_t
		#define COREARRAY_NATIVE_INT32_TRAIT_ID     COREARRAY_BE_INT32_TRAIT_ID
		/// native uint32_t
		#define COREARRAY_NATIVE_UINT32_TRAIT_ID    COREARRAY_BE_UINT32_TRAIT_ID
		/// native int64_t
		#define COREARRAY_NATIVE_INT64_TRAIT_ID     COREARRAY_BE_INT64_TRAIT_ID
		/// native uint64_t
		#define COREARRAY_NATIVE_UINT64_TRAIT_ID    COREARRAY_BE_UINT64_TRAIT_ID

		/// native 32-bit floating point
		#define COREARRAY_IEEE_NATIVE_FLOAT32_TRAIT_ID    COREARRAY_IEEE_BE_FLOAT32_TRAIT_ID
		/// native 64-bit floating point
		#define COREARRAY_IEEE_NATIVE_FLOAT64_TRAIT_ID    COREARRAY_IEEE_BE_FLOAT64_TRAIT_ID

	#else
	#  error "Unknown endianness"
    #endif



	#define COREARRAY_TR_UNKNOWN                  -1
	#define COREARRAY_TR_CUSTOM                    0

	#define COREARRAY_TR_INTEGER                   1
	#define COREARRAY_TR_BIT_INTEGER               2

	#define COREARRAY_TR_FLOAT                     3

	#define COREARRAY_TR_STRING                    4
	#define COREARRAY_TR_FIXED_LENGTH_STRING       5
	#define COREARRAY_TR_VARIABLE_LENGTH_STRING    6



	// Integer Traits

	template<typename T> struct TdTraits
	{
    	typedef T TType;
		typedef T ElmType;
		static const int trVal = COREARRAY_TR_UNKNOWN;
		static const unsigned BitOf = sizeof(T)*8u;
		static const bool isClass = false;
		static const C_SVType SVType = svCustom;
	};

	template<> struct TdTraits<C_Int8>
	{
		typedef C_Int8 TType;
		typedef C_Int8 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 8u;
		static const bool isClass = false;
		static const C_SVType SVType = svInt8;

		static const char * TraitName() { return "Int8"; }
		static const char * StreamName() { return "dInt8"; }

		COREARRAY_INLINE static short Min() { return INT8_MIN; }
		COREARRAY_INLINE static short Max() { return INT8_MAX; }
	};

	template<> struct TdTraits<C_UInt8>
	{
		typedef C_UInt8 TType;
		typedef C_UInt8 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 8u;
		static const bool isClass = false;
		static const C_SVType SVType = svUInt8;
		enum {
			isNumeric = true
		};
		static const char * TraitName() { return "UInt8"; }
		static const char * StreamName() { return "dUInt8"; }

		COREARRAY_INLINE static unsigned short Min() { return 0; }
		COREARRAY_INLINE static unsigned short Max() { return UINT8_MAX; }
	};

	template<> struct TdTraits<C_Int16>
	{
		typedef C_Int16 TType;
		typedef C_Int16 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 16u;
		static const bool isClass = false;
		static const C_SVType SVType = svInt16;

		static const char * TraitName() { return "Int16"; }
		static const char * StreamName() { return "dInt16"; }

		COREARRAY_INLINE static C_Int16 Min() { return INT16_MIN; }
		COREARRAY_INLINE static C_Int16 Max() { return INT16_MAX; }
	};

	template<> struct TdTraits<C_UInt16>
	{
		typedef C_UInt16 TType;
		typedef C_UInt16 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 16u;
		static const bool isClass = false;
		static const C_SVType SVType = svUInt16;

		static const char * TraitName() { return "UInt16"; }
		static const char * StreamName() { return "dUInt16"; }

		COREARRAY_INLINE static C_UInt16 Min() { return 0; }
		COREARRAY_INLINE static C_UInt16 Max() { return UINT16_MAX; }
	};

	template<> struct TdTraits<C_Int32>
	{
		typedef C_Int32 TType;
		typedef C_Int32 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 32u;
		static const bool isClass = false;
		static const C_SVType SVType = svInt32;

		static const char * TraitName() { return "Int32"; }
		static const char * StreamName() { return "dInt32"; }

		COREARRAY_INLINE static C_Int32 Min() { return INT32_MIN; }
		COREARRAY_INLINE static C_Int32 Max() { return INT32_MAX; }
	};

	template<> struct TdTraits<C_UInt32>
	{
		typedef C_UInt32 TType;
		typedef C_UInt32 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 32u;
		static const bool isClass = false;
		static const C_SVType SVType = svUInt32;

		static const char * TraitName() { return "UInt32"; }
		static const char * StreamName() { return "dUInt32"; }

		COREARRAY_INLINE static C_UInt32 Min() { return 0; }
		COREARRAY_INLINE static C_UInt32 Max() { return UINT32_MAX; }
	};

	template<> struct TdTraits<C_Int64>
	{
		typedef C_Int64 TType;
		typedef C_Int64 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 64u;
		static const bool isClass = false;
		static const C_SVType SVType = svInt64;

		static const char * TraitName() { return "Int64"; }
		static const char * StreamName() { return "dInt64"; }

		COREARRAY_INLINE static C_Int64 Min() { return std::numeric_limits<C_Int64>::min(); }
		COREARRAY_INLINE static C_Int64 Max() { return std::numeric_limits<C_Int64>::max(); }
	};

	template<> struct TdTraits<C_UInt64>
	{
		typedef C_UInt64 TType;
		typedef C_UInt64 ElmType;
		static const int trVal = COREARRAY_TR_INTEGER;
		static const unsigned BitOf = 64u;
		static const bool isClass = false;
		static const C_SVType SVType = svUInt64;

		static const char * TraitName() { return "UInt64"; }
		static const char * StreamName() { return "dUInt64"; }

		COREARRAY_INLINE static C_UInt64 Min() { return 0; }
		COREARRAY_INLINE static C_UInt64 Max() { return std::numeric_limits<C_UInt64>::max(); }
	};



  	// Float Traits

	template<> struct TdTraits<C_Float32>
	{
		typedef C_Float32 TType;
		typedef C_Float32 ElmType;
		static const int trVal = COREARRAY_TR_FLOAT;
		static const unsigned BitOf = 32u;
		static const bool isClass = false;
		static const C_SVType SVType = svFloat32;

		static const char * TraitName() { return "Float32"; }
		static const char * StreamName() { return "dFloat32"; }

		COREARRAY_INLINE static C_Float32 Min() { return FLT_MIN; }
		COREARRAY_INLINE static C_Float32 Max() { return FLT_MAX; }
		COREARRAY_INLINE static C_Float32 Epsilon() { return FLT_EPSILON; }
		COREARRAY_INLINE static int Digits() { return FLT_MANT_DIG; }
	};

	template<> struct TdTraits<C_Float64>
	{
		typedef C_Float64 TType;
		typedef C_Float64 ElmType;
		static const int trVal = COREARRAY_TR_FLOAT;
		static const unsigned BitOf = 64u;
		static const bool isClass = false;
		static const C_SVType SVType = svFloat64;

		static const char * TraitName() { return "Float64"; }
		static const char * StreamName() { return "dFloat64"; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }
		COREARRAY_INLINE static C_Float64 Epsilon() { return DBL_EPSILON; }
		COREARRAY_INLINE static int Digits() { return DBL_MANT_DIG; }
	};

	template<> struct TdTraits<C_LongFloat>
	{
		typedef C_LongFloat TType;
		typedef C_LongFloat ElmType;
		static const int trVal = COREARRAY_TR_FLOAT;
		static const unsigned BitOf = sizeof(C_LongFloat)*8u;
		static const bool isClass = false;
		static const C_SVType SVType = svCustomFloat;

		static const char * StreamName()
		{
		#if defined(COREARRAY_HAVE_FLOAT128)
			return "dFloat128";
		#elif defined(COREARRAY_LONGFLOAT_IS_DOUBLE)
			return "dFloat64";
		#else
        	return "dFloat80";
		#endif
		}
		static const char * TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_LongFloat Min() { return LDBL_MIN; }
		COREARRAY_INLINE static C_LongFloat Max() { return LDBL_MAX; }
		COREARRAY_INLINE static C_LongFloat Epsilon() { return LDBL_EPSILON; }
		COREARRAY_INLINE static int Digits() { return LDBL_MANT_DIG; }
	};



	/// Customized integer type
	/** \tparam TYPE  any data type, e.g integer or float number
	 *  \tparam SIZE  to specify the structure size, can be != sizeof(TYPE)
	**/
	template<typename TYPE, ssize_t SIZE> struct TdNumber
	{
	public:
		/// The size of this type
		static const ssize_t size = SIZE;

		TdNumber() {}
		TdNumber(TYPE val) { fVal = val; }

		TdNumber<TYPE, SIZE> & operator+= (TYPE val)
			{ fVal += val; return *this; }
		TdNumber<TYPE, SIZE> & operator-= (TYPE val)
			{ fVal -= val; return *this; }
		TdNumber<TYPE, SIZE> & operator++ ()
			{ fVal++; return *this; }
		TdNumber<TYPE, SIZE> & operator-- ()
			{ fVal--; return *this; }
		/// Assignment
		TdNumber<TYPE, SIZE> & operator= (TYPE val)
			{ fVal = val; return *this; }

		bool operator== (const TdNumber<TYPE, SIZE> &val) const
			{ return fVal == val.fVal; }
		bool operator!= (const TdNumber<TYPE, SIZE> &val) const
			{ return fVal != val.fVal; }

		operator TYPE() const { return fVal; }
		TYPE &get() { return fVal; }
		const TYPE &get() const { return fVal; }

		/// Return minimum value of the type
		static const TYPE min() { return TdTraits<TYPE>::Min(); }
		/// Return maximum value of the type
		static const TYPE max() { return TdTraits<TYPE>::Max(); }

	private:
		TYPE fVal;
    };
}

#endif /* _HEADER_COREARRAY_TRAIT_ */
