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
 *	\file     dBitGDS_Bit2.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2017
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
		inline static MEM_TYPE* Decode(const C_UInt8 *s, size_t n_byte, MEM_TYPE *p)
		{
			for (; n_byte > 0; n_byte--)
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

		inline static MEM_TYPE* Decode2(const C_UInt8 *s, size_t n_byte,
			MEM_TYPE *p, const C_BOOL sel[])
		{
			for (; n_byte > 0; n_byte--)
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

		inline static const MEM_TYPE *Encode(const MEM_TYPE *s, C_UInt8 *p,
			size_t n_byte)
		{
			for (; n_byte > 0; n_byte--)
			{
				*p++ = (VAL_CONV_TO_U8(MEM_TYPE, s[0]) & 0x03) |
					((VAL_CONV_TO_U8(MEM_TYPE, s[1]) & 0x03) << 2) |
					((VAL_CONV_TO_U8(MEM_TYPE, s[2]) & 0x03) << 4) |
					((VAL_CONV_TO_U8(MEM_TYPE, s[3]) & 0x03) << 6);
				s += 4;
			}
			return s;
		}
	};


#ifdef COREARRAY_SIMD_SSE2

	#define WRITE_BIT2_DECODE    \
		{ \
			C_UInt8 Ch = *s++; \
			p[0] = (Ch & 0x03); p[1] = (Ch >> 2) & 0x03; \
			p[2] = (Ch >> 4) & 0x03; p[3] = (Ch >> 6); \
			p += 4; \
		}

	#define WRITE_BIT2_SEL_DECODE    \
		{ \
			C_UInt8 Ch = *s++; \
			if (*sel++) *p++ = Ch & 0x03; \
			Ch >>= 2; if (*sel++) *p++ = Ch & 0x03; \
			Ch >>= 2; if (*sel++) *p++ = Ch & 0x03; \
			Ch >>= 2; if (*sel++) *p++ = Ch; \
		}

	#define WRITE_BIT2_ENCODE    \
		{ \
			*p++ = (C_UInt8(s[0]) & 0x03) | \
				((C_UInt8(s[1]) & 0x03) << 2) | \
				((C_UInt8(s[2]) & 0x03) << 4) | \
				((C_UInt8(s[3]) & 0x03) << 6); \
			s += 4; \
		}


	static const __m128i BIT2_REP_x03 = _mm_set1_epi8(0x03);
	static const __m128i BIT2_UInt16_x03 = _mm_set1_epi16(0x03);
	static const __m128i BIT2_UInt32_x03 = _mm_set1_epi32(0x03);
	static const __m128i BIT2_UInt32_xFF = _mm_set1_epi32(0xFF);

#ifdef COREARRAY_SIMD_AVX2
	static const __m256i BIT2_AVX_REP_x03 = _mm256_set1_epi8(0x03);
	static const __m256i BIT2_AVX_UInt32_x03 = _mm256_set1_epi32(0x03);
	static const __m256i BIT2_AVX_UInt32_xFF = _mm256_set1_epi32(0xFF);
	static const __m256i BIT2_AVX_UInt64_SHR = _mm256_set_epi64x(0, 32, 0, 0);
#endif

	#define WRITE_BIT2_DECODE_B4_UINT8(val)    \
		{ \
			__m128i v = _mm_set1_epi32(val); \
			__m128i v1 = v & BIT2_REP_x03; \
			__m128i v2 = _mm_srli_epi32(v, 2) & BIT2_REP_x03; \
			__m128i v3 = _mm_srli_epi32(v, 4) & BIT2_REP_x03; \
			__m128i v4 = _mm_srli_epi32(v, 6) & BIT2_REP_x03; \
			__m128i w1 = _mm_unpacklo_epi8(v1, v2); \
			__m128i w2 = _mm_unpacklo_epi8(v3, v4); \
			_mm_storeu_si128((__m128i*)p, _mm_unpacklo_epi16(w1, w2)); \
		}

	#define WRITE_BIT2_DECODE_B4_INT32(val)    \
		{ \
			__m128i v = _mm_set1_epi32(val); \
			const __m128i zero = _mm_setzero_si128(); \
			v = _mm_unpacklo_epi16(_mm_unpacklo_epi8(v, zero), zero); \
			__m128i v1 = v & BIT2_UInt32_x03; \
			__m128i v2 = _mm_srli_epi32(v, 2) & BIT2_UInt32_x03; \
			__m128i v3 = _mm_srli_epi32(v, 4) & BIT2_UInt32_x03; \
			__m128i v4 = _mm_srli_epi32(v, 6); \
			__m128i w1 = _mm_unpacklo_epi32(v1, v2); \
			__m128i w2 = _mm_unpacklo_epi32(v3, v4); \
			_mm_storeu_si128((__m128i*)p, _mm_unpacklo_epi64(w1, w2)); \
			_mm_storeu_si128((__m128i*)(p+4), _mm_unpackhi_epi64(w1, w2)); \
			w1 = _mm_unpackhi_epi32(v1, v2); \
			w2 = _mm_unpackhi_epi32(v3, v4); \
			_mm_storeu_si128((__m128i*)(p+8), _mm_unpacklo_epi64(w1, w2)); \
			_mm_storeu_si128((__m128i*)(p+12), _mm_unpackhi_epi64(w1, w2)); \
		}


	template<> struct COREARRAY_DLL_LOCAL BIT2_CONV<C_UInt8>
	{
		inline static C_UInt8* Decode(const C_UInt8 *s, size_t n_byte, C_UInt8 *p)
		{
		#ifdef COREARRAY_SIMD_AVX2
			for (; n_byte >= 32; n_byte-=32)
			{
				__m256i v = _mm256_loadu_si256((__m256i const*)s);
				s += 32;
				__m256i v1 = v & BIT2_AVX_REP_x03;
				__m256i v2 = _mm256_srli_epi32(v, 2) & BIT2_AVX_REP_x03;
				__m256i v3 = _mm256_srli_epi32(v, 4) & BIT2_AVX_REP_x03;
				__m256i v4 = _mm256_srli_epi32(v, 6) & BIT2_AVX_REP_x03;

				__m256i w1 = _mm256_unpacklo_epi8(v1, v2);
				__m256i w2 = _mm256_unpacklo_epi8(v3, v4);
				__m256i x1 = _mm256_unpacklo_epi16(w1, w2);
				__m256i x2 = _mm256_unpackhi_epi16(w1, w2);

				_mm256_storeu_si256((__m256i*)p,
					_mm256_permute2x128_si256(x1, x2, 0x20));
				_mm256_storeu_si256((__m256i*)(p + 64),
					_mm256_permute2x128_si256(x1, x2, 0x31));

				__m256i w3 = _mm256_unpackhi_epi8(v1, v2);
				__m256i w4 = _mm256_unpackhi_epi8(v3, v4);
				__m256i x3 = _mm256_unpacklo_epi16(w3, w4);
				__m256i x4 = _mm256_unpackhi_epi16(w3, w4);

				_mm256_storeu_si256((__m256i*)(p + 32),
					_mm256_permute2x128_si256(x3, x4, 0x20));
				_mm256_storeu_si256((__m256i*)(p + 96),
					_mm256_permute2x128_si256(x3, x4, 0x31));
				p += 128;
			}
		#endif
			for (; n_byte >= 16; n_byte-=16)
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
			for (; n_byte >= 4; n_byte-=4)
			{
				WRITE_BIT2_DECODE_B4_UINT8(*((const int*)s))
				s += 4; p += 16;
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT2_DECODE
			return p;
		}

		inline static C_UInt8* Decode2(const C_UInt8 *s, size_t n_byte, C_UInt8 *p,
			const C_BOOL sel[])
		{
		#ifdef COREARRAY_SIMD_AVX2
			for (; n_byte >= 8; n_byte -= 8)
			{
				__m256i sv = _mm256_loadu_si256((__m256i const*)sel);
				sv = _mm256_cmpeq_epi8(sv, _mm256_setzero_si256());
				int sv32 = _mm256_movemask_epi8(sv);
				if (sv32 == 0)  // all selected
				{
					__m256i v = _mm256_set1_epi64x(*((const C_Int64*)s));
					__m256i v1 = v & BIT2_AVX_REP_x03;
					__m256i v2 = _mm256_srli_epi64(v, 2) & BIT2_AVX_REP_x03;
					__m256i v3 = _mm256_srli_epi64(v, 4) & BIT2_AVX_REP_x03;
					__m256i v4 = _mm256_srli_epi64(v, 6) & BIT2_AVX_REP_x03;

					__m256i w1 = _mm256_unpacklo_epi8(v1, v2);
					__m256i w2 = _mm256_unpacklo_epi8(v3, v4);
					__m256i wl = _mm256_unpacklo_epi16(w1, w2);
					__m256i wh = _mm256_unpackhi_epi16(w1, w2);
					__m256i w  = _mm256_permute2f128_si256(wl, wh, 0x20);

					_mm256_storeu_si256((__m256i*)p, w);
					s += 8; p += 32; sel += 32;
				} else if (sv32 == -1)  // not selected
				{
					s += 8; sel += 32;
				} else {
					int sv32_low = sv32 & 0xFFFF;
					if (sv32_low == 0)
					{
						WRITE_BIT2_DECODE_B4_UINT8(*((const int*)s))
						s += 4; sel += 16; p += 16;
					} else if (sv32_low == 0xFFFF)
					{
						s += 4; sel += 16;
					} else {
						WRITE_BIT2_SEL_DECODE
						WRITE_BIT2_SEL_DECODE
						WRITE_BIT2_SEL_DECODE
						WRITE_BIT2_SEL_DECODE
					}

					int sv32_high = C_UInt32(sv32) >> 16;
					if (sv32_high == 0)
					{
						WRITE_BIT2_DECODE_B4_UINT8(*((const int*)s))
						s += 4; sel += 16; p += 16;
					} else if (sv32_high == 0xFFFF)
					{
						s += 4; sel += 16;
					} else {
						WRITE_BIT2_SEL_DECODE
						WRITE_BIT2_SEL_DECODE
						WRITE_BIT2_SEL_DECODE
						WRITE_BIT2_SEL_DECODE
					}
				}
			}
		#endif
			for (; n_byte >= 4; n_byte -= 4)
			{
				__m128i sv = _mm_loadu_si128((__m128i const*)sel);
				sv = _mm_cmpeq_epi8(sv, _mm_setzero_si128());
				int sv16 = _mm_movemask_epi8(sv);
				if (sv16 == 0)  // all selected
				{
					WRITE_BIT2_DECODE_B4_UINT8(*((const int*)s))
					s += 4; p += 16; sel += 16;
				} else if (sv16 == 0xFFFF)  // not selected
				{
					s += 4; sel += 16;
				} else {
					WRITE_BIT2_SEL_DECODE
					WRITE_BIT2_SEL_DECODE
					WRITE_BIT2_SEL_DECODE
					WRITE_BIT2_SEL_DECODE
				}
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT2_SEL_DECODE
			return p;
		}

		inline static const C_UInt8 *Encode(const C_UInt8 *s, C_UInt8 *p,
			size_t n_byte)
		{
		#ifdef COREARRAY_SIMD_AVX2
			for (; n_byte >= 8; n_byte-=8)
			{
				__m256i v = _mm256_loadu_si256((__m256i const*)s);
				s += 32;
				__m256i w1 = _mm256_slli_epi32(v, 7);
				__m256i w2 = _mm256_slli_epi32(v, 6);
				__m256i x1 = _mm256_unpacklo_epi8(w1, w2);
				__m256i x2 = _mm256_unpackhi_epi8(w1, w2);
				C_UInt32 r1 = _mm256_movemask_epi8(_mm256_permute2x128_si256(x1, x2, 0x20));
				C_UInt32 r2 = _mm256_movemask_epi8(_mm256_permute2x128_si256(x1, x2, 0x31));
				*((C_UInt64*)p) = r1 | (C_UInt64(r2) << 32);
				p += 8;
			}
		#endif
			for (; n_byte >= 4; n_byte-=4)
			{
				__m128i v = _mm_loadu_si128((__m128i const*)s);
				s += 16;
				__m128i w1 = _mm_slli_epi32(v, 7);
				__m128i w2 = _mm_slli_epi32(v, 6);
				int r1 = _mm_movemask_epi8(_mm_unpacklo_epi8(w1, w2));
				int r2 = _mm_movemask_epi8(_mm_unpackhi_epi8(w1, w2));
				*((C_Int32*)p) = r1 | (r2 << 16);
				p += 4;
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT2_ENCODE
			return s;
		}
	};

	template<> struct COREARRAY_DLL_LOCAL BIT2_CONV<C_Int8>
	{
		inline static C_Int8* Decode(const C_UInt8 *s, size_t n_byte, C_Int8 *p)
		{
			return (C_Int8*)BIT2_CONV<C_UInt8>::Decode(s, n_byte, (C_UInt8*)p);
		}
		inline static C_Int8* Decode2(const C_UInt8 *s, size_t n_byte, C_Int8 *p,
			const C_BOOL sel[])
		{
			return (C_Int8*)BIT2_CONV<C_UInt8>::Decode2(s, n_byte, (C_UInt8*)p, sel);
		}
		inline static const C_Int8 *Encode(const C_Int8 *s, C_UInt8 *p,
			size_t n_byte)
		{
			return (C_Int8*)BIT2_CONV<C_UInt8>::Encode((C_UInt8*)s, p, n_byte);
		}
	};


	// ===========================================================

	template<> struct COREARRAY_DLL_LOCAL BIT2_CONV<C_Int16>
	{
		inline static C_Int16* Decode(const C_UInt8 *s, size_t n_byte, C_Int16 *p)
		{
			for (; n_byte > 0; n_byte--) WRITE_BIT2_DECODE
			return p;
		}

		inline static C_Int16* Decode2(const C_UInt8 *s, size_t n_byte, C_Int16 *p,
			const C_BOOL sel[])
		{
			for (; n_byte > 0; n_byte--) WRITE_BIT2_SEL_DECODE
			return p;
		}

		inline static const C_Int16 *Encode(const C_Int16 *s, C_UInt8 *p,
			size_t n_byte)
		{
			for (; n_byte >= 4; n_byte-=4)
			{
				__m128i v = _mm_loadu_si128((__m128i const*)s) & BIT2_UInt16_x03;
				int r1 = _mm_movemask_epi8(_mm_slli_epi16(v, 7) | _mm_slli_epi16(v, 14));
				s += 8;
				v = _mm_loadu_si128((__m128i const*)s) & BIT2_UInt16_x03;
				int r2 = _mm_movemask_epi8(_mm_slli_epi16(v, 7) | _mm_slli_epi16(v, 14));
				s += 8;
				*((C_Int32*)p) = r1 | (r2 << 16);
				p += 4;
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT2_ENCODE
			return s;
		}
	};

	template<> struct COREARRAY_DLL_LOCAL BIT2_CONV<C_UInt16>
	{
		inline static C_UInt16* Decode(const C_UInt8 *s, size_t n_byte, C_UInt16 *p)
		{
			return (C_UInt16*)BIT2_CONV<C_Int16>::Decode(s, n_byte, (C_Int16*)p);
		}
		inline static C_UInt16* Decode2(const C_UInt8 *s, size_t n_byte, C_UInt16 *p,
			const C_BOOL sel[])
		{
			return (C_UInt16*)BIT2_CONV<C_Int16>::Decode2(s, n_byte, (C_Int16*)p, sel);
		}
		inline static const C_UInt16 *Encode(const C_UInt16 *s, C_UInt8 *p,
			size_t n_byte)
		{
			return (C_UInt16*)BIT2_CONV<C_Int16>::Encode((C_Int16*)s, p, n_byte);
		}
	};


	// ===========================================================

	template<> struct COREARRAY_DLL_LOCAL BIT2_CONV<C_Int32>
	{
		inline static C_Int32* Decode(const C_UInt8 *s, size_t n_byte, C_Int32 *p)
		{
		#ifdef COREARRAY_SIMD_AVX2
			for (; n_byte >= 8; n_byte-=8)
			{
				__m256i v = _mm256_set1_epi64x(*((const C_Int64*)s));
				v = _mm256_srlv_epi64(v, BIT2_AVX_UInt64_SHR);
				s += 8;
				const __m256i zero = _mm256_setzero_si256();
				v = _mm256_unpacklo_epi16(_mm256_unpacklo_epi8(v, zero), zero);

				__m256i v1 = v & BIT2_AVX_UInt32_x03;
				__m256i v2 = _mm256_srli_epi32(v, 2) & BIT2_AVX_UInt32_x03;
				__m256i v3 = _mm256_srli_epi32(v, 4) & BIT2_AVX_UInt32_x03;
				__m256i v4 = _mm256_srli_epi32(v, 6);

				__m256i w1 = _mm256_unpacklo_epi32(v1, v2);
				__m256i w2 = _mm256_unpacklo_epi32(v3, v4);
				__m256i x1 = _mm256_unpacklo_epi64(w1, w2);
				__m256i x2 = _mm256_unpackhi_epi64(w1, w2);
				_mm256_storeu_si256((__m256i*)p, _mm256_permute2x128_si256(x1, x2, 0x20));
				_mm256_storeu_si256((__m256i*)(p+16), _mm256_permute2x128_si256(x1, x2, 0x31));

				w1 = _mm256_unpackhi_epi32(v1, v2);
				w2 = _mm256_unpackhi_epi32(v3, v4);
				x1 = _mm256_unpacklo_epi64(w1, w2);
				x2 = _mm256_unpackhi_epi64(w1, w2);
				_mm256_storeu_si256((__m256i*)(p+8), _mm256_permute2x128_si256(x1, x2, 0x20));
				_mm256_storeu_si256((__m256i*)(p+24), _mm256_permute2x128_si256(x1, x2, 0x31));
				p += 32;
			}
		#endif
			for (; n_byte >= 4; n_byte-=4)
			{
				WRITE_BIT2_DECODE_B4_INT32(*((const int*)s));
				s += 4; p += 16;
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT2_DECODE
			return p;
		}

		inline static C_Int32* Decode2(const C_UInt8 *s, size_t n_byte,
			C_Int32 *p, const C_BOOL sel[])
		{
		#ifdef COREARRAY_SIMD_AVX2
			for (; n_byte >= 8; n_byte -= 8)
			{
				__m256i sv = _mm256_loadu_si256((__m256i const*)sel);
				const __m256i zero = _mm256_setzero_si256();
				sv = _mm256_cmpeq_epi8(sv, zero);
				int sv32 = _mm256_movemask_epi8(sv);
				if (sv32 == 0)  // all selected
				{
					__m256i v = _mm256_set1_epi64x(*((const C_Int64*)s));
					v = _mm256_srlv_epi64(v, BIT2_AVX_UInt64_SHR);
					s += 8;
					v = _mm256_unpacklo_epi16(_mm256_unpacklo_epi8(v, zero), zero);

					__m256i v1 = v & BIT2_AVX_UInt32_x03;
					__m256i v2 = _mm256_srli_epi32(v, 2) & BIT2_AVX_UInt32_x03;
					__m256i v3 = _mm256_srli_epi32(v, 4) & BIT2_AVX_UInt32_x03;
					__m256i v4 = _mm256_srli_epi32(v, 6);

					__m256i w1 = _mm256_unpacklo_epi32(v1, v2);
					__m256i w2 = _mm256_unpacklo_epi32(v3, v4);
					__m256i x1 = _mm256_unpacklo_epi64(w1, w2);
					__m256i x2 = _mm256_unpackhi_epi64(w1, w2);
					_mm256_storeu_si256((__m256i*)p, _mm256_permute2x128_si256(x1, x2, 0x20));
					_mm256_storeu_si256((__m256i*)(p+16), _mm256_permute2x128_si256(x1, x2, 0x31));

					w1 = _mm256_unpackhi_epi32(v1, v2);
					w2 = _mm256_unpackhi_epi32(v3, v4);
					x1 = _mm256_unpacklo_epi64(w1, w2);
					x2 = _mm256_unpackhi_epi64(w1, w2);
					_mm256_storeu_si256((__m256i*)(p+8), _mm256_permute2x128_si256(x1, x2, 0x20));
					_mm256_storeu_si256((__m256i*)(p+24), _mm256_permute2x128_si256(x1, x2, 0x31));
					p += 32; sel += 32;
				} else if (sv32 == -1)  // not selected
				{
					s += 8; sel += 32;
				} else {
					int sv32_low = sv32 & 0xFFFF;
					if (sv32_low == 0)
					{
						WRITE_BIT2_DECODE_B4_INT32(*((const int*)s));
						s += 4; sel += 16; p += 16;
					} else if (sv32_low == 0xFFFF)
					{
						s += 4; sel += 16;
					} else {
						WRITE_BIT2_SEL_DECODE
						WRITE_BIT2_SEL_DECODE
						WRITE_BIT2_SEL_DECODE
						WRITE_BIT2_SEL_DECODE
					}

					int sv32_high = C_UInt32(sv32) >> 16;
					if (sv32_high == 0)
					{
						WRITE_BIT2_DECODE_B4_INT32(*((const int*)s));
						s += 4; sel += 16; p += 16;
					} else if (sv32_high == 0xFFFF)
					{
						s += 4; sel += 16;
					} else {
						WRITE_BIT2_SEL_DECODE
						WRITE_BIT2_SEL_DECODE
						WRITE_BIT2_SEL_DECODE
						WRITE_BIT2_SEL_DECODE
					}
				}
			}
		#endif
			for (; n_byte >= 4; n_byte -= 4)
			{
				__m128i sv = _mm_loadu_si128((__m128i const*)sel);
				const __m128i zero = _mm_setzero_si128();
				sv = _mm_cmpeq_epi8(sv, zero);
				int sv16 = _mm_movemask_epi8(sv);
				if (sv16 == 0)
				{
					WRITE_BIT2_DECODE_B4_INT32(*((const int*)s));
					s += 4; p += 16; sel += 16;
				} else if (sv16 == 0xFFFF)
				{
					s += 4; sel += 16;
				} else {
					WRITE_BIT2_SEL_DECODE
					WRITE_BIT2_SEL_DECODE
					WRITE_BIT2_SEL_DECODE
					WRITE_BIT2_SEL_DECODE
				}
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT2_SEL_DECODE
			return p;
		}

		inline static const C_Int32 *Encode(const C_Int32 *s, C_UInt8 *p,
			size_t n_byte)
		{
			for (; n_byte >= 4; n_byte-=4)
			{
				__m128i mask = BIT2_UInt32_x03;
				__m128i v = _mm_packs_epi32(_mm_loadu_si128((__m128i const*)s) & mask,
					_mm_loadu_si128((__m128i const*)(s+4)) & mask);
				int r1 = _mm_movemask_epi8(_mm_slli_epi16(v, 7) | _mm_slli_epi16(v, 14));
				s += 8;
				v = _mm_packs_epi32(_mm_loadu_si128((__m128i const*)s) & mask,
					_mm_loadu_si128((__m128i const*)(s+4)) & mask);
				int r2 = _mm_movemask_epi8(_mm_slli_epi16(v, 7) | _mm_slli_epi16(v, 14));
				s += 8;
				*((C_Int32*)p) = r1 | (r2 << 16);
				p += 4;
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT2_ENCODE
			return s;
		}
	};

	template<> struct COREARRAY_DLL_LOCAL BIT2_CONV<C_UInt32>
	{
		inline static C_UInt32* Decode(const C_UInt8 *s, size_t n_byte, C_UInt32 *p)
		{
			return (C_UInt32*)BIT2_CONV<C_Int32>::Decode(s, n_byte, (C_Int32*)p);
		}
		inline static C_UInt32* Decode2(const C_UInt8 *s, size_t n_byte, C_UInt32 *p,
			const C_BOOL sel[])
		{
			return (C_UInt32*)BIT2_CONV<C_Int32>::Decode2(s, n_byte, (C_Int32*)p, sel);
		}
		inline static const C_UInt32 *Encode(const C_UInt32 *s, C_UInt8 *p,
			size_t n_byte)
		{
			return (C_UInt32*)BIT2_CONV<C_Int32>::Encode((C_Int32*)s, p, n_byte);
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
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE] COREARRAY_SIMD_ATTR_ALIGN;
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
				p = BIT2_CONV<MEM_TYPE>::Decode(Buffer, L, p);
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

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			// buffer
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE] COREARRAY_SIMD_ATTR_ALIGN;
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
				p = BIT2_CONV<MEM_TYPE>::Decode2(Buffer, L, p, sel);
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
			C_UInt8 Buffer[MEMORY_BUFFER_SIZE] COREARRAY_SIMD_ATTR_ALIGN;
			while (n >= 4)
			{
				size_t nn = n >> 2;
				if (nn > MEMORY_BUFFER_SIZE) nn = MEMORY_BUFFER_SIZE;
				p = BIT2_CONV<MEM_TYPE>::Encode(p, Buffer, nn);
				I.Allocator->WriteData(Buffer, nn);
				n -= (nn << 2);
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
