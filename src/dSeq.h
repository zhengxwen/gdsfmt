// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dSeq.h: CoreArray Containers for extended types
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
 *	\file     dSeq.h
 *	\author   Xiuwen Zheng
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    CoreArray Containers for extended types
 *	\details
**/

#ifndef _dSeq_H_
#define _dSeq_H_

#include <dStruct.h>


namespace CoreArray
{
	using namespace std;

	// ***********************************************************
	//
	// bit classes and functions
	//
	// ***********************************************************

	// Functions
	// bit operators

	/// Clear array of bits from p, with a length of Len
	void bitClear(TdAllocator &alloc, TdPtr64 p, Int64 Len);
	/// Copy array of bits from buffer to the position pD in an allocator
	void bitBufToCpy(TdAllocator &alloc, TdPtr64 pD, void *Buf, size_t L);
	/// Right shift of bits in an allocator
	void bitBinShr(void *Buf, size_t NByte, UInt8 NShr);
	/// Left shift of bits in an allocator
	void bitBinShl(void *Buf, size_t NByte, UInt8 NShl);
	/// Move array of bits from pS to pD in an allocator
	void bitMoveBits(TdAllocator &alloc, TdPtr64 pS, TdPtr64 pD, TdPtr64 Len);


	/// bit array { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 }
	extern const UInt8 CoreArray_MaskBit1Array[];
	/// bit array { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F }
	extern const UInt8 CoreArray_MaskBit1ArrayNot[];

	/// bit array { 0x03, 0x0C, 0x30, 0xC0 }
	extern const UInt8 CoreArray_MaskBit2Array[];
	/// bit array { 0xFC, 0xF3, 0xCF, 0x3F }
	extern const UInt8 CoreArray_MaskBit2ArrayNot[];

	/// bit array { 0x0F, 0xF0 }
	extern const UInt8 CoreArray_MaskBit4Array[];
	/// bit array { 0xF0, 0x0F }
	extern const UInt8 CoreArray_MaskBit4ArrayNot[];



	/// Bit-array container
	/** \tparam bits  number of bits = abs(bits),
	 *                signed integer if bits < 0; unsigned integer if bits > 0
	 *  \sa  TdBit1, TdSBit2, etc
	**/
	template<int bits> class CdBaseBit: public CdVector< BITS<bits> >
	{
	public:
		typedef BITS<bits> ElmType;
		typedef typename TdTraits<ElmType>::TType ElmTypeEx;

    	static const unsigned N_BIT = (bits > 0) ? bits : (-bits);

		CdBaseBit(size_t vDimCnt=0): CdVector< BITS<bits> >(vDimCnt)
			{ this->SetElmSize(1); }

		/// Return number of bits for the element type
		virtual unsigned BitOf() { return N_BIT; }

        virtual CdGDSObj *NewOne(void *Param = NULL)
		{
        	CdBaseBit<bits> *rv = new CdBaseBit<bits>;
			this->xAssignToDim(*rv);
			if (this->fPipeInfo)
				rv->fPipeInfo = this->fPipeInfo->NewOne();
			return rv;
        }

		virtual void SetDLen(int DimIndex, Int32 Value)
		{
			#ifdef COREARRAY_DEBUG_CODE
			this->fDims.at(DimIndex);
			#endif

			TdIterator it;
			Int64 MDimOld, MDimNew, LStep, DCnt, DResid;
			TdPtr64 pS, pD;
			CdVectorX::TdDimItem &pDim = this->fDims[DimIndex];

			if (pDim.DimLen != Value)
			{
				if (pDim.DimElmSize > 0)
				{
					DCnt = 1;
					for (int i=DimIndex-1; i >= 0; i--)
						DCnt *= this->fDims[i].DimLen;
					if (DCnt > 0)
					{
						MDimOld = pDim.DimLen * pDim.DimElmSize;
						MDimNew = Value * pDim.DimElmSize;
						if (pDim.DimLen < Value)
						{
							this->NeedMemory(DCnt * MDimNew);
							DResid = (Value - pDim.DimLen) * pDim.DimElmCnt;
							pS = (DCnt-1)*MDimOld; pD = (DCnt-1)*MDimNew;
							it.Handler = this;
							while (DCnt > 0)
							{
								bitMoveBits(this->fAllocator, pS*N_BIT, pD*N_BIT, MDimOld*N_BIT);
								it.Ptr = pD + MDimOld;
								this->xInitIter(it, DResid);
								pS -= MDimOld; pD -= MDimNew;
								--DCnt;
							}
						} else {
							LStep = MDimOld - MDimNew;
							DResid = (pDim.DimLen - Value) * pDim.DimElmCnt;
							it.Handler = this; it.Ptr = 0;
							pD = pS = 0;
							while (DCnt > 0) {
								it.Ptr += MDimNew;
								this->xDoneIter(it, DResid);
								it.Ptr += LStep;
								bitMoveBits(this->fAllocator, pS*N_BIT, pD*N_BIT, MDimNew*N_BIT);
								pS += MDimOld; pD += MDimNew;
								--DCnt;
							}
						}
					}
				}
				pDim.DimLen = Value;
				this->xDimAuto(DimIndex);
				this->Notify32(mcDimLength, DimIndex);
			}
		}

	protected:

	#ifdef COREARRAY_MSC
		static const int ExtAllocNeed = TdVectorData<UInt8, BITS<bits>, false, COREARRAY_TR_INTEGER, COREARRAY_TR_INTEGER>::ExtAllocNeed;
	#else
		static const int ExtAllocNeed = TdVectorData< UInt8, BITS<bits> >::ExtAllocNeed;
	#endif

		virtual void _InitIter(TdIterator &it, ssize_t Len)
			{ bitClear(this->fAllocator, it.Ptr*N_BIT, Len*N_BIT); };
		virtual void _Assign(TdIterator &it, TdIterator &source)
			{ this->_IntTo(it, source.toInt()); };
		virtual int _Compare(TdIterator &it1, TdIterator &it2)
		{
			Int64 I1=it1.toInt(), I2=it2.toInt();
			if (I1 > I2)
				return 1;
			else if (I1 == I2)
				return 0;
			else
				return -1;
		}

		virtual TdPtr64 AllocNeed(bool Full)
		{
			TdPtr64 rv = this->fEndPtr * N_BIT;
			if (Full)
			{
				rv = (((UInt8)rv) & 0x07) ? ((rv >> 3)+1) : (rv >> 3);
				return rv + ExtAllocNeed;
			} else
				return (rv >> 3);
		}
		virtual void NeedMemory(const TdPtr64 NewMem)
		{
			TdPtr64 rv = NewMem * N_BIT;
			rv = ((((UInt8)rv) & 0x07) ? ((rv >> 3)+1) : (rv >> 3))
				+ ExtAllocNeed;
			CdVectorX::NeedMemory(rv);
		}
		virtual void SaveDirect(CdSerial &Writer)
		{
			Int64 Total = (this->fEndPtr * N_BIT) >> 3;
			this->Notify64(mcBeginSave, Total);

			{   // Primary Part
				char Buf[COREARRAY_STREAM_BUFFER];
				Int64 Cur;
				clock_t tm;

				if (this->fPipeInfo)
					this->fPipeInfo->PushWritePipe(Writer);
				Cur = 0; tm = clock();
				while (Total > 0)
				{
					ssize_t L = (Total >= (Int64)sizeof(Buf)) ? sizeof(Buf) : Total;
					this->fAllocator.Read(Cur, Buf, L);
					Writer.Write((void*)Buf, L);
					Cur += L;
					if ((clock()-tm) > COREARRAY_NOTIFY_TICK)
					{
						tm = clock();
						Int64 D[2] = { Cur, (ssize_t)Writer.Stream() };
						this->Notify64(mcSaving, D);
					}
					Total -= L;
				}
			}
			{	// Remaining Part
				TdPtr64 LEnd = this->fEndPtr * N_BIT;
				unsigned char Remainder = ((UInt8)LEnd) & 0x07;
				typename ElmType::IntTypeEx I = 0;

				if (Remainder != 0)
				{
					this->fAllocator.Read(LEnd >> 3, &I, sizeof(I));
					I &= ~(Int64(-1) << Remainder);

					if (this->fPipeInfo)
					{
						this->fPipeInfo->Remainder().Size = sizeof(I);
						memcpy(this->fPipeInfo->Remainder().Buf, &I, sizeof(I));
					} else
						Writer.Write(&I, sizeof(I));
				} else {
					if (this->fPipeInfo)
					{
						memset(this->fPipeInfo->Remainder().Buf, 0, ExtAllocNeed);
						this->fPipeInfo->Remainder().Size = ExtAllocNeed;
					} else
						Writer.Write(&I, ExtAllocNeed);			
				}
			}

			Writer.FlushWrite();
			this->Notify64(mcSaved, Total);
		}
	};



