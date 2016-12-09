// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dBitGDS.h: Bit operators and classes of GDS format
//
// Copyright (C) 2007-2016    Xiuwen Zheng
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
 *	\file     dBitGDS.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2016
 *	\brief    Bit operators and classes of GDS format
 *	\details
**/

#ifndef _HEADER_COREARRAY_BIT_GDS_
#define _HEADER_COREARRAY_BIT_GDS_

#include "dBit.h"
#include "dStruct.h"
#include <typeinfo>

#ifdef COREARRAY_SIMD_SSE
#include <xmmintrin.h>
#endif
#ifdef COREARRAY_SIMD_SSE2
#include <emmintrin.h>
#endif
#ifdef COREARRAY_SIMD_AVX
#include <immintrin.h>
#endif


namespace CoreArray
{
	using namespace std;

	// =====================================================================
	// Bit operators
	// =====================================================================

	COREARRAY_DLL_DEFAULT void BitBinShr(void *Buf, size_t NByte, C_UInt8 NShr);

	/// Left shift of bits in an allocator
	COREARRAY_DLL_DEFAULT void BitBinShl(void *Buf, size_t NByte, C_UInt8 NShl);

	/// Clear array of bits from p, with a length of Len
	COREARRAY_DLL_DEFAULT void BitClear(CdAllocator &alloc,
		SIZE64 p, SIZE64 Len);

	/// Copy array of bits from buffer to the position pD in an allocator
	COREARRAY_DLL_DEFAULT void BitBufToCpy(CdAllocator &alloc,
		SIZE64 pD, void *Buf, size_t L);

	/// Right shift of bits in an allocator
	/// Move array of bits from pS to pD in an allocator
	COREARRAY_DLL_DEFAULT void BitMoveBits(CdAllocator &alloc,
		SIZE64 pS, SIZE64 pD, SIZE64 Len);


	/// bit array { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 }
	extern const C_UInt8 CoreArray_MaskBit1Array[];
	/// bit array { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F }
	extern const C_UInt8 CoreArray_MaskBit1ArrayNot[];

	/// bit array { 0x03, 0x0C, 0x30, 0xC0 }
	extern const C_UInt8 CoreArray_MaskBit2Array[];
	/// bit array { 0xFC, 0xF3, 0xCF, 0x3F }
	extern const C_UInt8 CoreArray_MaskBit2ArrayNot[];

	/// bit array { 0x0F, 0xF0 }
	extern const C_UInt8 CoreArray_MaskBit4Array[];
	/// bit array { 0xF0, 0x0F }
	extern const C_UInt8 CoreArray_MaskBit4ArrayNot[];


	C_Int32 BitSet_IfSigned(C_Int32 val, unsigned nbit);


	// =====================================================================
	// Bit classes of GDS format
	// =====================================================================

	/// Bit-array container
	/** \tparam BIT_TYPE    should be BIT0, BIT1, ...
	 *  \sa  CdBit1, CdBit2, etc
	**/
	template<typename BIT_TYPE>
		class COREARRAY_DLL_DEFAULT CdBaseBit: public CdArray<BIT_TYPE>
	{
	public:
		typedef BIT_TYPE ElmType;
		typedef typename TdTraits<ElmType>::TType ElmTypeEx;

		CdBaseBit(): CdArray<BIT_TYPE>(1) { }

		/// return the number of bits for the element type
		virtual unsigned BitOf()
		{
			return BIT_TYPE::BIT_NUM;
		}

		virtual CdGDSObj *NewObject()
		{
			return (new CdBaseBit<BIT_TYPE>)->AssignPipe(*this);
		}

