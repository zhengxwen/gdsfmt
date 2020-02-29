// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dSparse.h: Sparse array in GDS format
//
// Copyright (C) 2020    Xiuwen Zheng
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
 *	\file     dSparse.h
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2020
 *	\brief    Sparse array in GDS format
 *	\details
**/

#ifndef _HEADER_COREARRAY_SPARSE_GDS_
#define _HEADER_COREARRAY_SPARSE_GDS_

#include "dStruct.h"
#include <math.h>
#include <typeinfo>


namespace CoreArray
{
	using namespace std;

	/// Sparse value type
	/** \tparam TYPE  data type, e.g C_Int8, ...
	**/
	template<typename TYPE> struct COREARRAY_DLL_DEFAULT TSpVal
	{
		typedef TYPE TType;
		C_Int64 Index;
		TType Val;
	};

	typedef TSpVal<C_Int8>    TSpInt8;    ///< 8-bit sparse integer (signed int)
	typedef TSpVal<C_UInt8>   TSpUInt8;   ///< 8-bit sparse integer (unsigned int)
	typedef TSpVal<C_Int16>   TSpInt16;   ///< 16-bit sparse integer (signed int)
	typedef TSpVal<C_UInt16>  TSpUInt16;  ///< 16-bit sparse integer (unsigned int)
	typedef TSpVal<C_Int32>   TSpInt32;   ///< 32-bit sparse integer (signed int)
	typedef TSpVal<C_UInt32>  TSpUInt32;  ///< 32-bit sparse integer (unsigned int)
	typedef TSpVal<C_Int64>   TSpInt64;   ///< 64-bit sparse integer (signed int)
	typedef TSpVal<C_UInt64>  TSpUInt64;  ///< 64-bit sparse integer (unsigned int)
	typedef TSpVal<C_Float32> TSpReal32;  ///< 32-bit sparse real number
	typedef TSpVal<C_Float64> TSpReal64;  ///< 64-bit sparse real number


	/// Trait of sparse Int8
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TSpInt8>
	{
		typedef C_Int8 TType;
		typedef C_Int8 ElmType;
		static const int trVal = COREARRAY_TR_SPARSE_INTEGER;
		static const unsigned BitOf = 8u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svInt8;

		static const char *TraitName() { return StreamName()+1; }
		static const char *StreamName() { return "dSparseInt8"; }

		COREARRAY_INLINE static short Min() { return INT8_MIN; }
		COREARRAY_INLINE static short Max() { return INT8_MAX; }
	};


	/// Trait of sparse UInt8
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TSpUInt8>
	{
		typedef C_UInt8 TType;
		typedef C_UInt8 ElmType;
		static const int trVal = COREARRAY_TR_SPARSE_INTEGER;
		static const unsigned BitOf = 8u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svUInt8;

		static const char *TraitName() { return StreamName()+1; }
		static const char *StreamName() { return "dSparseUInt8"; }

		COREARRAY_INLINE static unsigned short Min() { return 0; }
		COREARRAY_INLINE static unsigned short Max() { return UINT8_MAX; }
	};


	/// Trait of sparse Int16
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TSpInt16>
	{
		typedef C_Int16 TType;
		typedef C_Int16 ElmType;
		static const int trVal = COREARRAY_TR_SPARSE_INTEGER;
		static const unsigned BitOf = 16u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svInt16;

		static const char *TraitName() { return StreamName()+1; }
		static const char *StreamName() { return "dSparseInt16"; }

		COREARRAY_INLINE static C_Int16 Min() { return INT16_MIN; }
		COREARRAY_INLINE static C_Int16 Max() { return INT16_MAX; }
	};


	/// Trait of sparse UInt16
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TSpUInt16>
	{
		typedef C_UInt16 TType;
		typedef C_UInt16 ElmType;
		static const int trVal = COREARRAY_TR_SPARSE_INTEGER;
		static const unsigned BitOf = 16u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svUInt16;

		static const char *TraitName() { return StreamName()+1; }
		static const char *StreamName() { return "dSparseUInt16"; }

		COREARRAY_INLINE static C_UInt16 Min() { return 0; }
		COREARRAY_INLINE static C_UInt16 Max() { return UINT16_MAX; }
	};