	template<typename TOutside, int bits, int O>
		struct TdVectorData<TOutside, BITS<bits>, false, O, COREARRAY_TR_INTEGER>
	{
    	static const unsigned N_BIT = (bits > 0) ? bits : (-bits);
		static const unsigned ExtAllocNeed = sizeof(typename BITS<bits>::IntTypeEx)-1;

		// Read
		static void rArray(TIterVDataExt &Rec)
		{
			unsigned char Stack[ARRAY_BUF_LEN];

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim, LStack = 0;
			TdPtr64 pPtr = Rec.p64 * N_BIT;
			typename BITS<bits>::IntTypeEx *pSt =
				(typename BITS<bits>::IntTypeEx*)Stack;

			unsigned char offset, B;
			offset = ((UInt8)pPtr) & 0x07;
			pPtr >>= 3;

			while (Len > 0)
			{
				// Prepare Stack Buffer
				if (LStack < (ssize_t)sizeof(typename BITS<bits>::IntTypeEx))
				{
					ssize_t L = Len*N_BIT + offset;
					if ((L & 0x07) > 0)
						L = (L >> 3) + sizeof(typename BITS<bits>::IntTypeEx);
					else
						L = (L >> 3) + sizeof(typename BITS<bits>::IntTypeEx) - 1;
					pPtr -= LStack;
					LStack = (L<=(ssize_t)sizeof(Stack)) ? L : (ssize_t)sizeof(Stack);
					Rec.Seq->Allocator().Read(pPtr, (void*)Stack, LStack);
					pPtr += LStack;
					pSt = (typename BITS<bits>::IntTypeEx*)Stack;
				}
				// Write to Buffer
				if (bits > 0)
				{
					*p++ = ValCvt<TOutside, typename BITS<bits>::IntTypeEx>(
						(*pSt >> offset) & BITS<bits>::Mask);
				} else {
					*p++ = ValCvt<TOutside, typename BITS<bits>::IntType>(
						BITS_ifsign<typename BITS<bits>::IntType, bits>(
						(*pSt >> offset) & BITS<bits>::Mask));
				}

				offset += N_BIT; B = offset >> 3; offset &= 0x07;
				if (B > 0)
				{
					char *ps = (char*)pSt + B;
					pSt = (typename BITS<bits>::IntTypeEx*)ps;
					LStack -= B;
				}
				Len--;
			};
			Rec.pBuf = (char*)p;
		}

