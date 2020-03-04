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
#include <vector>
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

		COREARRAY_INLINE static C_Int64 Min() { return numeric_limits<C_Int64>::min(); }
		COREARRAY_INLINE static C_Int64 Max() { return numeric_limits<C_Int64>::max(); }
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
		COREARRAY_INLINE static C_UInt64 Max() { return numeric_limits<C_UInt64>::max(); }
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

		inline static void SET_ZERO(C_Int8   *p, size_t n)
			{ memset(p, 0, n); }
		inline static void SET_ZERO(C_UInt8  *p, size_t n)
			{ memset(p, 0, n); }
		inline static void SET_ZERO(C_Int16  *p, size_t n)
			{ memset(p, 0, n << 1); }
		inline static void SET_ZERO(C_UInt16 *p, size_t n)
			{ memset(p, 0, n << 1); }
		inline static void SET_ZERO(C_Int32  *p, size_t n)
			{ memset(p, 0, n << 2); }
		inline static void SET_ZERO(C_UInt32 *p, size_t n)
			{ memset(p, 0, n << 2); }
		inline static void SET_ZERO(C_Int64  *p, size_t n)
			{ memset(p, 0, n << 3); }
		inline static void SET_ZERO(C_UInt64 *p, size_t n)
			{ memset(p, 0, n << 3); }
		inline static void SET_ZERO(C_Float32 *p, size_t n)
			{ memset(p, 0, n << 2); }
		inline static void SET_ZERO(C_Float64 *p, size_t n)
			{ memset(p, 0, n << 3); }
		inline static void SET_ZERO(UTF8String *p, size_t n)
			{ for (; n > 0; n--, p++) p->clear(); }
		inline static void SET_ZERO(UTF16String *p, size_t n)
			{ for (; n > 0; n--, p++) p->clear(); }

		/// read the number of zeros in the next block
		inline static C_Int64 read_nzero(BYTE_LE<CdAllocator> &SS, int &sz)
		{
			C_UInt16 v; SS >> v;
			if (v < 0xFFFF)
			{
				sz = sizeof(C_UInt16);
				return v;
			} else  {
				TdGDSPos w; SS >> w;
				sz = sizeof(C_UInt16) + GDS_POS_SIZE;
				return w;
			}
		}
	}


	// =====================================================================
	// Sparse integer/real number classes of GDS format
	// =====================================================================

	/// Extended sparse structure
	class COREARRAY_DLL_DEFAULT CdSpExStruct
	{
	public:
		/// constructor
		CdSpExStruct(int sz);

		/// read a matrix in a form of compressed sparse structure (nrow=cnt1, ncol=cnt2)
		virtual void SpRead(int st1, int st2, int cnt1, int cnt2,
			const C_BOOL *sel1, const C_BOOL *sel2,
			vector<int> &out_i, vector<int> &out_p, vector<double> &out_x,
			int &out_ncol, int &out_nrow) = 0;

	protected:
		const int SpElmSize;  ///< the size of element (e.g., 4 for C_Int32)
		TdGDSBlockID fIndexingID;       ///< indexing block ID
		CdBlockStream *fIndexingStream; ///< the GDS stream for indexing
		SIZE64 fTotalStreamSize;    ///< the total stream size
		SIZE64 fCurStreamPosition;  ///< the current stream position
		C_Int64 fCurIndex;   ///< the current array index
		C_Int64 fNumRecord;  ///< the total number of zero and non-zero records
		vector<C_Int64> fArrayIndex;  ///< array indices in fIndexingStream
		C_Int64 fNumZero;    ///< the number of remaining zeros

		/// loading function for serialization
		void SpLoad(CdReader &Reader, CdBlockStream *GDSStream,
			CdPipeMgrItem *PipeInfo, CdAllocator &Allocator);
		/// saving function for serialization
		void SpSave(CdWriter &Writer, CdBlockStream *GDSStream);
		/// write if the number of zero > 0
		void SpWriteZero(CdAllocator &Allocator);
		/// set stream position according to the index
		void SpSetPos(C_Int64 idx, CdAllocator &Allocator, C_Int64 TotalCount);

	private:
		/// load array indices for random access
		inline void LoadArrayIndex();
	};


	/// Container of sparse real number
	/** \tparam SP_TYPE    should be TSpReal32, TSpReal64
	**/
	template<typename SP_TYPE>
		class COREARRAY_DLL_DEFAULT CdSpArray: public CdArray<SP_TYPE>, public CdSpExStruct
	{
	public:
		template<typename ALLOC_TYPE, typename MEM_TYPE> friend struct ALLOC_FUNC;

		typedef SP_TYPE ElmType;
		typedef typename TdTraits<ElmType>::TType ElmTypeEx;

		/// constructor
		CdSpArray(): CdArray<SP_TYPE>(1), CdSpExStruct(sizeof(ElmTypeEx)) { }
		/// destructor
		virtual ~CdSpArray() { SpWriteZero(this->fAllocator); }

    	/// create a new CdSpArray<SP_TYPE> object
		virtual CdGDSObj *NewObject()
		{
			return (new CdSpArray<SP_TYPE>())->AssignPipe(*this);
		}

		/// synchronize data
		virtual void Synchronize()
		{
			SpWriteZero(this->fAllocator);
			CdArray<SP_TYPE>::Synchronize();
		}

		/// close the writing mode and sync the file
		virtual void CloseWriter()
		{
			SpWriteZero(this->fAllocator);
			CdArray<SP_TYPE>::CloseWriter();
		}

		/// read a matrix in a form of compressed sparse structure (nrow=cnt2, ncol=cnt1)
		virtual void SpRead(int st1, int st2, int cnt1, int cnt2,
			const C_BOOL *sel1, const C_BOOL *sel2,
			vector<int> &out_i, vector<int> &out_p, vector<double> &out_x,
			int &out_ncol, int &out_nrow)
		{
			if (st1<0 || st2<0 || cnt1<0 || cnt2<0)
				throw ErrContainer("Invalid input in SpRead()");
			switch (this->DimCnt())
			{
			case 1:
				{
					SpWriteZero(this->fAllocator);
					out_i.clear(); out_p.clear(); out_x.clear();
					out_p.push_back(0);
					out_p.push_back(1);
					CdIterator I = this->IterBegin();
					I.Ptr = st1;
					read_sp(I, cnt1, sel1, out_i, out_x);
					out_ncol = 1;
					out_nrow = cnt1;
					if (sel1)
					{
						out_nrow = 0;
						for (int i=0; i < cnt1; i++) if (sel1[i]) out_nrow++;
					}
					break;
				}
			case 2:
				{
					SpWriteZero(this->fAllocator);
					out_i.clear(); out_p.clear(); out_x.clear();
					out_p.push_back(0);
					CdIterator I = this->IterBegin();
					for (int i1=0; i1 < cnt1; i1++)
					{
						if (!sel1 || sel1[i1])
						{
							I.Ptr = C_Int64(this->fDimension[1].DimLen) *
								(st1 + i1) + st2;
							read_sp(I, cnt2, sel2, out_i, out_x);
							out_p.push_back(out_x.size());
						}
					}
					out_ncol = out_p.size() - 1;
					out_nrow = cnt2;
					if (sel2)
					{
						out_nrow = 0;
						for (int i=0; i < cnt2; i++) if (sel2[i]) out_nrow++;
					}
					break;
				}
			default:
				throw ErrContainer(
					"CdSpArray<SP_TYPE> should be a vector or matrix.");
			}
		}

	protected:
		/// get the size in byte corresponding to the count 'Num'
		virtual SIZE64 AllocSize(C_Int64 Num)
		{
			if (Num >= this->fTotalCount)
				return fTotalStreamSize;
			else
				return CdArray<SP_TYPE>::AllocSize(Num);
		}

		/// loading function for serialization
		virtual void Loading(CdReader &Reader, TdVersion Version)
		{
			CdArray<SP_TYPE>::Loading(Reader, Version);
			SpLoad(Reader, this->fGDSStream, this->fPipeInfo, this->fAllocator);
		}

		/// saving function for serialization
		virtual void Saving(CdWriter &Writer)
		{
			CdArray<SP_TYPE>::Saving(Writer);
			SpSave(Writer, this->fGDSStream);
		}

		/// set stream position to the corresponding array index
		inline void SetStreamPos(C_Int64 idx)
		{
			SpSetPos(idx, this->fAllocator, this->fTotalCount);
		}

		/// read data in a form of sparse structure
		void read_sp(CdIterator &I, int n, const C_BOOL *sel,
			vector<int> &out_i, vector<double> &out_x)
		{
			if (n <= 0) return;
			if (sel)
			{
				for (; n>0 && !*sel; n--, sel++) I.Ptr++;
			}
			int ii = 0;
			SetStreamPos(I.Ptr);
			BYTE_LE<CdAllocator> SS(this->fAllocator);
			while (n > 0)
			{
				// get the skip count
				ssize_t n_skip = 0;
				if (sel)
				{
					const C_BOOL *base_sel = sel;
					for (; n>0 && !*sel; n--) sel++;
					n_skip = sel - base_sel;
					if (n <= 0) { I.Ptr += n_skip; break; }
				}
				// skip
				int sz = sizeof(C_UInt16);
				C_Int64 nzero = -1;
				while (n_skip > 0)
				{
					nzero = _INTERNAL::read_nzero(SS, sz);
					if (nzero == 0)
					{
						// fCurIndex should be = I.Ptr
						this->fCurStreamPosition += sz + sizeof(ElmTypeEx);
						I.Allocator->SetPosition(this->fCurStreamPosition);
						I.Ptr++; this->fCurIndex = I.Ptr;
						nzero = -1; n_skip --;
					} else {					
						C_Int64 m = nzero;
						if (this->fCurIndex < I.Ptr) m -= I.Ptr - this->fCurIndex;
						if (m > n_skip) m = n_skip;
						I.Ptr += m; n_skip -= m;
						if (I.Ptr - this->fCurIndex >= nzero)
						{
							this->fCurIndex = I.Ptr;
							this->fCurStreamPosition += sz;
							nzero = -1;
						}
					}
				}
				// read
				if (nzero < 0)
					nzero = _INTERNAL::read_nzero(SS, sz);
				if (nzero == 0)
				{
					// IT->fCurIndex should be = I.Ptr, *sel = TRUE
					ElmTypeEx Val; SS >> Val;
					out_i.push_back(ii++);
					out_x.push_back(Val);
					if (sel) sel++;
					this->fCurStreamPosition += sz + sizeof(Val);
					I.Ptr++; this->fCurIndex = I.Ptr; n--;
				} else {
					C_Int64 m = nzero;
					if (this->fCurIndex < I.Ptr) m -= I.Ptr - this->fCurIndex;
					if (m > n) m = n;
					I.Ptr += m; n -= m;
					if (sel)
					{
						for (; m > 0; m--) if (*sel++) ii++;
					} else
						ii += m;
					if (I.Ptr - this->fCurIndex >= nzero)
					{
						this->fCurIndex = I.Ptr;
						this->fCurStreamPosition += sz;
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
			CdSpArray<SP_TYPE> *IT = static_cast<CdSpArray<SP_TYPE>*>(I.Handler);
			IT->SpWriteZero(IT->fAllocator);
			IT->SetStreamPos(I.Ptr);
			BYTE_LE<CdAllocator> SS(I.Allocator);
			while (n > 0)
			{
				int sz;
				C_Int64 nzero = _INTERNAL::read_nzero(SS, sz);
				if (nzero == 0)
				{
					// IT->fCurIndex should be = I.Ptr (calling SetStreamPos)
					TYPE Val; SS >> Val;
					*p++ = VAL_CONVERT(MEM_TYPE, TYPE, Val);
					IT->fCurStreamPosition += sizeof(C_UInt16) + sizeof(Val);
					I.Ptr++; IT->fCurIndex = I.Ptr; n--;
				} else {
					C_Int64 m = nzero;
					if (IT->fCurIndex < I.Ptr) m -= I.Ptr - IT->fCurIndex;
					if (m > n) m = n;
					_INTERNAL::SET_ZERO(p, m); p += m;
					I.Ptr += m; n -= m;
					if (I.Ptr - IT->fCurIndex >= nzero)
					{
						IT->fCurIndex = I.Ptr;
						IT->fCurStreamPosition += sz;
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
			CdSpArray<SP_TYPE> *IT = static_cast<CdSpArray<SP_TYPE>*>(I.Handler);
			IT->SpWriteZero(IT->fAllocator);
			IT->SetStreamPos(I.Ptr);
			BYTE_LE<CdAllocator> SS(I.Allocator);
			size_t n_zero_fill = 0;
			while (n > 0)
			{
				// get the skip count
				const C_BOOL *base_sel = sel;
				for (; n>0 && !*sel; n--) sel++;
				ssize_t n_skip = sel - base_sel;				
				if (n <= 0) { I.Ptr += n_skip; break; }
				// skip
				int sz = sizeof(C_UInt16);
				C_Int64 nzero = -1;
				while (n_skip > 0)
				{
					nzero = _INTERNAL::read_nzero(SS, sz);
					if (nzero == 0)
					{
						// IT->fCurIndex should be = I.Ptr
						IT->fCurStreamPosition += sz + sizeof(TYPE);
						I.Allocator->SetPosition(IT->fCurStreamPosition);
						I.Ptr++; IT->fCurIndex = I.Ptr;
						nzero = -1; n_skip --;
					} else {					
						C_Int64 m = nzero;
						if (IT->fCurIndex < I.Ptr) m -= I.Ptr - IT->fCurIndex;
						if (m > n_skip) m = n_skip;
						I.Ptr += m; n_skip -= m;
						if (I.Ptr - IT->fCurIndex >= nzero)
						{
							IT->fCurIndex = I.Ptr;
							IT->fCurStreamPosition += sz;
							nzero = -1;
						}
					}
				}
				// read
				if (nzero < 0)
					nzero = _INTERNAL::read_nzero(SS, sz);
				if (nzero == 0)
				{
					// IT->fCurIndex should be = I.Ptr, *sel = TRUE
					if (n_zero_fill > 0)
					{
						_INTERNAL::SET_ZERO(p, n_zero_fill); p += n_zero_fill;
						n_zero_fill = 0;
					}
					TYPE Val; SS >> Val;
					*p++ = VAL_CONVERT(MEM_TYPE, TYPE, Val);
					sel++;
					IT->fCurStreamPosition += sz + sizeof(Val);
					I.Ptr++; IT->fCurIndex = I.Ptr; n--;
				} else {
					C_Int64 m = nzero;
					if (IT->fCurIndex < I.Ptr) m -= I.Ptr - IT->fCurIndex;
					if (m > n) m = n;
					I.Ptr += m; n -= m;
					for (; m > 0; m--)
						if (*sel++) n_zero_fill ++;
					if (I.Ptr - IT->fCurIndex >= nzero)
					{
						IT->fCurIndex = I.Ptr;
						IT->fCurStreamPosition += sz;
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

		inline static void append_index(C_Int64 I, CdSpArray<SP_TYPE> *IT)
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
			CdSpArray<SP_TYPE> *IT = static_cast<CdSpArray<SP_TYPE>*>(I.Handler);
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
					if (!_INTERNAL::IS_ZERO(*p))
					{
						if (IT->fNumZero > 0)
						{
							const int up_bound = 0xFFFF - 1;
							if (IT->fNumZero <= up_bound*3)
							{
								while (IT->fNumZero > 0)
								{
									C_UInt16 L = (IT->fNumZero <= up_bound) ?
										IT->fNumZero : up_bound;
									SS << L;
									IT->fTotalStreamSize += sizeof(L);
									append_index(I.Ptr-1 - IT->fNumZero + L, IT);
									IT->fNumZero -= L;
								}
							} else {
								SS << C_UInt16(0xFFFF) << TdGDSPos(IT->fNumZero);
								IT->fTotalStreamSize += sizeof(C_UInt16) + GDS_POS_SIZE;
								IT->fNumZero = 0;
								append_index(I.Ptr-1, IT);
							}
						}
						SS << C_UInt16(0) << VAL_CONVERT(TYPE, MEM_TYPE, *p);
						IT->fTotalStreamSize += sizeof(C_UInt16) + sizeof(TYPE);
						append_index(I.Ptr, IT);
					} else {
						IT->fNumZero ++;
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

	typedef CdSpArray<TSpInt8>      CdSparseInt8;
	typedef CdSpArray<TSpUInt8>     CdSparseUInt8;
	typedef CdSpArray<TSpInt16>     CdSparseInt16;
	typedef CdSpArray<TSpUInt16>    CdSparseUInt16;
	typedef CdSpArray<TSpInt32>     CdSparseInt32;
	typedef CdSpArray<TSpUInt32>    CdSparseUInt32;
	typedef CdSpArray<TSpInt64>     CdSparseInt64;
	typedef CdSpArray<TSpUInt64>    CdSparseUInt64;
	typedef CdSpArray<TSpReal32>    CdSparseReal32;
	typedef CdSpArray<TSpReal64>    CdSparseReal64;


	/// get whether it is a sparse array or not
	bool IsSparseArray(CdGDSObj *Obj);

}

#endif /* _HEADER_COREARRAY_SPARSE_GDS_ */
