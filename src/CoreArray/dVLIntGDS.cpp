// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dVLIntGDS.cpp: Encoding variable-length integers in GDS
//
// Copyright (C) 2016    Xiuwen Zheng
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

#include <typeinfo>
#include "dVLIntGDS.h"


using namespace std;
using namespace CoreArray;

static const char *VAR_INDEX = "INDEX";


// =====================================================================

CdVL_Int::CdVL_Int(): CdArray<TVL_Int>()
{
	fCurStreamPosition = 0;
	fCurIndex = 0;
	fTotalStreamSize = 0;
	fIndexingID = 0;
	fIndexingStream = NULL;
}

void CdVL_Int::AppendIter(CdIterator &I, C_Int64 Count)
{
	if ((Count >= 65536) && (typeid(*this) == typeid(*I.Handler)))
	{
		if (fAllocator.BufStream())
		{
			CdVL_Int *Src = (CdVL_Int *)I.Handler;
			Src->Allocator().BufStream()->FlushWrite();

			Src->SetStreamPos(I.Ptr);
			SIZE64 P1 = Src->fCurStreamPosition;
			Src->SetStreamPos(I.Ptr + Count);
			SIZE64 P2 = Src->fCurStreamPosition;
			fAllocator.BufStream()->CopyFrom(
				*(Src->Allocator().BufStream()->Stream()), P1, P2-P1);

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
	CdArray<TVL_Int>::AppendIter(I, Count);
}

void CdVL_Int::GetOwnBlockStream(vector<const CdBlockStream*> &Out) const
{
	CdArray<TVL_Int>::GetOwnBlockStream(Out);
	if (fIndexingStream) Out.push_back(fIndexingStream);
}

void CdVL_Int::GetOwnBlockStream(vector<CdStream*> &Out)
{
	CdArray<TVL_Int>::GetOwnBlockStream(Out);
	if (fIndexingStream) Out.push_back(fIndexingStream);
}

void CdVL_Int::Loading(CdReader &Reader, TdVersion Version)
{
	CdArray<TVL_Int>::Loading(Reader, Version);
	// load the content
	if (fGDSStream)
	{
		Reader[VAR_INDEX] >> fIndexingID;
		fIndexingStream = fGDSStream->Collection()[fIndexingID];
	}
	// get the total size
	if (fGDSStream)
	{
		if (fPipeInfo)
		{
			fTotalStreamSize = fPipeInfo->StreamTotalIn();
		} else {
			if (fAllocator.BufStream())
				fTotalStreamSize = fAllocator.BufStream()->GetSize();
		}
	}
}

void CdVL_Int::Saving(CdWriter &Writer)
{
	CdArray<TVL_Int>::Saving(Writer);
	// save data
	if (fGDSStream != NULL)
	{
		if (!fIndexingStream)
			fIndexingStream = fGDSStream->Collection().NewBlockStream();
		TdGDSBlockID Entry = fIndexingStream->ID();
		Writer[VAR_INDEX] << Entry;
	}
}

void CdVL_Int::SetStreamPos(C_Int64 idx)
{
	if (fCurIndex == idx)
	{
		fAllocator.SetPosition(fCurStreamPosition);
		return;
	} else {
		if (idx == fTotalCount)
		{
			fCurIndex = fTotalCount;
			fCurStreamPosition = fTotalStreamSize;
			fAllocator.SetPosition(fCurStreamPosition);
			return;
		} else if ((idx > fTotalCount) || (idx < 0))
		{
			throw ErrArray("CdVL_Int::SetStreamPos: Invalid Index.");
		} else if (idx < fCurIndex)
		{
			C_Int64 i = idx >> 16;
			if ((i == 0) || !fIndexingStream)
			{
				fCurIndex = fCurStreamPosition = 0;
			} else {
				fIndexingStream->SetPosition((i-1)*GDS_POS_SIZE);
				TdGDSPos pos;
				BYTE_LE<CdStream>(fIndexingStream) >> pos;
				fCurIndex = i << 16;
				fCurStreamPosition = pos;
			}
		} else {
			// idx > fCurIndex
			C_Int64 i = idx >> 16;
			if (((i << 16) > idx) && fIndexingStream)
			{
				fIndexingStream->SetPosition((i-1)*GDS_POS_SIZE);
				TdGDSPos pos;
				BYTE_LE<CdStream>(fIndexingStream) >> pos;
				fCurIndex = i << 16;
				fCurStreamPosition = pos;
			}
		}

		fAllocator.SetPosition(fCurStreamPosition);
		C_UInt8 Buf[COREARRAY_ALLOC_FUNC_BUFFER];
		while (fCurIndex < idx)
		{
			C_Int64 n = idx - fCurIndex;
			ssize_t m = (n <= sizeof(Buf)) ? n : sizeof(Buf);
			fAllocator.ReadData(Buf, m);
			C_UInt8 *s = Buf;
			for (; m > 0; m--)
			{
				if (!(*s++ & 0x80))
					fCurIndex ++;
			}
		}
		fCurStreamPosition = fAllocator.Position();
	}
}


// =====================================================================

CdVL_UInt::CdVL_UInt(): CdArray<TVL_UInt>()
{
	fCurStreamPosition = 0;
	fCurIndex = 0;
	fTotalStreamSize = 0;
	fIndexingID = 0;
	fIndexingStream = NULL;
}

void CdVL_UInt::AppendIter(CdIterator &I, C_Int64 Count)
{
	if ((Count >= 65536) && (typeid(*this) == typeid(*I.Handler)))
	{
		if (fAllocator.BufStream())
		{
			CdVL_UInt *Src = (CdVL_UInt *)I.Handler;
			Src->Allocator().BufStream()->FlushWrite();

			Src->SetStreamPos(I.Ptr);
			SIZE64 P1 = Src->fCurStreamPosition;
			Src->SetStreamPos(I.Ptr + Count);
			SIZE64 P2 = Src->fCurStreamPosition;
			fAllocator.BufStream()->CopyFrom(
				*(Src->Allocator().BufStream()->Stream()), P1, P2-P1);

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

void CdVL_UInt::GetOwnBlockStream(vector<const CdBlockStream*> &Out) const
{
	CdArray<TVL_UInt>::GetOwnBlockStream(Out);
	if (fIndexingStream) Out.push_back(fIndexingStream);
}

void CdVL_UInt::GetOwnBlockStream(vector<CdStream*> &Out)
{
	CdArray<TVL_UInt>::GetOwnBlockStream(Out);
	if (fIndexingStream) Out.push_back(fIndexingStream);
}

void CdVL_UInt::Loading(CdReader &Reader, TdVersion Version)
{
	CdArray<TVL_UInt>::Loading(Reader, Version);
	// load the content
	if (fGDSStream)
	{
		Reader[VAR_INDEX] >> fIndexingID;
		fIndexingStream = fGDSStream->Collection()[fIndexingID];
	}
	// get the total size
	if (fGDSStream)
	{
		if (fPipeInfo)
		{
			fTotalStreamSize = fPipeInfo->StreamTotalIn();
		} else {
			if (fAllocator.BufStream())
				fTotalStreamSize = fAllocator.BufStream()->GetSize();
		}
	}
}

void CdVL_UInt::Saving(CdWriter &Writer)
{
	CdArray<TVL_UInt>::Saving(Writer);
	// save data
	if (fGDSStream != NULL)
	{
		if (!fIndexingStream)
			fIndexingStream = fGDSStream->Collection().NewBlockStream();
		TdGDSBlockID Entry = fIndexingStream->ID();
		Writer[VAR_INDEX] << Entry;
	}
}

void CdVL_UInt::SetStreamPos(C_Int64 idx)
{
	if (fCurIndex == idx)
	{
		fAllocator.SetPosition(fCurStreamPosition);
		return;
	} else {
		if (idx == fTotalCount)
		{
			fCurIndex = fTotalCount;
			fCurStreamPosition = fTotalStreamSize;
			fAllocator.SetPosition(fCurStreamPosition);
			return;
		} else if ((idx > fTotalCount) || (idx < 0))
		{
			throw ErrArray("CdVL_UInt::SetStreamPos: Invalid Index.");
		} else if (idx < fCurIndex)
		{
			C_Int64 i = idx >> 16;
			if ((i == 0) || !fIndexingStream)
			{
				fCurIndex = fCurStreamPosition = 0;
			} else {
				fIndexingStream->SetPosition((i-1)*GDS_POS_SIZE);
				TdGDSPos pos;
				BYTE_LE<CdStream>(fIndexingStream) >> pos;
				fCurIndex = i << 16;
				fCurStreamPosition = pos;
			}
		} else {
			// idx > fCurIndex
			C_Int64 i = idx >> 16;
			if (((i << 16) > idx) && fIndexingStream)
			{
				fIndexingStream->SetPosition((i-1)*GDS_POS_SIZE);
				TdGDSPos pos;
				BYTE_LE<CdStream>(fIndexingStream) >> pos;
				fCurIndex = i << 16;
				fCurStreamPosition = pos;
			}
		}

		fAllocator.SetPosition(fCurStreamPosition);
		C_UInt8 Buf[COREARRAY_ALLOC_FUNC_BUFFER];
		while (fCurIndex < idx)
		{
			C_Int64 n = idx - fCurIndex;
			ssize_t m = (n <= sizeof(Buf)) ? n : sizeof(Buf);
			fAllocator.ReadData(Buf, m);
			C_UInt8 *s = Buf;
			for (; m > 0; m--)
			{
				if (!(*s++ & 0x80))
					fCurIndex ++;
			}
		}
		fCurStreamPosition = fAllocator.Position();
	}
}
