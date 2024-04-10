// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dRealGDS.h: Packed real number in GDS format
//
// Copyright (C) 2015-2024    Xiuwen Zheng
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
 *	\file     dRealGDS.h
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2015-2024
 *	\brief    Packed real number in GDS format
 *	\details
**/

#ifndef _HEADER_COREARRAY_REAL_GDS_
#define _HEADER_COREARRAY_REAL_GDS_

#include "dStruct.h"
#include <math.h>
#include <typeinfo>

#ifdef COREARRAY_SIMD_SSE2
#   include <xmmintrin.h>
#   include <emmintrin.h>
#endif


namespace CoreArray
{
	using namespace std;

	/// define 8-bit packed real number (signed int)
	typedef struct { C_Int8 Val; } TReal8;
	/// define 8-bit packed real number (unsigned int)
	typedef struct { C_UInt8 Val; } TReal8u;

	/// define 16-bit packed real number (signed int)
	typedef struct { C_Int16 Val; } TReal16;
	/// define 16-bit packed real number (unsigned int)
	typedef struct { C_UInt16 Val; } TReal16u;

	/// define 24-bit packed real number (signed int)
	typedef struct { C_Int8 Val[3]; } TReal24;
	/// define 24-bit packed real number (unsigned int)
	typedef struct { C_UInt8 Val[3]; } TReal24u;

	/// define 32-bit packed real number (signed int)
	typedef struct { C_Int32 Val; } TReal32;
	/// define 32-bit packed real number (unsigned int)
	typedef struct { C_UInt32 Val; } TReal32u;


	/// Traits of 8-bit packed real number (signed int)
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TReal8>
	{
		typedef C_Float64 TType;
		typedef C_Int8 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 8u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal8"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static C_Float64 InitialOffset() { return 0; }
		static C_Float64 InitialScale() { return 0.01; }
		static const C_Int8 MissingValue = (C_Int8)0x80;
		static const size_t LookupTableSize = 256;
	};

	/// Traits of 8-bit packed real number (unsigned int)
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TReal8u>
	{
		typedef C_Float64 TType;
		typedef C_UInt8 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 8u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal8U"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static C_Float64 InitialOffset() { return 0; }
		static C_Float64 InitialScale() { return 0.01; }
		static const C_UInt8 MissingValue = 0xFF;
		static const size_t LookupTableSize = 256;
	};

	/// Traits of 16-bit packed real number (signed int)
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TReal16>
	{
		typedef C_Float64 TType;
		typedef C_Int16 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 16u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal16"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static C_Float64 InitialOffset() { return 0; }
		static C_Float64 InitialScale() { return 0.0001; }
		static const C_Int16 MissingValue = (C_Int16)0x8000;
		static const size_t LookupTableSize = 1;
	};

	/// Traits of 16-bit packed real number (unsigned int)
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TReal16u>
	{
		typedef C_Float64 TType;
		typedef C_UInt16 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 16u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal16U"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static C_Float64 InitialOffset() { return 0; }
		static C_Float64 InitialScale() { return 0.0001; }
		static const C_UInt16 MissingValue = 0xFFFF;
		static const size_t LookupTableSize = 1;
	};

	/// Traits of 24-bit packed real number (signed int)
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TReal24>
	{
		typedef C_Float64 TType;
		typedef C_Int32 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 24u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal24"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static C_Float64 InitialOffset() { return 0; }
		static C_Float64 InitialScale() { return 0.00001; }
		static const C_Int32 MissingValue = 0x800000;
		static const size_t LookupTableSize = 1;
	};

	/// Traits of 24-bit packed real number (unsigned int)
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TReal24u>
	{
		typedef C_Float64 TType;
		typedef C_UInt32 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 24u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal24U"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static C_Float64 InitialOffset() { return 0; }
		static C_Float64 InitialScale() { return 0.00001; }
		static const C_UInt32 MissingValue = 0xFFFFFF;
		static const size_t LookupTableSize = 1;
	};