		virtual void SetDLen(int I, C_Int32 Value)
		{
			this->_CheckSetDLen(I, Value);

			CdIterator it;
			C_Int64 MDimOld, MDimNew, LStep, DCnt, DResid;
			SIZE64 pS, pD;
			CdAllocArray::TDimItem &pDim = this->fDimension[I];

			if (pDim.DimLen != Value)
			{
				C_Int64 S = pDim.DimElmCnt * pDim.DimLen;
				if (this->fTotalCount > S)
				{
					it.Handler = this;
					it.Ptr = S;
					this->IterDone(it, this->fTotalCount - S);
				}

				if (pDim.DimElmSize > 0)
				{
					const unsigned N_BIT = this->BitOf();
					DCnt = 1;
					for (int i=I-1; i >= 0; i--)
						DCnt *= this->fDimension[i].DimLen;
					if (DCnt > 0)
					{
						MDimOld = pDim.DimLen * pDim.DimElmSize;
						MDimNew = Value * pDim.DimElmSize;
						if (pDim.DimLen < Value)
						{
							this->fAllocator.SetSize(this->AllocSize(DCnt * MDimNew));
							DResid = (Value - pDim.DimLen) * pDim.DimElmCnt;
							pS = (DCnt-1)*MDimOld; pD = (DCnt-1)*MDimNew;
							it.Handler = this;
							while (DCnt > 0)
							{
								BitMoveBits(this->fAllocator, pS*N_BIT, pD*N_BIT, MDimOld*N_BIT);
								it.Ptr = pD + MDimOld;
								this->IterInit(it, DResid);
								pS -= MDimOld; pD -= MDimNew;
								DCnt --;
							}
						} else {
							LStep = MDimOld - MDimNew;
							DResid = (pDim.DimLen - Value) * pDim.DimElmCnt;
							it.Handler = this; it.Ptr = 0;
							pD = pS = 0;
							while (DCnt > 0)
							{
								it.Ptr += MDimNew;
								this->IterDone(it, DResid);
								it.Ptr += LStep;
								BitMoveBits(this->fAllocator, pS*N_BIT, pD*N_BIT, MDimNew*N_BIT);
								pS += MDimOld; pD += MDimNew;
								DCnt --;
							}
						}
					}
				}
				pDim.DimLen = Value;
				this->_SetDimAuto(I);

				// Notify32(mcDimLength, DimIndex);
				// Notify(mcDimChanged);

				this->fChanged = true;
				if (this->fGDSStream) this->SaveToBlockStream();
			}
		}

		virtual const void *Append(const void *Buffer, ssize_t Cnt, C_SVType InSV)
		{
			if (Cnt <= 0) return Buffer;

			// writing
			this->_SetLargeBuffer();
			CdIterator I = this->IterEnd();
			switch (InSV)
			{
				case svInt8:
					Buffer = ALLOC_FUNC<BIT_TYPE, C_Int8>::Append(I, (const C_Int8*)Buffer, Cnt);
					break;
				case svUInt8:
					Buffer = ALLOC_FUNC<BIT_TYPE, C_UInt8>::Append(I, (const C_UInt8*)Buffer, Cnt);
					break;
				case svInt16:
					Buffer = ALLOC_FUNC<BIT_TYPE, C_Int16>::Append(I, (const C_Int16*)Buffer, Cnt);
					break;
				case svUInt16:
					Buffer = ALLOC_FUNC<BIT_TYPE, C_UInt16>::Append(I, (const C_UInt16*)Buffer, Cnt);
					break;
				case svInt32:
					Buffer = ALLOC_FUNC<BIT_TYPE, C_Int32>::Append(I, (const C_Int32*)Buffer, Cnt);
					break;
				case svUInt32:
					Buffer = ALLOC_FUNC<BIT_TYPE, C_UInt32>::Append(I, (const C_UInt32*)Buffer, Cnt);
					break;
				case svInt64:
					Buffer = ALLOC_FUNC<BIT_TYPE, C_Int64>::Append(I, (const C_Int64*)Buffer, Cnt);
					break;
				case svUInt64:
					Buffer = ALLOC_FUNC<BIT_TYPE, C_UInt64>::Append(I, (const C_UInt64*)Buffer, Cnt);
					break;
				case svFloat32:
					Buffer = ALLOC_FUNC<BIT_TYPE, C_Float32>::Append(I, (const C_Float32*)Buffer, Cnt);
					break;
				case svFloat64:
					Buffer = ALLOC_FUNC<BIT_TYPE, C_Float64>::Append(I, (const C_Float64*)Buffer, Cnt);
					break;
				case svStrUTF8:
					Buffer = ALLOC_FUNC<BIT_TYPE, UTF8String>::Append(I, (const UTF8String*)Buffer, Cnt);
					break;
				case svStrUTF16:
					Buffer = ALLOC_FUNC<BIT_TYPE, UTF16String>::Append(I, (const UTF16String*)Buffer, Cnt);
					break;
				default:
					Buffer = CdAllocArray::Append(Buffer, Cnt, InSV);
			}

			// check
			CdAllocArray::TDimItem &R = this->fDimension.front();
			this->fTotalCount += Cnt;
			if (this->fTotalCount >= R.DimElmCnt*(R.DimLen+1))
			{
				R.DimLen = this->fTotalCount / R.DimElmCnt;
				this->_SetFlushEvent();
				this->fNeedUpdate = true;
			}
			return Buffer;
		}

