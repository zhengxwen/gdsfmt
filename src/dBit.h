// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dBit.h: Integers with specific numbers of bits and bitwise operation
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
 *	\file     dBit.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Integers with specific number of bits and bitwise operation
 *	\details
**/


#ifndef _HEADER_COREARRAY_BIT_
#define _HEADER_COREARRAY_BIT_

#include "dType.h"
#include "dTrait.h"


namespace CoreArray
{
	/// Integer with specific number of bits
	/** \tparam bits       the number of bits
	 *  \tparam is_signed  whether is signed or unsigned integer
	 *  \tparam int_type   integer type storing the value
	 *  \tparam mask       bit mask according to bits
	**/
	template<unsigned bits, bool is_signed, typename int_type, C_Int64 mask>
		struct COREARRAY_DLL_DEFAULT BIT_INTEGER
	{
	public:
		/// The integer type storing the value
		typedef int_type IntType;

		/// Whether it is a signed or unsigned integer
		static const bool      IS_SIGNED       = is_signed;
		/// The number of bits
		static const unsigned  BIT_NUM         = bits;
		/// The bit mask for integer type
		static const IntType   BIT_MASK        = mask;
		/// The complement bit mask for integer type
		static const IntType   BIT_NOT_MASK    = ~mask;

		/// Construct a BIT_INTEGER object
		BIT_INTEGER() {}
		/// Construct a BIT_INTEGER object with initial value
		BIT_INTEGER(IntType val) { fVal = val & BIT_MASK; }

		/// Basic assignment
		BIT_INTEGER<bits, is_signed, int_type, mask> & operator=(IntType val)
			{ fVal = val & BIT_MASK; return (*this); }
		/// Addition assignment
		BIT_INTEGER<bits, is_signed, int_type, mask> & operator+= (IntType val)
			{ fVal = (fVal + val) & BIT_MASK; return (*this); }
		/// Subtraction assignment
		BIT_INTEGER<bits, is_signed, int_type, mask> & operator-= (IntType val)
			{ fVal = (fVal - val) & BIT_MASK; return (*this); }
		/// Multiplication assignment
		BIT_INTEGER<bits, is_signed, int_type, mask> & operator*= (IntType val)
			{ fVal = (Value()*val) & BIT_MASK; return (*this); }
		/// Division assignment
		BIT_INTEGER<bits, is_signed, int_type, mask> & operator/= (IntType val)
			{ fVal = (Value() / val) & BIT_MASK; return (*this); }
		/// Modulo assignment
		BIT_INTEGER<bits, is_signed, int_type, mask> & operator%= (IntType val)
			{ fVal = (Value() % val) & BIT_MASK; return (*this); }
		/// Bitwise AND assignment
		BIT_INTEGER<bits, is_signed, int_type, mask> & operator&= (IntType val)
			{ fVal = (fVal & val) & BIT_MASK; return (*this); }
		/// Bitwise OR assignment
		BIT_INTEGER<bits, is_signed, int_type, mask> & operator|= (IntType val)
			{ fVal = (fVal | val) & BIT_MASK; return (*this); }
		/// Bitwise XOR assignment
		BIT_INTEGER<bits, is_signed, int_type, mask> & operator^= (IntType val)
			{ fVal = (fVal ^ val) & BIT_MASK; return (*this); }
		/// Bitwise left shift assignment
		BIT_INTEGER<bits, is_signed, int_type, mask> & operator<<= (IntType val)
			{ fVal = (fVal << val) & BIT_MASK; return (*this); }
		/// Bitwise right shift assignment[d]
		BIT_INTEGER<bits, is_signed, int_type, mask> & operator>>= (IntType val)
			{ fVal = (fVal >> val); return (*this); }

		/// Return the integer value according to IntType
		IntType Value() const
		{
			if (is_signed)
			{
				static const IntType FLAG = (BIT_NOT_MASK >> 1) & BIT_MASK;
				return ((fVal & FLAG) ? (fVal | BIT_NOT_MASK) : fVal);
			} else
				return fVal;
		}

		operator IntType() const
			{ return Value(); }

	protected:
		IntType fVal;
	};


	// ===================   unsigned integer   ===================

	/// bit unsigned integer
	typedef BIT_INTEGER<0u,  false, C_UInt32, 0x0> BIT0;
	/// 1-bit unsigned integer
	typedef BIT_INTEGER<1u,  false, C_UInt8, 0x01> BIT1;
	/// 2-bit unsigned integer
	typedef BIT_INTEGER<2u,  false, C_UInt8, 0x03> BIT2;
	/// 3-bit unsigned integer
	typedef BIT_INTEGER<3u,  false, C_UInt8, 0x07> BIT3;
	/// 4-bit unsigned integer
	typedef BIT_INTEGER<4u,  false, C_UInt8, 0x0F> BIT4;
	/// 5-bit unsigned integer
	typedef BIT_INTEGER<5u,  false, C_UInt8, 0x1F> BIT5;
	/// 6-bit unsigned integer
	typedef BIT_INTEGER<6u,  false, C_UInt8, 0x3F> BIT6;
	/// 7-bit unsigned integer
	typedef BIT_INTEGER<7u,  false, C_UInt8, 0x7F> BIT7;
	/// 8-bit unsigned integer
	typedef BIT_INTEGER<8u,  false, C_UInt8, 0xFF> BIT8;