	/// Traits of 32-bit packed real number (signed int)
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TReal32>
	{
		typedef C_Float64 TType;
		typedef C_Int32 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal32"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static C_Float64 InitialOffset() { return 0; }
		static C_Float64 InitialScale() { return 0.000001; }
		static const C_Int32 MissingValue = 0x80000000;
		static const size_t LookupTableSize = 1;
	};

	/// Traits of 32-bit packed real number (unsigned int)
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TReal32u>
	{
		typedef C_Float64 TType;
		typedef C_UInt32 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal32U"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static C_Float64 InitialOffset() { return 0; }
		static C_Float64 InitialScale() { return 0.000001; }
		static const C_UInt32 MissingValue = 0xFFFFFFFF;
		static const size_t LookupTableSize = 1;
	};



	// =====================================================================
	// Packed real number classes of GDS format
	// =====================================================================

	/// Container of packed real number
	/** \tparam REAL_TYPE    should be TReal8, TReal16, TReal24 or TReal32
	**/
	template<typename REAL_TYPE>
		class COREARRAY_DLL_DEFAULT CdPackedReal: public CdArray<REAL_TYPE>
	{
	public:
		typedef REAL_TYPE ElmType;

		CdPackedReal(): CdArray<REAL_TYPE>()
		{
			fOffset = TdTraits<REAL_TYPE>::InitialOffset();
			fScale  = TdTraits<REAL_TYPE>::InitialScale();
			fInvScale = 1.0 / fScale;
			_ChangeLookup();
		}

		virtual CdGDSObj *NewObject()
		{
			return (new CdPackedReal<REAL_TYPE>())->AssignPipe(*this);
		}

		/// append new data from an iterator
		virtual void AppendIter(CdIterator &I, C_Int64 Count)
		{
			if ((Count >= 65536) && (typeid(*this) == typeid(*I.Handler)))
			{
				CdPackedReal<REAL_TYPE> *Src = (CdPackedReal<REAL_TYPE> *)I.Handler;
				if ((this->fOffset == Src->fOffset) &&
					(this->fScale == Src->fScale) &&
					this->fAllocator.BufStream())
				{
					Src->Allocator().BufStream()->FlushWrite();
					this->fAllocator.BufStream()->CopyFrom(
						*(Src->Allocator().BufStream()->Stream()),
						I.Ptr, Count * this->fElmSize);

					// check
					CdAllocArray::TDimItem &R = this->fDimension.front();
					this->fTotalCount += Count;
					if (this->fTotalCount >= R.DimElmCnt*(R.DimLen+1))
					{
						R.DimLen = this->fTotalCount / R.DimElmCnt;
						this->fNeedUpdate = true;
					}

					return;
				}
			}
			CdAbstractArray::AppendIter(I, Count);
		}

		COREARRAY_INLINE C_Float64 Offset() const
		{
			return fOffset;
		}
		void SetOffset(C_Float64 val)
		{
			if (val != fOffset)
			{
				fOffset = val;
				_ChangeLookup();
				this->fChanged = true;
			}
		}

		COREARRAY_INLINE C_Float64 Scale() const
		{
			return fScale;
		}
		void SetScale(C_Float64 val)
		{
			if (val != fScale)
			{
				fScale = val; fInvScale = 1.0 / fScale;
				_ChangeLookup();
				this->fChanged = true;
			}
		}

		COREARRAY_INLINE C_Float64 InvScale() const
		{
			return fInvScale;
		}

		COREARRAY_INLINE const C_Float64 *LookupTable() const
		{
			return _LookupTable;
		}

	protected:

		/// loading function for serialization
		virtual void Loading(CdReader &Reader, TdVersion Version)
		{
			CdAllocArray::Loading(Reader, Version);
			Reader["OFFSET"] >> fOffset;
			Reader["SCALE"]  >> fScale;
			fInvScale = 1.0 / fScale;
			_ChangeLookup();
		}