		/// append new data from an iterator
		virtual void AppendIter(CdIterator &I, C_Int64 Count)
		{
			if ((Count >= 65536) && (typeid(*this) == typeid(*I.Handler)))
			{
				const unsigned N_BIT = this->BitOf();
				unsigned u1 = (this->fTotalCount * N_BIT) & 0x07;
				unsigned u2 = (I.Ptr * N_BIT) & 0x07;

				if ((u1 == u2) && this->fAllocator.BufStream())
				{
					int nHead = 0;
					while (u1 != 0)
					{
						u1 = (u1 + N_BIT) & 0x07;
						nHead ++;
					}

					if (nHead > 0)
					{
						if (nHead <= Count)
						{
							CdAbstractArray::AppendIter(I, nHead);
							Count -= nHead;
						} else {
							CdAbstractArray::AppendIter(I, Count);
							return;
						}
					}

					C_Int64 tmp=0, num_bit=0;
					for (C_Int64 n=0; n < Count; n++)
					{
						tmp += N_BIT;
						u1 = (u1 + N_BIT) & 0x07;
						if (u1 == 0) num_bit = tmp;
					}

					if (num_bit > 0)
					{
						CdBaseBit<BIT_TYPE> *Src = (CdBaseBit<BIT_TYPE> *)I.Handler;
						Src->Allocator().BufStream()->FlushWrite();
						this->fAllocator.BufStream()->CopyFrom(
							*(Src->Allocator().BufStream()->Stream()),
							(I.Ptr*N_BIT) >> 3, num_bit >> 3);

						C_Int64 n = num_bit / N_BIT;
						Count -= n;
						this->fTotalCount += n;
						I.Ptr += n;
					}

					if (Count > 0)
					{
						CdAbstractArray::AppendIter(I, Count);
					} else {
						// check
						CdAllocArray::TDimItem &R = this->fDimension.front();
						if (this->fTotalCount >= R.DimElmCnt*(R.DimLen+1))
						{
							R.DimLen = this->fTotalCount / R.DimElmCnt;
							this->fNeedUpdate = true;
						}
					}

					return;
				}
			}
			CdAbstractArray::AppendIter(I, Count);
		}

	protected:

		/// offset the iterator
		virtual void IterOffset(CdIterator &I, SIZE64 val)
		{
			I.Ptr += val;
		}

		/// initialize n array
		virtual void IterInit(CdIterator &I, SIZE64 n)
		{
			const unsigned N_BIT = this->BitOf();
			BitClear(this->fAllocator, I.Ptr*N_BIT, n*N_BIT);
		}

		/// get the size in byte corresponding to the count 'Num'
		virtual SIZE64 AllocSize(C_Int64 Num)
		{
			const unsigned N_BIT = this->BitOf();
			SIZE64 rv = Num * N_BIT;
			return (((C_UInt8)rv) & 0x07) ? ((rv >> 3) + 1) : (rv >> 3);
		}
	};


	/// Signed bit-array container
	/** \tparam nbit    the number of bits
	 *  \sa  CdSBit1, CdSBit2, etc
	**/
	template<unsigned nbit>
		class COREARRAY_DLL_DEFAULT CdSignedBit: public CdBaseBit<SBIT0>
	{
	public:
		/// return the number of bits for the element type
		virtual unsigned BitOf() { return nbit; }

		/// create a new object
		virtual CdGDSObj *NewObject()
		{
			return (new CdSignedBit<nbit>)->AssignPipe(*this);
		}

		/// Return a string specifying the class name in stream
		virtual char const* dName() { return SBitStreamNames[nbit-1]; }
		/// Return a string specifying the class name
		virtual char const* dTraitName() { return dName()+1; }
	};