	/// 9-bit unsigned integer
	typedef BIT_INTEGER<9u,  false, C_UInt16, 0x01FF> BIT9;
	/// 10-bit unsigned integer
	typedef BIT_INTEGER<10u, false, C_UInt16, 0x03FF> BIT10;
	/// 11-bit unsigned integer
	typedef BIT_INTEGER<11u, false, C_UInt16, 0x07FF> BIT11;
	/// 12-bit unsigned integer
	typedef BIT_INTEGER<12u, false, C_UInt16, 0x0FFF> BIT12;
	/// 13-bit unsigned integer
	typedef BIT_INTEGER<13u, false, C_UInt16, 0x1FFF> BIT13;
	/// 14-bit unsigned integer
	typedef BIT_INTEGER<14u, false, C_UInt16, 0x3FFF> BIT14;
	/// 15-bit unsigned integer
	typedef BIT_INTEGER<15u, false, C_UInt16, 0x7FFF> BIT15;
	/// 16-bit unsigned integer
	typedef BIT_INTEGER<16u, false, C_UInt16, 0xFFFF> BIT16;

	/// 17-bit unsigned integer
	typedef BIT_INTEGER<17u, false, C_UInt32, 0x01FFFF> BIT17;
	/// 18-bit unsigned integer
	typedef BIT_INTEGER<18u, false, C_UInt32, 0x03FFFF> BIT18;
	/// 19-bit unsigned integer
	typedef BIT_INTEGER<19u, false, C_UInt32, 0x07FFFF> BIT19;
	/// 20-bit unsigned integer
	typedef BIT_INTEGER<20u, false, C_UInt32, 0x0FFFFF> BIT20;
	/// 21-bit unsigned integer
	typedef BIT_INTEGER<21u, false, C_UInt32, 0x1FFFFF> BIT21;
	/// 22-bit unsigned integer
	typedef BIT_INTEGER<22u, false, C_UInt32, 0x3FFFFF> BIT22;
	/// 23-bit unsigned integer
	typedef BIT_INTEGER<23u, false, C_UInt32, 0x7FFFFF> BIT23;
	/// 24-bit unsigned integer
	typedef BIT_INTEGER<24u, false, C_UInt32, 0xFFFFFF> BIT24;
	/// 24-bit unsigned integer
	typedef BIT24                                       UInt24;

	/// 25-bit unsigned integer
	typedef BIT_INTEGER<25u, false, C_UInt32, 0x01FFFFFF> BIT25;
	/// 26-bit unsigned integer
	typedef BIT_INTEGER<26u, false, C_UInt32, 0x03FFFFFF> BIT26;
	/// 27-bit unsigned integer
	typedef BIT_INTEGER<27u, false, C_UInt32, 0x07FFFFFF> BIT27;
	/// 28-bit unsigned integer
	typedef BIT_INTEGER<28u, false, C_UInt32, 0x0FFFFFFF> BIT28;
	/// 29-bit unsigned integer
	typedef BIT_INTEGER<29u, false, C_UInt32, 0x1FFFFFFF> BIT29;
	/// 30-bit unsigned integer
	typedef BIT_INTEGER<30u, false, C_UInt32, 0x3FFFFFFF> BIT30;
	/// 31-bit unsigned integer
	typedef BIT_INTEGER<31u, false, C_UInt32, 0x7FFFFFFF> BIT31;
	/// 32-bit unsigned integer
	typedef BIT_INTEGER<32u, false, C_UInt32, 0xFFFFFFFF> BIT32;

	/// 64-bit unsigned integer
	typedef C_UInt64                                      BIT64;


	// ===================   signed integer   ===================

	/// bit signed integer
	typedef BIT_INTEGER<0u,  true, C_Int32, 0x0> SBIT0;
	/// 2-bit signed integer
	typedef BIT_INTEGER<2u,  true, C_Int8, 0x03> SBIT2;
	/// 3-bit signed integer
	typedef BIT_INTEGER<3u,  true, C_Int8, 0x07> SBIT3;
	/// 4-bit signed integer
	typedef BIT_INTEGER<4u,  true, C_Int8, 0x0F> SBIT4;
	/// 5-bit signed integer
	typedef BIT_INTEGER<5u,  true, C_Int8, 0x1F> SBIT5;
	/// 6-bit signed integer
	typedef BIT_INTEGER<6u,  true, C_Int8, 0x3F> SBIT6;
	/// 7-bit signed integer
	typedef BIT_INTEGER<7u,  true, C_Int8, 0x7F> SBIT7;
	/// 8-bit signed integer
	typedef BIT_INTEGER<8u,  true, C_Int8, 0xFF> SBIT8;

