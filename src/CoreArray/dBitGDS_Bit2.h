// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dBitGDS_Bit2.h: Bit operators and classes of GDS format for Bit2
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
 *	\file     dBitGDS_Bit2.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2016
 *	\brief    Bit operators and classes of GDS format for Bit2
 *	\details
**/

#ifndef _HEADER_COREARRAY_BIT2_GDS_
#define _HEADER_COREARRAY_BIT2_GDS_

namespace CoreArray
{
	/// Template for the conversion of 2-bit array
	template<typename MEM_TYPE> struct COREARRAY_DLL_LOCAL BIT2_CONV
	{
		inline static MEM_TYPE* Conv(const C_UInt8 *s, size_t n, MEM_TYPE *p)
		{
			for (; n > 0; n--)
			{
				C_UInt8 Ch = *s++;
				p[0] = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x03);
				p[1] = VAL_CONV_FROM_U8(MEM_TYPE, (Ch >> 2) & 0x03);
				p[2] = VAL_CONV_FROM_U8(MEM_TYPE, (Ch >> 4) & 0x03);
				p[3] = VAL_CONV_FROM_U8(MEM_TYPE, Ch >> 6);
				p += 4;
			}
			return p;
		}

		inline static MEM_TYPE* Conv2(const C_UInt8 *s, size_t n, MEM_TYPE *p,
			const C_BOOL sel[])
		{
			for (; n > 0; n--)
			{
				C_UInt8 Ch = *s++;
				if (*sel++) *p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x03);
				Ch >>= 2;
				if (*sel++) *p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x03);
				Ch >>= 2;
				if (*sel++) *p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x03);
				Ch >>= 2;
				if (*sel++) *p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch);
			}
			return p;
		}
	};


