// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dBit.h: Integers with specific number of bits and bitwise operation
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


#ifndef _H_COREARRAY_BIT_
#define _H_COREARRAY_BIT_

#include <CoreDEF.h>
#include <dType.h>


namespace CoreArray
{
	namespace _INTERNAL
	{
		/// The structure to define the extended integer types
		/** \tparam TYPE    integer type
		 *  \return         the extended integer type with more bytes,
		 *                  e.g., Int8 returns Int16
		 *  \sa  BIT_INTEGER
		**/
		template<typename TYPE> struct TypeExtend {};

		/// Integer extended: Int8 -> Int16
		template<> struct TypeExtend<Int8>   { typedef Int16  Type; };
		/// Integer extended: UInt8 -> UInt16
		template<> struct TypeExtend<UInt8>  { typedef UInt16 Type; };
		/// Integer extended: Int16 -> Int32
		template<> struct TypeExtend<Int16>  { typedef Int32  Type; };
		/// Integer extended: UInt16 -> UInt32
		template<> struct TypeExtend<UInt16> { typedef UInt32 Type; };
		/// Integer extended: Int32 -> Int64
		template<> struct TypeExtend<Int32>  { typedef Int64  Type; };
		/// Integer extended: UInt32 -> UInt64
		template<> struct TypeExtend<UInt32> { typedef UInt64 Type; };
	}


	/// Integer with specific number of bits
	/** \tparam bits       the number of bits
	 *  \tparam is_signed  whether is signed or unsigned integer
	 *  \tparam int_type   integer type storing the value
	 *  \tparam mask       bit mask according to bits
	**/
	template<unsigned bits, bool is_signed, typename int_type, Int64 mask>
		struct BIT_INTEGER
	{
	public:
		/// The integer type storing the value
		typedef int_type IntType;
		/// The extended integer type according to IntType
		typedef typename _INTERNAL::TypeExtend<int_type>::Type IntTypeEx;

		/// Whether it is a signed or unsigned integer
		static const bool      IS_SIGNED       = is_signed;
		/// The number of bits
		static const unsigned  BIT_NUM         = bits;
		/// The bit mask for integer type
		static const IntType   BIT_MASK        = mask;
		/// The complement bit mask for integer type
		static const IntType   BIT_NOT_MASK    = ~mask;
		/// The bit mask for extended integer type
		static const IntTypeEx BIT_MASK_EX     = mask;
		/// The complement bit mask for extended integer type
		static const IntTypeEx BIT_NOT_MASK_EX = ~mask;

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


	// *******************   unsigned integer   *******************

	/// 1-bit unsigned integer
	typedef BIT_INTEGER<1u,  false, UInt8, 0x01> Bit1;
	/// 2-bit unsigned integer
	typedef BIT_INTEGER<2u,  false, UInt8, 0x03> Bit2;
	/// 3-bit unsigned integer
	typedef BIT_INTEGER<3u,  false, UInt8, 0x07> Bit3;
	/// 4-bit unsigned integer
	typedef BIT_INTEGER<4u,  false, UInt8, 0x0F> Bit4;
	/// 5-bit unsigned integer
	typedef BIT_INTEGER<5u,  false, UInt8, 0x1F> Bit5;
	/// 6-bit unsigned integer
	typedef BIT_INTEGER<6u,  false, UInt8, 0x3F> Bit6;
	/// 7-bit unsigned integer
	typedef BIT_INTEGER<7u,  false, UInt8, 0x7F> Bit7;
	/// 8-bit unsigned integer
	typedef BIT_INTEGER<8u,  false, UInt8, 0xFF> Bit8;