		// Read
		static void rArrayEx(TIterVDataExt &Rec, const CBOOL *Sel)
		{
			unsigned char Stack[ARRAY_BUF_LEN];

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim, LStack = 0;
			TdPtr64 pPtr = Rec.p64 * N_BIT;
			typename BITS<bits>::IntTypeEx *pSt =
				(typename BITS<bits>::IntTypeEx*)Stack;

			unsigned char offset, B;
			offset = ((UInt8)pPtr) & 0x07;
			pPtr >>= 3;

			while (Len > 0)
			{
				// Prepare Stack Buffer
				if (LStack < (ssize_t)sizeof(typename BITS<bits>::IntTypeEx))
				{
					ssize_t L = Len*N_BIT + offset;
					if ((L & 0x07) > 0)
						L = (L >> 3) + sizeof(typename BITS<bits>::IntTypeEx);
					else
						L = (L >> 3) + sizeof(typename BITS<bits>::IntTypeEx) - 1;
					pPtr -= LStack;
					LStack = (L<=(ssize_t)sizeof(Stack)) ? L : (ssize_t)sizeof(Stack);
					Rec.Seq->Allocator().Read(pPtr, (void*)Stack, LStack);
					pPtr += LStack;
					pSt = (typename BITS<bits>::IntTypeEx*)Stack;
				}
				// Write to Buffer
				if (*Sel++)
				{
					if (bits > 0)
					{
						*p++ = ValCvt<TOutside, typename BITS<bits>::IntTypeEx>(
							(*pSt >> offset) & BITS<bits>::Mask);
					} else {
						*p++ = ValCvt<TOutside, typename BITS<bits>::IntType>(
							BITS_ifsign<typename BITS<bits>::IntType, bits>(
							(*pSt >> offset) & BITS<bits>::Mask));
					}
				}

				offset += N_BIT; B = offset >> 3; offset &= 0x07;
				if (B > 0)
				{
					char *ps = (char*)pSt + B;
					pSt = (typename BITS<bits>::IntTypeEx*)ps;
					LStack -= B;
				}
				Len--;
			};
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void rItem(void *OutBuffer, TdPtr64 p, CdVectorX &Seq)
		{
			typename BITS<bits>::IntTypeEx I = 0;
			p *= N_BIT;
			Seq.fAllocator.Read(p >> 3, &I, sizeof(I));
			I >>= ((UInt8)p) & 0x07;
			I &= BITS<bits>::Mask;
			if (bits > 0)
			{
				*((TOutside*)OutBuffer) =
					ValCvt<TOutside, typename BITS<bits>::IntTypeEx>(I);
			} else {
				*((TOutside*)OutBuffer) =
					ValCvt<TOutside, typename BITS<bits>::IntType>(
					BITS_ifsign<typename BITS<bits>::IntType, bits>(I));
			}
		}

		// Write
		static void wArray(TIterVDataExt &Rec)
		{
			const size_t IntTypeEx_Size = sizeof(typename BITS<bits>::IntTypeEx);
			unsigned char Stack[ARRAY_BUF_LEN];

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim, LStack, L;
			TdPtr64 pPtr = Rec.p64 * N_BIT;
			TdAllocator &alloc = Rec.Seq->Allocator();
			bool vCompressed = (Rec.Seq->PipeInfo() != NULL);
			TdCompressRemainder *ar = vCompressed ? &Rec.Seq->PipeInfo()->Remainder() : NULL;
			typename BITS<bits>::IntTypeEx *pSt;
			unsigned char B = 0, offset = (UInt8)(pPtr) & 0x07;

			while (Len > 0)
			{
				if (Len <= (ssize_t)((sizeof(Stack) - IntTypeEx_Size)*8 / N_BIT))
					L = ((Len * N_BIT) >> 3) + IntTypeEx_Size;
				else
					L = sizeof(Stack);
				memset((void*)Stack, 0, L);

				if (Rec.AppendMode)
				{
					if (vCompressed)
					{
						memcpy((void*)Stack, (void*)ar->Buf, ar->Size);
						ar->Size = 0;
					} else
						Stack[0] = alloc.r8(pPtr >> 3) & ~(0xFF << ((UInt8)(pPtr) & 0x07));
				}

				pSt = (typename BITS<bits>::IntTypeEx *)Stack;
				LStack = 0; L = 0;
				while ((Len > 0) && (LStack <= (ssize_t)(sizeof(Stack)-IntTypeEx_Size)))
				{
					*pSt |= (ValCvt<typename BITS<bits>::IntTypeEx, TOutside>(
						*p++) & BITS<bits>::Mask) << offset;
					L += N_BIT;
					offset += N_BIT; B = offset >> 3; offset &= 0x07;
					if (B > 0)
					{
						char *ps = (char*)pSt + B;
						pSt = (typename BITS<bits>::IntTypeEx *)ps;
						LStack += B;
					}
					Len --;
				}

				if (Rec.AppendMode)
				{
					alloc.Write(pPtr >> 3, (void*)Stack, LStack);
					if (offset > 0)
					{
						if (vCompressed)
						{
							pSt = (typename BITS<bits>::IntTypeEx *)(ar->Buf);
							void *tmp = &Stack[LStack];
							*pSt = *((typename BITS<bits>::IntTypeEx *)tmp);
							ar->Size = sizeof(*pSt);
						} else
							alloc.Write((pPtr >> 3) + LStack, &Stack[LStack], sizeof(*pSt));
					} else {
						if (vCompressed)
						{
							memset((void*)ar->Buf, 0, ExtAllocNeed);
							ar->Size = ExtAllocNeed;
						} else
							alloc.Fill((pPtr >> 3) + LStack, ExtAllocNeed, 0);
					}
				} else
					bitBufToCpy(alloc, pPtr, (void*)Stack, L);

				pPtr += L;
			}

			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void wItem(void const* InBuffer, TdPtr64 p, CdVectorX &Seq)
		{
			typename BITS<bits>::IntTypeEx I, Val;
			Val = ValCvt<typename BITS<bits>::IntTypeEx, TOutside>(
				*((TOutside*)InBuffer)) & BITS<bits>::Mask;
			p *= N_BIT;
			unsigned char B = ((UInt8)p) & 0x07;
			p >>= 3;
			_Internal_::TdIterMove<sizeof(I)>::Read((void*)&I, Seq.fAllocator, p);
			I = (I & (~(BITS<bits>::Mask2 << B))) | (Val << B);
			_Internal_::TdIterMove<sizeof(I)>::Write((void*)&I, Seq.fAllocator, p);
		}
	};


	// Specify BITS<1>

	template<typename TOutside, int O>
		struct TdVectorData<TOutside, BITS<1>, false, O, COREARRAY_TR_INTEGER>
	{
		static const unsigned ExtAllocNeed = 0;

		// Read
		static void rArray(TIterVDataExt &Rec)
		{
			unsigned char Stack[ARRAY_BUF_LEN];
			unsigned char B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			TdPtr64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();
			// The header
			offset = ((UInt8)pPtr) & 0x07;
			pPtr >>= 3;
			if (offset != 0)
			{
				B = alloc.r8(pPtr) >> offset;
				offset = (8-offset < Len) ? (8-offset) : Len;
				Len -= offset; pPtr++;
				for (; offset > 0; offset--)
					{ *p++ = ValCvt<TOutside, UInt8>(B & 0x01); B >>= 1; }
			}

			// The middle
			while (Len >= 8)
			{
				ssize_t L = Len >> 3;
				if ((size_t)L > sizeof(Stack)) L = sizeof(Stack);
				alloc.Read(pPtr, Stack, L);
				s = Stack;
				for (ssize_t i = 1; i <= L; i++)
				{
					B = *s++;
					*p++ = ValCvt<TOutside, UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, UInt8>(B);
				}
				pPtr += L; Len -= L << 3;
			}

			// The tail
			if (Len > 0)
			{
				B = alloc.r8(pPtr);
				for (ssize_t i = 1; i <= Len; i++)
					{ *p++ = ValCvt<TOutside, UInt8>(B & 0x01); B >>= 1; }
			}

			// finally
			Rec.pBuf = (char*)p;
		}

		// Read
		static void rArrayEx(TIterVDataExt &Rec, const CBOOL *Sel)
		{
			unsigned char Stack[ARRAY_BUF_LEN];
			unsigned char B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			TdPtr64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();
			// The header
			offset = ((UInt8)pPtr) & 0x07;
			pPtr >>= 3;
			if (offset != 0)
			{
				B = alloc.r8(pPtr) >> offset;
				offset = (8-offset < Len) ? (8-offset) : Len;
				Len -= offset; pPtr++;
				for (; offset > 0; offset--)
				{
					if (*Sel++)
						*p++ = ValCvt<TOutside, UInt8>(B & 0x01);
					B >>= 1;
				}
			}

			// The middle
			while (Len >= 8)
			{
				ssize_t L = Len >> 3;
				if ((size_t)L > sizeof(Stack)) L = sizeof(Stack);
				alloc.Read(pPtr, Stack, L);
				s = Stack;
				for (ssize_t i = 1; i <= L; i++)
				{
					B = *s++;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B);
				}
				pPtr += L; Len -= L << 3;
			}

			// The tail
			if (Len > 0)
			{
				B = alloc.r8(pPtr);
				for (ssize_t i = 1; i <= Len; i++)
				{
					if (*Sel++)
						*p++ = ValCvt<TOutside, UInt8>(B & 0x01);
					B >>= 1;
				}
			}

			// finally
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void rItem(void *OutBuffer, const TdPtr64 p, CdVectorX &Seq)
		{
			unsigned char B = Seq.fAllocator.r8(p >> 3);
			*((TOutside*)OutBuffer) =
				ValCvt<TOutside, UInt8>(
					(B & CoreArray_MaskBit1Array[((UInt8)p) & 0x07]) ? 1 : 0);
		}

		// Write
		static void wArray(TIterVDataExt &Rec)
		{
			unsigned char Stack[ARRAY_BUF_LEN];
			unsigned char B, E, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			TdPtr64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();
			bool vCompressed = (Rec.Seq->PipeInfo()!=NULL);

			// The header
			offset = ((UInt8)pPtr) & 0x07;
			pPtr >>= 3;
			if (offset != 0)
			{
				B = (!vCompressed) ? alloc.r8(pPtr) :
					Rec.Seq->PipeInfo()->Remainder().Buf[0];
				E = (offset+Len-1 < 7) ? (offset+Len-1) : 7;
				for (; offset <= E; offset++, Len--)
				{
					B = (B & CoreArray_MaskBit1ArrayNot[offset]) |
						(((ValCvt<UInt8, TOutside>(*p++)) & 0x01) << offset);
				}
				if ((E < 7) && vCompressed)
				{
					Rec.Seq->PipeInfo()->Remainder().Buf[0] = B;
					Rec.Seq->PipeInfo()->Remainder().Size = 1u;
				} else
					alloc.w8(pPtr, B);
				pPtr++;
			}

			// The middle
			while (Len >= 8)
			{
				ssize_t L = Len >> 3;
				if ((size_t)L > sizeof(Stack)) L = sizeof(Stack);
				s = Stack;
				for (ssize_t i = 1; i <= L; i++)
				{
					B = ValCvt<UInt8, TOutside>(*p++) & 0x01;
					B |= (ValCvt<UInt8, TOutside>(*p++) & 0x01) << 1;
					B |= (ValCvt<UInt8, TOutside>(*p++) & 0x01) << 2;
					B |= (ValCvt<UInt8, TOutside>(*p++) & 0x01) << 3;
					B |= (ValCvt<UInt8, TOutside>(*p++) & 0x01) << 4;
					B |= (ValCvt<UInt8, TOutside>(*p++) & 0x01) << 5;
					B |= (ValCvt<UInt8, TOutside>(*p++) & 0x01) << 6;
					B |= (ValCvt<UInt8, TOutside>(*p++) & 0x01) << 7;
					*s++ = B;
				}
				alloc.Write(pPtr, Stack, L);
				pPtr += L; Len -= L << 3;
			}

			// The tail
			if (Len > 0)
			{
				B = (!Rec.AppendMode) ? alloc.r8(pPtr) : 0;
				for (offset = 0; offset < Len; offset++)
				{
					B = (B & CoreArray_MaskBit1ArrayNot[offset]) |
						((ValCvt<UInt8, TOutside>(*p++) & 0x01) << offset);
				}
				if (vCompressed)
				{
					Rec.Seq->PipeInfo()->Remainder().Size = 1u;
					Rec.Seq->PipeInfo()->Remainder().Buf[0] = B;
				} else
					alloc.w8(pPtr, B);
			}
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void wItem(void const* InBuffer, TdPtr64 p, CdVectorX &Seq)
		{
			unsigned char i = ((UInt8)p) & 0x07;
			UInt8 B = ValCvt<UInt8, TOutside>(*((TOutside*)InBuffer));
			B = B & 0x01; p >>= 3;
			B = (Seq.fAllocator.r8(p) & CoreArray_MaskBit1ArrayNot[i]) | (B << i);
			Seq.fAllocator.w8(p, B);
		}
	};


	// Specify BITS<2>

	template<typename TOutside, int O>
		struct TdVectorData<TOutside, BITS<2>, false, O, COREARRAY_TR_INTEGER>
	{
		static const unsigned ExtAllocNeed = 0;

		// Read
		static void rArray(TIterVDataExt &Rec)
		{
			unsigned char Stack[ARRAY_BUF_LEN];
			unsigned char B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			TdPtr64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();

			// The header
			offset = ((UInt8)pPtr) & 0x03; pPtr >>= 2;
			if (offset != 0)
			{
				B = alloc.r8(pPtr) >> (offset << 1);
				offset = (4-offset < Len) ? (4-offset) : Len;
				Len -= offset; pPtr++;
				for (; offset > 0; offset--)
					{ *p++ = ValCvt<TOutside, UInt8>(B & 0x03); B >>= 2; }
			}

			// The middle
			while (Len >= 4)
			{
				ssize_t L = Len >> 2;
				if ((size_t)L > sizeof(Stack)) L = sizeof(Stack);
				alloc.Read(pPtr, Stack, L);
				s = Stack;
				for (ssize_t i = 1; i <= L; i++)
				{
					B = *s++;
					*p++ = ValCvt<TOutside, UInt8>(B & 0x03); B >>= 2;
					*p++ = ValCvt<TOutside, UInt8>(B & 0x03); B >>= 2;
					*p++ = ValCvt<TOutside, UInt8>(B & 0x03); B >>= 2;
					*p++ = ValCvt<TOutside, UInt8>(B);
				}
				pPtr += L; Len -= L << 2;
			}

			// The tail
			if (Len > 0)
			{
				B = alloc.r8(pPtr);
				for (ssize_t i = 1; i <= Len; i++)
					{ *p++ = ValCvt<TOutside, UInt8>(B & 0x03); B >>= 2; }
			}

			// finally
			Rec.pBuf = (char*)p;
		}

		// Read
		static void rArrayEx(TIterVDataExt &Rec, const CBOOL *Sel)
		{
			unsigned char Stack[ARRAY_BUF_LEN];
			unsigned char B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			TdPtr64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();

			// The header
			offset = ((UInt8)pPtr) & 0x03; pPtr >>= 2;
			if (offset != 0)
			{
				B = alloc.r8(pPtr) >> (offset << 1);
				offset = (4-offset < Len) ? (4-offset) : Len;
				Len -= offset; pPtr++;
				for (; offset > 0; offset--)
				{
					if (*Sel++)
						*p++ = ValCvt<TOutside, UInt8>(B & 0x03);
					B >>= 2;
				}
			}

			// The middle
			while (Len >= 4)
			{
				ssize_t L = (Len >> 2);
				if ((size_t)L > sizeof(Stack)) L = sizeof(Stack);
				alloc.Read(pPtr, Stack, L);
				s = Stack;
				for (ssize_t i = 1; i <= L; i++)
				{
					B = *s++;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B & 0x03);
					B >>= 2;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B & 0x03);
					B >>= 2;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B & 0x03);
					B >>= 2;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B & 0x03);
				}
				pPtr += L; Len -= (L << 2);
			}

