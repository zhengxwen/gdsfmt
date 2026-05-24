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
// Copyright (C) 2007-2026    Xiuwen Zheng
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
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2007 - 2026
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


#if defined(COREARRAY_SIMD_SSE2) || defined(COREARRAY_SIMD_NEON)

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

#endif


#ifdef COREARRAY_SIMD_SSE2

	#define WRITE_BIT2_SEL_DECODE_B4    \
		{ \
			WRITE_BIT2_SEL_DECODE \
			WRITE_BIT2_SEL_DECODE \
			WRITE_BIT2_SEL_DECODE \
			WRITE_BIT2_SEL_DECODE \
		}

	#define WRITE_BIT2_SEL_DECODE_B4_PACKED(v_b32, sel_b16)    \
		{ \
			sel_b16 = ~sel_b16; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32 & 0x03; \
			sel_b16 >>= 1; v_b32 >>= 2; \
			if (sel_b16 & 0x01) *p++ = v_b32; \
		}


	static const __m128i BIT2_REP_x03 = _mm_set1_epi8(0x03);
	static const __m128i BIT2_UInt16_x03 = _mm_set1_epi16(0x03);
	static const __m128i BIT2_UInt32_x03 = _mm_set1_epi32(0x03);

#ifdef COREARRAY_SIMD_AVX2
	static const __m256i BIT2_AVX_REP_x03 = _mm256_set1_epi8(0x03);
	static const __m256i BIT2_AVX_UInt32_x03 = _mm256_set1_epi32(0x03);
	static const __m256i BIT2_AVX_UInt64_SHR = _mm256_set_epi64x(0, 32, 0, 0);