	/// Trait of sparse Int32
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TSpInt32>
	{
		typedef C_Int32 TType;
		typedef C_Int32 ElmType;
		static const int trVal = COREARRAY_TR_SPARSE_INTEGER;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svInt32;

		static const char *TraitName() { return StreamName()+1; }
		static const char *StreamName() { return "dSparseInt32"; }

		COREARRAY_INLINE static C_Int32 Min() { return INT32_MIN; }
		COREARRAY_INLINE static C_Int32 Max() { return INT32_MAX; }
	};


	/// Trait of sparse UInt32
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TSpUInt32>
	{
		typedef C_UInt32 TType;
		typedef C_UInt32 ElmType;
		static const int trVal = COREARRAY_TR_SPARSE_INTEGER;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svUInt32;

		static const char *TraitName() { return StreamName()+1; }
		static const char *StreamName() { return "dSparseUInt32"; }

		COREARRAY_INLINE static C_UInt32 Min() { return 0; }
		COREARRAY_INLINE static C_UInt32 Max() { return UINT32_MAX; }
	};


	/// Trait of sparse Int64
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TSpInt64>
	{
		typedef C_Int64 TType;
		typedef C_Int64 ElmType;
		static const int trVal = COREARRAY_TR_SPARSE_INTEGER;
		static const unsigned BitOf = 64u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svInt64;

		static const char *TraitName() { return StreamName()+1; }
		static const char *StreamName() { return "dSparseInt64"; }

		COREARRAY_INLINE static C_Int64 Min() { return std::numeric_limits<C_Int64>::min(); }
		COREARRAY_INLINE static C_Int64 Max() { return std::numeric_limits<C_Int64>::max(); }
	};


	/// Trait of sparse UInt64
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TSpUInt64>
	{
		typedef C_UInt64 TType;
		typedef C_UInt64 ElmType;
		static const int trVal = COREARRAY_TR_SPARSE_INTEGER;
		static const unsigned BitOf = 64u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svUInt64;

		static const char *TraitName() { return StreamName()+1; }
		static const char *StreamName() { return "dSparseUInt64"; }

		COREARRAY_INLINE static C_UInt64 Min() { return 0; }
		COREARRAY_INLINE static C_UInt64 Max() { return std::numeric_limits<C_UInt64>::max(); }
	};


