// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dBitGDS_Bit4.h: Bit operators and classes of GDS format for Bit4
//
// Copyright (C) 2007-2017    Xiuwen Zheng
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
 *	\file     dBitGDS_Bit4.h
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2007 - 2017
 *	\brief    Bit operators and classes of GDS format for Bit4
 *	\details
**/

#ifndef _HEADER_COREARRAY_BIT4_GDS_
#define _HEADER_COREARRAY_BIT4_GDS_

namespace CoreArray
{
	// =====================================================================
	// 4-bit unsigned integer functions for allocator

	/// template for allocate function for 4-bit integer
	/** in the case that MEM_TYPE is numeric **/
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<BIT4, MEM_TYPE>
	{
		/// integer type
		typedef C_UInt8 IntType;
		/// the number of bits
		static const unsigned N_BIT = 4u;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			// buffer
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE];
			SIZE64 pI = I.Ptr;
			I.Ptr += n;

			// header
			I.Allocator->SetPosition(pI >> 1);
			if (pI & 0x01)
			{
				*p++ = VAL_CONV_FROM_U8(MEM_TYPE, I.Allocator->R8b() >> 4);
				n --;
			}

			// body
			while (n >= 2)
			{
				// read buffer
				ssize_t L = (n >> 1);
				if (L > MEMORY_BUFFER_SIZE) L = MEMORY_BUFFER_SIZE;
				I.Allocator->ReadData(Buffer, L);
				n -= (L << 1);
				// extract bits
				C_UInt8 *s = Buffer;
				for (; L > 0; L--)
				{
					C_UInt8 Ch = *s++;
					p[0] = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x0F);
					p[1] = VAL_CONV_FROM_U8(MEM_TYPE, Ch >> 4);
					p += 2;
				}
			}

			// tail
			if (n > 0)
				*p++ = VAL_CONV_FROM_U8(MEM_TYPE, I.Allocator->R8b() & 0x0F);

			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			// buffer
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE];
			SIZE64 pI = I.Ptr;
			I.Ptr += n;

			// header
			I.Allocator->SetPosition(pI >> 1);
			if (pI & 0x01)
			{
				C_UInt8 Ch = I.Allocator->R8b();
				if (*sel++)
					*p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch >> 4);
				n --;
			}

			// body
			while (n >= 2)
			{
				// read buffer
				ssize_t L = (n >> 1);
				if (L > MEMORY_BUFFER_SIZE) L = MEMORY_BUFFER_SIZE;
				I.Allocator->ReadData(Buffer, L);
				n -= (L << 1);
				// extract bits
				C_UInt8 *s = Buffer;
				for (; L > 0; L--)
				{
					C_UInt8 Ch = *s++;
					if (*sel++)
						*p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x0F);
					if (*sel++)
						*p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch >> 4);
				}
			}

			// tail
			if (n > 0)
			{
				C_UInt8 Ch = I.Allocator->R8b();
				if (*sel++)
					*p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x0F);
			}

			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			// initialize
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
				ss.WriteBit(VAL_CONV_TO_U8(MEM_TYPE, *p++), N_BIT);
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
		static const MEM_TYPE *Append(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			// compression extended info
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

			if (ss.Offset && (n > 0))
			{
				ss.WriteBit(VAL_CONV_TO_U8(MEM_TYPE, *p++), N_BIT);
				n --;
			}

			// buffer writing with bytes
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE];
			while (n >= 2)
			{
				ssize_t nn = 0;
				for (C_UInt8 *s=Buffer; (n >= 2) && (nn < MEMORY_BUFFER_SIZE); n-=2, nn++)
				{
					*s++ = (VAL_CONV_TO_U8(MEM_TYPE, p[0]) & 0x0F) |
						((VAL_CONV_TO_U8(MEM_TYPE, p[1]) & 0x0F) << 4);
					p += 2;
				}
				I.Allocator->WriteData(Buffer, nn);
			}

			for (; n > 0; n--)
				ss.WriteBit(VAL_CONV_TO_U8(MEM_TYPE, *p++), N_BIT);
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

#endif /* _HEADER_COREARRAY_BIT4_GDS_ */