			// The tail
			if (Len > 0)
			{
				B = alloc.r8(pPtr);
				for (ssize_t i = 1; i <= Len; i++)
				{
					if (*Sel++)
						*p++ = ValCvt<TOutside, UInt8>(B & 0x03);
					B >>= 2;
				}
			}

			// finally
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void rItem(void *OutBuffer, const TdPtr64 p, CdVectorX &Seq)
		{
			unsigned char B = Seq.fAllocator.r8(p >> 2);
			*((TOutside*)OutBuffer) = ValCvt<TOutside, UInt8>(
				(B >> ((((UInt8)p) & 0x03) << 1)) & 0x03);
		}

		// Write
		static void wArray(TIterVDataExt &Rec)
		{
			unsigned char Stack[ARRAY_BUF_LEN];
			unsigned char E, offset, *s, B=0;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			TdPtr64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();
			bool vCompressed = (Rec.Seq->PipeInfo()!=NULL);

			// The header
			offset = ((UInt8)pPtr) & 0x03;
			pPtr = pPtr >> 2;
			if ((offset != 0) && (Len > 0))
			{
				B = (!vCompressed) ? alloc.r8(pPtr) :
					Rec.Seq->PipeInfo()->Remainder().Buf[0];
				E = ((offset+Len-1) < 3) ? (offset+Len-1) : 3;
				for (; offset <= E; offset++, Len--)
				{
					B = (B & CoreArray_MaskBit2ArrayNot[offset]) |
						(((ValCvt<UInt8, TOutside>(*p++)) & 0x03) << (offset << 1));
                }
				if ((E < 3) && vCompressed)
				{
					Rec.Seq->PipeInfo()->Remainder().Buf[0] = B;
					Rec.Seq->PipeInfo()->Remainder().Size = 1u;
				} else
					alloc.w8(pPtr, B);
				pPtr++;
			}

			// The middle
			while (Len >= 4)
			{
				ssize_t L = Len >> 2;
				if ((size_t)L > sizeof(Stack)) L = sizeof(Stack);
				s = Stack;
				for (ssize_t i = 1; i <= L; i++)
				{
					B = ValCvt<UInt8, TOutside>(*p++) & 0x03;
					B |= (ValCvt<UInt8, TOutside>(*p++) & 0x03) << 2;
					B |= (ValCvt<UInt8, TOutside>(*p++) & 0x03) << 4;
					B |= (ValCvt<UInt8, TOutside>(*p++) & 0x03) << 6;
					*s++ = B;
				}
				alloc.Write(pPtr, Stack, L);
				pPtr += L; Len -= L << 2;
			}

			// The tail
			if (Len > 0)
			{
				B = (!Rec.AppendMode) ? alloc.r8(pPtr) : 0;
				for (offset = 0; offset < Len; offset++)
				{
					B = (B & CoreArray_MaskBit2ArrayNot[offset]) |
						((ValCvt<UInt8, TOutside>(*p++) & 0x03) << (offset << 1));
                }
				if (vCompressed)
				{
					Rec.Seq->PipeInfo()->Remainder().Size = 1u;
					Rec.Seq->PipeInfo()->Remainder().Buf[0] = B;
				} else
					alloc.w8(pPtr, B);
			}

			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void wItem(void const* InBuffer, TdPtr64 p, CdVectorX &Seq)
		{
			unsigned char i = ((UInt8)p) & 0x03;
			UInt8 B = ValCvt<UInt8, TOutside>(*((TOutside*)InBuffer));
			B = B & 0x03; p >>= 2;
			B = (Seq.fAllocator.r8(p) & CoreArray_MaskBit2ArrayNot[i]) | (B << (i << 1));
			Seq.fAllocator.w8(p, B);
		}
	};