	/// Traits of 32-bit sparse real number
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TSpReal32>
	{
		typedef C_Float32 TType;
		typedef C_Float32 ElmType;

		static const int trVal = COREARRAY_TR_SPARSE_REAL;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svFloat32;

		static const char *StreamName() { return "dSparseReal32"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float32 Min() { return FLT_MIN; }
		COREARRAY_INLINE static C_Float32 Max() { return FLT_MAX; }
		COREARRAY_INLINE static C_Float32 Epsilon() { return FLT_EPSILON; }
		COREARRAY_INLINE static int Digits() { return FLT_MANT_DIG; }
	};


	/// Traits of 64-bit sparse real number
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TSpReal64>
	{
		typedef C_Float64 TType;
		typedef C_Float64 ElmType;

		static const int trVal = COREARRAY_TR_SPARSE_REAL;
		static const unsigned BitOf = 64u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svFloat64;

		static const char *StreamName() { return "dSparseReal64"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }
		COREARRAY_INLINE static C_Float64 Epsilon() { return DBL_EPSILON; }
		COREARRAY_INLINE static int Digits() { return DBL_MANT_DIG; }
	};


	namespace _INTERNAL
	{
		// test whether it is zero not

		inline static bool IS_ZERO(C_Int8 v)   { return v==0; }
		inline static bool IS_ZERO(C_UInt8 v)  { return v==0; }
		inline static bool IS_ZERO(C_Int16 v)  { return v==0; }
		inline static bool IS_ZERO(C_UInt16 v) { return v==0; }
		inline static bool IS_ZERO(C_Int32 v)  { return v==0; }
		inline static bool IS_ZERO(C_UInt32 v) { return v==0; }
		inline static bool IS_ZERO(C_Int64 v)  { return v==0; }
		inline static bool IS_ZERO(C_UInt64 v) { return v==0; }
		inline static bool IS_ZERO(C_Float32 v) { return *((C_Int32*)&v)==0; }
		inline static bool IS_ZERO(C_Float64 v) { return *((C_Int64*)&v)==0; }
		inline static bool IS_ZERO(const UTF8String &v)
			{ return v.empty() || (v=="0"); }
		inline static bool IS_ZERO(const UTF16String &v)
			{ return v.empty() || (v.size()==1 && v[0]=='0'); }

		// fill a vector with zero

		inline static void SET_ZERO(C_Int8   *p, size_t n) { std::memset(p, 0, n); }
		inline static void SET_ZERO(C_UInt8  *p, size_t n) { std::memset(p, 0, n); }
		inline static void SET_ZERO(C_Int16  *p, size_t n) { std::memset(p, 0, n<<1); }
		inline static void SET_ZERO(C_UInt16 *p, size_t n) { std::memset(p, 0, n<<1); }
		inline static void SET_ZERO(C_Int32  *p, size_t n) { std::memset(p, 0, n<<2); }
		inline static void SET_ZERO(C_UInt32 *p, size_t n) { std::memset(p, 0, n<<2); }
		inline static void SET_ZERO(C_Int64  *p, size_t n) { std::memset(p, 0, n<<3); }
		inline static void SET_ZERO(C_UInt64 *p, size_t n) { std::memset(p, 0, n<<3); }
		inline static void SET_ZERO(C_Float32 *p, size_t n) { std::memset(p, 0, n<<2); }
		inline static void SET_ZERO(C_Float64 *p, size_t n) { std::memset(p, 0, n<<3); }
		inline static void SET_ZERO(UTF8String *p, size_t n)
			{ for (; n > 0; n--, p++) p->clear(); }
		inline static void SET_ZERO(UTF16String *p, size_t n)
			{ for (; n > 0; n--, p++) p->clear(); }
	}


	// =====================================================================
	// Sparse integer/real number classes of GDS format
	// =====================================================================

	/// Container of sparse real number
	/** \tparam SP_TYPE    should be TSpReal32, TSpReal64
	**/
	template<typename SP_TYPE>
		class COREARRAY_DLL_DEFAULT CdSparseArray: public CdArray<SP_TYPE>
	{
	public:
		template<typename ALLOC_TYPE, typename MEM_TYPE> friend struct ALLOC_FUNC;

		typedef SP_TYPE ElmType;
		typedef typename TdTraits<ElmType>::TType ElmTypeEx;

		CdSparseArray(): CdArray<SP_TYPE>(1)
		{
			fIndexingID = 0;
			fIndexingStream = NULL;
			fTotalStreamSize = fCurStreamPosition = 0;
			fCurIndex = fNumRecord = fNumZero = 0;
		}

		virtual CdGDSObj *NewObject()
		{
			return (new CdSparseArray<SP_TYPE>())->AssignPipe(*this);
		}

		virtual void CloseWriter()
		{
			WriteNumZero();
			CdArray<SP_TYPE>::CloseWriter();
		}

		/// append new data from an iterator
		virtual void AppendIter(CdIterator &I, C_Int64 Count)
		{
			CdAbstractArray::AppendIter(I, Count);
		}

	protected:

		const char *const VAR_INDEX = "INDEX";

		TdGDSBlockID fIndexingID;       ///< indexing block ID
		CdBlockStream *fIndexingStream; ///< the GDS stream for indexing
		SIZE64 fTotalStreamSize;    ///< the total stream size
		SIZE64 fCurStreamPosition;  ///< the current stream position
		C_Int64 fCurIndex;   ///< the current array index
		C_Int64 fNumRecord;  ///< the total number of zero and non-zero records
		int fNumZero;  ///< the number of remaining zeros

		/// loading function for serialization
		virtual void Loading(CdReader &Reader, TdVersion Version)
		{
			CdArray<SP_TYPE>::Loading(Reader, Version);
			if (this->fGDSStream)
			{
				// get the indexing stream
				Reader[VAR_INDEX] >> fIndexingID;
				fIndexingStream = this->fGDSStream->Collection()[fIndexingID];
				fNumRecord = fIndexingStream->GetSize() /
					(sizeof(SIZE64) + GDS_POS_SIZE);
				// get the total size
				fTotalStreamSize = 0;
				if (this->fPipeInfo)
				{
					fTotalStreamSize = this->fPipeInfo->StreamTotalIn();
				} else {
					if (this->fAllocator.BufStream())
						fTotalStreamSize = this->fAllocator.BufStream()->GetSize();
				}
				//
				fCurIndex = fCurStreamPosition = 0;
				fNumZero = 0;
			}
		}

		/// saving function for serialization
		virtual void Saving(CdWriter &Writer)
		{
			CdArray<SP_TYPE>::Saving(Writer);
			if (this->fGDSStream)
			{
				if (!fIndexingStream)
					fIndexingStream = this->fGDSStream->Collection().NewBlockStream();
				TdGDSBlockID Entry = fIndexingStream->ID();
				Writer[VAR_INDEX] << Entry;
			}
		}

		/// write to stream if the number of zeros > 0
		inline void WriteNumZero()
		{
			if (fNumZero > 0)
			{
				this->fAllocator.SetPosition(fTotalStreamSize);
				BYTE_LE<CdAllocator>(this->fAllocator) << C_UInt16(fNumZero);
				fTotalStreamSize += sizeof(C_UInt16);
				this->fTotalCount += fNumZero;
				fNumZero = 0;
			}
		}

		/// set stream position to the corresponding array index
		void SetStreamPos(C_Int64 idx)
		{
			if (fCurIndex == idx)
			{
				this->fAllocator.SetPosition(fCurStreamPosition);
				return;
			} else {
				if (idx == this->fTotalCount)
				{
					fCurIndex = this->fTotalCount;
					fCurStreamPosition = fTotalStreamSize;
					this->fAllocator.SetPosition(fCurStreamPosition);
					return;
				} else if ((idx > this->fTotalCount) || (idx < 0))
				{
					throw ErrArray("CdSparseArray::SetStreamPos: Invalid Index.");
				} else if (idx > fCurIndex)
				{
					this->fAllocator.SetPosition(fCurStreamPosition);
					C_UInt16 NZero;
					BYTE_LE<CdAllocator>(this->fAllocator) >> NZero;
					if (idx < fCurIndex + NZero)
					{
						this->fAllocator.SetPosition(fCurStreamPosition);
						return;
					}
				} else {
					fCurIndex = 0;
					fCurStreamPosition = 0;
				}
				// binary search
				if (fIndexingStream && fNumRecord > 0)
				{
					const int SIZE = sizeof(SIZE64) + GDS_POS_SIZE;
					BYTE_LE<CdAllocator> SS(this->fAllocator);
					C_Int64 st=0, ed=fNumRecord, CI=0, CI_i=0;
					while (st < ed)
					{
						C_Int64 mid = (st + ed) / 2;
						this->fAllocator.SetPosition(mid * SIZE);
						C_Int64 I; SS >> I;
						if (I <= idx)
						{
							CI = I; CI_i = mid;
							if (I == idx) break; else st = mid + 1;
						} else
							ed = mid;
					}
					if (CI > fCurIndex)
					{
						fCurIndex = CI;
						this->fAllocator.SetPosition(CI_i * SIZE);
						TdGDSPos s; SS >> s;
						fCurStreamPosition = s;
					}
				}
				// move forward to the correct position (fCurIndex <= idx)
				this->fAllocator.SetPosition(fCurStreamPosition);
				while (fCurIndex < idx)
				{
					C_UInt16 NZero;
					BYTE_LE<CdAllocator>(this->fAllocator) >> NZero;
					if (NZero == 0)
					{
						fCurStreamPosition += sizeof(NZero) + sizeof(ElmTypeEx);
						this->fAllocator.SetPosition(fCurStreamPosition);
						fCurIndex ++;
					} else if (fCurIndex + NZero <= idx)
					{
						fCurStreamPosition += sizeof(NZero);
						fCurIndex += NZero;
					} else {
						this->fAllocator.SetPosition(fCurStreamPosition);
						break;
					}
				}
			}
		}
	};


	// =====================================================================
	// Template for Allocator for sparse array
	// =====================================================================

	/// Template functions for allocator of variable-length signed integer
	template<typename TYPE, typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TSpVal<TYPE>, MEM_TYPE>
	{
		typedef TSpVal<TYPE> SP_TYPE;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			CdSparseArray<SP_TYPE> *IT = static_cast<CdSparseArray<SP_TYPE>*>(I.Handler);
			IT->WriteNumZero();
			IT->SetStreamPos(I.Ptr);
			BYTE_LE<CdAllocator> SS(I.Allocator);
			while (n > 0)
			{
				C_UInt16 NZero; SS >> NZero;
				if (NZero == 0)
				{
					// IT->fCurIndex should be = I.Ptr (calling SetStreamPos)
					TYPE Val; SS >> Val;
					*p++ = VAL_CONVERT(MEM_TYPE, TYPE, Val);
					IT->fCurStreamPosition += sizeof(NZero) + sizeof(Val);
					I.Ptr++; IT->fCurIndex = I.Ptr; n--;
				} else {
					ssize_t m = NZero;
					if (IT->fCurIndex < I.Ptr) m -= I.Ptr - IT->fCurIndex;
					if (m > n) m = n;
					_INTERNAL::SET_ZERO(p, m); p += m;
					I.Ptr += m; n -= m;
					if (I.Ptr - IT->fCurIndex >= NZero)
					{
						IT->fCurIndex = I.Ptr;
						IT->fCurStreamPosition += sizeof(NZero);
					}
				}
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr++;
			CdSparseArray<SP_TYPE> *IT = static_cast<CdSparseArray<SP_TYPE>*>(I.Handler);
			IT->WriteNumZero();
			IT->SetStreamPos(I.Ptr);
			BYTE_LE<CdAllocator> SS(I.Allocator);
			size_t n_zero_fill = 0;
			while (n > 0)
			{
				TYPE Val;
				C_UInt16 NZero;
				// get the skip count
				const C_BOOL *base_sel = sel;
				for (; n>0 && !*sel; n--) sel++;
				ssize_t n_skip = sel - base_sel;
				if (n <= 0) { I.Ptr += n_skip; break; }
				// skip
				bool NeedNZero = true;
				while (n_skip > 0)
				{
					SS >> NZero;
					if (NZero == 0)
					{
						SS >> Val;
						IT->fCurStreamPosition += sizeof(NZero) + sizeof(Val);
						I.Ptr++; IT->fCurIndex = I.Ptr;
						n_skip --;
					} else if (NZero <= n_skip)
					{
						I.Ptr += NZero; IT->fCurIndex = I.Ptr;
						IT->fCurStreamPosition += sizeof(NZero);
						n_skip -= NZero;
					} else {
						NeedNZero = false;
						I.Ptr += n_skip;
						n_skip = 0;
					}
				}
				// read
				if (NeedNZero) SS >> NZero;
				if (NZero == 0)
				{
					// IT->fCurIndex should be = I.Ptr
					if (n_zero_fill > 0)
					{
						_INTERNAL::SET_ZERO(p, n_zero_fill); p += n_zero_fill;
						n_zero_fill = 0;
					}
					SS >> Val;
					*p++ = VAL_CONVERT(MEM_TYPE, TYPE, Val);
					IT->fCurStreamPosition += sizeof(NZero) + sizeof(Val);
					I.Ptr++; IT->fCurIndex = I.Ptr; n--;
				} else {
					ssize_t m = NZero;
					if (IT->fCurIndex < I.Ptr) m -= I.Ptr - IT->fCurIndex;
					if (m > n) m = n;
					n_zero_fill += m; I.Ptr += m; n -= m;
					if (I.Ptr - IT->fCurIndex >= NZero)
					{
						IT->fCurIndex = I.Ptr;
						IT->fCurStreamPosition += sizeof(NZero);
					}
				}
			}
			if (n_zero_fill > 0)
			{
				_INTERNAL::SET_ZERO(p, n_zero_fill);
				p += n_zero_fill;
			}
			return p;
		}

		inline static void append_index(C_Int64 I, CdSparseArray<SP_TYPE> *IT)
		{
			IT->fNumRecord ++;
			if ((IT->fNumRecord & 0xFFFF) == 0) // every 65536
			{
				BYTE_LE<CdStream>(IT->fIndexingStream) << I <<
					TdGDSPos(IT->fTotalStreamSize);
			}
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			CdSparseArray<SP_TYPE> *IT = static_cast<CdSparseArray<SP_TYPE>*>(I.Handler);
			if (I.Ptr < IT->fTotalCount)
			{
				throw ErrArray("Insert integers wrong, only append integers.");
			} else if (I.Ptr == IT->fTotalCount)
			{
				// append
				I.Allocator->SetPosition(IT->fTotalStreamSize);
				BYTE_LE<CdAllocator> SS(I.Allocator);
				// for-loop
				for (; n > 0; n--, p++)
				{
					I.Ptr ++;
					if (_INTERNAL::IS_ZERO(*p))
					{
						IT->fNumZero ++;
						if (IT->fNumZero == 0xFFFF)
						{
							SS << C_UInt16(IT->fNumZero);
							IT->fTotalStreamSize += sizeof(C_UInt16);
							IT->fNumZero = 0;
							append_index(I.Ptr, IT);
						}
					} else {
						if (IT->fNumZero > 0)
						{
							SS << C_UInt16(IT->fNumZero);
							IT->fTotalStreamSize += sizeof(C_UInt16);
							IT->fNumZero = 0;
							append_index(I.Ptr, IT);
						}
						SS << C_UInt16(0) << VAL_CONVERT(TYPE, MEM_TYPE, *p);
						IT->fTotalStreamSize += sizeof(C_UInt16) + sizeof(TYPE);
						append_index(I.Ptr, IT);
					}
				}
			} else
				throw ErrArray("Invalid position for writing data.");
			return p;
		}
	};


	// =====================================================================
	// Sparse integer/real numbers in GDS files
	// =====================================================================

	typedef CdSparseArray<TSpInt8>      CdSparseInt8;
	typedef CdSparseArray<TSpUInt8>     CdSparseUInt8;
	typedef CdSparseArray<TSpInt16>     CdSparseInt16;
	typedef CdSparseArray<TSpUInt16>    CdSparseUInt16;
	typedef CdSparseArray<TSpInt32>     CdSparseInt32;
	typedef CdSparseArray<TSpUInt32>    CdSparseUInt32;
	typedef CdSparseArray<TSpInt64>     CdSparseInt64;
	typedef CdSparseArray<TSpUInt64>    CdSparseUInt64;
	typedef CdSparseArray<TSpReal32>    CdSparseReal32;
	typedef CdSparseArray<TSpReal64>    CdSparseReal64;


	/// get whether it is a sparse array or not
	bool IsSparseArray(CdGDSObj *Obj);

}

#endif /* _HEADER_COREARRAY_SPARSE_GDS_ */