	/// 9-bit unsigned integer
	typedef BIT_INTEGER<9u,  false, UInt16, 0x01FF> Bit9;
	/// 10-bit unsigned integer
	typedef BIT_INTEGER<10u, false, UInt16, 0x03FF> Bit10;
	/// 11-bit unsigned integer
	typedef BIT_INTEGER<11u, false, UInt16, 0x07FF> Bit11;
	/// 12-bit unsigned integer
	typedef BIT_INTEGER<12u, false, UInt16, 0x0FFF> Bit12;
	/// 13-bit unsigned integer
	typedef BIT_INTEGER<13u, false, UInt16, 0x1FFF> Bit13;
	/// 14-bit unsigned integer
	typedef BIT_INTEGER<14u, false, UInt16, 0x3FFF> Bit14;
	/// 15-bit unsigned integer
	typedef BIT_INTEGER<15u, false, UInt16, 0x7FFF> Bit15;
	/// 16-bit unsigned integer
	typedef BIT_INTEGER<16u, false, UInt16, 0xFFFF> Bit16;

	/// 17-bit unsigned integer
	typedef BIT_INTEGER<17u, false, UInt32, 0x01FFFF> Bit17;
	/// 18-bit unsigned integer
	typedef BIT_INTEGER<18u, false, UInt32, 0x03FFFF> Bit18;
	/// 19-bit unsigned integer
	typedef BIT_INTEGER<19u, false, UInt32, 0x07FFFF> Bit19;
	/// 20-bit unsigned integer
	typedef BIT_INTEGER<20u, false, UInt32, 0x0FFFFF> Bit20;
	/// 21-bit unsigned integer
	typedef BIT_INTEGER<21u, false, UInt32, 0x1FFFFF> Bit21;
	/// 22-bit unsigned integer
	typedef BIT_INTEGER<22u, false, UInt32, 0x3FFFFF> Bit22;
	/// 23-bit unsigned integer
	typedef BIT_INTEGER<23u, false, UInt32, 0x7FFFFF> Bit23;
	/// 24-bit unsigned integer
	typedef BIT_INTEGER<24u, false, UInt32, 0xFFFFFF> Bit24;
	/// 24-bit unsigned integer
	typedef Bit24                                     UInt24;

	/// 25-bit unsigned integer
	typedef BIT_INTEGER<25u, false, UInt32, 0x01FFFFFF> Bit25;
	/// 26-bit unsigned integer
	typedef BIT_INTEGER<26u, false, UInt32, 0x03FFFFFF> Bit26;
	/// 27-bit unsigned integer
	typedef BIT_INTEGER<27u, false, UInt32, 0x07FFFFFF> Bit27;
	/// 28-bit unsigned integer
	typedef BIT_INTEGER<28u, false, UInt32, 0x0FFFFFFF> Bit28;
	/// 29-bit unsigned integer
	typedef BIT_INTEGER<29u, false, UInt32, 0x1FFFFFFF> Bit29;
	/// 30-bit unsigned integer
	typedef BIT_INTEGER<30u, false, UInt32, 0x3FFFFFFF> Bit30;
	/// 31-bit unsigned integer
	typedef BIT_INTEGER<31u, false, UInt32, 0x7FFFFFFF> Bit31;
	/// 32-bit unsigned integer
	typedef BIT_INTEGER<32u, false, UInt32, 0xFFFFFFFF> Bit32;


	// *******************   signed integer   *******************

	/// 2-bit signed integer
	typedef BIT_INTEGER<2u,  true, Int8, 0x03> SBit2;
	/// 3-bit signed integer
	typedef BIT_INTEGER<3u,  true, Int8, 0x07> SBit3;
	/// 4-bit signed integer
	typedef BIT_INTEGER<4u,  true, Int8, 0x0F> SBit4;
	/// 5-bit signed integer
	typedef BIT_INTEGER<5u,  true, Int8, 0x1F> SBit5;
	/// 6-bit signed integer
	typedef BIT_INTEGER<6u,  true, Int8, 0x3F> SBit6;
	/// 7-bit signed integer
	typedef BIT_INTEGER<7u,  true, Int8, 0x7F> SBit7;
	/// 8-bit signed integer
	typedef BIT_INTEGER<8u,  true, Int8, 0xFF> SBit8;