	// Specify BITS<4>

	template<typename TOutside, int O>
		struct TdVectorData<TOutside, BITS<4>, false, O, COREARRAY_TR_INTEGER>
	{
		static const unsigned ExtAllocNeed = 0;

		// Read
		static void rArray(TIterVDataExt &Rec)
		{
			unsigned char Stack[ARRAY_BUF_LEN];
			unsigned char B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			TdPtr64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();

			// The header
			offset = ((UInt8)pPtr) & 0x01;
			pPtr >>= 1;
			if ((offset > 0) && (Len > 0))
			{
				*p++ = ValCvt<TOutside, UInt8>(alloc.r8(pPtr) >> 4);
				pPtr++; Len--;
			}

			// The middle
			while (Len >= 2)
			{
				ssize_t L = Len >> 1;
				if ((size_t)L > sizeof(Stack)) L = sizeof(Stack);
				alloc.Read(pPtr, Stack, L);
				s = Stack;
				for (ssize_t i = 1; i <= L; i++)
				{
					B = *s++;
					*p++ = ValCvt<TOutside, UInt8>(B & 0x0F); B >>= 4;
					*p++ = ValCvt<TOutside, UInt8>(B);
				}
				pPtr += L; Len -= L << 1;
			}

			// The tail
			if (Len > 0)
				*p++ = ValCvt<TOutside, UInt8>(alloc.r8(pPtr) & 0x0F);

			// finally
			Rec.pBuf = (char*)p;
		}

