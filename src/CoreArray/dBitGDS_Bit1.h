// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dBitGDS_Bit1.h: Bit operators and classes of GDS format for Bit1
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
 *	\file     dBitGDS_Bit1.h
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2007 - 2017
 *	\brief    Bit operators and classes of GDS format for Bit1
 *	\details
**/

#ifndef _HEADER_COREARRAY_BIT1_GDS_
#define _HEADER_COREARRAY_BIT1_GDS_

namespace CoreArray
{
	/// Template for the conversion of 2-bit array
	template<typename MEM_TYPE> struct COREARRAY_DLL_LOCAL BIT1_CONV
	{
		inline static MEM_TYPE* Decode(const C_UInt8 *s, size_t n_byte, MEM_TYPE *p)
		{
			for (; n_byte > 0; n_byte--)
			{
				C_UInt8 Ch = *s++;
				p[0] = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x01);
				p[1] = VAL_CONV_FROM_U8(MEM_TYPE, (Ch >> 1) & 0x01);
				p[2] = VAL_CONV_FROM_U8(MEM_TYPE, (Ch >> 2) & 0x01);
				p[3] = VAL_CONV_FROM_U8(MEM_TYPE, (Ch >> 3) & 0x01);
				p[4] = VAL_CONV_FROM_U8(MEM_TYPE, (Ch >> 4) & 0x01);
				p[5] = VAL_CONV_FROM_U8(MEM_TYPE, (Ch >> 5) & 0x01);
				p[6] = VAL_CONV_FROM_U8(MEM_TYPE, (Ch >> 6) & 0x01);
				p[7] = VAL_CONV_FROM_U8(MEM_TYPE, Ch >> 7);
				p += 8;
			}
			return p;
		}

		inline static MEM_TYPE* Decode2(const C_UInt8 *s, size_t n_byte,
			MEM_TYPE *p, const C_BOOL sel[])
		{
			for (; n_byte > 0; n_byte--)
			{
				C_UInt8 Ch = *s++;
				if (*sel++) *p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x01);
				Ch >>= 1;
				if (*sel++) *p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x01);
				Ch >>= 1;
				if (*sel++) *p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x01);
				Ch >>= 1;
				if (*sel++) *p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x01);
				Ch >>= 1;
				if (*sel++) *p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x01);
				Ch >>= 1;
				if (*sel++) *p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x01);
				Ch >>= 1;
				if (*sel++) *p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x01);
				Ch >>= 1;
				if (*sel++) *p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch);
			}
			return p;
		}

		inline static const MEM_TYPE *Encode(const MEM_TYPE *s, C_UInt8 *p,
			size_t n_byte)
		{
			for (; n_byte > 0; n_byte--)
			{
				*p++ = (VAL_CONV_TO_U8(MEM_TYPE, s[0]) & 0x01) |
					((VAL_CONV_TO_U8(MEM_TYPE, s[1]) & 0x01) << 1) |
					((VAL_CONV_TO_U8(MEM_TYPE, s[2]) & 0x01) << 2) |
					((VAL_CONV_TO_U8(MEM_TYPE, s[3]) & 0x01) << 3) |
					((VAL_CONV_TO_U8(MEM_TYPE, s[4]) & 0x01) << 4) |
					((VAL_CONV_TO_U8(MEM_TYPE, s[5]) & 0x01) << 5) |
					((VAL_CONV_TO_U8(MEM_TYPE, s[6]) & 0x01) << 6) |
					((VAL_CONV_TO_U8(MEM_TYPE, s[7]) & 0x01) << 7);
				s += 8;
			}
			return s;
		}
	};