	/// Unsigned bit-array container
	/** \tparam nbit    the number of bits
	 *  \sa  CdBit1, CdBit2, etc
	**/
	template<unsigned nbit>
		class COREARRAY_DLL_DEFAULT CdUnsignedBit: public CdBaseBit<BIT0>
	{
	public:
		/// return the number of bits for the element type
		virtual unsigned BitOf() { return nbit; }

		/// create a new object
		virtual CdGDSObj *NewObject()
		{
			return (new CdUnsignedBit<nbit>)->AssignPipe(*this);
		}

		/// Return a string specifying the class name in stream
		virtual char const* dName() { return BitStreamNames[nbit-1]; }
		/// Return a string specifying the class name
		virtual char const* dTraitName() { return dName()+1; }
	};



	// =====================================================================
	// Template bit functions for allocator
	// =====================================================================

	/// Template for allocate function, such like SBIT0, BIT0
	/** in the case that MEM_TYPE is numeric **/
	template<bool is_signed, typename int_type, C_Int64 mask, typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<BIT_INTEGER<0u, is_signed, int_type, mask>, MEM_TYPE>
	{
		/// integer type
		typedef typename
			BIT_INTEGER<0u, is_signed, int_type, mask>::IntType IntType;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			// initialize
			const unsigned N_BIT = (I.Handler->BitOf());
			SIZE64 pI = I.Ptr * N_BIT;
			I.Ptr += n;
			BIT_LE_R<CdAllocator> ss(I.Allocator);

			I.Allocator->SetPosition(pI >> 3);
			C_UInt8 offset = pI & 0x07;
			if (offset)
				ss.SkipBit(offset);

			for (; n > 0; n--)
			{
				IntType v = ss.ReadBit(N_BIT);
				if (is_signed)
					v = BitSet_IfSigned(v, N_BIT);
				*p++ = VAL_CONVERT(MEM_TYPE, IntType, v);
			}

			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			// initialize
			const unsigned N_BIT = (I.Handler->BitOf());
			SIZE64 pI = I.Ptr * N_BIT;
			I.Ptr += n;
			BIT_LE_R<CdAllocator> ss(I.Allocator);

			I.Allocator->SetPosition(pI >> 3);
			C_UInt8 offset = pI & 0x07;
			if (offset)
				ss.SkipBit(offset);

			for (; n > 0; n--)
			{
				if (*sel++)
				{
					IntType v = ss.ReadBit(N_BIT);
					if (is_signed)
						v = BitSet_IfSigned(v, N_BIT);
					*p++ = VAL_CONVERT(MEM_TYPE, IntType, v);
				} else
					ss.SkipBit(N_BIT);
			}

			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p, ssize_t n)
		{
			// initialize
			const unsigned N_BIT = (I.Handler->BitOf());
			SIZE64 pI = I.Ptr * N_BIT;
			I.Ptr += n;
			BIT_LE_W<CdAllocator> ss(I.Allocator);

			I.Allocator->SetPosition(pI >> 3);
			C_UInt8 offset = pI & 0x07;
			if (offset)
			{
				C_UInt8 Ch = I.Allocator->R8b();
				I.Allocator->SetPosition(I.Allocator->Position() - 1);
				ss.WriteBit(Ch, offset);
			}

			pI += n * N_BIT;
			for (; n > 0; n--)
				ss.WriteBit(VAL_CONVERT(IntType, MEM_TYPE, *p++), N_BIT);
			if (ss.Offset > 0)
			{
				I.Allocator->SetPosition(pI >> 3);
				C_UInt8 Ch = I.Allocator->R8b();
				I.Allocator->SetPosition(I.Allocator->Position() - 1);
				ss.WriteBit(Ch >> ss.Offset, 8 - ss.Offset);
			}

			return p;
		}