		// Read
		static void rArrayEx(TIterVDataExt &Rec, const CBOOL *Sel)
		{
			unsigned char Stack[ARRAY_BUF_LEN];
			unsigned char B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			TdPtr64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();

			// The header
			offset = ((UInt8)pPtr) & 0x01;
			pPtr >>= 1;
			if ((offset > 0) && (Len > 0))
			{
				if (*Sel++)
					*p++ = ValCvt<TOutside, UInt8>(alloc.r8(pPtr) >> 4);
				pPtr++; Len--;
			}

			// The middle
			while (Len >= 2)
			{
				ssize_t L = Len >> 1;
				if ((size_t)L > sizeof(Stack)) L = sizeof(Stack);
				alloc.Read(pPtr, Stack, L);
				s = Stack;
				for (ssize_t i = 1; i <= L; i++)
				{
					B = *s++;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B & 0x0F);
					B >>= 4;
					if (*Sel++) *p++ = ValCvt<TOutside, UInt8>(B);
				}
				pPtr += L; Len -= L << 1;
			}

			// The tail
			if ((Len > 0) && (*Sel++))
				*p++ = ValCvt<TOutside, UInt8>(alloc.r8(pPtr) & 0x0F);

			// finally
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void rItem(void *OutBuffer, const TdPtr64 p, CdVectorX &Seq)
		{
			unsigned char B = Seq.fAllocator.r8(p >> 1);
			*((TOutside*)OutBuffer) = ValCvt<TOutside, UInt8>(
				(((UInt8)p) & 0x01) ? (B >> 4) : (B & 0x0F));
		}

