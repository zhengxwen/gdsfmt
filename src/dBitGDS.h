// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dBitGDS.h: CoreArray Containers for extended types
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
 *	\file     dBitGDS.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    CoreArray Containers for extended types
 *	\details
**/

#ifndef _HEADER_COREARRAY_BIT_GDS_
#define _HEADER_COREARRAY_BIT_GDS_

#include <dBit.h>
#include <dStruct.h>
#include <dStrGDS.h>


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
	COREARRAY_DLL_DEFAULT void bitClear(
		TdAllocator &alloc, SIZE64 p, C_Int64 Len);

	/// Copy array of bits from buffer to the position pD in an allocator
	COREARRAY_DLL_DEFAULT void bitBufToCpy(
		TdAllocator &alloc, SIZE64 pD, void *Buf, size_t L);

	/// Right shift of bits in an allocator
	COREARRAY_DLL_DEFAULT void bitBinShr(
		void *Buf, size_t NByte, C_UInt8 NShr);

	/// Left shift of bits in an allocator
	COREARRAY_DLL_DEFAULT void bitBinShl(
		void *Buf, size_t NByte, C_UInt8 NShl);

	/// Move array of bits from pS to pD in an allocator
	COREARRAY_DLL_DEFAULT void bitMoveBits(
		TdAllocator &alloc, SIZE64 pS, SIZE64 pD, SIZE64 Len);


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



	/// Bit-array container
	/** \tparam bits  number of bits = abs(bits),
	 *                signed integer if bits < 0; unsigned integer if bits > 0
	 *  \sa  TdBit1, TdSBit2, etc
	**/
	template<typename BIT_TYPE>
	class COREARRAY_DLL_DEFAULT CdBaseBit: public CdVector<BIT_TYPE>
	{
	public:
		typedef BIT_TYPE ElmType;
		typedef typename TdTraits<ElmType>::TType ElmTypeEx;

		static const unsigned N_BIT = BIT_TYPE::BIT_NUM;

		CdBaseBit(): CdVector<BIT_TYPE>()
			{ this->SetElmSize(1); }

		/// Return number of bits for the element type
		virtual unsigned BitOf() { return N_BIT; }

        virtual CdGDSObj *NewOne(void *Param = NULL)
		{
        	CdBaseBit<BIT_TYPE> *rv = new CdBaseBit<BIT_TYPE>;
			this->xAssignToDim(*rv);
			if (this->fPipeInfo)
				rv->fPipeInfo = this->fPipeInfo->NewOne();
			return rv;
        }

		virtual void SetDLen(int DimIndex, C_Int32 Value)
		{
			#ifdef COREARRAY_CODE_DEBUG
			this->fDims.at(DimIndex);
			#endif

			TdIterator it;
			C_Int64 MDimOld, MDimNew, LStep, DCnt, DResid;
			SIZE64 pS, pD;
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

		static const int ExtAllocNeed = VEC_DATA<C_UInt8, BIT_TYPE>::ExtAllocNeed;

		virtual void _InitIter(TdIterator &it, ssize_t Len)
			{ bitClear(this->fAllocator, it.Ptr*N_BIT, Len*N_BIT); };
		virtual void _Assign(TdIterator &it, TdIterator &source)
			{ this->_IntTo(it, source.toInt()); };
		virtual int _Compare(TdIterator &it1, TdIterator &it2)
		{
			C_Int64 I1=it1.toInt(), I2=it2.toInt();
			if (I1 > I2)
				return 1;
			else if (I1 == I2)
				return 0;
			else
				return -1;
		}

		virtual SIZE64 AllocNeed(bool Full)
		{
			SIZE64 rv = this->fEndPtr * N_BIT;
			if (Full)
			{
				rv = (((C_UInt8)rv) & 0x07) ? ((rv >> 3)+1) : (rv >> 3);
				return rv + ExtAllocNeed;
			} else
				return (rv >> 3);
		}
		virtual void NeedMemory(const SIZE64 NewMem)
		{
			SIZE64 rv = NewMem * N_BIT;
			rv = ((((C_UInt8)rv) & 0x07) ? ((rv >> 3)+1) : (rv >> 3))
				+ ExtAllocNeed;
			CdVectorX::NeedMemory(rv);
		}
		virtual void SaveDirect(CdSerial &Writer)
		{
			C_Int64 Total = (this->fEndPtr * N_BIT) >> 3;
			this->Notify64(mcBeginSave, Total);

			{   // Primary Part
				char Buf[COREARRAY_STREAM_BUFFER];
				C_Int64 Cur;
				clock_t tm;

				if (this->fPipeInfo)
					this->fPipeInfo->PushWritePipe(Writer);
				Cur = 0; tm = clock();
				while (Total > 0)
				{
					ssize_t L = (Total >= (C_Int64)sizeof(Buf)) ? sizeof(Buf) : Total;
					this->fAllocator.Read(Cur, Buf, L);
					Writer.Write((void*)Buf, L);
					Cur += L;
					if ((clock()-tm) > COREARRAY_NOTIFY_TICK)
					{
						tm = clock();
						C_Int64 D[2] = { Cur, (ssize_t)Writer.Stream() };
						this->Notify64(mcSaving, D);
					}
					Total -= L;
				}
			}
			{	// Remaining Part
				SIZE64 LEnd = this->fEndPtr * N_BIT;
				unsigned char Remainder = ((C_UInt8)LEnd) & 0x07;
				typename ElmType::IntTypeEx I = 0;

				if (Remainder != 0)
				{
					this->fAllocator.Read(LEnd >> 3, &I, sizeof(I));
					I &= ~(C_Int64(-1) << Remainder);

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


	// --------------------------------------------------------------------

	/// template functions for BIT_INTEGER
	template<typename TOutside, typename BIT_TYPE, int O>
		struct COREARRAY_DLL_DEFAULT VEC_DATA
		<TOutside, BIT_TYPE, false, O, COREARRAY_TR_BIT_INTEGER>
	{
		static const unsigned N_BIT = BIT_TYPE::BIT_NUM;
		static const unsigned ExtAllocNeed = sizeof(typename BIT_TYPE::IntTypeEx)-1;

		// Read
		static void COREARRAY_FLATTEN rArray(TIterVDataExt &Rec)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE_PLUS];

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim, LStack = 0;
			SIZE64 pPtr = Rec.p64 * N_BIT;
			typename BIT_TYPE::IntTypeEx *pSt =
				(typename BIT_TYPE::IntTypeEx*)Stack;

			unsigned char offset, B;
			offset = ((C_UInt8)pPtr) & 0x07;
			pPtr >>= 3;

			while (Len > 0)
			{
				// Prepare Stack Buffer
				if (LStack < (ssize_t)sizeof(typename BIT_TYPE::IntTypeEx))
				{
					ssize_t L = Len*N_BIT + offset;
					if ((L & 0x07) > 0)
						L = (L >> 3) + sizeof(typename BIT_TYPE::IntTypeEx);
					else
						L = (L >> 3) + sizeof(typename BIT_TYPE::IntTypeEx) - 1;
					pPtr -= LStack;
					LStack = (L<=MEMORY_BUFFER_SIZE) ? L : MEMORY_BUFFER_SIZE;
					Rec.Seq->Allocator().Read(pPtr, (void*)Stack, LStack);
					pPtr += LStack;
					pSt = (typename BIT_TYPE::IntTypeEx*)Stack;
				}
				// Write to Buffer
				if (!BIT_TYPE::IS_SIGNED)
				{
					*p++ = ValCvt<TOutside, typename BIT_TYPE::IntTypeEx>(
						(GET_VAL_UNALIGNED_PTR(pSt) >> offset) &
						BIT_TYPE::BIT_MASK);
				} else {
					*p++ = ValCvt<TOutside, typename BIT_TYPE::IntType>(
						BITS_ifsign<BIT_TYPE>(
						(GET_VAL_UNALIGNED_PTR(pSt) >> offset) &
						BIT_TYPE::BIT_MASK));
				}

				offset += N_BIT; B = offset >> 3; offset &= 0x07;
				if (B > 0)
				{
					char *ps = (char*)pSt + B;
					pSt = (typename BIT_TYPE::IntTypeEx*)ps;
					LStack -= B;
				}
				Len--;
			}

			Rec.pBuf = (char*)p;
		}

		// Read
		static void COREARRAY_FLATTEN rArrayEx(TIterVDataExt &Rec,
			const C_BOOL *Sel)
		{
			static const ssize_t EXSIZE = sizeof(typename BIT_TYPE::IntTypeEx);

			C_UInt8 Stack[MEMORY_BUFFER_SIZE_PLUS];

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim, LStack = 0;
			SIZE64 pPtr = Rec.p64 * N_BIT;
			typename BIT_TYPE::IntTypeEx *pSt =
				(typename BIT_TYPE::IntTypeEx*)Stack;

			unsigned char offset, B;
			offset = ((C_UInt8)pPtr) & 0x07;
			pPtr >>= 3;

			while (Len > 0)
			{
				// Prepare Stack Buffer
				if (LStack < EXSIZE)
				{
					ssize_t L = Len*N_BIT + offset;
					if ((L & 0x07) > 0)
						L = (L >> 3) + EXSIZE;
					else
						L = (L >> 3) + EXSIZE - 1;
					pPtr -= LStack;
					LStack = (L<=MEMORY_BUFFER_SIZE) ? L : MEMORY_BUFFER_SIZE;
					Rec.Seq->Allocator().Read(pPtr, Stack, LStack);
					pPtr += LStack;
					pSt = (typename BIT_TYPE::IntTypeEx*)Stack;
				}
				// Write to Buffer
				if (*Sel++)
				{
					if (!BIT_TYPE::IS_SIGNED)
					{
						*p++ = ValCvt<TOutside, typename BIT_TYPE::IntTypeEx>(
							(GET_VAL_UNALIGNED_PTR(pSt) >> offset) &
							BIT_TYPE::BIT_MASK);
					} else {
						*p++ = ValCvt<TOutside, typename BIT_TYPE::IntType>(
							BITS_ifsign<BIT_TYPE>(
							(GET_VAL_UNALIGNED_PTR(pSt) >> offset) &
							BIT_TYPE::BIT_MASK));
					}
				}

				offset += N_BIT; B = offset >> 3; offset &= 0x07;
				if (B > 0)
				{
					char *ps = (char*)pSt + B;
					pSt = (typename BIT_TYPE::IntTypeEx*)ps;
					LStack -= B;
				}
				Len--;
			}

			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void COREARRAY_FLATTEN rItem(
			void *OutBuffer, SIZE64 p, CdVectorX &Seq)
		{
			typename BIT_TYPE::IntTypeEx I = 0;
			p *= N_BIT;
			Seq.fAllocator.Read(p >> 3, &I, sizeof(I));
			I >>= ((C_UInt8)p) & 0x07;
			I &= BIT_TYPE::BIT_MASK;
			if (!BIT_TYPE::IS_SIGNED)
			{
				*((TOutside*)OutBuffer) =
					ValCvt<TOutside, typename BIT_TYPE::IntTypeEx>(I);
			} else {
				*((TOutside*)OutBuffer) =
					ValCvt<TOutside, typename BIT_TYPE::IntType>(
					BITS_ifsign<BIT_TYPE>(I));
			}
		}

		// Write
		static void COREARRAY_FLATTEN wArray(TIterVDataExt &Rec)
		{
			static const ssize_t EXSIZE = sizeof(typename BIT_TYPE::IntTypeEx);

			C_UInt8 Stack[MEMORY_BUFFER_SIZE_PLUS];

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim, LStack, L;
			SIZE64 pPtr = Rec.p64 * N_BIT;
			TdAllocator &alloc = Rec.Seq->Allocator();
			bool vCompressed = (Rec.Seq->PipeInfo() != NULL);
			TdCompressRemainder *ar = vCompressed ? &Rec.Seq->PipeInfo()->Remainder() : NULL;
			typename BIT_TYPE::IntTypeEx *pSt;
			unsigned char B = 0, offset = (C_UInt8)(pPtr) & 0x07;

			while (Len > 0)
			{
				if (Len <= ((MEMORY_BUFFER_SIZE - EXSIZE)*8 / (ssize_t)N_BIT))
					L = ((Len * N_BIT) >> 3) + EXSIZE;
				else
					L = MEMORY_BUFFER_SIZE;
				memset((void*)Stack, 0, L);

				if (Rec.AppendMode)
				{
					if (vCompressed)
					{
						memcpy(Stack, (void*)ar->Buf, ar->Size);
						ar->Size = 0;
					} else
						Stack[0] = alloc.r8(pPtr >> 3) & ~(0xFF << ((C_UInt8)(pPtr) & 0x07));
				}

				pSt = (typename BIT_TYPE::IntTypeEx *)Stack;
				LStack = 0; L = 0;
				while ((Len > 0) && (LStack <= (MEMORY_BUFFER_SIZE-EXSIZE)))
				{
					typename BIT_TYPE::IntTypeEx V =
						(ValCvt<typename BIT_TYPE::IntTypeEx, TOutside>(
							*p++) & BIT_TYPE::BIT_MASK) << offset;
					SET_VAL_UNALIGNED_PTR(pSt, GET_VAL_UNALIGNED_PTR(pSt) | V);
					L += N_BIT;
					offset += N_BIT; B = offset >> 3; offset &= 0x07;
					if (B > 0)
					{
						char *ps = (char*)pSt + B;
						pSt = (typename BIT_TYPE::IntTypeEx *)ps;
						LStack += B;
					}
					Len --;
				}

				if (Rec.AppendMode)
				{
					alloc.Write(pPtr >> 3, Stack, LStack);
					if (offset > 0)
					{
						if (vCompressed)
						{
							pSt = (typename BIT_TYPE::IntTypeEx *)(ar->Buf);
							void *tmp = &Stack[LStack];
							SET_VAL_UNALIGNED_PTR(pSt,
								GET_VAL_UNALIGNED_PTR((typename BIT_TYPE::IntTypeEx *)tmp));
							ar->Size = sizeof(*pSt);
						} else
							alloc.Write((pPtr >> 3) + LStack, &Stack[LStack], EXSIZE);
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

		COREARRAY_INLINE static void COREARRAY_FLATTEN wItem(
			void const* InBuffer, SIZE64 p, CdVectorX &Seq)
		{
			typename BIT_TYPE::IntTypeEx I, Val;
			Val = ValCvt<typename BIT_TYPE::IntTypeEx, TOutside>(
				*((TOutside*)InBuffer)) & BIT_TYPE::BIT_MASK;
			p *= N_BIT;
			C_UInt8 B = ((C_UInt8)p) & 0x07;
			p >>= 3;
			_INTERNAL::TdIterMove<sizeof(I)>::Read((void*)&I, Seq.fAllocator, p);
			I = (I & (~(BIT_TYPE::BIT_MASK_EX << B))) | (Val << B);
			_INTERNAL::TdIterMove<sizeof(I)>::Write((void*)&I, Seq.fAllocator, p);
		}
	};



	// --------------------------------------------------------------------

	/// Template specialization for VEC_DATA with Bit1
	template<typename TOutside, int O>
		struct COREARRAY_DLL_DEFAULT VEC_DATA
		<TOutside, Bit1, false, O, COREARRAY_TR_BIT_INTEGER>
	{
		static const unsigned ExtAllocNeed = 0;

		// Read
		static void COREARRAY_FLATTEN rArray(TIterVDataExt &Rec)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();
			// The header
			offset = ((C_UInt8)pPtr) & 0x07;
			pPtr >>= 3;
			if (offset != 0)
			{
				B = alloc.r8(pPtr) >> offset;
				offset = (8-offset < Len) ? (8-offset) : Len;
				Len -= offset; pPtr++;
				for (; offset > 0; offset--)
					{ *p++ = ValCvt<TOutside, C_UInt8>(B & 0x01); B >>= 1; }
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
					*p++ = ValCvt<TOutside, C_UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, C_UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, C_UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, C_UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, C_UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, C_UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, C_UInt8>(B & 0x01); B >>= 1;
					*p++ = ValCvt<TOutside, C_UInt8>(B);
				}
				pPtr += L; Len -= L << 3;
			}

			// The tail
			if (Len > 0)
			{
				B = alloc.r8(pPtr);
				for (ssize_t i = 1; i <= Len; i++)
					{ *p++ = ValCvt<TOutside, C_UInt8>(B & 0x01); B >>= 1; }
			}

			// finally
			Rec.pBuf = (char*)p;
		}

		// Read
		static void COREARRAY_FLATTEN rArrayEx(TIterVDataExt &Rec,
			const C_BOOL *Sel)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();
			// The header
			offset = ((C_UInt8)pPtr) & 0x07;
			pPtr >>= 3;
			if (offset != 0)
			{
				B = alloc.r8(pPtr) >> offset;
				offset = (8-offset < Len) ? (8-offset) : Len;
				Len -= offset; pPtr++;
				for (; offset > 0; offset--)
				{
					if (*Sel++)
						*p++ = ValCvt<TOutside, C_UInt8>(B & 0x01);
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
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B & 0x01);
					B >>= 1;
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B);
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
						*p++ = ValCvt<TOutside, C_UInt8>(B & 0x01);
					B >>= 1;
				}
			}

			// finally
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void COREARRAY_FLATTEN rItem(void *OutBuffer,
			const SIZE64 p, CdVectorX &Seq)
		{
			C_UInt8 B = Seq.fAllocator.r8(p >> 3);
			*((TOutside*)OutBuffer) =
				ValCvt<TOutside, C_UInt8>(
					(B & CoreArray_MaskBit1Array[((C_UInt8)p) & 0x07]) ? 1 : 0);
		}

		// Write
		static void COREARRAY_FLATTEN wArray(TIterVDataExt &Rec)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 B, E, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();
			bool vCompressed = (Rec.Seq->PipeInfo()!=NULL);

			// The header
			offset = ((C_UInt8)pPtr) & 0x07;
			pPtr >>= 3;
			if (offset != 0)
			{
				B = (!vCompressed) ? alloc.r8(pPtr) :
					Rec.Seq->PipeInfo()->Remainder().Buf[0];
				E = (offset+Len-1 < 7) ? (offset+Len-1) : 7;
				for (; offset <= E; offset++, Len--)
				{
					B = (B & CoreArray_MaskBit1ArrayNot[offset]) |
						(((ValCvt<C_UInt8, TOutside>(*p++)) & 0x01) << offset);
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
					B = ValCvt<C_UInt8, TOutside>(*p++) & 0x01;
					B |= (ValCvt<C_UInt8, TOutside>(*p++) & 0x01) << 1;
					B |= (ValCvt<C_UInt8, TOutside>(*p++) & 0x01) << 2;
					B |= (ValCvt<C_UInt8, TOutside>(*p++) & 0x01) << 3;
					B |= (ValCvt<C_UInt8, TOutside>(*p++) & 0x01) << 4;
					B |= (ValCvt<C_UInt8, TOutside>(*p++) & 0x01) << 5;
					B |= (ValCvt<C_UInt8, TOutside>(*p++) & 0x01) << 6;
					B |= (ValCvt<C_UInt8, TOutside>(*p++) & 0x01) << 7;
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
						((ValCvt<C_UInt8, TOutside>(*p++) & 0x01) << offset);
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

		COREARRAY_INLINE static void COREARRAY_FLATTEN wItem(
			void const* InBuffer, SIZE64 p, CdVectorX &Seq)
		{
			unsigned char i = ((C_UInt8)p) & 0x07;
			C_UInt8 B = ValCvt<C_UInt8, TOutside>(*((TOutside*)InBuffer));
			B = B & 0x01; p >>= 3;
			B = (Seq.fAllocator.r8(p) & CoreArray_MaskBit1ArrayNot[i]) | (B << i);
			Seq.fAllocator.w8(p, B);
		}
	};


	// --------------------------------------------------------------------

	/// Template specialization for VEC_DATA with Bit2
	template<typename TOutside, int O>
		struct COREARRAY_DLL_DEFAULT VEC_DATA
		<TOutside, Bit2, false, O, COREARRAY_TR_BIT_INTEGER>
	{
		static const unsigned ExtAllocNeed = 0;

		// Read
		static void COREARRAY_FLATTEN rArray(TIterVDataExt &Rec)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();

			// The header
			offset = ((C_UInt8)pPtr) & 0x03; pPtr >>= 2;
			if (offset != 0)
			{
				B = alloc.r8(pPtr) >> (offset << 1);
				offset = (4-offset < Len) ? (4-offset) : Len;
				Len -= offset; pPtr++;
				for (; offset > 0; offset--)
					{ *p++ = ValCvt<TOutside, C_UInt8>(B & 0x03); B >>= 2; }
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
					*p++ = ValCvt<TOutside, C_UInt8>(B & 0x03); B >>= 2;
					*p++ = ValCvt<TOutside, C_UInt8>(B & 0x03); B >>= 2;
					*p++ = ValCvt<TOutside, C_UInt8>(B & 0x03); B >>= 2;
					*p++ = ValCvt<TOutside, C_UInt8>(B);
				}
				pPtr += L; Len -= L << 2;
			}

			// The tail
			if (Len > 0)
			{
				B = alloc.r8(pPtr);
				for (ssize_t i = 1; i <= Len; i++)
				{
					*p++ = ValCvt<TOutside, C_UInt8>(B & 0x03);
					B >>= 2;
				}
			}

			// finally
			Rec.pBuf = (char*)p;
		}

		// Read
		static void COREARRAY_FLATTEN rArrayEx(TIterVDataExt &Rec,
			const C_BOOL *Sel)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();

			// The header
			offset = ((C_UInt8)pPtr) & 0x03; pPtr >>= 2;
			if (offset != 0)
			{
				B = alloc.r8(pPtr) >> (offset << 1);
				offset = (4-offset < Len) ? (4-offset) : Len;
				Len -= offset; pPtr++;
				for (; offset > 0; offset--)
				{
					if (*Sel++)
						*p++ = ValCvt<TOutside, C_UInt8>(B & 0x03);
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
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B & 0x03);
					B >>= 2;
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B & 0x03);
					B >>= 2;
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B & 0x03);
					B >>= 2;
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B & 0x03);
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
						*p++ = ValCvt<TOutside, C_UInt8>(B & 0x03);
					B >>= 2;
				}
			}

			// finally
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void COREARRAY_FLATTEN rItem(void *OutBuffer,
			const SIZE64 p, CdVectorX &Seq)
		{
			C_UInt8 B = Seq.fAllocator.r8(p >> 2);
			*((TOutside*)OutBuffer) = ValCvt<TOutside, C_UInt8>(
				(B >> ((((C_UInt8)p) & 0x03) << 1)) & 0x03);
		}

		// Write
		static void COREARRAY_FLATTEN wArray(TIterVDataExt &Rec)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 E, offset, *s, B=0;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();
			bool vCompressed = (Rec.Seq->PipeInfo()!=NULL);

			// The header
			offset = ((C_UInt8)pPtr) & 0x03;
			pPtr = pPtr >> 2;
			if ((offset != 0) && (Len > 0))
			{
				B = (!vCompressed) ? alloc.r8(pPtr) :
					Rec.Seq->PipeInfo()->Remainder().Buf[0];
				E = ((offset+Len-1) < 3) ? (offset+Len-1) : 3;
				for (; offset <= E; offset++, Len--)
				{
					B = (B & CoreArray_MaskBit2ArrayNot[offset]) |
						(((ValCvt<C_UInt8, TOutside>(*p++)) & 0x03) << (offset << 1));
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
					B = ValCvt<C_UInt8, TOutside>(*p++) & 0x03;
					B |= (ValCvt<C_UInt8, TOutside>(*p++) & 0x03) << 2;
					B |= (ValCvt<C_UInt8, TOutside>(*p++) & 0x03) << 4;
					B |= (ValCvt<C_UInt8, TOutside>(*p++) & 0x03) << 6;
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
						((ValCvt<C_UInt8, TOutside>(*p++) & 0x03) << (offset << 1));
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

		COREARRAY_INLINE static void COREARRAY_FLATTEN wItem(
			void const* InBuffer, SIZE64 p, CdVectorX &Seq)
		{
			C_UInt8 i = ((C_UInt8)p) & 0x03;
			C_UInt8 B = ValCvt<C_UInt8, TOutside>(*((TOutside*)InBuffer));
			B = B & 0x03; p >>= 2;
			B = (Seq.fAllocator.r8(p) & CoreArray_MaskBit2ArrayNot[i]) | (B << (i << 1));
			Seq.fAllocator.w8(p, B);
		}
	};


	// --------------------------------------------------------------------

	/// Template specialization for VEC_DATA with SBit2
	template<typename TOutside, int O>
		struct COREARRAY_DLL_DEFAULT VEC_DATA
		<TOutside, SBit2, false, O, COREARRAY_TR_BIT_INTEGER>
	{
		static const unsigned ExtAllocNeed = 0;

		// Read
		static void COREARRAY_FLATTEN rArray(TIterVDataExt &Rec)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();

			// The header
			offset = ((C_UInt8)pPtr) & 0x03; pPtr >>= 2;
			if (offset != 0)
			{
				B = alloc.r8(pPtr) >> (offset << 1);
				offset = (4-offset < Len) ? (4-offset) : Len;
				Len -= offset; pPtr++;
				for (; offset > 0; offset--)
					{ *p++ = ValCvt<TOutside, C_Int8>(B & 0x03); B >>= 2; }
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
					*p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit2>(B & 0x03)); B >>= 2;
					*p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit2>(B & 0x03)); B >>= 2;
					*p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit2>(B & 0x03)); B >>= 2;
					*p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit2>(B));
				}
				pPtr += L; Len -= L << 2;
			}

			// The tail
			if (Len > 0)
			{
				B = alloc.r8(pPtr);
				for (ssize_t i = 1; i <= Len; i++)
				{
					*p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit2>(B & 0x03));
					B >>= 2;
				}
			}

			// finally
			Rec.pBuf = (char*)p;
		}

		// Read
		static void COREARRAY_FLATTEN rArrayEx(TIterVDataExt &Rec,
			const C_BOOL *Sel)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();

			// The header
			offset = ((C_UInt8)pPtr) & 0x03; pPtr >>= 2;
			if (offset != 0)
			{
				B = alloc.r8(pPtr) >> (offset << 1);
				offset = (4-offset < Len) ? (4-offset) : Len;
				Len -= offset; pPtr++;
				for (; offset > 0; offset--)
				{
					if (*Sel++)
						*p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit2>(B & 0x03));
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
					if (*Sel++) *p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit2>(B & 0x03));
					B >>= 2;
					if (*Sel++) *p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit2>(B & 0x03));
					B >>= 2;
					if (*Sel++) *p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit2>(B & 0x03));
					B >>= 2;
					if (*Sel++) *p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit2>(B & 0x03));
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
						*p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit2>(B & 0x03));
					B >>= 2;
				}
			}

			// finally
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void COREARRAY_FLATTEN rItem(void *OutBuffer,
			const SIZE64 p, CdVectorX &Seq)
		{
			C_UInt8 B = Seq.fAllocator.r8(p >> 2);
			*((TOutside*)OutBuffer) = ValCvt<TOutside, C_Int8>(
				BITS_ifsign<SBit2>((B >> ((((C_UInt8)p) & 0x03) << 1)) & 0x03));
		}

		// Write
		static void COREARRAY_FLATTEN wArray(TIterVDataExt &Rec)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 E, offset, *s, B=0;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();
			bool vCompressed = (Rec.Seq->PipeInfo()!=NULL);

			// The header
			offset = ((C_UInt8)pPtr) & 0x03;
			pPtr = pPtr >> 2;
			if ((offset != 0) && (Len > 0))
			{
				B = (!vCompressed) ? alloc.r8(pPtr) :
					Rec.Seq->PipeInfo()->Remainder().Buf[0];
				E = ((offset+Len-1) < 3) ? (offset+Len-1) : 3;
				for (; offset <= E; offset++, Len--)
				{
					B = (B & CoreArray_MaskBit2ArrayNot[offset]) |
						(((ValCvt<C_Int8, TOutside>(*p++)) & 0x03) << (offset << 1));
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
					B = ValCvt<C_Int8, TOutside>(*p++) & 0x03;
					B |= (ValCvt<C_Int8, TOutside>(*p++) & 0x03) << 2;
					B |= (ValCvt<C_Int8, TOutside>(*p++) & 0x03) << 4;
					B |= (ValCvt<C_Int8, TOutside>(*p++) & 0x03) << 6;
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
						((ValCvt<C_Int8, TOutside>(*p++) & 0x03) << (offset << 1));
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

		COREARRAY_INLINE static void COREARRAY_FLATTEN wItem(
			void const* InBuffer, SIZE64 p, CdVectorX &Seq)
		{
			C_UInt8 i = ((C_UInt8)p) & 0x03;
			C_UInt8 B = ValCvt<C_Int8, TOutside>(*((TOutside*)InBuffer));
			B = B & 0x03; p >>= 2;
			B = (Seq.fAllocator.r8(p) & CoreArray_MaskBit2ArrayNot[i]) | (B << (i << 1));
			Seq.fAllocator.w8(p, B);
		}
	};


	// --------------------------------------------------------------------

	/// Template specialization for VEC_DATA with Bit4
	template<typename TOutside, int O>
		struct COREARRAY_DLL_DEFAULT VEC_DATA
		<TOutside, Bit4, false, O, COREARRAY_TR_BIT_INTEGER>
	{
		static const unsigned ExtAllocNeed = 0;

		// Read
		static void COREARRAY_FLATTEN rArray(TIterVDataExt &Rec)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();

			// The header
			offset = ((C_UInt8)pPtr) & 0x01;
			pPtr >>= 1;
			if ((offset > 0) && (Len > 0))
			{
				*p++ = ValCvt<TOutside, C_UInt8>(alloc.r8(pPtr) >> 4);
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
					*p++ = ValCvt<TOutside, C_UInt8>(B & 0x0F); B >>= 4;
					*p++ = ValCvt<TOutside, C_UInt8>(B);
				}
				pPtr += L; Len -= L << 1;
			}

			// The tail
			if (Len > 0)
				*p++ = ValCvt<TOutside, C_UInt8>(alloc.r8(pPtr) & 0x0F);

			// finally
			Rec.pBuf = (char*)p;
		}

		// Read
		static void COREARRAY_FLATTEN rArrayEx(TIterVDataExt &Rec,
			const C_BOOL *Sel)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();

			// The header
			offset = ((C_UInt8)pPtr) & 0x01;
			pPtr >>= 1;
			if ((offset > 0) && (Len > 0))
			{
				if (*Sel++)
					*p++ = ValCvt<TOutside, C_UInt8>(alloc.r8(pPtr) >> 4);
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
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B & 0x0F);
					B >>= 4;
					if (*Sel++) *p++ = ValCvt<TOutside, C_UInt8>(B);
				}
				pPtr += L; Len -= L << 1;
			}

			// The tail
			if ((Len > 0) && (*Sel++))
				*p++ = ValCvt<TOutside, C_UInt8>(alloc.r8(pPtr) & 0x0F);

			// finally
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void COREARRAY_FLATTEN rItem(
			void *OutBuffer, const SIZE64 p, CdVectorX &Seq)
		{
			C_UInt8 B = Seq.fAllocator.r8(p >> 1);
			*((TOutside*)OutBuffer) = ValCvt<TOutside, C_UInt8>(
				(((C_UInt8)p) & 0x01) ? (B >> 4) : (B & 0x0F));
		}

		// Write
		static void COREARRAY_FLATTEN wArray(TIterVDataExt &Rec)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 offset, *s, B=0;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();
			bool vCompressed = (Rec.Seq->PipeInfo()!=NULL);

			// The header
			offset = ((C_UInt8)pPtr) & 0x01;
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
					(((ValCvt<C_UInt8, TOutside>(*p++)) & 0x0F) << 4);
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
					B = ValCvt<C_UInt8, TOutside>(*p++) & 0x0F;
					B |= (ValCvt<C_UInt8, TOutside>(*p++) & 0x0F) << 4;
					*s++ = B;
				}
				alloc.Write(pPtr, Stack, L);
				pPtr += L; Len -= L << 1;
			}

			// The tail
			if (Len > 0)
			{
				B = (!Rec.AppendMode) ? alloc.r8(pPtr) : 0;
				B = (B & 0xF0) | (ValCvt<C_UInt8, TOutside>(*p++) & 0x0F);
				if (vCompressed)
				{
					Rec.Seq->PipeInfo()->Remainder().Size = 1u;
					Rec.Seq->PipeInfo()->Remainder().Buf[0] = B;
				} else
					alloc.w8(pPtr, B);
			}

			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void COREARRAY_FLATTEN wItem(
			void const* InBuffer, SIZE64 p, CdVectorX &Seq)
		{
			C_UInt8 i = ((C_UInt8)p) & 0x01;
			C_UInt8 B = ValCvt<C_UInt8, TOutside>(*((TOutside*)InBuffer));
			B = B & 0x0F; p >>= 1;
			B = (Seq.fAllocator.r8(p) & CoreArray_MaskBit4ArrayNot[i]) | (B << (i << 2));
			Seq.fAllocator.w8(p, B);
		}
	};


	// --------------------------------------------------------------------

	/// Template specialization for VEC_DATA with SBit4
	template<typename TOutside, int O>
		struct COREARRAY_DLL_DEFAULT VEC_DATA
		<TOutside, SBit4, false, O, COREARRAY_TR_BIT_INTEGER>
	{
		static const unsigned ExtAllocNeed = 0;

		// Read
		static void COREARRAY_FLATTEN rArray(TIterVDataExt &Rec)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();

			// The header
			offset = ((C_UInt8)pPtr) & 0x01;
			pPtr >>= 1;
			if ((offset > 0) && (Len > 0))
			{
				*p++ = ValCvt<TOutside, C_Int8>(
					BITS_ifsign<SBit4>(alloc.r8(pPtr) >> 4));
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
					*p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit4>(B & 0x0F));
					B >>= 4;
					*p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit4>(B));
				}
				pPtr += L; Len -= L << 1;
			}

			// The tail
			if (Len > 0)
			{
				*p++ = ValCvt<TOutside, C_Int8>(
					BITS_ifsign<SBit4>(alloc.r8(pPtr) & 0x0F));
			}

			// finally
			Rec.pBuf = (char*)p;
		}

		// Read
		static void COREARRAY_FLATTEN rArrayEx(TIterVDataExt &Rec,
			const C_BOOL *Sel)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 B, offset, *s;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();

			// The header
			offset = ((C_UInt8)pPtr) & 0x01;
			pPtr >>= 1;
			if ((offset > 0) && (Len > 0))
			{
				if (*Sel++)
				{
					*p++ = ValCvt<TOutside, C_Int8>(
						BITS_ifsign<SBit4>(alloc.r8(pPtr) >> 4));
				}
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
					if (*Sel++) *p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit4>(B & 0x0F));
					B >>= 4;
					if (*Sel++) *p++ = ValCvt<TOutside, C_Int8>(BITS_ifsign<SBit4>(B));
				}
				pPtr += L; Len -= L << 1;
			}

			// The tail
			if ((Len > 0) && (*Sel++))
			{
				*p++ = ValCvt<TOutside, C_Int8>(
					BITS_ifsign<SBit4>(alloc.r8(pPtr) & 0x0F));
			}

			// finally
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void COREARRAY_FLATTEN rItem(void *OutBuffer,
			const SIZE64 p, CdVectorX &Seq)
		{
			C_UInt8 B = Seq.fAllocator.r8(p >> 1);
			*((TOutside*)OutBuffer) = ValCvt<TOutside, C_Int8>(
				BITS_ifsign<SBit4>((((C_UInt8)p) & 0x01) ? (B >> 4) : (B & 0x0F)));
		}

		// Write
		static void COREARRAY_FLATTEN wArray(TIterVDataExt &Rec)
		{
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt8 offset, *s, B=0;

			// Initialize
			TOutside *p = (TOutside*)Rec.pBuf;
			ssize_t Len = Rec.LastDim;
			SIZE64 pPtr = Rec.p64;
			TdAllocator &alloc = Rec.Seq->Allocator();
			bool vCompressed = (Rec.Seq->PipeInfo()!=NULL);

			// The header
			offset = ((C_UInt8)pPtr) & 0x01;
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
					(((ValCvt<C_Int8, TOutside>(*p++)) & 0x0F) << 4);
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
					B = ValCvt<C_Int8, TOutside>(*p++) & 0x0F;
					B |= (ValCvt<C_Int8, TOutside>(*p++) & 0x0F) << 4;
					*s++ = B;
				}
				alloc.Write(pPtr, Stack, L);
				pPtr += L; Len -= L << 1;
			}

			// The tail
			if (Len > 0)
			{
				B = (!Rec.AppendMode) ? alloc.r8(pPtr) : 0;
				B = (B & 0xF0) | (ValCvt<C_Int8, TOutside>(*p++) & 0x0F);
				if (vCompressed)
				{
					Rec.Seq->PipeInfo()->Remainder().Size = 1u;
					Rec.Seq->PipeInfo()->Remainder().Buf[0] = B;
				} else
					alloc.w8(pPtr, B);
			}

			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void COREARRAY_FLATTEN wItem(
			void const* InBuffer, SIZE64 p, CdVectorX &Seq)
		{
			C_UInt8 i = ((C_UInt8)p) & 0x01;
			C_UInt8 B = ValCvt<C_Int8, TOutside>(*((TOutside*)InBuffer));
			B = B & 0x0F; p >>= 1;
			B = (Seq.fAllocator.r8(p) & CoreArray_MaskBit4ArrayNot[i]) | (B << (i << 2));
			Seq.fAllocator.w8(p, B);
		}
	};


	// --------------------------------------------------------------------

	/// Template specialization for VEC_DATA with Bit24
	template<typename TOutside, int O>
		struct COREARRAY_DLL_DEFAULT VEC_DATA
		<TOutside, Bit24, false, O, COREARRAY_TR_BIT_INTEGER>
	{
		// Read
		static void COREARRAY_FLATTEN rArray(TIterVDataExt &Rec)
		{
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE_PLUS];
			C_Int32 Len = Rec.LastDim;
			TOutside *p = (TOutside*)Rec.pBuf;
			while (Len > 0)
			{
				ssize_t L = ((size_t)Len >= (MEMORY_BUFFER_SIZE/3)) ? (MEMORY_BUFFER_SIZE/3) : Len;
				ssize_t Lx = L * 3;
				Len -= L;
				Rec.Seq->Allocator().Read(Rec.p64, Buffer, Lx);
				Rec.p64 += Lx;
				C_UInt8 *pc = Buffer;
				for (; L > 0; L--)
				{
					*p++ = ValCvt<TOutside, C_UInt32>(C_UInt32(pc[0]) |
						(C_UInt32(pc[1]) << 8) | (C_UInt32(pc[2]) << 16));
					pc += 3;
				}
			}
			Rec.pBuf = (char*)p;
		}

		// Read
		static void COREARRAY_FLATTEN rArrayEx(TIterVDataExt &Rec,
			const C_BOOL *Sel)
		{
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE_PLUS];
			C_Int32 Len = Rec.LastDim;
			TOutside *p = (TOutside*)Rec.pBuf;
			while (Len > 0)
			{
				ssize_t L = (Len >= (MEMORY_BUFFER_SIZE/3)) ? (MEMORY_BUFFER_SIZE/3) : Len;
				ssize_t Lx = L * 3;
				Len -= L;
				Rec.Seq->Allocator().Read(Rec.p64, Buffer, Lx);
				Rec.p64 += Lx;
				C_UInt8 *pc = Buffer;
				for (; L > 0; L--)
				{
					if (*Sel++)
					{
						*p++ = ValCvt<TOutside, C_UInt32>(C_UInt32(pc[0]) |
							(C_UInt32(pc[1]) << 8) | (C_UInt32(pc[2]) << 16));
                    }
					pc += 3;
				}
			}
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void COREARRAY_FLATTEN rItem(void *OutBuffer,
			const SIZE64 p, CdVectorX &Seq)
		{
			C_UInt32 val = 0;
			Seq.fAllocator.Read(p, (void*)&val, 3);
			*((TOutside*)OutBuffer) = ValCvt<TOutside, C_UInt32>(
				COREARRAY_ENDIAN_CVT_I32(val));
		}

		// Write
		static void COREARRAY_FLATTEN wArray(TIterVDataExt &Rec)
		{
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE];
			SIZE64 p64 = Rec.p64;
			C_Int32 Len = Rec.LastDim;
			ssize_t L, Lx;
			TOutside *s = (TOutside*)Rec.pBuf;
			while (Len > 0)
			{
				L = (Len >= (MEMORY_BUFFER_SIZE/3)) ? (MEMORY_BUFFER_SIZE/3) : Len;
				Len -= L; Lx = L * 3;
				C_UInt8 *pc = Buffer;
				for (; L > 0; L--)
				{
					C_UInt32 i = ValCvt<C_UInt32, TOutside>(*s++);
					*pc++ = i & 0xFF; *pc++ = (i >> 8) & 0xFF; *pc++ = (i >> 16) & 0xFF;
				}
				Rec.Seq->Allocator().Write(p64, Buffer, Lx);
				p64 += Lx;
			}
			Rec.pBuf = (char*)s;
		}

		COREARRAY_INLINE static void COREARRAY_FLATTEN wItem(
			void const* InBuffer, const SIZE64 p, CdVectorX &Seq)
		{
			C_UInt32 i = ValCvt<C_UInt32, TOutside>(*((TOutside*)InBuffer));
			C_UInt32 val = COREARRAY_ENDIAN_CVT_I32(i);
			Seq.fAllocator.Write(p, (void*)&val, 3);
		}
	};


	// --------------------------------------------------------------------

	/// Template specialization for VEC_DATA with SBit24
	template<typename TOutside, int O>
		struct COREARRAY_DLL_DEFAULT VEC_DATA
		<TOutside, SBit24, false, O, COREARRAY_TR_BIT_INTEGER>
	{
		// Read
		static void COREARRAY_FLATTEN rArray(TIterVDataExt &Rec)
		{
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE_PLUS];
			C_Int32 Len = Rec.LastDim;
			TOutside *p = (TOutside*)Rec.pBuf;
			while (Len > 0)
			{
				ssize_t L = (Len >= (MEMORY_BUFFER_SIZE/3)) ? (MEMORY_BUFFER_SIZE/3) : Len;
				ssize_t Lx = L * 3;
				Len -= L;
				Rec.Seq->Allocator().Read(Rec.p64, Buffer, Lx);
				Rec.p64 += Lx;
				C_UInt8 *pc = Buffer;
				for (; L > 0; L--)
				{
					*p++ = ValCvt<TOutside, C_Int32>(BITS_ifsign<SBit24>(
						C_UInt32(pc[0]) | (C_UInt32(pc[1]) << 8) |
						(C_UInt32(pc[2]) << 16)));
					pc += 3;
				}
			}
			Rec.pBuf = (char*)p;
		}

		// Read
		static void COREARRAY_FLATTEN rArrayEx(TIterVDataExt &Rec,
			const C_BOOL *Sel)
		{
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE_PLUS];
			C_Int32 Len = Rec.LastDim;
			TOutside *p = (TOutside*)Rec.pBuf;
			while (Len > 0)
			{
				ssize_t L = (Len >= (MEMORY_BUFFER_SIZE/3)) ? (MEMORY_BUFFER_SIZE/3) : Len;
				ssize_t Lx = L * 3;
				Len -= L;
				Rec.Seq->Allocator().Read(Rec.p64, Buffer, Lx);
				Rec.p64 += Lx;
				C_UInt8 *pc = Buffer;
				for (; L > 0; L--)
				{
					if (*Sel++)
					{
						*p++ = ValCvt<TOutside, C_Int32>(BITS_ifsign<SBit24>(
							C_UInt32(pc[0]) | (C_UInt32(pc[1]) << 8) |
							(C_UInt32(pc[2]) << 16)));
                    }
					pc += 3;
				}
			}
			Rec.pBuf = (char*)p;
		}

		COREARRAY_INLINE static void COREARRAY_FLATTEN rItem(void *OutBuffer,
			const SIZE64 p, CdVectorX &Seq)
		{
			C_UInt32 val = 0;
			Seq.fAllocator.Read(p, (void*)&val, 3);
			*((TOutside*)OutBuffer) = ValCvt<TOutside, C_Int32>(
				BITS_ifsign<SBit24>(COREARRAY_ENDIAN_CVT_I32(val)));
		}

		// Write
		static void COREARRAY_FLATTEN wArray(TIterVDataExt &Rec)
		{
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE];
			C_Int32 Len = Rec.LastDim;
			TOutside *s = (TOutside*)Rec.pBuf;
			while (Len > 0)
			{
				ssize_t L = (Len >= (MEMORY_BUFFER_SIZE/3)) ? (MEMORY_BUFFER_SIZE/3) : Len;
				ssize_t Lx = L * 3;
				Len -= L;
				C_UInt8 *pc = Buffer;
				for (; L > 0; L--)
				{
					C_Int32 i = ValCvt<C_Int32, TOutside>(*s++);
					*pc++ = i & 0xFF; *pc++ = (i >> 8) & 0xFF; *pc++ = (i >> 16) & 0xFF;
				}
				Rec.Seq->Allocator().Write(Rec.p64, Buffer, Lx);
				Rec.p64 += Lx;
			}
			Rec.pBuf = (char*)s;
		}

		COREARRAY_INLINE static void COREARRAY_FLATTEN wItem(
			void const* InBuffer, const SIZE64 p, CdVectorX &Seq)
		{
			C_Int32 i = ValCvt<C_Int32, TOutside>(*((TOutside*)InBuffer));
			C_UInt32 val = COREARRAY_ENDIAN_CVT_I32(i);
			Seq.fAllocator.Write(p, (void*)&val, 3);
		}
	};



	// --------------------------------------------------------------------

	// unsigned integer

	typedef CdBaseBit<Bit1>       CdBit1; // *
	typedef CdBaseBit<Bit2>       CdBit2; // *
	typedef CdBaseBit<Bit3>       CdBit3;
	typedef CdBaseBit<Bit4>       CdBit4; // *
	typedef CdBaseBit<Bit5>       CdBit5;
	typedef CdBaseBit<Bit6>       CdBit6;
	typedef CdBaseBit<Bit7>       CdBit7;
	typedef CdUInt8               CdBit8; // *

	typedef CdBaseBit<Bit9>       CdBit9;
	typedef CdBaseBit<Bit10>      CdBit10;
	typedef CdBaseBit<Bit11>      CdBit11;
	typedef CdBaseBit<Bit12>      CdBit12;
	typedef CdBaseBit<Bit13>      CdBit13;
	typedef CdBaseBit<Bit14>      CdBit14;
	typedef CdBaseBit<Bit15>      CdBit15;
	typedef CdUInt16              CdBit16; // *

	typedef CdVector<UInt24>      CdBit24; // *
	typedef CdUInt32              CdBit32; // *
	typedef CdUInt64              CdBit64; // *

	// signed integer

	typedef CdBaseBit<SBit2>      CdSBit2; // *
	typedef CdBaseBit<SBit3>      CdSBit3;
	typedef CdBaseBit<SBit4>      CdSBit4; // *
	typedef CdBaseBit<SBit5>      CdSBit5;
	typedef CdBaseBit<SBit6>      CdSBit6;
	typedef CdBaseBit<SBit7>      CdSBit7;
	typedef CdInt8                CdSBit8; // *

	typedef CdBaseBit<SBit9>      CdSBit9;
	typedef CdBaseBit<SBit10>     CdSBit10;
	typedef CdBaseBit<SBit11>     CdSBit11;
	typedef CdBaseBit<SBit12>     CdSBit12;
	typedef CdBaseBit<SBit13>     CdSBit13;
	typedef CdBaseBit<SBit14>     CdSBit14;
	typedef CdBaseBit<SBit15>     CdSBit15;
	typedef CdInt16               CdSBit16; // *

	typedef CdVector<Int24>       CdSBit24; // *
	typedef CdInt32               CdSBit32; // *
	typedef CdInt64               CdSBit64; // *
}

#endif /* _HEADER_COREARRAY_BIT_GDS_ */