	/// 9-bit signed integer
	typedef BIT_INTEGER<9u,  true, Int16, 0x01FF> SBit9;
	/// 10-bit signed integer
	typedef BIT_INTEGER<10u, true, Int16, 0x03FF> SBit10;
	/// 11-bit signed integer
	typedef BIT_INTEGER<11u, true, Int16, 0x07FF> SBit11;
	/// 12-bit signed integer
	typedef BIT_INTEGER<12u, true, Int16, 0x0FFF> SBit12;
	/// 13-bit signed integer
	typedef BIT_INTEGER<13u, true, Int16, 0x1FFF> SBit13;
	/// 14-bit signed integer
	typedef BIT_INTEGER<14u, true, Int16, 0x3FFF> SBit14;
	/// 15-bit signed integer
	typedef BIT_INTEGER<15u, true, Int16, 0x7FFF> SBit15;
	/// 16-bit signed integer
	typedef BIT_INTEGER<16u, true, Int16, 0xFFFF> SBit16;

	/// 17-bit signed integer
	typedef BIT_INTEGER<17u, true, Int32, 0x01FFFF> SBit17;
	/// 18-bit signed integer
	typedef BIT_INTEGER<18u, true, Int32, 0x03FFFF> SBit18;
	/// 19-bit signed integer
	typedef BIT_INTEGER<19u, true, Int32, 0x07FFFF> SBit19;
	/// 20-bit signed integer
	typedef BIT_INTEGER<20u, true, Int32, 0x0FFFFF> SBit20;
	/// 21-bit signed integer
	typedef BIT_INTEGER<21u, true, Int32, 0x1FFFFF> SBit21;
	/// 22-bit signed integer
	typedef BIT_INTEGER<22u, true, Int32, 0x3FFFFF> SBit22;
	/// 23-bit signed integer
	typedef BIT_INTEGER<23u, true, Int32, 0x7FFFFF> SBit23;
	/// 24-bit signed integer
	typedef BIT_INTEGER<24u, true, Int32, 0xFFFFFF> SBit24;
	/// 24-bit signed integer
	typedef SBit24                                  Int24;

	/// 25-bit signed integer
	typedef BIT_INTEGER<25u, true, Int32, 0x01FFFFFF> SBit25;
	/// 26-bit signed integer
	typedef BIT_INTEGER<26u, true, Int32, 0x03FFFFFF> SBit26;
	/// 27-bit signed integer
	typedef BIT_INTEGER<27u, true, Int32, 0x07FFFFFF> SBit27;
	/// 28-bit signed integer
	typedef BIT_INTEGER<28u, true, Int32, 0x0FFFFFFF> SBit28;
	/// 29-bit signed integer
	typedef BIT_INTEGER<29u, true, Int32, 0x1FFFFFFF> SBit29;
	/// 30-bit signed integer
	typedef BIT_INTEGER<30u, true, Int32, 0x3FFFFFFF> SBit30;
	/// 31-bit signed integer
	typedef BIT_INTEGER<31u, true, Int32, 0x7FFFFFFF> SBit31;
	/// 32-bit signed integer
	typedef BIT_INTEGER<32u, true, Int32, 0xFFFFFFFF> SBit32;





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

	template<unsigned bits, bool is_signed, typename int_type, Int64 mask>
		struct TdTraits< BIT_INTEGER<bits, is_signed, int_type, mask> >
	{
		typedef typename BIT_INTEGER<bits, is_signed, int_type, mask>::IntType TType;
		typedef TType ElmType;

		static const int trVal = COREARRAY_TR_BIT_INTEGER;
		static const unsigned BitOf = bits;
		static const bool isClass = false;
		static const TSVType SVType = svCustomInt;

		static const char * StreamName()
		{
			if (is_signed)
				return SBitStreamNames[bits-1];
			else
				return BitStreamNames[bits-1];
		}
		static const char * TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static Int64 Min()
		{
			return is_signed ? (Int64(-1) << (bits-1)) : 0;
		}

		COREARRAY_INLINE static Int64 Max()
		{
			return is_signed ?
				(BIT_INTEGER<bits, is_signed, int_type, mask>::BIT_MASK >> 1) :
				BIT_INTEGER<bits, is_signed, int_type, mask>::BIT_MASK;
		}
	};
}

#endif /* _H_COREARRAY_BIT_ */