#endif

	#define WRITE_BIT2_DECODE_B4_UINT8_RAW(val)    \
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

	#define WRITE_BIT2_DECODE_B4_UINT8(val)    \
		{ \
			C_UInt32 vv = val; \
			if (vv == 0) \
			{ \
				_mm_storeu_si128((__m128i*)p, _mm_setzero_si128()); \
			} else { \
				WRITE_BIT2_DECODE_B4_UINT8_RAW(vv) \
			} \
		}


	#define WRITE_BIT2_DECODE_B4_INT32_RAW(val)    \
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

	#define WRITE_BIT2_DECODE_B4_INT32(val)    \
		{ \
			if (val == 0) \
			{ \
				__m128i zero = _mm_setzero_si128(); \
				__m128i *pp = (__m128i*)p; \
				_mm_storeu_si128(pp, zero); \
				_mm_storeu_si128(pp+1, zero); \
				_mm_storeu_si128(pp+2, zero); \
				_mm_storeu_si128(pp+3, zero); \
			} else \
				WRITE_BIT2_DECODE_B4_INT32_RAW(val) \
		}


	#define WRITE_BIT2_ZERO_FILL(size)    \
		if (zero_len > 0) \
		{ \
			memset(p, 0, size); \
			p += zero_len; zero_len = 0; \
		}


	template<> struct COREARRAY_DLL_LOCAL BIT2_CONV<C_UInt8>
	{
		inline static C_UInt8* Decode(const C_UInt8 *s, size_t n_byte, C_UInt8 *p)
		{
		#ifdef COREARRAY_SIMD_AVX2
			for (; n_byte >= 32; n_byte-=32)
			{
				__m256i v = _mm256_loadu_si256((__m256i const*)s); s += 32;
				if (_mm256_testz_si256(v, v))
				{
					__m256i zero = _mm256_setzero_si256();
					_mm256_storeu_si256((__m256i*)p, zero); p += 32;
					_mm256_storeu_si256((__m256i*)p, zero); p += 32;
					_mm256_storeu_si256((__m256i*)p, zero); p += 32;
					_mm256_storeu_si256((__m256i*)p, zero); p += 32;
				} else {
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
			}
		#endif
			for (; n_byte >= 16; n_byte-=16)
			{
				__m128i v = _mm_loadu_si128((__m128i const*)s);
				s += 16;
				__m128i zero = _mm_setzero_si128();
				if (_mm_movemask_epi8(_mm_cmpeq_epi8(v, zero))==0xFFFF)
				{
					_mm_storeu_si128((__m128i*)p, zero); p += 16;
					_mm_storeu_si128((__m128i*)p, zero); p += 16;
					_mm_storeu_si128((__m128i*)p, zero); p += 16;
					_mm_storeu_si128((__m128i*)p, zero); p += 16;
				} else {
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
			}
			for (; n_byte >= 4; n_byte-=4)
			{
				WRITE_BIT2_DECODE_B4_UINT8(*((const C_UInt32*)s))
				s += 4; p += 16;
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT2_DECODE
			return p;
		}

		inline static C_UInt8* Decode2(const C_UInt8 *s, size_t n_byte, C_UInt8 *p,
			const C_BOOL sel[])
		{
			size_t zero_len = 0;

		#ifdef COREARRAY_SIMD_AVX2
			for (; n_byte >= 8; n_byte -= 8)
			{
				__m256i sv = _mm256_loadu_si256((__m256i const*)sel);
				sv = _mm256_cmpeq_epi8(sv, _mm256_setzero_si256());
				sel += 32;
				C_UInt64 vv = *((const C_UInt64*)s);
				s += 8;
				if (vv == 0)
				{
					zero_len += 32 - _mm_popcnt_u32(_mm256_movemask_epi8(sv));
				} else {
					int sv32 = _mm256_movemask_epi8(sv);
					if (sv32 == 0)  // all selected
					{
						WRITE_BIT2_ZERO_FILL(zero_len)
						__m256i v = _mm256_set1_epi64x(vv);
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
						p += 32;
					} else if (sv32 != -1)  // at least one selected
					{
						// low 16 bits
						int sv32_low = sv32 & 0xFFFF;
						if (sv32_low == 0)  // all selected
						{
							WRITE_BIT2_ZERO_FILL(zero_len)
							WRITE_BIT2_DECODE_B4_UINT8_RAW(vv)
							p += 16;
						} else if (sv32_low != 0xFFFF)  // at least one selected
						{
							WRITE_BIT2_ZERO_FILL(zero_len)
							C_UInt32 vvv = vv;
							WRITE_BIT2_SEL_DECODE_B4_PACKED(vvv, sv32_low)
						}
						// high 16 bits
						int sv32_high = C_UInt32(sv32) >> 16;
						if (sv32_high == 0)  // all selected
						{
							WRITE_BIT2_ZERO_FILL(zero_len)
							WRITE_BIT2_DECODE_B4_UINT8_RAW(vv >> 32)
							p += 16;
						} else if (sv32_high != 0xFFFF)  // at least one selected
						{
							WRITE_BIT2_ZERO_FILL(zero_len)
							C_UInt32 vvv = vv >> 32;
							WRITE_BIT2_SEL_DECODE_B4_PACKED(vvv, sv32_high)
						}
					}
				}
			}
		#endif
			for (; n_byte >= 4; n_byte -= 4)
			{
				__m128i sv = _mm_loadu_si128((__m128i const*)sel);
				sv = _mm_cmpeq_epi8(sv, _mm_setzero_si128());
				sel += 16;
				C_UInt32 vv = *((const C_UInt32*)s);
				s += 4;
				if (vv == 0)
				{
				#ifdef COREARRAY_POPCNT
					zero_len += 16 - _mm_popcnt_u32(_mm_movemask_epi8(sv));
				#else
					// calculate the number of zeros
					sv = _mm_add_epi8(sv, _mm_shuffle_epi32(sv, _MM_SHUFFLE(1,0,3,2)));
					sv = _mm_add_epi8(sv, _mm_shuffle_epi32(sv, _MM_SHUFFLE(0,0,0,1)));
					sv = _mm_add_epi8(sv, _mm_shufflelo_epi16(sv, _MM_SHUFFLE(0,0,0,1)));
					int x = _mm_cvtsi128_si32(sv);
					zero_len += (C_Int8(x) + C_Int8(x >> 8)) + 16;
				#endif
				} else {
					int sv16 = _mm_movemask_epi8(sv);
					if (sv16 == 0)  // all selected
					{
						WRITE_BIT2_ZERO_FILL(zero_len)
						WRITE_BIT2_DECODE_B4_UINT8_RAW(vv)
						p += 16;
					} else if (sv16 != 0xFFFF)  // at least one selected
					{
						WRITE_BIT2_ZERO_FILL(zero_len)
						WRITE_BIT2_SEL_DECODE_B4_PACKED(vv, sv16)
					}
				}
			}
			WRITE_BIT2_ZERO_FILL(zero_len)
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
				WRITE_BIT2_DECODE_B4_INT32(*((const C_UInt32*)s));
				s += 4; p += 16;
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT2_DECODE
			return p;
		}

		inline static C_Int32* Decode2(const C_UInt8 *s, size_t n_byte,
			C_Int32 *p, const C_BOOL sel[])
		{
			size_t zero_len = 0;

		#ifdef COREARRAY_SIMD_AVX2
			const __m256i zero = _mm256_setzero_si256();
			for (; n_byte >= 8; n_byte -= 8)
			{
				__m256i sv = _mm256_loadu_si256((__m256i const*)sel);
				sv = _mm256_cmpeq_epi8(sv, zero);
				sel += 32;
				C_UInt64 vv = *((const C_UInt64*)s);
				s += 8;
				if (vv == 0)
				{
					zero_len += 32 - _mm_popcnt_u32(_mm256_movemask_epi8(sv));
				} else {
					int sv32 = _mm256_movemask_epi8(sv);
					if (sv32 == 0)  // all selected
					{
						WRITE_BIT2_ZERO_FILL(zero_len << 2)

						__m256i v = _mm256_set1_epi64x(vv);
						v = _mm256_srlv_epi64(v, BIT2_AVX_UInt64_SHR);
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
					} else if (sv32 != -1)  // at least one selected
					{
						// low 16 bits
						int sv32_low = sv32 & 0xFFFF;
						if (sv32_low == 0)  // all selected
						{
							WRITE_BIT2_ZERO_FILL(zero_len << 2)
							WRITE_BIT2_DECODE_B4_INT32_RAW(vv)
							p += 16;
						} else if (sv32_low != 0xFFFF)  // at least one selected
						{
							WRITE_BIT2_ZERO_FILL(zero_len << 2)
							C_UInt32 vvv = vv;
							WRITE_BIT2_SEL_DECODE_B4_PACKED(vvv, sv32_low)
						}
						// high 16 bits
						int sv32_high = C_UInt32(sv32) >> 16;
						if (sv32_high == 0)  // all selected
						{
							WRITE_BIT2_ZERO_FILL(zero_len << 2)
							WRITE_BIT2_DECODE_B4_INT32_RAW(vv >> 32)
							p += 16;
						} else if (sv32_high != 0xFFFF)  // at least one selected
						{
							WRITE_BIT2_ZERO_FILL(zero_len << 2)
							C_UInt32 vvv = vv >> 32;
							WRITE_BIT2_SEL_DECODE_B4_PACKED(vvv, sv32_high)
						}
					}
				}
			}
		#endif
			for (; n_byte >= 4; n_byte -= 4)
			{
				__m128i sv = _mm_loadu_si128((__m128i const*)sel);
				sv = _mm_cmpeq_epi8(sv, _mm_setzero_si128());
				sel += 16;
				C_UInt32 vv = *((const C_UInt32*)s);
				s += 4;
				if (vv == 0)
				{
				#ifdef COREARRAY_POPCNT
					zero_len += 16 - _mm_popcnt_u32(_mm_movemask_epi8(sv));
				#else
					// calculate the number of zeros
					sv = _mm_add_epi8(sv, _mm_shuffle_epi32(sv, _MM_SHUFFLE(1,0,3,2)));
					sv = _mm_add_epi8(sv, _mm_shuffle_epi32(sv, _MM_SHUFFLE(0,0,0,1)));
					sv = _mm_add_epi8(sv, _mm_shufflelo_epi16(sv, _MM_SHUFFLE(0,0,0,1)));
					int x = _mm_cvtsi128_si32(sv);
					zero_len += (C_Int8(x) + C_Int8(x >> 8)) + 16;
				#endif
				} else {
					int sv16 = _mm_movemask_epi8(sv);
					if (sv16 == 0)  // all selected
					{
						WRITE_BIT2_ZERO_FILL(zero_len << 2)
						WRITE_BIT2_DECODE_B4_INT32_RAW(vv)
						p += 16;
					} else if (sv16 != 0xFFFF)  // at least one selected
					{
						WRITE_BIT2_ZERO_FILL(zero_len << 2)
						WRITE_BIT2_SEL_DECODE_B4_PACKED(vv, sv16)
					}
				}
			}
			WRITE_BIT2_ZERO_FILL(zero_len << 2)
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

#elif defined(COREARRAY_SIMD_NEON)

	template<> struct COREARRAY_DLL_LOCAL BIT2_CONV<C_UInt8>
	{
		inline static C_UInt8* Decode(const C_UInt8 *s, size_t n_byte, C_UInt8 *p)
		{
			const uint8x16_t mask03 = vdupq_n_u8(0x03);
			for (; n_byte >= 16; n_byte -= 16)
			{
				uint8x16_t v = vld1q_u8(s); s += 16;
				// check if all zero
				uint8x16_t vz = vceqq_u8(v, vdupq_n_u8(0));
				uint64x2_t vz64 = vreinterpretq_u64_u8(vz);
				if (vgetq_lane_u64(vz64, 0) == ~(uint64_t)0 &&
				    vgetq_lane_u64(vz64, 1) == ~(uint64_t)0)
				{
					// all bytes zero, output 64 zero bytes
					vst1q_u8(p, vdupq_n_u8(0)); p += 16;
					vst1q_u8(p, vdupq_n_u8(0)); p += 16;
					vst1q_u8(p, vdupq_n_u8(0)); p += 16;
					vst1q_u8(p, vdupq_n_u8(0)); p += 16;
				} else {
					// extract 2-bit values: v1=bits[1:0], v2=bits[3:2], v3=bits[5:4], v4=bits[7:6]
					uint8x16_t v1 = vandq_u8(v, mask03);
					uint8x16_t v2 = vandq_u8(vshrq_n_u8(v, 2), mask03);
					uint8x16_t v3 = vandq_u8(vshrq_n_u8(v, 4), mask03);
					uint8x16_t v4 = vshrq_n_u8(v, 6);

					// interleave: v1[0],v2[0],v3[0],v4[0], v1[1],v2[1],...
					uint8x16x2_t z12 = vzipq_u8(v1, v2);  // {v1[0],v2[0],v1[1],v2[1],...}
					uint8x16x2_t z34 = vzipq_u8(v3, v4);  // {v3[0],v4[0],v3[1],v4[1],...}

					// now interleave 16-bit pairs
					uint16x8_t z12_lo = vreinterpretq_u16_u8(z12.val[0]);
					uint16x8_t z34_lo = vreinterpretq_u16_u8(z34.val[0]);
					uint16x8x2_t w_lo = vzipq_u16(z12_lo, z34_lo);

					uint16x8_t z12_hi = vreinterpretq_u16_u8(z12.val[1]);
					uint16x8_t z34_hi = vreinterpretq_u16_u8(z34.val[1]);
					uint16x8x2_t w_hi = vzipq_u16(z12_hi, z34_hi);

					vst1q_u8(p, vreinterpretq_u8_u16(w_lo.val[0])); p += 16;
					vst1q_u8(p, vreinterpretq_u8_u16(w_lo.val[1])); p += 16;
					vst1q_u8(p, vreinterpretq_u8_u16(w_hi.val[0])); p += 16;
					vst1q_u8(p, vreinterpretq_u8_u16(w_hi.val[1])); p += 16;
				}
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT2_DECODE
			return p;
		}

		inline static C_UInt8* Decode2(const C_UInt8 *s, size_t n_byte, C_UInt8 *p,
			const C_BOOL sel[])
		{
			const uint8x16_t mask03 = vdupq_n_u8(0x03);
			for (; n_byte >= 4; n_byte -= 4)
			{
				// load 16 selection bytes
				uint8x16_t sv = vld1q_u8((const uint8_t*)sel);
				sel += 16;
				C_UInt32 vv = *((const C_UInt32*)s);
				s += 4;
				if (vv == 0)
				{
					// count selected elements (non-zero sel values)
					uint8x16_t cmp = vceqq_u8(sv, vdupq_n_u8(0));
					// count zeros in cmp (each 0xFF means sel was 0)
					int cnt = 16;
					uint8_t tmp[16];
					vst1q_u8(tmp, cmp);
					for (int i = 0; i < 16; i++)
						if (tmp[i]) cnt--;
					for (int i = 0; i < cnt; i++)
						*p++ = 0;
				} else {
					// check if all selected
					uint8x16_t cmp = vceqq_u8(sv, vdupq_n_u8(0));
					uint64x2_t cmp64 = vreinterpretq_u64_u8(cmp);
					if (vgetq_lane_u64(cmp64, 0) == 0 &&
					    vgetq_lane_u64(cmp64, 1) == 0)
					{
						// all selected - decode all 16 values
						uint8x16_t v = vreinterpretq_u8_u32(vdupq_n_u32(vv));
						uint8x16_t v1 = vandq_u8(v, mask03);
						uint8x16_t v2 = vandq_u8(vshrq_n_u8(v, 2), mask03);
						uint8x16_t v3 = vandq_u8(vshrq_n_u8(v, 4), mask03);
						uint8x16_t v4 = vshrq_n_u8(v, 6);
						uint8x16x2_t z12 = vzipq_u8(v1, v2);
						uint8x16x2_t z34 = vzipq_u8(v3, v4);
						uint16x8_t z12_lo = vreinterpretq_u16_u8(z12.val[0]);
						uint16x8_t z34_lo = vreinterpretq_u16_u8(z34.val[0]);
						uint16x8x2_t w = vzipq_u16(z12_lo, z34_lo);
						vst1q_u8(p, vreinterpretq_u8_u16(w.val[0]));
						p += 16;
					} else {
						// partial selection - scalar fallback
						const C_BOOL *sel2 = sel - 16;
						const C_UInt8 *s2 = s - 4;
						for (int i = 0; i < 4; i++)
						{
							C_UInt8 Ch = s2[i];
							if (*sel2++) *p++ = Ch & 0x03;
							if (*sel2++) *p++ = (Ch >> 2) & 0x03;
							if (*sel2++) *p++ = (Ch >> 4) & 0x03;
							if (*sel2++) *p++ = Ch >> 6;
						}
					}
				}
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT2_SEL_DECODE
			return p;
		}

		inline static const C_UInt8 *Encode(const C_UInt8 *s, C_UInt8 *p,
			size_t n_byte)
		{
			const uint8x16_t mask03 = vdupq_n_u8(0x03);
			for (; n_byte >= 16; n_byte -= 16)
			{
				// load 64 bytes (16 groups of 4 values)
				uint8x16_t s0 = vld1q_u8(s);       // values 0..15
				uint8x16_t s1 = vld1q_u8(s + 16);  // values 16..31
				uint8x16_t s2 = vld1q_u8(s + 32);  // values 32..47
				uint8x16_t s3 = vld1q_u8(s + 48);  // values 48..63
				s += 64;

				// mask to 2 bits
				s0 = vandq_u8(s0, mask03);
				s1 = vandq_u8(s1, mask03);
				s2 = vandq_u8(s2, mask03);
				s3 = vandq_u8(s3, mask03);

				// pack: for each group of 4 bytes at positions 4i..4i+3,
				// result[i] = s[4i] | (s[4i+1]<<2) | (s[4i+2]<<4) | (s[4i+3]<<6)
				// Use shift-and-or with uzp to gather even/odd indexed bytes

				// Combine s0 pairs: s0[0]|s0[1]<<2, s0[2]|s0[3]<<2, ...
				// Use the fact that for 16 input bytes we get 4 output bytes
				uint8_t buf[64];
				vst1q_u8(buf, s0);
				vst1q_u8(buf + 16, s1);
				vst1q_u8(buf + 32, s2);
				vst1q_u8(buf + 48, s3);
				for (int i = 0; i < 16; i++)
				{
					p[i] = (buf[4*i]) | (buf[4*i+1] << 2) |
					        (buf[4*i+2] << 4) | (buf[4*i+3] << 6);
				}
				p += 16;
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

	template<> struct COREARRAY_DLL_LOCAL BIT2_CONV<C_Int32>
	{
		inline static C_Int32* Decode(const C_UInt8 *s, size_t n_byte, C_Int32 *p)
		{
			const uint32x4_t mask03_32 = vdupq_n_u32(0x03);
			for (; n_byte >= 4; n_byte -= 4)
			{
				C_UInt32 val = *((const C_UInt32*)s); s += 4;
				if (val == 0)
				{
					uint32x4_t zero = vdupq_n_u32(0);
					vst1q_u32((uint32_t*)p, zero);
					vst1q_u32((uint32_t*)(p+4), zero);
					vst1q_u32((uint32_t*)(p+8), zero);
					vst1q_u32((uint32_t*)(p+12), zero);
				} else {
					// Decode each byte into 4 x int32 values
					// byte k: extract bits [1:0],[3:2],[5:4],[7:6] -> 4 int32's
					for (int k = 0; k < 4; k++)
					{
						C_UInt8 b = (val >> (k*8)) & 0xFF;
						uint32x4_t vb = vdupq_n_u32(b);
						// shift amounts: 0, 2, 4, 6
						static const int32_t shifts[4] = {0, 2, 4, 6};
						int32x4_t sh = vld1q_s32(shifts);
						uint32x4_t shifted = vshlq_u32(vb, vnegq_s32(sh));
						uint32x4_t masked = vandq_u32(shifted, mask03_32);
						vst1q_u32((uint32_t*)(p + k*4), masked);
					}
				}
				p += 16;
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT2_DECODE
			return p;
		}

		inline static C_Int32* Decode2(const C_UInt8 *s, size_t n_byte,
			C_Int32 *p, const C_BOOL sel[])
		{
			const uint32x4_t mask03_32 = vdupq_n_u32(0x03);
			for (; n_byte >= 4; n_byte -= 4)
			{
				// load 16 selection bytes
				uint8x16_t sv = vld1q_u8((const uint8_t*)sel);
				sel += 16;
				C_UInt32 vv = *((const C_UInt32*)s);
				s += 4;
				if (vv == 0)
				{
					// count selected (non-zero sel)
					uint8x16_t cmp = vceqq_u8(sv, vdupq_n_u8(0));
					uint8x16_t ones = vandq_u8(cmp, vdupq_n_u8(1));
					size_t cnt = 16 - (size_t)vaddlvq_u8(ones);
					memset(p, 0, cnt * sizeof(C_Int32));
					p += cnt;
				} else {
					// check if all selected
					uint8x16_t cmp = vceqq_u8(sv, vdupq_n_u8(0));
					uint64x2_t cmp64 = vreinterpretq_u64_u8(cmp);
					if (vgetq_lane_u64(cmp64, 0) == 0 &&
					    vgetq_lane_u64(cmp64, 1) == 0)
					{
						// all selected - vectorized decode per byte
						static const int32_t shifts[4] = {0, 2, 4, 6};
						int32x4_t sh = vld1q_s32(shifts);
						int32x4_t neg_sh = vnegq_s32(sh);
						for (int k = 0; k < 4; k++)
						{
							C_UInt8 b = (vv >> (k*8)) & 0xFF;
							uint32x4_t vb = vdupq_n_u32(b);
							uint32x4_t shifted = vshlq_u32(vb, neg_sh);
							uint32x4_t masked = vandq_u32(shifted, mask03_32);
							vst1q_u32((uint32_t*)(p + k*4), masked);
						}
						p += 16;
					} else {
						// partial selection - scalar
						const C_BOOL *sel2 = sel - 16;
						const C_UInt8 *s2 = s - 4;
						for (int i = 0; i < 4; i++)
						{
							C_UInt8 Ch = s2[i];
							if (*sel2++) *p++ = Ch & 0x03;
							if (*sel2++) *p++ = (Ch >> 2) & 0x03;
							if (*sel2++) *p++ = (Ch >> 4) & 0x03;
							if (*sel2++) *p++ = Ch >> 6;
						}
					}
				}
			}
			for (; n_byte > 0; n_byte--) WRITE_BIT2_SEL_DECODE
			return p;
		}

		inline static const C_Int32 *Encode(const C_Int32 *s, C_UInt8 *p,
			size_t n_byte)
		{
			const uint32x4_t mask03_32 = vdupq_n_u32(0x03);
			for (; n_byte >= 4; n_byte -= 4)
			{
				// encode 16 int32 values into 4 bytes
				// load 16 int32 values in 4 vector registers
				uint32x4_t q0 = vld1q_u32((const uint32_t*)s);
				uint32x4_t q1 = vld1q_u32((const uint32_t*)(s+4));
				uint32x4_t q2 = vld1q_u32((const uint32_t*)(s+8));
				uint32x4_t q3 = vld1q_u32((const uint32_t*)(s+12));
				s += 16;

				// mask to 2 bits
				q0 = vandq_u32(q0, mask03_32);
				q1 = vandq_u32(q1, mask03_32);
				q2 = vandq_u32(q2, mask03_32);
				q3 = vandq_u32(q3, mask03_32);

				// narrow 32->16->8
				uint16x4_t h0 = vmovn_u32(q0);
				uint16x4_t h1 = vmovn_u32(q1);
				uint16x4_t h2 = vmovn_u32(q2);
				uint16x4_t h3 = vmovn_u32(q3);
				uint16x8_t h01 = vcombine_u16(h0, h1);
				uint16x8_t h23 = vcombine_u16(h2, h3);
				uint8x8_t b01 = vmovn_u16(h01);
				uint8x8_t b23 = vmovn_u16(h23);
				uint8x16_t bytes = vcombine_u8(b01, b23);

				// Now bytes[i] has the 2-bit value for element i (0..15)
				// Pack groups of 4: result[j] = bytes[4j] | (bytes[4j+1]<<2) | (bytes[4j+2]<<4) | (bytes[4j+3]<<6)
				uint8_t tmp[16];
				vst1q_u8(tmp, bytes);
				for (int j = 0; j < 4; j++)
				{
					p[j] = tmp[4*j] | (tmp[4*j+1] << 2) |
					        (tmp[4*j+2] << 4) | (tmp[4*j+3] << 6);
				}
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
			if (n <= 0) return p;
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
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr++;
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
			if (n <= 0) return p;
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
			if (n <= 0) return p;
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
				ssize_t nn = n >> 2;
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
