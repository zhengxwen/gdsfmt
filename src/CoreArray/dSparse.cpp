// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dSparse.cpp: Sparse array in GDS format
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

#ifndef COREARRAY_COMPILER_OPTIMIZE_FLAG
#   define COREARRAY_COMPILER_OPTIMIZE_FLAG  3
#endif

#include "dSparse.h"


namespace CoreArray
{
	template<typename TClass> static CdObjRef *OnObjCreate()
	{
		return new TClass();
	}

	COREARRAY_DLL_LOCAL void RegisterClass_Sparse()
	{
		#define REG_CLASS(T, CLASS, CType, Desp)	\
			dObjManager().AddClass(TdTraits< T >::StreamName(), \
				OnObjCreate< CLASS >, CdObjClassMgr::CType, Desp)

		// integers
		REG_CLASS(TSpInt8,  CdSparseInt8,  ctArray, "sparse signed integer of 8 bits");
		REG_CLASS(TSpInt16, CdSparseInt16, ctArray, "sparse signed integer of 16 bits");
		REG_CLASS(TSpInt32, CdSparseInt32, ctArray, "sparse signed integer of 32 bits");
		REG_CLASS(TSpInt64, CdSparseInt64, ctArray, "sparse signed integer of 64 bits");
		REG_CLASS(TSpUInt8,  CdSparseUInt8,  ctArray, "sparse unsigned integer of 8 bits");
		REG_CLASS(TSpUInt16, CdSparseUInt16, ctArray, "sparse unsigned integer of 16 bits");
		REG_CLASS(TSpUInt32, CdSparseUInt32, ctArray, "sparse unsigned integer of 32 bits");
		REG_CLASS(TSpUInt64, CdSparseUInt64, ctArray, "sparse unsigned integer of 64 bits");

		// real numbers
		REG_CLASS(TSpReal32, CdSparseReal32, ctArray, "sparse real number (32 bits)");
		REG_CLASS(TSpReal64, CdSparseReal64, ctArray, "sparse real number (64 bits)");

		#undef REG_CLASS
	}


	/// get whether it is a sparse array or not
	bool IsSparseArray(CdGDSObj *Obj)
	{
		if (dynamic_cast<CdContainer*>(Obj))
		{
			CdContainer *p = static_cast<CdContainer*>(Obj);
			int tr = p->TraitFlag();
			return (tr == COREARRAY_TR_SPARSE_INTEGER) ||
				(tr == COREARRAY_TR_SPARSE_REAL);
		} else
			return false;
	}
}


// ===========================================================

using namespace CoreArray;

static const char *VAR_INDEX = "INDEX";

CdSpExStruct::CdSpExStruct(int sz): SpElmSize(sz)
{
	fIndexingID = 0;
	fIndexingStream = NULL;
	fTotalStreamSize = fCurStreamPosition = 0;
	fCurIndex = fNumRecord = fNumZero = 0;
}

void CdSpExStruct::SpLoad(CdReader &Reader, CdBlockStream *GDSStream,
	CdPipeMgrItem *PipeInfo, CdAllocator &Allocator)
{
	if (GDSStream)
	{
		// get the indexing stream
		Reader[VAR_INDEX] >> fIndexingID;
		fIndexingStream = GDSStream->Collection()[fIndexingID];
		fNumRecord = fIndexingStream->GetSize() / (sizeof(SIZE64) + GDS_POS_SIZE);
		// get the total size
		fTotalStreamSize = 0;
		if (PipeInfo)
		{
			fTotalStreamSize = PipeInfo->StreamTotalIn();
		} else {
			if (Allocator.BufStream())
				fTotalStreamSize = Allocator.BufStream()->GetSize();
		}
		// initialize
		fCurIndex = fCurStreamPosition = 0;
		fNumZero = 0;
	}
}

void CdSpExStruct::SpSave(CdWriter &Writer, CdBlockStream *GDSStream)
{
	if (GDSStream)
	{
		if (!fIndexingStream)
			fIndexingStream = GDSStream->Collection().NewBlockStream();
		TdGDSBlockID Entry = fIndexingStream->ID();
		Writer[VAR_INDEX] << Entry;
	}
}

void CdSpExStruct::SpWriteZero(CdAllocator &Allocator)
{
	if (fNumZero > 0)
	{
		const int up_bound = 0xFFFF - 1;
		Allocator.SetPosition(fTotalStreamSize);
		BYTE_LE<CdAllocator> SS(Allocator);
		if (fNumZero <= up_bound*3)
		{
			for (int m=fNumZero; m > 0; )
			{
				C_UInt16 n = (m <= up_bound) ? m : up_bound;
				SS << n;
				fTotalStreamSize += sizeof(n);
				m -= n;
			}
		} else {
			SS << C_UInt16(0xFFFF) << TdGDSPos(fNumZero);
			fTotalStreamSize += sizeof(C_UInt16) + GDS_POS_SIZE;
		}
		fNumZero = 0;
	}
}

void CdSpExStruct::SpSetPos(C_Int64 idx, CdAllocator &Allocator,
	C_Int64 TotalCount)
{
	if (fCurIndex == idx)
	{
		Allocator.SetPosition(fCurStreamPosition);
		return;
	} else {
		BYTE_LE<CdAllocator> SS(Allocator);
		int sz;
		if (idx == TotalCount)
		{
			fCurIndex = TotalCount;
			fCurStreamPosition = fTotalStreamSize;
			Allocator.SetPosition(fCurStreamPosition);
			return;
		} else if ((idx > TotalCount) || (idx < 0))
		{
			throw ErrArray("CdSpArray::SetStreamPos: Invalid Index.");
		} else if (idx > fCurIndex)
		{
			Allocator.SetPosition(fCurStreamPosition);
			C_Int64 NZero = _INTERNAL::read_nzero(SS, sz);
			if (idx < fCurIndex + NZero)
			{
				Allocator.SetPosition(fCurStreamPosition);
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
			BYTE_LE<CdStream> IS(fIndexingStream);
			C_Int64 st=0, ed=fNumRecord, CI=0, CI_i=0;
			while (st < ed)
			{
				C_Int64 mid = (st + ed) / 2;
				IS.SetPosition(mid * SIZE);
				C_Int64 I; IS >> I;
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
				IS.SetPosition(CI_i * SIZE + sizeof(C_Int64));
				TdGDSPos s; IS >> s;
				fCurStreamPosition = s;
			}
		}

		// move forward to the correct position (fCurIndex <= idx)
		Allocator.SetPosition(fCurStreamPosition);
		while (fCurIndex < idx)
		{
			C_Int64 NZero = _INTERNAL::read_nzero(SS, sz);
			if (NZero == 0)
			{
				fCurStreamPosition += sz + SpElmSize;
				Allocator.SetPosition(fCurStreamPosition);
				fCurIndex ++;
			} else if (fCurIndex + NZero <= idx)
			{
				fCurStreamPosition += sz;
				fCurIndex += NZero;
			} else {
				Allocator.SetPosition(fCurStreamPosition);
				break;
			}
		}
	}
}