		/// append an array to CdAllocator
		static const MEM_TYPE *Append(CdIterator &I, const MEM_TYPE *p, ssize_t n)
		{
			// compression extended info
			const unsigned N_BIT = (I.Handler->BitOf());
			TdCompressRemainder *ar = (I.Handler->PipeInfo() != NULL) ?
				&(I.Handler->PipeInfo()->Remainder()) : NULL;

			// initialize
			SIZE64 pI = I.Ptr * N_BIT;
			I.Ptr += n;
			BIT_LE_W<CdAllocator> ss(I.Allocator);

			// extract bits
			C_UInt8 offset = pI & 0x07;
			if (offset)
			{
				C_UInt8 Ch;
				if (!ar)
				{
					I.Allocator->SetPosition(pI >> 3);
					Ch = I.Allocator->R8b();
					I.Allocator->SetPosition(I.Allocator->Position() - 1);
				} else
					Ch = I.Handler->PipeInfo()->Remainder().Buf[0];
				ss.WriteBit(Ch, offset);
			} else {
				if (!ar)
					I.Allocator->SetPosition(pI >> 3);
			}

			for (; n > 0; n--)
				ss.WriteBit(VAL_CONVERT(IntType, MEM_TYPE, *p++), N_BIT);
			if (ss.Offset > 0)
			{
				if (ar)
				{
					I.Handler->PipeInfo()->Remainder().Size = 1u;
					I.Handler->PipeInfo()->Remainder().Buf[0] = ss.Reminder;
					ss.Offset = 0;
				}
			} else {
				if (ar)
					I.Handler->PipeInfo()->Remainder().Size = 0;
			}

			return p;
		}
	};
}


#include "dBitGDS_Bit1.h"
#include "dBitGDS_Bit2.h"
#include "dBitGDS_Bit4.h"
#include "dBitGDS_Bit24.h"

namespace CoreArray
{
	// =====================================================================
	// Bit integer types
	// =====================================================================

	// unsigned integer

	typedef CdBaseBit<BIT1>       CdBit1;  // *
	typedef CdBaseBit<BIT2>       CdBit2;  // *
	typedef CdUnsignedBit<3u>     CdBit3;
	typedef CdBaseBit<BIT4>       CdBit4;  // *
	typedef CdUnsignedBit<5u>     CdBit5;
	typedef CdUnsignedBit<6u>     CdBit6;
	typedef CdUnsignedBit<7u>     CdBit7;
	typedef CdUInt8               CdBit8;  // *

	typedef CdUnsignedBit<9u>     CdBit9;
	typedef CdUnsignedBit<10u>    CdBit10;
	typedef CdUnsignedBit<11u>    CdBit11;
	typedef CdUnsignedBit<12u>    CdBit12;
	typedef CdUnsignedBit<13u>    CdBit13;
	typedef CdUnsignedBit<14u>    CdBit14;
	typedef CdUnsignedBit<15u>    CdBit15;
	typedef CdUInt16              CdBit16;  // *

	typedef CdUnsignedBit<17u>    CdBit17;
	typedef CdUnsignedBit<18u>    CdBit18;
	typedef CdUnsignedBit<19u>    CdBit19;
	typedef CdUnsignedBit<20u>    CdBit20;
	typedef CdUnsignedBit<21u>    CdBit21;
	typedef CdUnsignedBit<22u>    CdBit22;
	typedef CdUnsignedBit<23u>    CdBit23;
	typedef CdArray<UInt24>       CdBit24;  // *

	typedef CdUInt32              CdBit32;  // *
	typedef CdUInt64              CdBit64;  // *


	// signed integer

	typedef CdSignedBit<2u>     CdSBit2;  // *
	typedef CdSignedBit<3u>     CdSBit3;
	typedef CdSignedBit<4u>     CdSBit4;  // *
	typedef CdSignedBit<5u>     CdSBit5;
	typedef CdSignedBit<6u>     CdSBit6;
	typedef CdSignedBit<7u>     CdSBit7;
	typedef CdInt8              CdSBit8;  // *

	typedef CdSignedBit<9u>     CdSBit9;
	typedef CdSignedBit<10u>    CdSBit10;
	typedef CdSignedBit<11u>    CdSBit11;
	typedef CdSignedBit<12u>    CdSBit12;
	typedef CdSignedBit<13u>    CdSBit13;
	typedef CdSignedBit<14u>    CdSBit14;
	typedef CdSignedBit<15u>    CdSBit15;
	typedef CdInt16             CdSBit16; // *

	typedef CdArray<Int24>      CdSBit24; // *
	typedef CdInt32             CdSBit32; // *
	typedef CdInt64             CdSBit64; // *
}

#endif /* _HEADER_COREARRAY_BIT_GDS_ */