		// Write
		static void wArray(TIterVDataExt &Rec)
		{
			unsigned char Stack[ARRAY_BUF_LEN];
			unsigned char offset, *s, B=0;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			TdPtr64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();
			bool vCompressed = (Rec.Seq->PipeInfo()!=NULL);

			// The header
			offset = ((UInt8)pPtr) & 0x01;
			pPtr = pPtr >> 1;
			if ((offset > 0) && (Len > 0))
			{
				if (vCompressed)
				{
                	B = Rec.Seq->PipeInfo()->Remainder().Buf[0];
					Rec.Seq->PipeInfo()->Remainder().Size = 0;
				} else
					B = alloc.r8(pPtr);
				B = (B & CoreArray_MaskBit4ArrayNot[offset]) |
					(((ValCvt<UInt8, TOutside>(*p++)) & 0x0F) << 4);
				alloc.w8(pPtr, B);
				pPtr++; Len--;
			}

			// The middle
			while (Len >= 2)
			{
				ssize_t L = Len >> 1;
				if ((size_t)L > sizeof(Stack)) L = sizeof(Stack);
				s = Stack;
				for (ssize_t i = 1; i <= L; i++)
				{
					B = ValCvt<UInt8, TOutside>(*p++) & 0x0F;
					B |= (ValCvt<UInt8, TOutside>(*p++) & 0x0F) << 4;
					*s++ = B;
				}
				alloc.Write(pPtr, Stack, L);
				pPtr += L; Len -= L << 1;
			}

			// The tail
			if (Len > 0)
			{
				B = (!Rec.AppendMode) ? alloc.r8(pPtr) : 0;
				B = (B & 0xF0) | (ValCvt<UInt8, TOutside>(*p++) & 0x0F);
				if (vCompressed)
				{
					Rec.Seq->PipeInfo()->Remainder().Size = 1u;
					Rec.Seq->PipeInfo()->Remainder().Buf[0] = B;
				} else
					alloc.w8(pPtr, B);
			}

			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void wItem(void const* InBuffer, TdPtr64 p, CdVectorX &Seq)
		{
			unsigned char i = ((UInt8)p) & 0x01;
			UInt8 B = ValCvt<UInt8, TOutside>(*((TOutside*)InBuffer));
			B = B & 0x0F; p >>= 1;
			B = (Seq.fAllocator.r8(p) & CoreArray_MaskBit4ArrayNot[i]) | (B << (i << 2));
			Seq.fAllocator.w8(p, B);
		}
	};


	// Specify BITS<24>

	template<typename TOutside, int O>
		struct TdVectorData<TOutside, BITS<24>, false, O, COREARRAY_TR_INTEGER>
	{
		// Read
		static void rArray(TIterVDataExt &Rec)
		{
			unsigned char buf[ARRAY_BUF_LEN];
			Int32 Len = Rec.LastDim;
			TOutside *p = (TOutside*)Rec.pBuf;
			while (Len > 0)
			{
				ssize_t L = ((size_t)Len >= (sizeof(buf)/3)) ? (sizeof(buf)/3) : Len;
				ssize_t Lx = L * 3;
				Len -= L;
				Rec.Seq->Allocator().Read(Rec.p64, (void*)buf, Lx);
				Rec.p64 += Lx;
				unsigned char *pc = buf;
				for (; L > 0; L--)
				{
					UInt32 i = *((Int32*)pc) & 0xFFFFFF;
					*p++ = ValCvt<TOutside, UInt32>(i);
					pc += 3;
				}
			}
			Rec.pBuf = (char*)p;
		}

		// Read
		static void rArrayEx(TIterVDataExt &Rec, const CBOOL *Sel)
		{
			unsigned char buf[ARRAY_BUF_LEN];
			Int32 Len = Rec.LastDim;
			TOutside *p = (TOutside*)Rec.pBuf;
			while (Len > 0)
			{
				ssize_t L = ((size_t)Len >= (sizeof(buf)/3)) ? (sizeof(buf)/3) : Len;
				ssize_t Lx = L * 3;
				Len -= L;
				Rec.Seq->Allocator().Read(Rec.p64, (void*)buf, Lx);
				Rec.p64 += Lx;
				unsigned char *pc = buf;
				for (; L > 0; L--)
				{
					if (*Sel++)
					{
						UInt32 i = *((Int32*)pc) & 0xFFFFFF;
						*p++ = ValCvt<TOutside, UInt32>(i);
                    }
					pc += 3;
				}
			}
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void rItem(void *OutBuffer, const TdPtr64 p, CdVectorX &Seq)
		{
			UInt32 i = 0;
			Seq.fAllocator.Read(p, (void*)&i, 3);
			*((TOutside*)OutBuffer) = ValCvt<TOutside, UInt32>(i);
		}

		// Write
		static void wArray(TIterVDataExt &Rec)
		{
			unsigned char buf[ARRAY_BUF_LEN];
			TdPtr64 p64 = Rec.p64;
			Int32 Len = Rec.LastDim;
			ssize_t L, Lx;
			TOutside *s = (TOutside*)Rec.pBuf;
			while (Len > 0)
			{
				L = ((size_t)Len >= (sizeof(buf)/3)) ? (sizeof(buf)/3) : Len;
				Len -= L; Lx = L * 3;
				unsigned char *pc = buf;
				for (; L > 0; L--)
				{
					UInt32 i = ValCvt<UInt32, TOutside>(*s++);
					*pc++ = i & 0xFF; *pc++ = (i >> 8) & 0xFF; *pc++ = (i >> 16) & 0xFF;
				}
				Rec.Seq->Allocator().Write(p64, (void*)buf, Lx);
				p64 += Lx;
			}
			Rec.pBuf = (char*)s;
		}

		COREARRAY_INLINE static void wItem(void const* InBuffer, const TdPtr64 p, CdVectorX &Seq)
		{
			UInt32 i = ValCvt<UInt32, TOutside>(*((TOutside*)InBuffer));
			Seq.fAllocator.Write(p, (void*)&i, 3);
		}
	};

	// Specify BITS<-24>