		/// saving function for serialization
		virtual void Saving(CdWriter &Writer)
		{
			CdAllocArray::Saving(Writer);
			Writer["OFFSET"] << fOffset;
			Writer["SCALE"]  << fScale;
		}

		C_Float64 fOffset;
		C_Float64 fScale;
		C_Float64 fInvScale;
		C_Float64 _LookupTable[TdTraits<REAL_TYPE>::LookupTableSize];

		void _ChangeLookup()
		{
			for (size_t k=0; k < TdTraits<REAL_TYPE>::LookupTableSize; k++)
			{
				typename TdTraits<REAL_TYPE>::ElmType I = k;
				if (I != TdTraits<REAL_TYPE>::MissingValue)
					_LookupTable[k] = fScale*I + fOffset;
				else
					_LookupTable[k] = NaN;
			}
		}
	};


	// =====================================================================
	// Packed real numbers in GDS files
	// =====================================================================

	typedef CdPackedReal<TReal8>      CdPackedReal8;
	typedef CdPackedReal<TReal8u>     CdPackedReal8U;
	typedef CdPackedReal<TReal16>     CdPackedReal16;
	typedef CdPackedReal<TReal16u>    CdPackedReal16U;
	typedef CdPackedReal<TReal24>     CdPackedReal24;
	typedef CdPackedReal<TReal24u>    CdPackedReal24U;
	typedef CdPackedReal<TReal32>     CdPackedReal32;
	typedef CdPackedReal<TReal32u>    CdPackedReal32U;

	/// get whether it is packed real or not
	static inline bool IsPackedReal(CdGDSObj *Obj)
	{
		return dynamic_cast<CdPackedReal8*>(Obj) ||
			dynamic_cast<CdPackedReal8U*>(Obj)   ||
			dynamic_cast<CdPackedReal16*>(Obj)   ||
			dynamic_cast<CdPackedReal16U*>(Obj)  ||
			dynamic_cast<CdPackedReal24*>(Obj)   ||
			dynamic_cast<CdPackedReal24U*>(Obj)  ||
			dynamic_cast<CdPackedReal32*>(Obj)   ||
			dynamic_cast<CdPackedReal32U*>(Obj);
	}



	// =====================================================================
	// Template for Allocator
	// =====================================================================