#ifdef COREARRAY_SIMD_SSE2

	#define WRITE_BIT2_DECODE    \
		{ \
			C_UInt8 Ch = *s++; \
			if (*sel++) *p++ = Ch & 0x03; \
			Ch >>= 2; if (*sel++) *p++ = Ch & 0x03; \
			Ch >>= 2; if (*sel++) *p++ = Ch & 0x03; \
			Ch >>= 2; if (*sel++) *p++ = Ch; \
		}

	static const __m128i BIT2_REP_x03 = _mm_set1_epi8(0x03);
	static const __m128i BIT2_x03 = _mm_set1_epi32(0x00000003);


	template<> struct COREARRAY_DLL_LOCAL BIT2_CONV<C_UInt8>
	{
		inline static C_UInt8* Conv(const C_UInt8 *s, size_t n, C_UInt8 *p)
		{
			for (; n >= 16; n-=16)
			{
				__m128i v = _mm_loadu_si128((__m128i const*)s);
				s += 16;
				__m128i v1 = v & BIT2_REP_x03;
				__m128i v2 = _mm_srli_epi32(v, 2) & BIT2_REP_x03;
				__m128i v3 = _mm_srli_epi32(v, 4) & BIT2_REP_x03;
				__m128i v4 = _mm_srli_epi32(v, 6) & BIT2_REP_x03;

				__m128i w1 = _mm_unpacklo_epi8(v1, v2);
				__m128i w2 = _mm_unpacklo_epi8(v3, v4);
				_mm_storeu_si128((__m128i*)p, _mm_unpacklo_epi16(w1, w2));
				p += 16;
				_mm_storeu_si128((__m128i*)p, _mm_unpackhi_epi16(w1, w2));
				p += 16;

				w1 = _mm_unpackhi_epi8(v1, v2);
				w2 = _mm_unpackhi_epi8(v3, v4);
				_mm_storeu_si128((__m128i*)p, _mm_unpacklo_epi16(w1, w2));
				p += 16;
				_mm_storeu_si128((__m128i*)p, _mm_unpackhi_epi16(w1, w2));
				p += 16;
			}
			for (; n >= 4; n-=4)
			{
				__m128i v = _mm_set1_epi32(*((const int*)s));
				s += 4;
				__m128i v1 = v & BIT2_REP_x03;
				__m128i v2 = _mm_srli_epi32(v, 2) & BIT2_REP_x03;
				__m128i v3 = _mm_srli_epi32(v, 4) & BIT2_REP_x03;
				__m128i v4 = _mm_srli_epi32(v, 6) & BIT2_REP_x03;
				__m128i w1 = _mm_unpacklo_epi8(v1, v2);
				__m128i w2 = _mm_unpacklo_epi8(v3, v4);
				_mm_storeu_si128((__m128i*)p, _mm_unpacklo_epi16(w1, w2));
				p += 16;
			}
			for (; n > 0; n--)
			{
				C_UInt8 Ch = *s++;
				p[0] = (Ch & 0x03); p[1] = (Ch >> 2) & 0x03;
				p[2] = (Ch >> 4) & 0x03; p[3] = (Ch >> 6);
				p += 4;
			}
			return p;
		}

		inline static C_UInt8* Conv2(const C_UInt8 *s, size_t n, C_UInt8 *p,
			const C_BOOL sel[])
		{
			for (; n >= 4; n -= 4)
			{
				__m128i sv = _mm_loadu_si128((__m128i const*)sel);
				sv = _mm_cmpeq_epi8(sv, _mm_setzero_si128());
				int sv16 = _mm_movemask_epi8(sv);
				if (sv16 == 0)
				{
					__m128i v = _mm_set1_epi32(*((const int*)s));
					s += 4;
					__m128i v1 = v & BIT2_REP_x03;
					__m128i v2 = _mm_srli_epi32(v, 2) & BIT2_REP_x03;
					__m128i v3 = _mm_srli_epi32(v, 4) & BIT2_REP_x03;
					__m128i v4 = _mm_srli_epi32(v, 6) & BIT2_REP_x03;
					__m128i w1 = _mm_unpacklo_epi8(v1, v2);
					__m128i w2 = _mm_unpacklo_epi8(v3, v4);
					_mm_storeu_si128((__m128i*)p, _mm_unpacklo_epi16(w1, w2));
					p += 16; sel += 16;
				} else if (sv16 == 0xFFFF)
				{
					s += 4; sel += 16;
				} else {
					WRITE_BIT2_DECODE
					WRITE_BIT2_DECODE
					WRITE_BIT2_DECODE
					WRITE_BIT2_DECODE
				}
			}
			for (; n > 0; n--) WRITE_BIT2_DECODE
			return p;
		}
	};


	template<> struct COREARRAY_DLL_LOCAL BIT2_CONV<C_Int8>
	{
		inline static C_Int8* Conv(const C_UInt8 *s, size_t n, C_Int8 *p)
		{
			return (C_Int8*)BIT2_CONV<C_UInt8>::Conv(s, n, (C_UInt8*)p);
		}

		inline static C_Int8* Conv2(const C_UInt8 *s, size_t n, C_Int8 *p,
			const C_BOOL sel[])
		{
			return (C_Int8*)BIT2_CONV<C_UInt8>::Conv2(s, n, (C_UInt8*)p, sel);
		}
	};


	template<> struct COREARRAY_DLL_LOCAL BIT2_CONV<C_Int32>
	{
		inline static C_Int32* Conv(const C_UInt8 *s, size_t n, C_Int32 *p)
		{
			for (; n >= 4; n-=4)
			{
				__m128i v = _mm_set1_epi32(*((const int*)s));
				s += 4;
				__m128i zero = _mm_setzero_si128();
				v = _mm_unpackhi_epi16(_mm_unpacklo_epi8(v, zero), zero);

				__m128i v1 = v & BIT2_x03;
				__m128i v2 = _mm_srli_epi32(v, 2) & BIT2_x03;
				__m128i v3 = _mm_srli_epi32(v, 4) & BIT2_x03;
				__m128i v4 = _mm_srli_epi32(v, 6);

				__m128i w1 = _mm_unpacklo_epi32(v1, v2);
				__m128i w2 = _mm_unpacklo_epi32(v3, v4);
				_mm_storeu_si128((__m128i*)p, _mm_unpacklo_epi64(w1, w2));
				p += 4;
				_mm_storeu_si128((__m128i*)p, _mm_unpackhi_epi64(w1, w2));
				p += 4;

				w1 = _mm_unpackhi_epi32(v1, v2);
				w2 = _mm_unpackhi_epi32(v3, v4);
				_mm_storeu_si128((__m128i*)p, _mm_unpacklo_epi64(w1, w2));
				p += 4;
				_mm_storeu_si128((__m128i*)p, _mm_unpackhi_epi64(w1, w2));
				p += 4;
			}
			for (; n > 0; n--)
			{
				C_UInt8 Ch = *s++;
				p[0] = (Ch & 0x03); p[1] = (Ch >> 2) & 0x03;
				p[2] = (Ch >> 4) & 0x03; p[3] = (Ch >> 6);
				p += 4;
			}
			return p;
		}

		inline static C_Int32* Conv2(const C_UInt8 *s, size_t n, C_Int32 *p,
			const C_BOOL sel[])
		{
			for (; n >= 4; n -= 4)
			{
				__m128i sv = _mm_loadu_si128((__m128i const*)sel);
				sv = _mm_cmpeq_epi8(sv, _mm_setzero_si128());
				int sv16 = _mm_movemask_epi8(sv);
				if (sv16 == 0)
				{
					sel += 16;
					__m128i v = _mm_set1_epi32(*((const int*)s));
					s += 4;
					__m128i zero = _mm_setzero_si128();
					v = _mm_unpackhi_epi16(_mm_unpacklo_epi8(v, zero), zero);

					__m128i v1 = v & BIT2_x03;
					__m128i v2 = _mm_srli_epi32(v, 2) & BIT2_x03;
					__m128i v3 = _mm_srli_epi32(v, 4) & BIT2_x03;
					__m128i v4 = _mm_srli_epi32(v, 6);

					__m128i w1 = _mm_unpacklo_epi32(v1, v2);
					__m128i w2 = _mm_unpacklo_epi32(v3, v4);
					_mm_storeu_si128((__m128i*)p, _mm_unpacklo_epi64(w1, w2));
					p += 4;
					_mm_storeu_si128((__m128i*)p, _mm_unpackhi_epi64(w1, w2));
					p += 4;

					w1 = _mm_unpackhi_epi32(v1, v2);
					w2 = _mm_unpackhi_epi32(v3, v4);
					_mm_storeu_si128((__m128i*)p, _mm_unpacklo_epi64(w1, w2));
					p += 4;
					_mm_storeu_si128((__m128i*)p, _mm_unpackhi_epi64(w1, w2));
					p += 4;
				} else if (sv16 == 0xFFFF)
				{
					s += 4; sel += 16;
				} else {
					WRITE_BIT2_DECODE
					WRITE_BIT2_DECODE
					WRITE_BIT2_DECODE
					WRITE_BIT2_DECODE
				}
			}
			for (; n > 0; n--) WRITE_BIT2_DECODE
			return p;
		}
	};


	template<> struct COREARRAY_DLL_LOCAL BIT2_CONV<C_UInt32>
	{
		inline static C_UInt32* Conv(const C_UInt8 *s, size_t n, C_UInt32 *p)
		{
			return (C_UInt32*)BIT2_CONV<C_Int32>::Conv(s, n, (C_Int32*)p);
		}

		inline static C_UInt32* Conv2(const C_UInt8 *s, size_t n, C_UInt32 *p,
			const C_BOOL sel[])
		{
			return (C_UInt32*)BIT2_CONV<C_Int32>::Conv2(s, n, (C_Int32*)p, sel);
		}
	};

