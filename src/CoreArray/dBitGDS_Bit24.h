// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dBitGDS_Bit24.h: Bit operators and classes of GDS format for Bit24/SBit24
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
 *	\file     dBitGDS_Bit24.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2016
 *	\brief    Bit operators and classes of GDS format for Bit24/SBit24
 *	\details
**/

#ifndef _HEADER_COREARRAY_BIT24_GDS_
#define _HEADER_COREARRAY_BIT24_GDS_

namespace CoreArray
{
	// =====================================================================
	// 24-bit unsigned integer functions for allocator

	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<BIT24, MEM_TYPE>
	{
		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *Buffer, ssize_t n)
		{
			const ssize_t NMAX = MEMORY_BUFFER_SIZE / sizeof(C_UInt32);
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt32 IntBit[NMAX];
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n*3;

			while (n > 0)
			{
				// read buffer
				ssize_t M = (n <= NMAX) ? n : NMAX;
				I.Allocator->ReadData(Stack, M*3);
				n -= M;
				C_UInt8 *s = Stack;
				C_UInt32 *p = IntBit;
				for (ssize_t m=M; m > 0; m--)
				{
					*p++ = GET_VAL_UNALIGNED_LE_PTR((C_UInt32*)s) & 0xFFFFFF;
					s += 3;
				}
				Buffer = VAL_CONV<MEM_TYPE, C_UInt32>::Cvt(Buffer, IntBit, M);
			}

			return Buffer;
		}

		/// read an array from CdAllocator
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *Buffer, ssize_t n,
			const C_BOOL sel[])
		{
			const ssize_t NMAX = MEMORY_BUFFER_SIZE / sizeof(C_UInt32);
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_UInt32 IntBit[NMAX];
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n*3;

			while (n > 0)
			{
				// read buffer
				ssize_t M = (n <= NMAX) ? n : NMAX;
				I.Allocator->ReadData(Stack, M*3);
				n -= M;
				C_UInt8 *s = Stack;
				C_UInt32 *p = IntBit;
				for (ssize_t m=M; m > 0; m--)
				{
					*p++ = GET_VAL_UNALIGNED_LE_PTR((C_UInt32*)s) & 0xFFFFFF;
					s += 3;
				}
				Buffer = VAL_CONV<MEM_TYPE, C_UInt32>::CvtSub(
					Buffer, IntBit, M, sel);
				sel += M;
			}

			return Buffer;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *Buffer,
			ssize_t n)
		{
			const ssize_t NMAX = MEMORY_BUFFER_SIZE / sizeof(C_UInt32);
			C_UInt32 IntBit[NMAX];
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n*3;

			while (n > 0)
			{
				ssize_t M = (n <= NMAX) ? n : NMAX;
				VAL_CONV<C_UInt32, MEM_TYPE>::Cvt(IntBit, Buffer, M);
				Buffer += M;
				n -= M;
				for (C_UInt32 *p=IntBit; M > 0; M--)
				{
					C_UInt32 D = *p++;
					C_UInt8 s[3] =
						{ C_UInt8(D), C_UInt8(D >> 8), C_UInt8(D >> 16) };
					I.Allocator->WriteData(s, 3);
				}
			}

			return Buffer;
		}
	};



	// =====================================================================
	// 24-bit signed integer functions for allocator

	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<SBIT24, MEM_TYPE>
	{
		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *Buffer, ssize_t n)
		{
			const ssize_t NMAX = MEMORY_BUFFER_SIZE / sizeof(C_UInt32);
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_Int32 IntBit[NMAX];
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n*3;

			while (n > 0)
			{
				// read buffer
				ssize_t M = (n <= NMAX) ? n : NMAX;
				I.Allocator->ReadData(Stack, M*3);
				n -= M;
				C_UInt8 *s = Stack;
				C_Int32 *p = IntBit;
				for (ssize_t m=M; m > 0; m--)
				{
					*p++ = BITS_ifsign<SBIT24>(
						GET_VAL_UNALIGNED_LE_PTR((C_UInt32*)s) & 0xFFFFFF);
					s += 3;
				}
				Buffer = VAL_CONV<MEM_TYPE, C_Int32>::Cvt(Buffer, IntBit, M);
			}

			return Buffer;
		}

		/// read an array from CdAllocator
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *Buffer, ssize_t n,
			const C_BOOL sel[])
		{
			const ssize_t NMAX = MEMORY_BUFFER_SIZE / sizeof(C_UInt32);
			C_UInt8 Stack[MEMORY_BUFFER_SIZE];
			C_Int32 IntBit[NMAX];
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n*3;

			while (n > 0)
			{
				// read buffer
				ssize_t M = (n <= NMAX) ? n : NMAX;
				I.Allocator->ReadData(Stack, M*3);
				n -= M;
				C_UInt8 *s = Stack;
				C_Int32 *p = IntBit;
				for (ssize_t m=M; m > 0; m--)
				{
					*p++ = BITS_ifsign<SBIT24>(
						GET_VAL_UNALIGNED_LE_PTR((C_UInt32*)s) & 0xFFFFFF);
					s += 3;
				}
				Buffer = VAL_CONV<MEM_TYPE, C_Int32>::CvtSub(
					Buffer, IntBit, M, sel);
				sel += M;
			}

			return Buffer;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *Buffer,
			ssize_t n)
		{
			const ssize_t NMAX = MEMORY_BUFFER_SIZE / sizeof(C_UInt32);
			C_Int32 IntBit[NMAX];
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n*3;

			while (n > 0)
			{
				ssize_t M = (n <= NMAX) ? n : NMAX;
				VAL_CONV<C_Int32, MEM_TYPE>::Cvt(IntBit, Buffer, M);
				Buffer += M;
				n -= M;
				for (C_Int32 *p=IntBit; M > 0; M--)
				{
					C_UInt32 D = *p++;
					C_UInt8 s[3] =
						{ C_UInt8(D), C_UInt8(D >> 8), C_UInt8(D >> 16) };
					I.Allocator->WriteData(s, 3);
				}
			}

			return Buffer;
		}
	};
}

#endif /* _HEADER_COREARRAY_BIT24_GDS_ */