	/// Template functions for allocator of TReal8
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TReal8, MEM_TYPE>
	{
		static const ssize_t NBUF = COREARRAY_ALLOC_FUNC_BUFFER;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			C_UInt8 Buf[NBUF];
			const double *lookup = static_cast<CdPackedReal8*>(I.Handler)->LookupTable();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				I.Allocator->ReadData(Buf, Cnt);
				n -= Cnt;
				for (C_UInt8 *s=Buf; Cnt > 0; Cnt--)
					*p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[*s++]);
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr++;
			C_UInt8 Buf[NBUF];
			const double *lookup = static_cast<CdPackedReal8*>(I.Handler)->LookupTable();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				I.Allocator->ReadData(Buf, Cnt);
				n -= Cnt;
				C_UInt8 *s = Buf;
			#ifdef COREARRAY_SIMD_SSE2
				for (; Cnt >= 16; Cnt-=16, sel+=16, s+=16)
				{
					__m128i sv = _mm_loadu_si128((__m128i const*)sel);
					sv = _mm_cmpeq_epi8(sv, _mm_setzero_si128());
					int sv16 = _mm_movemask_epi8(sv);
					if (sv16 == 0)  // all selected
					{
						p[0] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[0]]);
						p[1] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[1]]);
						p[2] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[2]]);
						p[3] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[3]]);
						p[4] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[4]]);
						p[5] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[5]]);
						p[6] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[6]]);
						p[7] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[7]]);
						p[8] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[8]]);
						p[9] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[9]]);
						p[10] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[10]]);
						p[11] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[11]]);
						p[12] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[12]]);
						p[13] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[13]]);
						p[14] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[14]]);
						p[15] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[15]]);
						p += 16;
					} else if (sv16 != 0xFFFF)  // at least one selected
					{
						sv16 = ~sv16;
						if (sv16 & 0x0001) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[0]]);
						if (sv16 & 0x0002) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[1]]);
						if (sv16 & 0x0004) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[2]]);
						if (sv16 & 0x0008) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[3]]);
						if (sv16 & 0x0010) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[4]]);
						if (sv16 & 0x0020) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[5]]);
						if (sv16 & 0x0040) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[6]]);
						if (sv16 & 0x0080) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[7]]);
						if (sv16 & 0x0100) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[8]]);
						if (sv16 & 0x0200) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[9]]);
						if (sv16 & 0x0400) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[10]]);
						if (sv16 & 0x0800) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[11]]);
						if (sv16 & 0x1000) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[12]]);
						if (sv16 & 0x2000) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[13]]);
						if (sv16 & 0x4000) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[14]]);
						if (sv16 & 0x8000) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[15]]);
					}
				}
			#endif
				for (; Cnt > 0; Cnt--, s++)
				{
					if (*sel++)
						*p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[*s]);
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			C_Int8 Buf[NBUF];
			CdPackedReal8 *IT = static_cast<CdPackedReal8*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				C_Int8 *s = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((VAL_CONV_TO_F64(MEM_TYPE, *p++) - offset) * scale);
					C_Int8 I = TdTraits<TReal8>::MissingValue;
					if (IsFinite(v) && (-127.5 < v) && (v <= 127.5))
						I = (int)v;
					*s++ = I;
				}
				I.Allocator->WriteData(Buf, Cnt);
				n -= Cnt;
			}
			return p;
		}
	};


	// ---------------------------------------------------------------------

	/// Template functions for allocator of TReal8u
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TReal8u, MEM_TYPE>
	{
		static const ssize_t NBUF = COREARRAY_ALLOC_FUNC_BUFFER;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			C_UInt8 Buf[NBUF];
			const double *lookup = static_cast<CdPackedReal8U*>(I.Handler)->LookupTable();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				I.Allocator->ReadData(Buf, Cnt);
				n -= Cnt;
				for (C_UInt8 *s=Buf; Cnt > 0; Cnt--)
					*p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[*s++]);
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr++;
			C_UInt8 Buf[NBUF];
			const double *lookup = static_cast<CdPackedReal8U*>(I.Handler)->LookupTable();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				I.Allocator->ReadData(Buf, Cnt);
				n -= Cnt;
				C_UInt8 *s = Buf;
			#ifdef COREARRAY_SIMD_SSE2
				for (; Cnt >= 16; Cnt-=16, sel+=16, s+=16)
				{
					__m128i sv = _mm_loadu_si128((__m128i const*)sel);
					sv = _mm_cmpeq_epi8(sv, _mm_setzero_si128());
					int sv16 = _mm_movemask_epi8(sv);
					if (sv16 == 0)  // all selected
					{
						p[0] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[0]]);
						p[1] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[1]]);
						p[2] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[2]]);
						p[3] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[3]]);
						p[4] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[4]]);
						p[5] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[5]]);
						p[6] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[6]]);
						p[7] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[7]]);
						p[8] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[8]]);
						p[9] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[9]]);
						p[10] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[10]]);
						p[11] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[11]]);
						p[12] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[12]]);
						p[13] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[13]]);
						p[14] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[14]]);
						p[15] = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[15]]);
						p += 16;
					} else if (sv16 != 0xFFFF)  // at least one selected
					{
						sv16 = ~sv16;
						if (sv16 & 0x0001) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[0]]);
						if (sv16 & 0x0002) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[1]]);
						if (sv16 & 0x0004) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[2]]);
						if (sv16 & 0x0008) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[3]]);
						if (sv16 & 0x0010) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[4]]);
						if (sv16 & 0x0020) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[5]]);
						if (sv16 & 0x0040) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[6]]);
						if (sv16 & 0x0080) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[7]]);
						if (sv16 & 0x0100) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[8]]);
						if (sv16 & 0x0200) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[9]]);
						if (sv16 & 0x0400) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[10]]);
						if (sv16 & 0x0800) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[11]]);
						if (sv16 & 0x1000) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[12]]);
						if (sv16 & 0x2000) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[13]]);
						if (sv16 & 0x4000) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[14]]);
						if (sv16 & 0x8000) *p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[s[15]]);
					}
				}
			#endif
				for (; Cnt > 0; Cnt--, s++)
				{
					if (*sel++)
						*p++ = VAL_CONV_FROM_F64(MEM_TYPE, lookup[*s]);
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			C_UInt8 Buf[NBUF];
			CdPackedReal8U *IT = static_cast<CdPackedReal8U*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				C_UInt8 *s = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((VAL_CONV_TO_F64(MEM_TYPE, *p++) - offset) * scale);
					C_UInt8 I = 0xFF;
					if (IsFinite(v) && (-0.5 < v) && (v <= 254.5))
						I = (unsigned)v;
					*s++ = I;
				}
				I.Allocator->WriteData(Buf, Cnt);
				n -= Cnt;
			}
			return p;
		}
	};


	// ---------------------------------------------------------------------

	/// Template functions for allocator of TReal16
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TReal16, MEM_TYPE>
	{
		static const ssize_t NBUF = COREARRAY_ALLOC_FUNC_BUFFER >> 1;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			C_Int16 Buf[NBUF];
			CdPackedReal16 *IT = static_cast<CdPackedReal16*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();
			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int16 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
						(*s != C_Int16(0x8000)) ? ((*s) * scale + offset) : NaN);
				}
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr += 2;
			C_Int16 Buf[NBUF];
			CdPackedReal16 *IT = static_cast<CdPackedReal16*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();
			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int16 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					if (*sel++)
					{
						*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
							(*s != C_Int16(0x8000)) ? ((*s) * scale + offset) : NaN);
					}
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			C_Int16 Buf[NBUF];
			CdPackedReal16 *IT = static_cast<CdPackedReal16*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				C_Int16 *s = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((VAL_CONV_TO_F64(MEM_TYPE, *p++) - offset) * scale);
					C_Int16 I = TdTraits<TReal16>::MissingValue;
					if (IsFinite(v) && (-32767.5 < v) && (v <= 32767.5))
						I = (C_Int16)v;
					*s++ = I;
				}
				COREARRAY_ENDIAN_NT_TO_LE_ARRAY(Buf, Cnt);
				I.Allocator->WriteData(Buf, Cnt << 1);
				n -= Cnt;
			}
			return p;
		}
	};


	// ---------------------------------------------------------------------

	/// Template functions for allocator of TReal16u
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TReal16u, MEM_TYPE>
	{
		static const ssize_t NBUF = COREARRAY_ALLOC_FUNC_BUFFER >> 1;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			C_UInt16 Buf[NBUF];
			CdPackedReal16U *IT = static_cast<CdPackedReal16U*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();
			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_UInt16 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
						(*s != 0xFFFF) ? ((*s) * scale + offset) : NaN);
				}
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr += 2;
			C_UInt16 Buf[NBUF];
			CdPackedReal16U *IT = static_cast<CdPackedReal16U*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();
			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_UInt16 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					if (*sel++)
					{
						*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
							(*s != 0xFFFF) ? ((*s) * scale + offset) : NaN);
					}
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			C_UInt16 Buf[NBUF];
			CdPackedReal16U *IT = static_cast<CdPackedReal16U*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				C_UInt16 *s = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((VAL_CONV_TO_F64(MEM_TYPE, *p++) - offset) * scale);
					C_UInt16 I = 0xFFFF;
					if (IsFinite(v) && (-0.5 < v) && (v <= 65534.5))
						I = (C_UInt16)v;
					*s++ = I;
				}
				COREARRAY_ENDIAN_NT_TO_LE_ARRAY(Buf, Cnt);
				I.Allocator->WriteData(Buf, Cnt << 1);
				n -= Cnt;
			}
			return p;
		}
	};


	// ---------------------------------------------------------------------

	/// Template functions for allocator of TReal24
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TReal24, MEM_TYPE>
	{
		static const ssize_t NBUF = COREARRAY_ALLOC_FUNC_BUFFER / 3;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			C_UInt8 Buf[NBUF][3];
			CdPackedReal24 *IT = static_cast<CdPackedReal24*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n * 3);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				I.Allocator->ReadData(Buf, Cnt*3);
				n -= Cnt;
				for (C_UInt8 *s=Buf[0]; Cnt > 0; Cnt--, s+=3)
				{
					C_Int32 val = s[0] | (C_Int32(s[1]) << 8) | (C_Int32(s[2]) << 16);
					if (val != 0x800000)
					{
						if (val & 0x800000) val |= 0xFF000000;
						*p++ = VAL_CONV_FROM_F64(MEM_TYPE, val * scale + offset);
					} else
						*p++ = NaN;
				}
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr += 3;
			C_UInt8 Buf[NBUF][3];
			CdPackedReal24 *IT = static_cast<CdPackedReal24*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n * 3);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				I.Allocator->ReadData(Buf, Cnt*3);
				n -= Cnt;
				for (C_UInt8 *s=Buf[0]; Cnt > 0; Cnt--, s+=3)
				{
					if (*sel++)
					{
						C_Int32 val = s[0] | (C_Int32(s[1]) << 8) | (C_Int32(s[2]) << 16);
						if (val != 0x800000)
						{
							if (val & 0x800000) val |= 0xFF000000;
							*p++ = VAL_CONV_FROM_F64(MEM_TYPE, val * scale + offset);
						} else
							*p++ = NaN;
					}
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			C_UInt8 Buf[NBUF][3];
			CdPackedReal24 *IT = static_cast<CdPackedReal24*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n * 3);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				C_UInt8 *s = Buf[0];
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((VAL_CONV_TO_F64(MEM_TYPE, *p++) - offset) * scale);
					C_Int32 I = 0x800000;
					if (IsFinite(v) && (-8388607.5 < v) && (v <= 8388607.5))
						I = (C_Int32)v;
					s[0] = C_UInt8(I);
					s[1] = C_UInt8(I >> 8);
					s[2] = C_UInt8(I >> 16);
					s += 3;
				}
				I.Allocator->WriteData(Buf, Cnt*3);
				n -= Cnt;
			}
			return p;
		}
	};


	// ---------------------------------------------------------------------

	/// Template functions for allocator of TReal24u
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TReal24u, MEM_TYPE>
	{
		static const ssize_t NBUF = COREARRAY_ALLOC_FUNC_BUFFER / 3;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			C_UInt8 Buf[NBUF][3];
			CdPackedReal24U *IT = static_cast<CdPackedReal24U*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n * 3);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				I.Allocator->ReadData(Buf, Cnt*3);
				n -= Cnt;
				for (C_UInt8 *s=Buf[0]; Cnt > 0; Cnt--, s+=3)
				{
					C_UInt32 val = s[0] | (C_UInt32(s[1]) << 8) | (C_UInt32(s[2]) << 16);
					if (val != 0xFFFFFF)
						*p++ = VAL_CONV_FROM_F64(MEM_TYPE, val*scale + offset);
					else
						*p++ = NaN;
				}
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr += 3;
			C_UInt8 Buf[NBUF][3];
			CdPackedReal24U *IT = static_cast<CdPackedReal24U*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n * 3);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				I.Allocator->ReadData(Buf, Cnt*3);
				n -= Cnt;
				for (C_UInt8 *s=Buf[0]; Cnt > 0; Cnt--, s+=3)
				{
					if (*sel++)
					{
						C_UInt32 val = s[0] | (C_UInt32(s[1]) << 8) | (C_UInt32(s[2]) << 16);
						if (val != 0xFFFFFF)
							*p++ = VAL_CONV_FROM_F64(MEM_TYPE, val*scale + offset);
						else
							*p++ = NaN;
					}
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			C_UInt8 Buf[NBUF][3];
			CdPackedReal24U *IT = static_cast<CdPackedReal24U*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n * 3);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				C_UInt8 *s = Buf[0];
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((VAL_CONV_TO_F64(MEM_TYPE, *p++) - offset) * scale);
					C_UInt32 I = 0xFFFFFF;
					if (IsFinite(v) && (-0.5 < v) && (v <= 16777214.5))
						I = (C_UInt32)v;
					s[0] = C_UInt8(I);
					s[1] = C_UInt8(I >> 8);
					s[2] = C_UInt8(I >> 16);
					s += 3;
				}
				I.Allocator->WriteData(Buf, Cnt*3);
				n -= Cnt;
			}
			return p;
		}
	};


	// ---------------------------------------------------------------------

	/// Template functions for allocator of TReal32
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TReal32, MEM_TYPE>
	{
		static const ssize_t NBUF = COREARRAY_ALLOC_FUNC_BUFFER >> 2;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			C_Int32 Buf[NBUF];
			CdPackedReal32 *IT = static_cast<CdPackedReal32*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();
			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int32 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
						(*s != C_Int32(0x80000000)) ? ((*s) * scale + offset) : NaN);
				}
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr += 4;
			C_Int32 Buf[NBUF];
			CdPackedReal32 *IT = static_cast<CdPackedReal32*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();
			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int32 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					if (*sel++)
					{
						*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
							(*s != C_Int32(0x80000000)) ? ((*s) * scale + offset) : NaN);
					}
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			C_Int32 Buf[NBUF];
			CdPackedReal32 *IT = static_cast<CdPackedReal32*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				C_Int32 *s = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((VAL_CONV_TO_F64(MEM_TYPE, *p++) - offset) * scale);
					C_Int32 I = 0x80000000;
					if (IsFinite(v) && (-2147483647.5 < v) && (v <= 2147483647.5))
						I = (C_Int32)v;
					*s++ = I;
				}
				COREARRAY_ENDIAN_NT_TO_LE_ARRAY(Buf, Cnt);
				I.Allocator->WriteData(Buf, Cnt << 2);
				n -= Cnt;
			}
			return p;
		}
	};


	// ---------------------------------------------------------------------

	/// Template functions for allocator of TReal32u
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TReal32u, MEM_TYPE>
	{
		static const ssize_t NBUF = COREARRAY_ALLOC_FUNC_BUFFER >> 2;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			C_UInt32 Buf[NBUF];
			CdPackedReal32U *IT = static_cast<CdPackedReal32U*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();
			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_UInt32 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
						(*s != 0xFFFFFFFF) ? ((*s) * scale + offset) : NaN);
				}
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr += 4;
			C_UInt32 Buf[NBUF];
			CdPackedReal32U *IT = static_cast<CdPackedReal32U*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();
			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_UInt32 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					if (*sel++)
					{
						*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
							(*s != 0xFFFFFFFF) ? ((*s) * scale + offset) : NaN);
					}
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			C_UInt32 Buf[NBUF];
			CdPackedReal32U *IT = static_cast<CdPackedReal32U*>(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				C_UInt32 *s = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((VAL_CONV_TO_F64(MEM_TYPE, *p++) - offset) * scale);
					C_UInt32 I = 0xFFFFFFFF;
					if (IsFinite(v) && (-0.5 < v) && (v <= 4294967294.5))
						I = (C_UInt32)v;
					*s++ = I;
				}
				COREARRAY_ENDIAN_NT_TO_LE_ARRAY(Buf, Cnt);
				I.Allocator->WriteData(Buf, Cnt << 2);
				n -= Cnt;
			}
			return p;
		}
	};

}

#endif /* _HEADER_COREARRAY_REAL_GDS_ */