	/// 9-bit signed integer
	typedef BIT_INTEGER<9u,  true, C_Int16, 0x01FF> SBIT9;
	/// 10-bit signed integer
	typedef BIT_INTEGER<10u, true, C_Int16, 0x03FF> SBIT10;
	/// 11-bit signed integer
	typedef BIT_INTEGER<11u, true, C_Int16, 0x07FF> SBIT11;
	/// 12-bit signed integer
	typedef BIT_INTEGER<12u, true, C_Int16, 0x0FFF> SBIT12;
	/// 13-bit signed integer
	typedef BIT_INTEGER<13u, true, C_Int16, 0x1FFF> SBIT13;
	/// 14-bit signed integer
	typedef BIT_INTEGER<14u, true, C_Int16, 0x3FFF> SBIT14;
	/// 15-bit signed integer
	typedef BIT_INTEGER<15u, true, C_Int16, 0x7FFF> SBIT15;
	/// 16-bit signed integer
	typedef BIT_INTEGER<16u, true, C_Int16, 0xFFFF> SBIT16;

	/// 17-bit signed integer
	typedef BIT_INTEGER<17u, true, C_Int32, 0x01FFFF> SBIT17;
	/// 18-bit signed integer
	typedef BIT_INTEGER<18u, true, C_Int32, 0x03FFFF> SBIT18;
	/// 19-bit signed integer
	typedef BIT_INTEGER<19u, true, C_Int32, 0x07FFFF> SBIT19;
	/// 20-bit signed integer
	typedef BIT_INTEGER<20u, true, C_Int32, 0x0FFFFF> SBIT20;
	/// 21-bit signed integer
	typedef BIT_INTEGER<21u, true, C_Int32, 0x1FFFFF> SBIT21;
	/// 22-bit signed integer
	typedef BIT_INTEGER<22u, true, C_Int32, 0x3FFFFF> SBIT22;
	/// 23-bit signed integer
	typedef BIT_INTEGER<23u, true, C_Int32, 0x7FFFFF> SBIT23;
	/// 24-bit signed integer
	typedef BIT_INTEGER<24u, true, C_Int32, 0xFFFFFF> SBIT24;
	/// 24-bit signed integer
	typedef SBIT24                                    Int24;

	/// 25-bit signed integer
	typedef BIT_INTEGER<25u, true, C_Int32, 0x01FFFFFF> SBIT25;
	/// 26-bit signed integer
	typedef BIT_INTEGER<26u, true, C_Int32, 0x03FFFFFF> SBIT26;
	/// 27-bit signed integer
	typedef BIT_INTEGER<27u, true, C_Int32, 0x07FFFFFF> SBIT27;
	/// 28-bit signed integer
	typedef BIT_INTEGER<28u, true, C_Int32, 0x0FFFFFFF> SBIT28;
	/// 29-bit signed integer
	typedef BIT_INTEGER<29u, true, C_Int32, 0x1FFFFFFF> SBIT29;
	/// 30-bit signed integer
	typedef BIT_INTEGER<30u, true, C_Int32, 0x3FFFFFFF> SBIT30;
	/// 31-bit signed integer
	typedef BIT_INTEGER<31u, true, C_Int32, 0x7FFFFFFF> SBIT31;
	/// 32-bit signed integer
	typedef BIT_INTEGER<32u, true, C_Int32, 0xFFFFFFFF> SBIT32;

	/// 64-bit signed integer
	typedef C_Int64                                     SBIT64;



	template<typename BIT_TYPE>
		COREARRAY_INLINE typename BIT_TYPE::IntType
		BITS_ifsign(typename BIT_TYPE::IntType val)
	{
		static const typename BIT_TYPE::IntType Flag =
			(BIT_TYPE::BIT_NOT_MASK >> 1) & BIT_TYPE::BIT_MASK;
		return (val & Flag) ? (val | BIT_TYPE::BIT_NOT_MASK) : val;
	}


	// Bit Type Traits

	// "dBit1" ... "dBit32"
	extern const char *BitStreamNames[];

	// "dSBit1" ... "dSBit32"
	extern const char *SBitStreamNames[];

	template<unsigned bits, bool is_signed, typename int_type, C_Int64 mask>
		struct COREARRAY_DLL_DEFAULT TdTraits
		< BIT_INTEGER<bits, is_signed, int_type, mask> >
	{
		typedef typename
			BIT_INTEGER<bits, is_signed, int_type, mask>::IntType TType;
		typedef TType ElmType;

		static const int trVal = COREARRAY_TR_BIT_INTEGER;
		static const unsigned BitOf = bits;
		static const bool isClass = false;
		static const C_SVType SVType = svCustomInt;

		static const char * StreamName()
		{
			if (is_signed)
				return SBitStreamNames[bits-1];
			else
				return BitStreamNames[bits-1];
		}
		static const char * TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Int64 Min()
		{
			return is_signed ? (C_Int64(-1) << (bits-1)) : 0;
		}

		COREARRAY_INLINE static C_Int64 Max()
		{
			return is_signed ?
				(BIT_INTEGER<bits, is_signed, int_type, mask>::BIT_MASK >> 1) :
				BIT_INTEGER<bits, is_signed, int_type, mask>::BIT_MASK;
		}
	};
}

#endif /* _HEADER_COREARRAY_BIT_ */
