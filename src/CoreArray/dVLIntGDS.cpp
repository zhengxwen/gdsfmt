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
// Copyright (C) 2016-2019    Xiuwen Zheng
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

#include "dVLIntGDS.h"
#include <typeinfo>


using namespace std;
using namespace CoreArray;



// =====================================================================

CdVL_Int::CdVL_Int(): CdArray<TVL_Int>(1)
{
	fCurStreamPosition = 0;
	fCurIndex = 0;
	fTotalStreamSize = 0;
	fIndexingID = 0;
	fPersistIndex.Clear();
}

void CdVL_Int::AppendIter(CdIterator &I, C_Int64 Count)
{
	if ((Count >= 65536) && (typeid(*this) == typeid(*I.Handler)))
	{
		if (fAllocator.BufStream())
		{
			CdVL_Int *Src = (CdVL_Int *)I.Handler;
			Src->Allocator().BufStream()->FlushWrite();

			// copy stream
			Src->SetStreamPos(I.Ptr);
			SIZE64 P1 = Src->fCurStreamPosition;
			Src->SetStreamPos(I.Ptr + Count);
			SIZE64 SrcLen = Src->fCurStreamPosition - P1;
			fAllocator.BufStream()->CopyFrom(
				*(Src->Allocator().BufStream()->Stream()), P1, SrcLen);

			// indexing information (nothing to do without a checkpoint table)
			if (fPersistIndex.Stream())
			{
				// the checkpoints sit at the absolute indices that are
				// multiples of STRIDE in each container; if the source and the
				// destination share that alignment, every checkpoint of the
				// appended range has a counterpart in the source lying a
				// constant Delta away, so the table can be carried over
				// instead of parsing the copied bytes
				const C_Int64 SD = CdVarLenIndex::STRIDE;
				const C_Int64 tEnd = ((fTotalCount + Count) / SD) * SD;
				C_Int64 pi;  SIZE64 pp;
				// the source table is trusted only as far as it goes: if its
				// last needed checkpoint is missing, Lookup() settles for an
				// earlier one and the fast path is off
				bool aligned = !((fTotalCount - I.Ptr) % SD) &&
					Src->fPersistIndex.Lookup(I.Ptr + (tEnd - fTotalCount),
						pi, pp) && (pi == I.Ptr + (tEnd - fTotalCount));
				if (aligned)
				{
					SIZE64 Delta = fTotalStreamSize - P1;
					for (C_Int64 t = (fTotalCount / SD + 1) * SD;
						t <= tEnd; t += SD)
					{
						C_Int64 si = I.Ptr + (t - fTotalCount);
						Src->fPersistIndex.Lookup(si, pi, pp);
						fPersistIndex.Store(t, pp + Delta);
					}
				} else {
					Src->fAllocator.SetPosition(P1);
					const ssize_t NBuf = COREARRAY_ALLOC_FUNC_BUFFER;
					C_UInt8 Buf[NBuf];
					C_Int64 totcnt = fTotalCount;
					SIZE64 ppos = fTotalStreamSize;
					// Preserve in-progress VL decoder state (`shift`) across
					// buffer refills. `nn` is bounded by the *item* count
					// remaining, so a single refill may contain only a fraction
					// of a multi-byte integer; if we reset `shift` between
					// refills we lose track of where the 9-byte cap applies and
					// miscount integers whose 9th byte has its high bit set.
					ssize_t shift = 0;
					for (C_Int64 n = Count; n > 0; )
					{
						ssize_t nn = (n <= NBuf) ? n : NBuf;
						ssize_t mm = CdVarLenIndex::STRIDE -
							(totcnt % CdVarLenIndex::STRIDE);
						if (nn > mm) nn = mm;
						ssize_t save_nn = nn;
						Src->fAllocator.ReadData(Buf, nn);
						for (C_UInt8 *s = Buf; nn > 0; nn--)
						{
							bool flag;
							if (!(*s++ & 0x80))
								flag = true;
							else
								flag = ((++shift) == 9);
							if (flag)
							{
								shift = 0; n --; totcnt ++;
								if (!(totcnt % CdVarLenIndex::STRIDE))
									fPersistIndex.Store(totcnt, ppos + (s - Buf));
							}
						}
						ppos += save_nn;
					}
				}
			}

			// check
			CdAllocArray::TDimItem &R = this->fDimension.front();
			fTotalCount += Count;
			fTotalStreamSize += SrcLen;
			if (fTotalCount >= R.DimElmCnt*(R.DimLen+1))
			{
				R.DimLen = fTotalCount / R.DimElmCnt;
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
	if (fPersistIndex.Stream()) Out.push_back(fPersistIndex.Stream());
}

void CdVL_Int::GetOwnBlockStream(vector<CdStream*> &Out)
{
	CdArray<TVL_Int>::GetOwnBlockStream(Out);
	if (fPersistIndex.Stream()) Out.push_back(fPersistIndex.Stream());
}

void CdVL_Int::Loading(CdReader &Reader, TdVersion Version)
{
	CdArray<TVL_Int>::Loading(Reader, Version);
	// load the content
	if (fGDSStream)
	{
		// get the indexing stream
		Reader[CdVarLenIndex::VarName()] >> fIndexingID;
		fPersistIndex.Attach(fGDSStream->Collection()[fIndexingID]);
		// get the total size
		fTotalStreamSize = 0;
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
	if (fGDSStream)
	{
		if (!fPersistIndex.Stream())
			fPersistIndex.Attach(fGDSStream->Collection().NewBlockStream());
		fIndexingID = fPersistIndex.Stream()->ID();
		Writer[CdVarLenIndex::VarName()] << fIndexingID;
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
		} else {
			// a backward seek has to start over from the beginning of the
			// stream, a forward one keeps the cursor; either way the
			// checkpoint table may know a closer place to start from
			if (idx < fCurIndex) fCurIndex = fCurStreamPosition = 0;
			C_Int64 pi;  SIZE64 pp;
			if (fPersistIndex.Lookup(idx, pi, pp) && (pi > fCurIndex))
			{
				fCurIndex = pi;
				fCurStreamPosition = pp;
			}
		}

		fAllocator.SetPosition(fCurStreamPosition);
		// Preserve in-progress VL decoder state (`shift`) across buffer refills.
		// `m` is bounded by the *item* count remaining, so a single refill may
		// contain only a fraction of a multi-byte integer; if we reset `shift`
		// between refills we lose track of where the 9-byte cap applies and
		// miscount integers whose 9th byte has its high bit set.
		C_UInt8 Buf[COREARRAY_ALLOC_FUNC_BUFFER];
		ssize_t shift = 0;
		while (fCurIndex < idx)
		{
			C_Int64 n = idx - fCurIndex;
			ssize_t m = (n <= (ssize_t)sizeof(Buf)) ? n : sizeof(Buf);
			fAllocator.ReadData(Buf, m);
			C_UInt8 *s = Buf;
			for (; m > 0; m--)
			{
				if (!(*s++ & 0x80))
				{
					fCurIndex ++; shift = 0;
				} else {
					if ((++shift) == 9)
					{
						fCurIndex ++; shift = 0;
					}
				}
			}
		}
		fCurStreamPosition = fAllocator.Position();
	}
}


// =====================================================================

CdVL_UInt::CdVL_UInt(): CdArray<TVL_UInt>(1)
{
	fCurStreamPosition = 0;
	fCurIndex = 0;
	fTotalStreamSize = 0;
	fIndexingID = 0;
	fPersistIndex.Clear();
}

void CdVL_UInt::AppendIter(CdIterator &I, C_Int64 Count)
{
	if ((Count >= 65536) && (typeid(*this) == typeid(*I.Handler)))
	{
		if (fAllocator.BufStream())
		{
			CdVL_UInt *Src = (CdVL_UInt *)I.Handler;
			Src->Allocator().BufStream()->FlushWrite();

			// copy stream
			Src->SetStreamPos(I.Ptr);
			SIZE64 P1 = Src->fCurStreamPosition;
			Src->SetStreamPos(I.Ptr + Count);
			SIZE64 SrcLen = Src->fCurStreamPosition - P1;
			fAllocator.BufStream()->CopyFrom(
				*(Src->Allocator().BufStream()->Stream()), P1, SrcLen);

			// indexing information (nothing to do without a checkpoint table)
			if (fPersistIndex.Stream())
			{
				// the checkpoints sit at the absolute indices that are
				// multiples of STRIDE in each container; if the source and the
				// destination share that alignment, every checkpoint of the
				// appended range has a counterpart in the source lying a
				// constant Delta away, so the table can be carried over
				// instead of parsing the copied bytes
				const C_Int64 SD = CdVarLenIndex::STRIDE;
				const C_Int64 tEnd = ((fTotalCount + Count) / SD) * SD;
				C_Int64 pi;  SIZE64 pp;
				// the source table is trusted only as far as it goes: if its
				// last needed checkpoint is missing, Lookup() settles for an
				// earlier one and the fast path is off
				bool aligned = !((fTotalCount - I.Ptr) % SD) &&
					Src->fPersistIndex.Lookup(I.Ptr + (tEnd - fTotalCount),
						pi, pp) && (pi == I.Ptr + (tEnd - fTotalCount));
				if (aligned)
				{
					SIZE64 Delta = fTotalStreamSize - P1;
					for (C_Int64 t = (fTotalCount / SD + 1) * SD;
						t <= tEnd; t += SD)
					{
						C_Int64 si = I.Ptr + (t - fTotalCount);
						Src->fPersistIndex.Lookup(si, pi, pp);
						fPersistIndex.Store(t, pp + Delta);
					}
				} else {
					Src->fAllocator.SetPosition(P1);
					const ssize_t NBuf = COREARRAY_ALLOC_FUNC_BUFFER;
					C_UInt8 Buf[NBuf];
					C_Int64 totcnt = fTotalCount;
					SIZE64 ppos = fTotalStreamSize;
					// Preserve in-progress VL decoder state (`shift`) across
					// buffer refills. `nn` is bounded by the *item* count
					// remaining, so a single refill may contain only a fraction
					// of a multi-byte integer; if we reset `shift` between
					// refills we lose track of where the 9-byte cap applies and
					// miscount integers whose 9th byte has its high bit set.
					ssize_t shift = 0;
					for (C_Int64 n = Count; n > 0; )
					{
						ssize_t nn = (n <= NBuf) ? n : NBuf;
						ssize_t mm = CdVarLenIndex::STRIDE -
							(totcnt % CdVarLenIndex::STRIDE);
						if (nn > mm) nn = mm;
						ssize_t save_nn = nn;
						Src->fAllocator.ReadData(Buf, nn);
						for (C_UInt8 *s = Buf; nn > 0; nn--)
						{
							bool flag;
							if (!(*s++ & 0x80))
								flag = true;
							else
								flag = ((++shift) == 9);
							if (flag)
							{
								shift = 0; n --; totcnt ++;
								if (!(totcnt % CdVarLenIndex::STRIDE))
									fPersistIndex.Store(totcnt, ppos + (s - Buf));
							}
						}
						ppos += save_nn;
					}
				}
			}

			// check
			CdAllocArray::TDimItem &R = this->fDimension.front();
			fTotalCount += Count;
			fTotalStreamSize += SrcLen;
			if (fTotalCount >= R.DimElmCnt*(R.DimLen+1))
			{
				R.DimLen = fTotalCount / R.DimElmCnt;
				this->fNeedUpdate = true;
			}

			return;
		}
	}
	CdArray<TVL_UInt>::AppendIter(I, Count);
}

void CdVL_UInt::GetOwnBlockStream(vector<const CdBlockStream*> &Out) const
{
	CdArray<TVL_UInt>::GetOwnBlockStream(Out);
	if (fPersistIndex.Stream()) Out.push_back(fPersistIndex.Stream());
}

void CdVL_UInt::GetOwnBlockStream(vector<CdStream*> &Out)
{
	CdArray<TVL_UInt>::GetOwnBlockStream(Out);
	if (fPersistIndex.Stream()) Out.push_back(fPersistIndex.Stream());
}

void CdVL_UInt::Loading(CdReader &Reader, TdVersion Version)
{
	CdArray<TVL_UInt>::Loading(Reader, Version);
	// load the content
	if (fGDSStream)
	{
		Reader[CdVarLenIndex::VarName()] >> fIndexingID;
		fPersistIndex.Attach(fGDSStream->Collection()[fIndexingID]);
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
	if (fGDSStream)
	{
		if (!fPersistIndex.Stream())
			fPersistIndex.Attach(fGDSStream->Collection().NewBlockStream());
		fIndexingID = fPersistIndex.Stream()->ID();
		Writer[CdVarLenIndex::VarName()] << fIndexingID;
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
		} else {
			// a backward seek has to start over from the beginning of the
			// stream, a forward one keeps the cursor; either way the
			// checkpoint table may know a closer place to start from
			if (idx < fCurIndex) fCurIndex = fCurStreamPosition = 0;
			C_Int64 pi;  SIZE64 pp;
			if (fPersistIndex.Lookup(idx, pi, pp) && (pi > fCurIndex))
			{
				fCurIndex = pi;
				fCurStreamPosition = pp;
			}
		}

		fAllocator.SetPosition(fCurStreamPosition);
		// Preserve in-progress VL decoder state (`shift`) across buffer refills.
		// `m` is bounded by the *item* count remaining, so a single refill may
		// contain only a fraction of a multi-byte integer; if we reset `shift`
		// between refills we lose track of where the 9-byte cap applies and
		// miscount integers whose 9th byte has its high bit set.
		C_UInt8 Buf[COREARRAY_ALLOC_FUNC_BUFFER];
		ssize_t shift = 0;
		while (fCurIndex < idx)
		{
			C_Int64 n = idx - fCurIndex;
			ssize_t m = (n <= (ssize_t)sizeof(Buf)) ? n : sizeof(Buf);
			fAllocator.ReadData(Buf, m);
			C_UInt8 *s = Buf;
			for (; m > 0; m--)
			{
				if (!(*s++ & 0x80))
				{
					fCurIndex ++; shift = 0;
				} else {
					if ((++shift) == 9)
					{
						fCurIndex ++; shift = 0;
					}
				}
			}
		}
		fCurStreamPosition = fAllocator.Position();
	}
}


namespace CoreArray
{
	template<typename TClass> static CdObjRef *OnObjCreate()
	{
		return new TClass();
	}

	COREARRAY_DLL_LOCAL void RegisterClass_VLInt()
	{
		#define REG_CLASS(T, CLASS, CType, Desp)	\
			dObjManager().AddClass(TdTraits< T >::StreamName(), \
				OnObjCreate< CLASS >, CdObjClassMgr::CType, Desp)

		// variable-length integers
		REG_CLASS(TVL_Int, CdVL_Int, ctArray, "variable-length signed integer");
		REG_CLASS(TVL_UInt, CdVL_UInt, ctArray, "variable-length unsigned integer");

		#undef REG_CLASS
	}
}
