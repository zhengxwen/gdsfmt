// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dVLIntGDS.h: Encoding variable-length integers in GDS
//
// Copyright (C) 2016-2017    Xiuwen Zheng
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
 *	\file     dVLIntGDS.h
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2016 - 2017
 *	\brief    Encoding variable-length integers in GDS
 *	\details
**/

#ifndef _HEADER_COREARRAY_VL_INT_GDS_
#define _HEADER_COREARRAY_VL_INT_GDS_

#include "dStruct.h"


namespace CoreArray
{
	using namespace std;

	/// define variable-length signed integer
	typedef struct { C_Int8 Val; } TVL_Int;

	/// define variable-length unsigned integer
	typedef struct { C_UInt8 Val; } TVL_UInt;


	/// Traits of variable-length signed integer
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TVL_Int>
	{
		typedef C_Int64 ElmType;

		static const int trVal = COREARRAY_TR_VARIABLE_LEN_INTEGER;
		static const unsigned BitOf = 64u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svCustomInt;

		static const char *StreamName() { return "dVL_Int"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Int64 Min() { return std::numeric_limits<C_Int64>::min(); }
		COREARRAY_INLINE static C_Int64 Max() { return std::numeric_limits<C_Int64>::max(); }
	};

	/// Traits of variable-length unsigned integer
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TVL_UInt>
	{
		typedef C_UInt64 ElmType;

		static const int trVal = COREARRAY_TR_VARIABLE_LEN_INTEGER;
		static const unsigned BitOf = 64u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svCustomInt;

		static const char *StreamName() { return "dVL_UInt"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_UInt64 Min() { return 0; }
		COREARRAY_INLINE static C_UInt64 Max() { return std::numeric_limits<C_UInt64>::max(); }
	};



	// =====================================================================
	// Variable-length signed integers
	// =====================================================================

	/// Container of variable-length signed integers
	class COREARRAY_DLL_DEFAULT CdVL_Int: public CdArray<TVL_Int>
	{
	public:
		template<typename ALLOC_TYPE, typename MEM_TYPE> friend struct ALLOC_FUNC;
		typedef C_Int64 ElmType;

		/// constructor
		CdVL_Int();

		/// append new data from an iterator
		virtual void AppendIter(CdIterator &I, C_Int64 Count);
		/// get a list of CdBlockStream owned by this object, except fGDSStream
		virtual void GetOwnBlockStream(vector<const CdBlockStream*> &Out) const;
		/// get a list of CdStream owned by this object, except fGDSStream
		virtual void GetOwnBlockStream(vector<CdStream*> &Out);

	protected:

		C_Int64 fCurIndex;
		SIZE64 fCurStreamPosition;
		SIZE64 fTotalStreamSize;
		TdGDSBlockID fIndexingID;       ///< indexing block ID
		CdBlockStream *fIndexingStream; ///< the GDS stream for indexing

		/// loading function for serialization
		virtual void Loading(CdReader &Reader, TdVersion Version);
		/// saving function for serialization
		virtual void Saving(CdWriter &Writer);
		/// set stream position to the corresponding array index
		void SetStreamPos(C_Int64 idx);
	};


	// =====================================================================
	// Template for Allocator for variable-length signed integer
	// =====================================================================

	/// Template functions for allocator of variable-length signed integer
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TVL_Int, MEM_TYPE>
	{
		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			CdVL_Int *IT = static_cast<CdVL_Int*>(I.Handler);
			IT->SetStreamPos(I.Ptr);
			C_UInt8 Buf[COREARRAY_ALLOC_FUNC_BUFFER], *pBuf=Buf;
			for (ssize_t nn=n; nn > 0; )
			{
				ssize_t N = Buf + sizeof(Buf) - pBuf;
				ssize_t Cnt = (nn >= N) ? N : nn;
				C_UInt8 *pEnd = pBuf + Cnt;
				I.Allocator->ReadData(pBuf, Cnt);
				C_UInt64 v = 0;
				C_UInt8 shift = 0;
				for (C_UInt8 *s=Buf; s < pEnd; s++)
				{
					v |= C_UInt64(*s & 0x7F) << shift;
					if (!(*s & 0x80))
					{
						C_Int64 vv = !(v & 0x01) ? (v >> 1) : ~(v >> 1);
						*p++ = VAL_CONV_FROM_I64(MEM_TYPE, vv);
						v = shift = 0;
						nn --;
					} else {
						shift += 7;
						if (shift >= 63)
						{
							v |= 0x8000000000000000LL;
							C_Int64 vv = !(v & 0x01) ? (v >> 1) : ~(v >> 1);
							*p++ = VAL_CONV_FROM_I64(MEM_TYPE, vv);
							v = shift = 0;
							nn --;
						}
					}
				}
				pBuf = Buf;
				if (shift > 0)
				{
					shift /= 7;
					for (C_UInt8 *s=pEnd-shift; shift > 0; shift--)
						*pBuf++ = *s++;
				}
			}
			I.Ptr += n;
			IT->fCurIndex = I.Ptr;
			IT->fCurStreamPosition = I.Allocator->Position();
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr++;
			CdVL_Int *IT = static_cast<CdVL_Int*>(I.Handler);
			IT->SetStreamPos(I.Ptr);
			C_UInt8 Buf[COREARRAY_ALLOC_FUNC_BUFFER], *pBuf=Buf;
			for (ssize_t nn=n; nn > 0; )
			{
				ssize_t N = Buf + sizeof(Buf) - pBuf;
				ssize_t Cnt = (nn >= N) ? N : nn;
				C_UInt8 *pEnd = pBuf + Cnt;
				I.Allocator->ReadData(pBuf, Cnt);
				C_UInt64 v = 0;
				C_UInt8 shift = 0;
				for (C_UInt8 *s=Buf; s < pEnd; s++)
				{
					v |= C_UInt64(*s & 0x7F) << shift;
					if (!(*s & 0x80))
					{
						if (*sel++)
						{
							C_Int64 vv = !(v & 0x01) ? (v >> 1) : ~(v >> 1);
							*p++ = VAL_CONV_FROM_I64(MEM_TYPE, vv);
						}
						v = shift = 0;
						nn --;
					} else {
						shift += 7;
						if (shift >= 63)
						{
							v |= 0x8000000000000000LL;
							C_Int64 vv = !(v & 0x01) ? (v >> 1) : ~(v >> 1);
							*p++ = VAL_CONV_FROM_I64(MEM_TYPE, vv);
							v = shift = 0;
							nn --;
						}
					}
				}
				pBuf = Buf;
				if (shift > 0)
				{
					shift /= 7;
					for (C_UInt8 *s=pEnd-shift; shift > 0; shift--)
						*pBuf++ = *s++;
				}
			}
			I.Ptr += n;
			IT->fCurIndex = I.Ptr;
			IT->fCurStreamPosition = I.Allocator->Position();
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			const ssize_t NBuf = COREARRAY_ALLOC_FUNC_BUFFER / 9;
			CdVL_Int *IT = static_cast<CdVL_Int*>(I.Handler);
			if (I.Ptr < IT->fTotalCount)
			{
				throw ErrArray("Insert a variable-length encoding integer wrong.");
			} else if (I.Ptr == IT->fTotalCount)
			{
				// append
				I.Allocator->SetPosition(IT->fTotalStreamSize);
				// buffer
				C_UInt8 Buf[COREARRAY_ALLOC_FUNC_BUFFER];
				// for-loop
				while (n > 0)
				{
					C_UInt8 *s = Buf;
					ssize_t nn = (n <= NBuf) ? n : NBuf;
					ssize_t mm = 0x10000 - (I.Ptr & 0xFFFF);
					if (nn > mm) nn = mm;
					for (ssize_t m=nn; m > 0; m--)
					{
						C_UInt64 val;
						C_Int64 v = VAL_CONV_TO_I64(MEM_TYPE, *p++);
						if (v >= 0)
							val = C_UInt64(v) << 1;
						else
							val = (C_UInt64(~v) << 1) | 0x01;
						if (val <= 0x7F) {
							*s++ = val;
						} else if (val <= 0x3FFF) {
							*s++ = val | 0x80; *s++ = val >> 7;
						} else if (val <= 0x1FFFFF) {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = val >> 14;
						} else if (val <= 0xFFFFFFF) {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = (val >> 14) | 0x80; *s++ = val >> 21;
						} else if (val <= 0x7FFFFFFFFLL) {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = (val >> 14) | 0x80; *s++ = (val >> 21) | 0x80;
							*s++ = val >> 28;
						} else if (val <= 0x3FFFFFFFFFFLL) {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = (val >> 14) | 0x80; *s++ = (val >> 21) | 0x80;
							*s++ = (val >> 28) | 0x80; *s++ = val >> 35;
						} else if (val <= 0x1FFFFFFFFFFFFLL) {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = (val >> 14) | 0x80; *s++ = (val >> 21) | 0x80;
							*s++ = (val >> 28) | 0x80; *s++ = (val >> 35) | 0x80;
							*s++ = val >> 42;
						} else if (val <= 0xFFFFFFFFFFFFFFLL) {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = (val >> 14) | 0x80; *s++ = (val >> 21) | 0x80;
							*s++ = (val >> 28) | 0x80; *s++ = (val >> 35) | 0x80;
							*s++ = (val >> 42) | 0x80; *s++ = val >> 49;
						} else {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = (val >> 14) | 0x80; *s++ = (val >> 21) | 0x80;
							*s++ = (val >> 28) | 0x80; *s++ = (val >> 35) | 0x80;
							*s++ = (val >> 42) | 0x80; *s++ = (val >> 49) | 0x80;
							*s++ = val >> 56;
						}
					}
					ssize_t m = s - Buf;
					I.Allocator->WriteData(Buf, m);
					IT->fTotalStreamSize += m;
					I.Ptr += nn;
					if (!(I.Ptr & 0xFFFF) && IT->fIndexingStream)
					{
						IT->fIndexingStream->SetPosition(((I.Ptr>>16)-1) * GDS_POS_SIZE);
						TdGDSPos pp = I.Allocator->Position();
						BYTE_LE<CdStream>(IT->fIndexingStream) << pp;
					}
					n -= nn;
				}
			} else
				throw ErrArray("Invalid position for writing data.");
			return p;
		}
	};




	// =====================================================================
	// Variable-length unsigned integers
	// =====================================================================

	/// Container of variable-length unsigned integers
	class COREARRAY_DLL_DEFAULT CdVL_UInt: public CdArray<TVL_UInt>
	{
	public:
		template<typename ALLOC_TYPE, typename MEM_TYPE> friend struct ALLOC_FUNC;
		typedef C_UInt64 ElmType;

		/// constructor
		CdVL_UInt();

		/// append new data from an iterator
		virtual void AppendIter(CdIterator &I, C_Int64 Count);
		/// get a list of CdBlockStream owned by this object, except fGDSStream
		virtual void GetOwnBlockStream(vector<const CdBlockStream*> &Out) const;
		/// get a list of CdStream owned by this object, except fGDSStream
		virtual void GetOwnBlockStream(vector<CdStream*> &Out);

	protected:

		TdGDSBlockID fIndexingID;       ///< indexing block ID
		CdBlockStream *fIndexingStream; ///< the GDS stream for indexing
		SIZE64 fTotalStreamSize;    ///< the total stream size
		SIZE64 fCurStreamPosition;  ///< the current stream position
		C_Int64 fCurIndex;  ///< the current array index

		/// loading function for serialization
		virtual void Loading(CdReader &Reader, TdVersion Version);
		/// saving function for serialization
		virtual void Saving(CdWriter &Writer);
		/// set stream position to the corresponding array index
		void SetStreamPos(C_Int64 idx);
	};


	// =====================================================================
	// Template for Allocator for variable-length unsigned integer
	// =====================================================================

	/// Template functions for allocator of variable-length unsigned integer
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TVL_UInt, MEM_TYPE>
	{
		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			CdVL_UInt *IT = static_cast<CdVL_UInt*>(I.Handler);
			IT->SetStreamPos(I.Ptr);
			C_UInt8 Buf[COREARRAY_ALLOC_FUNC_BUFFER], *pBuf=Buf;
			for (ssize_t nn=n; nn > 0; )
			{
				ssize_t N = Buf + sizeof(Buf) - pBuf;
				ssize_t Cnt = (nn >= N) ? N : nn;
				C_UInt8 *pEnd = pBuf + Cnt;
				I.Allocator->ReadData(pBuf, Cnt);
				C_UInt64 v = 0;
				C_UInt8 shift = 0;
				for (C_UInt8 *s=Buf; s < pEnd; s++)
				{
					v |= C_UInt64(*s & 0x7F) << shift;
					if (!(*s & 0x80))
					{
						*p++ = VAL_CONV_FROM_U64(MEM_TYPE, v);
						v = shift = 0;
						nn --;
					} else {
						shift += 7;
						if (shift >= 63)
						{
							*p++ = VAL_CONV_FROM_U64(MEM_TYPE, v | 0x8000000000000000LL);
							v = shift = 0;
							nn --;
						}
					}
				}
				pBuf = Buf;
				if (shift > 0)
				{
					shift /= 7;
					for (C_UInt8 *s=pEnd-shift; shift > 0; shift--)
						*pBuf++ = *s++;
				}
			}
			I.Ptr += n;
			IT->fCurIndex = I.Ptr;
			IT->fCurStreamPosition = I.Allocator->Position();
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr++;
			CdVL_UInt *IT = static_cast<CdVL_UInt*>(I.Handler);
			IT->SetStreamPos(I.Ptr);
			C_UInt8 Buf[COREARRAY_ALLOC_FUNC_BUFFER], *pBuf=Buf;
			for (ssize_t nn=n; nn > 0; )
			{
				ssize_t N = Buf + sizeof(Buf) - pBuf;
				ssize_t Cnt = (nn >= N) ? N : nn;
				C_UInt8 *pEnd = pBuf + Cnt;
				I.Allocator->ReadData(pBuf, Cnt);
				C_UInt64 v = 0;
				C_UInt8 shift = 0;
				for (C_UInt8 *s=Buf; s < pEnd; s++)
				{
					v |= C_UInt64(*s & 0x7F) << shift;
					if (!(*s & 0x80))
					{
						if (*sel++)
							*p++ = VAL_CONV_FROM_U64(MEM_TYPE, v);
						v = shift = 0;
						nn --;
					} else {
						shift += 7;
						if (shift >= 63)
						{
							*p++ = VAL_CONV_FROM_U64(MEM_TYPE, v | 0x8000000000000000LL);
							v = shift = 0;
							nn --;
						}
					}
				}
				pBuf = Buf;
				if (shift > 0)
				{
					shift /= 7;
					for (C_UInt8 *s=pEnd-shift; shift > 0; shift--)
						*pBuf++ = *s++;
				}
			}
			I.Ptr += n;
			IT->fCurIndex = I.Ptr;
			IT->fCurStreamPosition = I.Allocator->Position();
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			const ssize_t NBuf = COREARRAY_ALLOC_FUNC_BUFFER / 9;
			CdVL_UInt *IT = static_cast<CdVL_UInt*>(I.Handler);
			if (I.Ptr < IT->fTotalCount)
			{
				throw ErrArray("Insert variable-length encoding integers wrong, only append integers.");
			} else if (I.Ptr == IT->fTotalCount)
			{
				// append
				I.Allocator->SetPosition(IT->fTotalStreamSize);
				// buffer
				C_UInt8 Buf[COREARRAY_ALLOC_FUNC_BUFFER];
				// for-loop
				while (n > 0)
				{
					C_UInt8 *s = Buf;
					ssize_t nn = (n <= NBuf) ? n : NBuf;
					ssize_t mm = 0x10000 - (I.Ptr & 0xFFFF);
					if (nn > mm) nn = mm;
					for (ssize_t m=nn; m > 0; m--)
					{
						C_UInt64 val = VAL_CONV_TO_U64(MEM_TYPE, *p++);
						if (val <= 0x7F) {
							*s++ = val;
						} else if (val <= 0x3FFF) {
							*s++ = val | 0x80; *s++ = val >> 7;
						} else if (val <= 0x1FFFFF) {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = val >> 14;
						} else if (val <= 0xFFFFFFF) {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = (val >> 14) | 0x80; *s++ = val >> 21;
						} else if (val <= 0x7FFFFFFFFLL) {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = (val >> 14) | 0x80; *s++ = (val >> 21) | 0x80;
							*s++ = val >> 28;
						} else if (val <= 0x3FFFFFFFFFFLL) {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = (val >> 14) | 0x80; *s++ = (val >> 21) | 0x80;
							*s++ = (val >> 28) | 0x80; *s++ = val >> 35;
						} else if (val <= 0x1FFFFFFFFFFFFLL) {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = (val >> 14) | 0x80; *s++ = (val >> 21) | 0x80;
							*s++ = (val >> 28) | 0x80; *s++ = (val >> 35) | 0x80;
							*s++ = val >> 42;
						} else if (val <= 0xFFFFFFFFFFFFFFLL) {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = (val >> 14) | 0x80; *s++ = (val >> 21) | 0x80;
							*s++ = (val >> 28) | 0x80; *s++ = (val >> 35) | 0x80;
							*s++ = (val >> 42) | 0x80; *s++ = val >> 49;
						} else {
							*s++ = val | 0x80; *s++ = (val >> 7) | 0x80;
							*s++ = (val >> 14) | 0x80; *s++ = (val >> 21) | 0x80;
							*s++ = (val >> 28) | 0x80; *s++ = (val >> 35) | 0x80;
							*s++ = (val >> 42) | 0x80; *s++ = (val >> 49) | 0x80;
							*s++ = val >> 56;
						}
					}
					ssize_t m = s - Buf;
					I.Allocator->WriteData(Buf, m);
					IT->fTotalStreamSize += m;
					I.Ptr += nn;
					if (!(I.Ptr & 0xFFFF) && IT->fIndexingStream)
					{
						IT->fIndexingStream->SetPosition(((I.Ptr>>16)-1) * GDS_POS_SIZE);
						TdGDSPos pp = I.Allocator->Position();
						BYTE_LE<CdStream>(IT->fIndexingStream) << pp;
					}
					n -= nn;
				}
			} else
				throw ErrArray("Invalid position for writing data.");
			return p;
		}
	};

}

#endif /* _HEADER_COREARRAY_VL_INT_GDS_ */