	template<typename TOutside, int O>
		struct TdVectorData<TOutside, BITS<-24>, false, O, COREARRAY_TR_INTEGER>
	{
		// Read
		static void rArray(TIterVDataExt &Rec)
		{
			unsigned char buf[ARRAY_BUF_LEN], *pc;
			Int32 Len = Rec.LastDim;
			TOutside *p = (TOutside*)Rec.pBuf;
			while (Len > 0)
			{
				ssize_t L = ((size_t)Len >= (sizeof(buf)/3)) ? (sizeof(buf)/3) : Len;
				ssize_t Lx = L * 3;
				Len -= L;
				Rec.Seq->Allocator().Read(Rec.p64, (void*)buf, Lx);
				Rec.p64 += Lx; pc = buf;
				for (; L > 0; L--)
				{
					Int32 i = *((Int32*)pc) & 0xFFFFFF;
					i = BITS_ifsign<Int32, 24>(i);
					*p++ = ValCvt<TOutside, Int32>(i);
					pc += 3;
				}
			}
			Rec.pBuf = (char*)p;
		}

		// Read
		static void rArrayEx(TIterVDataExt &Rec, const CBOOL *Sel)
		{
			unsigned char buf[ARRAY_BUF_LEN], *pc;
			Int32 Len = Rec.LastDim;
			TOutside *p = (TOutside*)Rec.pBuf;
			while (Len > 0)
			{
				ssize_t L = ((size_t)Len >= (sizeof(buf)/3)) ? (sizeof(buf)/3) : Len;
				ssize_t Lx = L * 3;
				Len -= L;
				Rec.Seq->Allocator().Read(Rec.p64, (void*)buf, Lx);
				Rec.p64 += Lx; pc = buf;
				for (; L > 0; L--)
				{
					if (*Sel++)
					{
						Int32 i = *((Int32*)pc) & 0xFFFFFF;
						i = BITS_ifsign<Int32, 24>(i);
						*p++ = ValCvt<TOutside, Int32>(i);
                    }
					pc += 3;
				}
			}
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void rItem(void *OutBuffer, const TdPtr64 p, CdVectorX &Seq)
		{
			Int32 i = 0;
			Seq.fAllocator.Read(p, (void*)&i, 3);
			i = BITS_ifsign<Int32, 24>(i);
			*((TOutside*)OutBuffer) = ValCvt<TOutside, Int32>(i);
		}

		// Write
		static void wArray(TIterVDataExt &Rec)
		{
			unsigned char buf[ARRAY_BUF_LEN];
			Int32 Len = Rec.LastDim;
			TOutside *s = (TOutside*)Rec.pBuf;
			while (Len > 0)
			{
				ssize_t L = ((size_t)Len >= (sizeof(buf)/3)) ? (sizeof(buf)/3) : Len;
				ssize_t Lx = L * 3;
				Len -= L;
				unsigned char *pc = buf;
				for (; L > 0; L--)
				{
					Int32 i = ValCvt<Int32, TOutside>(*s++);
					*pc++ = i & 0xFF; *pc++ = (i >> 8) & 0xFF; *pc++ = (i >> 16) & 0xFF;
				}
				Rec.Seq->Allocator().Write(Rec.p64, (void*)buf, Lx);
				Rec.p64 += Lx;
			}
			Rec.pBuf = (char*)s;
		}

		COREARRAY_INLINE static void wItem(void const* InBuffer, const TdPtr64 p, CdVectorX &Seq)
		{
			Int32 i = ValCvt<Int32, TOutside>(*((TOutside*)InBuffer));
			Seq.fAllocator.Write(p, (void*)&i, 3);
		}
	};


	// unsign integer

	typedef CdBaseBit<1>		CdBit1; // *
	typedef CdBaseBit<2>		CdBit2; // *
	typedef CdBaseBit<3>		CdBit3;
	typedef CdBaseBit<4>		CdBit4; // *
	typedef CdBaseBit<5>		CdBit5;
	typedef CdBaseBit<6>		CdBit6;
	typedef CdBaseBit<7>		CdBit7;
	typedef CdUInt8 			CdBit8; // *

	typedef CdBaseBit<9>		CdBit9;
	typedef CdBaseBit<10>		CdBit10;
	typedef CdBaseBit<11>		CdBit11;
	typedef CdBaseBit<12>		CdBit12;
	typedef CdBaseBit<13>		CdBit13;
	typedef CdBaseBit<14>		CdBit14;
	typedef CdBaseBit<15>		CdBit15;
	typedef CdUInt16			CdBit16; // *

	typedef CdVector<UInt24>	CdBit24; // *
	typedef CdUInt32 			CdBit32; // *
	typedef CdUInt64 			CdBit64; // *

	// sign integer

	typedef CdBaseBit<-2>		CdSBit2; // *
	typedef CdBaseBit<-3>		CdSBit3;
	typedef CdBaseBit<-4>		CdSBit4; // *
	typedef CdBaseBit<-5>		CdSBit5;
	typedef CdBaseBit<-6>		CdSBit6;
	typedef CdBaseBit<-7>		CdSBit7;
	typedef CdInt8	 			CdSBit8; // *

	typedef CdBaseBit<-9>		CdSBit9;
	typedef CdBaseBit<-10>		CdSBit10;
	typedef CdBaseBit<-11>		CdSBit11;
	typedef CdBaseBit<-12>		CdSBit12;
	typedef CdBaseBit<-13>		CdSBit13;
	typedef CdBaseBit<-14>		CdSBit14;
	typedef CdBaseBit<-15>		CdSBit15;
	typedef CdInt16				CdSBit16; // *

	typedef CdVector<Int24>		CdSBit24; // *
	typedef CdInt32       		CdSBit32; // *
	typedef CdInt64       		CdSBit64; // *





	// ***********************************************************
	// ***********************************************************

	/// Register CoreArray classes
	void RegisterClass();
}

#endif /* _dSeq_H_ */