#endif


	// =====================================================================
	// 2-bit unsigned integer functions for allocator

	/// Template for allocate function for 2-bit integer
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<BIT2, MEM_TYPE>
	{
		/// the number of bits
		static const unsigned N_BIT = 2u;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			// buffer
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE];
			SIZE64 pI = I.Ptr << 1;
			I.Ptr += n;

			// header
			I.Allocator->SetPosition(pI >> 3);
			C_UInt8 offset = (pI & 0x07);
			if (offset > 0)
			{
				C_UInt8 Ch = I.Allocator->R8b() >> offset;
				ssize_t m = (8 - offset) >> 1;
				if (m > n) m = n;
				n -= m;
				for (; m > 0; m--, Ch >>= 2)
					*p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x03);
			}

			// body
			while (n >= 4)
			{
				// read buffer
				ssize_t L = (n >> 2);
				if (L > MEMORY_BUFFER_SIZE) L = MEMORY_BUFFER_SIZE;
				I.Allocator->ReadData(Buffer, L);
				n -= (L << 2);
				// extract bits
				p = BIT2_CONV<MEM_TYPE>::Conv(Buffer, L, p);
			}

			// tail
			if (n > 0)
			{
				C_UInt8 Ch = I.Allocator->R8b();
				for (; n > 0; n--, Ch >>= 2)
					*p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x03);
			}

			return p;
		}

		/// read an array from CdAllocator
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			// buffer
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE];
			SIZE64 pI = I.Ptr << 1;
			I.Ptr += n;

			// header
			I.Allocator->SetPosition(pI >> 3);
			C_UInt8 offset = (pI & 0x07);
			if (offset > 0)
			{
				C_UInt8 Ch = I.Allocator->R8b() >> offset;
				ssize_t m = (8 - offset) >> 1;
				if (m > n) m = n;
				n -= m;
				for (; m > 0; m--, Ch >>= 2)
				{
					if (*sel++)
						*p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x03);
				}
			}

			// body
			while (n >= 4)
			{
				// read buffer
				ssize_t L = (n >> 2);
				if (L > MEMORY_BUFFER_SIZE) L = MEMORY_BUFFER_SIZE;
				I.Allocator->ReadData(Buffer, L);
				n -= (L << 2);
				// extract bits
				p = BIT2_CONV<MEM_TYPE>::Conv2(Buffer, L, p, sel);
				sel += (L << 2);
			}

			// tail
			if (n > 0)
			{
				C_UInt8 Ch = I.Allocator->R8b();
				for (; n > 0; n--, Ch >>= 2)
				{
					if (*sel++)
						*p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x03);
				}
			}

			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			// initialize
			SIZE64 pI = I.Ptr << 1;
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
			SIZE64 pI = I.Ptr << 1;
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

			if (ss.Offset)
			{
				ssize_t nn = 4 - (ss.Offset >> 1);
				for (; (n > 0) && (nn > 0); n--, nn--)
					ss.WriteBit(VAL_CONV_TO_U8(MEM_TYPE, *p++), N_BIT);
			}

			// buffer writing with bytes
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE];
			while (n >= 4)
			{
				ssize_t nn = 0;
				for (C_UInt8 *s=Buffer; (n >= 4) && (nn < MEMORY_BUFFER_SIZE); n-=4, nn++)
				{
					*s++ = (VAL_CONV_TO_U8(MEM_TYPE, p[0]) & 0x03) |
						((VAL_CONV_TO_U8(MEM_TYPE, p[1]) & 0x03) << 2) |
						((VAL_CONV_TO_U8(MEM_TYPE, p[2]) & 0x03) << 4) |
						((VAL_CONV_TO_U8(MEM_TYPE, p[3]) & 0x03) << 6);
					p += 4;
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

#endif /* _HEADER_COREARRAY_BIT2_GDS_ */