#ifdef COREARRAY_SIMD_SSE2

	#define WRITE_BIT1_DECODE    \
		{ \
			C_UInt8 Ch = *s++; \
			p[0] = (Ch & 0x01); p[1] = (Ch >> 1) & 0x01; \
			p[2] = (Ch >> 2) & 0x01; p[3] = (Ch >> 3) & 0x01; \
			p[4] = (Ch >> 4) & 0x01; p[5] = (Ch >> 5) & 0x01; \
			p[6] = (Ch >> 6) & 0x01; p[7] = (Ch >> 7); \
			p += 8; \
		}

	#define WRITE_BIT1_SEL_DECODE    \
		{ \
			C_UInt8 Ch = *s++; \
			if (*sel++) *p++ = Ch & 0x01; \
			Ch >>= 1; if (*sel++) *p++ = Ch & 0x01; \
			Ch >>= 1; if (*sel++) *p++ = Ch & 0x01; \
			Ch >>= 1; if (*sel++) *p++ = Ch & 0x01; \
			Ch >>= 1; if (*sel++) *p++ = Ch & 0x01; \
			Ch >>= 1; if (*sel++) *p++ = Ch & 0x01; \
			Ch >>= 1; if (*sel++) *p++ = Ch & 0x01; \
			Ch >>= 1; if (*sel++) *p++ = Ch; \
		}

	#define WRITE_BIT1_ENCODE    \
		{ \
			*p++ = (C_UInt8(s[0]) & 0x01) | ((C_UInt8(s[1]) & 0x01) << 1) | \
				((C_UInt8(s[2]) & 0x01) << 2) | ((C_UInt8(s[3]) & 0x01) << 3) | \
				((C_UInt8(s[4]) & 0x01) << 4) | ((C_UInt8(s[5]) & 0x01) << 5) | \
				((C_UInt8(s[6]) & 0x01) << 6) | ((C_UInt8(s[7]) & 0x01) << 7); \
			s += 8; \
		}


	static const __m128i BIT1_REP_x01 = _mm_set1_epi8(0x01);
	static const __m128i BIT1_B4_x01 = _mm_set1_epi32(0x01);

	#define WRITE_BIT1_DECODE_B2_UINT8(val)    \
		{ \
			__m128i v = _mm_set1_epi16(val); \
			__m128i w1 = _mm_unpacklo_epi8(v, _mm_srli_epi16(v,1)); \
			__m128i w2 = _mm_unpacklo_epi8(_mm_srli_epi16(v,2), _mm_srli_epi16(v,3)); \
			__m128i v1 = _mm_unpacklo_epi16(w1, w2); \
			__m128i w3 = _mm_unpacklo_epi8(_mm_srli_epi16(v,4), _mm_srli_epi16(v,5)); \
			__m128i w4 = _mm_unpacklo_epi8(_mm_srli_epi16(v,6), _mm_srli_epi16(v,7)); \
			__m128i v2 = _mm_unpacklo_epi16(w3, w4); \
			_mm_storeu_si128((__m128i*)p, _mm_unpacklo_epi32(v1, v2) & BIT1_REP_x01); \
		}

	#define WRITE_BIT1_DECODE_B2_INT32(val)    \
		{ \
			__m128i v = _mm_set1_epi16(val); \
			__m128i w1 = _mm_unpacklo_epi8(v, _mm_srli_epi16(v,1)); \
			__m128i w2 = _mm_unpacklo_epi8(_mm_srli_epi16(v,2), _mm_srli_epi16(v,3)); \
			__m128i v1 = _mm_unpacklo_epi16(w1, w2); \
			__m128i w3 = _mm_unpacklo_epi8(_mm_srli_epi16(v,4), _mm_srli_epi16(v,5)); \
			__m128i w4 = _mm_unpacklo_epi8(_mm_srli_epi16(v,6), _mm_srli_epi16(v,7)); \
			__m128i v2 = _mm_unpacklo_epi16(w3, w4); \
			v  = _mm_unpacklo_epi32(v1, v2) & BIT1_REP_x01; \
			v1 = _mm_unpacklo_epi8(v, zero); \
			_mm_storeu_si128((__m128i*)p, _mm_unpacklo_epi16(v1, zero)); \
			_mm_storeu_si128((__m128i*)(p+4), _mm_unpackhi_epi16(v1, zero)); \
			v1 = _mm_unpackhi_epi8(v, zero); \
			_mm_storeu_si128((__m128i*)(p+8), _mm_unpacklo_epi16(v1, zero)); \
			_mm_storeu_si128((__m128i*)(p+12), _mm_unpackhi_epi16(v1, zero)); \
		}


	template<> struct COREARRAY_DLL_LOCAL BIT1_CONV<C_UInt8>
	{
		inline static C_UInt8* Decode(const C_UInt8 *s, size_t n_byte, C_UInt8 *p)
		{
			for (; n_byte >= 2; n_byte-=2)
			{
				WRITE_BIT1_DECODE_B2_UINT8(*((const C_Int16*)s))
				s += 2; p += 16;
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT1_DECODE
			return p;
		}

		inline static C_UInt8* Decode2(const C_UInt8 *s, size_t n_byte, C_UInt8 *p,
			const C_BOOL sel[])
		{
			for (; n_byte >= 2; n_byte -= 2)
			{
				__m128i sv = _mm_loadu_si128((__m128i const*)sel);
				sv = _mm_cmpeq_epi8(sv, _mm_setzero_si128());
				int sv16 = _mm_movemask_epi8(sv);
				if (sv16 == 0)  // all selected
				{
					WRITE_BIT1_DECODE_B2_UINT8(*((const C_Int16*)s))
					s += 2; p += 16; sel += 16;
				} else if (sv16 == 0xFFFF)  // not selected
				{
					s += 2; sel += 16;
				} else {
					WRITE_BIT1_SEL_DECODE
					WRITE_BIT1_SEL_DECODE
				}
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT1_SEL_DECODE
			return p;
		}

		inline static const C_UInt8 *Encode(const C_UInt8 *s, C_UInt8 *p,
			size_t n_byte)
		{
			for (; n_byte >= 8; n_byte-=8)
			{
				C_UInt16 r1 = _mm_movemask_epi8(_mm_slli_epi32(
					_mm_loadu_si128((__m128i const*)s), 7));
				C_UInt16 r2 = _mm_movemask_epi8(_mm_slli_epi32(
					_mm_loadu_si128((__m128i const*)(s+16)), 7));
				C_UInt16 r3 = _mm_movemask_epi8(_mm_slli_epi32(
					_mm_loadu_si128((__m128i const*)(s+32)), 7));
				C_UInt16 r4 = _mm_movemask_epi8(_mm_slli_epi32(
					_mm_loadu_si128((__m128i const*)(s+48)), 7));
				*((C_UInt64*)p) = r1 | (C_UInt64(r2) << 16) |
					(C_UInt64(r3) << 32) | (C_UInt64(r4) << 48);
				p += 8; s += 64;
			}
			for (; n_byte >= 2; n_byte-=2)
			{
				*((C_Int16*)p) = _mm_movemask_epi8(
					_mm_slli_epi32(_mm_loadu_si128((__m128i const*)s), 7));
				p += 2; s += 16;
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT1_ENCODE
			return s;
		}
	};

	template<> struct COREARRAY_DLL_LOCAL BIT1_CONV<C_Int8>
	{
		inline static C_Int8* Decode(const C_UInt8 *s, size_t n_byte, C_Int8 *p)
		{
			return (C_Int8*)BIT1_CONV<C_UInt8>::Decode(s, n_byte, (C_UInt8*)p);
		}
		inline static C_Int8* Decode2(const C_UInt8 *s, size_t n_byte, C_Int8 *p,
			const C_BOOL sel[])
		{
			return (C_Int8*)BIT1_CONV<C_UInt8>::Decode2(s, n_byte, (C_UInt8*)p, sel);
		}
		inline static const C_Int8 *Encode(const C_Int8 *s, C_UInt8 *p,
			size_t n_byte)
		{
			return (C_Int8*)BIT1_CONV<C_UInt8>::Encode((C_UInt8*)s, p, n_byte);
		}
	};


	// ===========================================================

	template<> struct COREARRAY_DLL_LOCAL BIT1_CONV<C_Int32>
	{
		inline static C_Int32* Decode(const C_UInt8 *s, size_t n_byte, C_Int32 *p)
		{
			for (; n_byte >= 2; n_byte-=2)
			{
				const __m128i zero = _mm_setzero_si128();
				WRITE_BIT1_DECODE_B2_INT32(*((const C_Int16*)s))
				s += 2; p += 16;
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT1_DECODE
			return p;
		}

		inline static C_Int32* Decode2(const C_UInt8 *s, size_t n_byte, C_Int32 *p,
			const C_BOOL sel[])
		{
			for (; n_byte >= 2; n_byte -= 2)
			{
				__m128i sv = _mm_loadu_si128((__m128i const*)sel);
				const __m128i zero = _mm_setzero_si128();
				sv = _mm_cmpeq_epi8(sv, zero);
				int sv16 = _mm_movemask_epi8(sv);
				if (sv16 == 0)  // all selected
				{
					WRITE_BIT1_DECODE_B2_INT32(*((const C_Int16*)s))
					s += 2; p += 16; sel += 16;
				} else if (sv16 == 0xFFFF)  // not selected
				{
					s += 2; sel += 16;
				} else {
					WRITE_BIT1_SEL_DECODE
					WRITE_BIT1_SEL_DECODE
				}
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT1_SEL_DECODE
			return p;
		}

		inline static const C_Int32 *Encode(const C_Int32 *s, C_UInt8 *p,
			size_t n_byte)
		{
			for (; n_byte >= 2; n_byte-=2)
			{
				__m128i v1 = _mm_loadu_si128((__m128i const*)s) & BIT1_B4_x01;
				__m128i v2 = _mm_loadu_si128((__m128i const*)(s+4)) & BIT1_B4_x01;
				__m128i w1 = _mm_packs_epi32(v1, v2);
				v1 = _mm_loadu_si128((__m128i const*)(s+8)) & BIT1_B4_x01;
				v2 = _mm_loadu_si128((__m128i const*)(s+12)) & BIT1_B4_x01;
				__m128i w2 = _mm_packs_epi32(v1, v2);
				*((C_Int16*)p) = _mm_movemask_epi8(
					_mm_slli_epi32(_mm_packus_epi16(w1, w2), 7));
				p += 2; s += 16;
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT1_ENCODE
			return s;
		}
	};

	template<> struct COREARRAY_DLL_LOCAL BIT1_CONV<C_UInt32>
	{
		inline static C_UInt32* Decode(const C_UInt8 *s, size_t n_byte, C_UInt32 *p)
		{
			return (C_UInt32*)BIT1_CONV<C_Int32>::Decode(s, n_byte, (C_Int32*)p);
		}
		inline static C_UInt32* Decode2(const C_UInt8 *s, size_t n_byte, C_UInt32 *p,
			const C_BOOL sel[])
		{
			return (C_UInt32*)BIT1_CONV<C_Int32>::Decode2(s, n_byte, (C_Int32*)p, sel);
		}
		inline static const C_UInt32 *Encode(const C_UInt32 *s, C_UInt8 *p,
			size_t n_byte)
		{
			return (C_UInt32*)BIT1_CONV<C_Int32>::Encode((C_Int32*)s, p, n_byte);
		}
	};

#endif


	// =====================================================================
	// 1-bit unsigned integer functions for allocator

	/// template for allocate function for 1-bit integer
	/** in the case that MEM_TYPE is numeric **/
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<BIT1, MEM_TYPE>
	{
		/// the number of bits
		static const unsigned N_BIT = 1u;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			// buffer
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE] COREARRAY_SIMD_ATTR_ALIGN;
			SIZE64 pI = I.Ptr;
			I.Ptr += n;

			// header
			I.Allocator->SetPosition(pI >> 3);
			C_UInt8 offset = (pI & 0x07);
			if (offset > 0)
			{
				C_UInt8 Ch = I.Allocator->R8b() >> offset;
				ssize_t m = (8 - offset);
				if (m > n) m = n;
				n -= m;
				for (; m > 0; m--, Ch >>= 1)
					*p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x01);
			}

			// body
			while (n >= 8)
			{
				// read buffer
				ssize_t L = (n >> 3);
				if (L > MEMORY_BUFFER_SIZE) L = MEMORY_BUFFER_SIZE;
				I.Allocator->ReadData(Buffer, L);
				n -= (L << 3);
				// extract bits
				p = BIT1_CONV<MEM_TYPE>::Decode(Buffer, L, p);
			}

			// tail
			if (n > 0)
			{
				C_UInt8 Ch = I.Allocator->R8b();
				for (; n > 0; n--, Ch >>= 1)
					*p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x01);
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			// buffer
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE] COREARRAY_SIMD_ATTR_ALIGN;
			SIZE64 pI = I.Ptr;
			I.Ptr += n;

			// header
			I.Allocator->SetPosition(pI >> 3);
			C_UInt8 offset = (pI & 0x07);
			if (offset > 0)
			{
				C_UInt8 Ch = I.Allocator->R8b() >> offset;
				ssize_t m = (8 - offset);
				if (m > n) m = n;
				n -= m;
				for (; m > 0; m--, Ch >>= 1)
				{
					if (*sel++)
						*p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x01);
				}
			}

			// body
			while (n >= 8)
			{
				// read buffer
				ssize_t L = (n >> 3);
				if (L > MEMORY_BUFFER_SIZE) L = MEMORY_BUFFER_SIZE;
				I.Allocator->ReadData(Buffer, L);
				n -= (L << 3);
				// extract bits
				p = BIT1_CONV<MEM_TYPE>::Decode2(Buffer, L, p, sel);
				sel += (L << 3);
			}

			// tail
			if (n > 0)
			{
				C_UInt8 Ch = I.Allocator->R8b();
				for (; n > 0; n--, Ch >>= 1)
				{
					if (*sel++)
						*p++ = VAL_CONV_FROM_U8(MEM_TYPE, Ch & 0x01);
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			// initialize
			SIZE64 pI = I.Ptr;
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
			SIZE64 pI = I.Ptr;
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
				ssize_t nn = 8 - ss.Offset;
				for (; (n > 0) && (nn > 0); n--, nn--)
					ss.WriteBit(VAL_CONV_TO_U8(MEM_TYPE, *p++), N_BIT);
			}

			// buffer writing with bytes
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE] COREARRAY_SIMD_ATTR_ALIGN;
			while (n >= 8)
			{
				ssize_t nn = n >> 3;
				if (nn > MEMORY_BUFFER_SIZE) nn = MEMORY_BUFFER_SIZE;
				p = BIT1_CONV<MEM_TYPE>::Encode(p, Buffer, nn);
				I.Allocator->WriteData(Buffer, nn);
				n -= (nn << 3);
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

#endif /* _HEADER_COREARRAY_BIT1_GDS_ */
