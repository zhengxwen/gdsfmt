// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dRleGDS.cpp: Run-length encoded arrays in GDS format
//
// Copyright (C) 2026    Xiuwen Zheng
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

#include "dRleGDS.h"
#include <cmath>


namespace CoreArray
{
	template<typename TClass> static CdObjRef *OnObjCreate()
	{
		return new TClass();
	}

	COREARRAY_DLL_LOCAL void RegisterClass_RLE()
	{
		#define REG_CLASS(T, CLASS, CType, Desp)	\
			dObjManager().AddClass(TdTraits< T >::StreamName(), \
				OnObjCreate< CLASS >, CdObjClassMgr::CType, Desp)

		REG_CLASS(TRleUInt8,  CdRleUInt8,  ctArray, "run-length encoded unsigned integer of 8 bits");
		REG_CLASS(TRleInt32,  CdRleInt32,  ctArray, "run-length encoded signed integer of 32 bits");
		REG_CLASS(TRleInt64,  CdRleInt64,  ctArray, "run-length encoded signed integer of 64 bits");
		REG_CLASS(TRleReal32, CdRleReal32, ctArray, "run-length encoded real number (32 bits)");
		REG_CLASS(TRleReal64, CdRleReal64, ctArray, "run-length encoded real number (64 bits)");
		REG_CLASS(TRleStr8,   CdRleStr8,   ctArray, "run-length encoded UTF-8 string");

		#undef REG_CLASS
	}
}


// ===========================================================

using namespace std;
using namespace CoreArray;


// ===========================================================
// Floating-point step runs
// ===========================================================

// A value of a step run is v0 + i*d, the product and the sum each rounded
// on its own. Fusing the two into one multiply-add rounds once and can give
// another value; the encoder only accepts a run whose values it has checked
// with these very functions, so a file written on one machine would then
// read differently on another. The contraction is turned off here for every
// compiler that may otherwise do it.

#if defined(__clang__)
#   pragma STDC FP_CONTRACT OFF
#elif defined(__GNUC__)
#   pragma GCC push_options
#   pragma GCC optimize ("fp-contract=off")
#endif

C_Float64 CoreArray::_INTERNAL::RLE_StepValue(C_Float64 v0, C_Float64 d,
	C_Int64 i)
{
	const C_Float64 t = (C_Float64)i * d;
	return v0 + t;
}

C_Float32 CoreArray::_INTERNAL::RLE_StepValue(C_Float32 v0, C_Float32 d,
	C_Int64 i)
{
	const C_Float64 t = (C_Float64)i * (C_Float64)d;
	const C_Float64 s = (C_Float64)v0 + t;
	return (C_Float32)s;
}

void CoreArray::_INTERNAL::RLE_StepFill(C_Float64 *p, C_Float64 v0,
	C_Float64 d, C_Int64 i, ssize_t n)
{
	for (; n > 0; n--, i++)
	{
		const C_Float64 t = (C_Float64)i * d;
		*p++ = v0 + t;
	}
}

void CoreArray::_INTERNAL::RLE_StepFill(C_Float32 *p, C_Float32 v0,
	C_Float32 d, C_Int64 i, ssize_t n)
{
	const C_Float64 dd = d, vv = v0;
	for (; n > 0; n--, i++)
	{
		const C_Float64 t = (C_Float64)i * dd;
		const C_Float64 s = vv + t;
		*p++ = (C_Float32)s;
	}
}

#if defined(__clang__)
#   pragma STDC FP_CONTRACT DEFAULT
#elif defined(__GNUC__)
#   pragma GCC pop_options
#endif


/// exact powers of ten
static const double RLE_POW10[23] =
{
	1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,  1e8,  1e9,  1e10, 1e11,
	1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22
};

/// x rounded to k significant decimal digits, or x if it is out of range
static double RLE_Signif(double x, int k)
{
	if ((x == 0) || !IsFinite(x)) return x;
	const int p = k - 1 - (int)floor(log10(fabs(x)));
	if ((0 <= p) && (p <= 22))
	{
		const double m = RLE_POW10[p];
		return round(x * m) / m;
	} else if ((-22 <= p) && (p < 0))
	{
		const double m = RLE_POW10[-p];
		return round(x / m) * m;
	} else
		return x;
}

template<typename TYPE> static inline bool SameBits(TYPE a, TYPE b)
{
	return memcmp(&a, &b, sizeof(TYPE)) == 0;
}

/// keep c in out[] if it makes x0, x1 the first two values of a step run
template<typename TYPE>
	static inline void AddCand(TYPE c, TYPE x0, TYPE x1, TYPE out[], int &n)
{
	if (!IsFinite(c) || (c == 0)) return;
	if (!SameBits(_INTERNAL::RLE_StepValue(x0, c, 0), x0) ||
		!SameBits(_INTERNAL::RLE_StepValue(x0, c, 1), x1))
		return;
	for (int j=0; j < n; j++)
		if (SameBits(out[j], c)) return;
	out[n++] = c;
}

// The difference x1 - x0 is not always the step that generated a sequence:
// seq(1, 2, by=0.1) is 1 + (0:10)*0.1, but 1.1 - 1 is 0.10000000000000009
// and 1 + 2*0.10000000000000009 is not 1.2. The step written in decimal is
// usually recovered by rounding the difference to fewer significant digits,
// so a few such roundings are kept as candidates too, each checked against
// the values; the ones that disagree with a later value are dropped as the
// run grows.

int CoreArray::_INTERNAL::RLE_StepCand(C_Float64 x0, C_Float64 x1,
	C_Float64 out[])
{
	if (!IsFinite(x0) || !IsFinite(x1)) return 0;
	const C_Float64 d = x1 - x0;
	int n = 0;
	AddCand(d, x0, x1, out, n);
	static const int digits[RLE_MAX_CAND-1] = { 15, 12, 10, 8, 6 };
	for (int i=0; i < RLE_MAX_CAND-1; i++)
		AddCand(RLE_Signif(d, digits[i]), x0, x1, out, n);
	return n;
}

int CoreArray::_INTERNAL::RLE_StepCand(C_Float32 x0, C_Float32 x1,
	C_Float32 out[])
{
	if (!IsFinite(x0) || !IsFinite(x1)) return 0;
	const C_Float64 d = (C_Float64)x1 - (C_Float64)x0;
	int n = 0;
	AddCand((C_Float32)d, x0, x1, out, n);
	static const int digits[RLE_MAX_CAND-1] = { 7, 6, 5, 4, 3 };
	for (int i=0; i < RLE_MAX_CAND-1; i++)
		AddCand((C_Float32)RLE_Signif(d, digits[i]), x0, x1, out, n);
	return n;
}



// ===========================================================
// CdRLEBase
// ===========================================================

static const char *VAR_INDEX = "INDEX";
/// the bytes of a checkpoint: the element, and the byte offset
static const int CK_SIZE = sizeof(C_Int64) + GDS_POS_SIZE;

const int CdRLEBase::MAX_LITERAL;

CdRLEBase::CdRLEBase(bool HasStep, C_Int64 Stride):
	fHasStep(HasStep), fStride(Stride)
{
	fIndexingID = 0;
	fIndexingStream = NULL;
	fNumIndex = 0;
	fTotalStreamSize = 0;
	fStreamCount = 0;
}

int CdRLEBase::HeaderSize(C_Int64 L, TKind Kind) const
{
	if (Kind == rkLiteral) return 1;
	C_UInt64 v = (C_UInt64)(L - 1) >> (fHasStep ? 5 : 6);
	int n = 1;
	for (; v > 0; v >>= 7) n++;
	return n;
}

int CdRLEBase::PutHeader(C_UInt8 *s, C_Int64 L, TKind Kind) const
{
	C_UInt64 v = (C_UInt64)(L - 1);
	if (Kind == rkLiteral)
	{
		*s = (C_UInt8)(v << 1);
		return 1;
	}
	C_UInt8 b;
	if (fHasStep)
	{
		b = ((Kind == rkStep) ? 0x03 : 0x01) | (C_UInt8)((v & 0x1F) << 2);
		v >>= 5;
	} else {
		b = 0x01 | (C_UInt8)((v & 0x3F) << 1);
		v >>= 6;
	}
	C_UInt8 *p = s;
	for (;;)
	{
		if (v > 0) b |= 0x80;
		*p++ = b;
		if (v == 0) break;
		b = v & 0x7F;
		v >>= 7;
	}
	return p - s;
}

int CdRLEBase::GetHeader(CdAllocator &A, TKind &Kind, C_Int64 &L) const
{
	C_UInt8 b = A.R8b();
	if (!(b & 0x01))
	{
		Kind = rkLiteral;
		L = (b >> 1) + 1;
		return 1;
	}
	C_UInt64 v;
	int shift;
	if (fHasStep)
	{
		Kind = (b & 0x02) ? rkStep : rkRepeat;
		v = (b >> 2) & 0x1F; shift = 5;
	} else {
		Kind = rkRepeat;
		v = (b >> 1) & 0x3F; shift = 6;
	}
	int n = 1;
	while (b & 0x80)
	{
		// L - 1 has at most 63 bits
		if (shift >= 63) ThrowCorrupt();
		b = A.R8b(); n ++;
		C_UInt64 part = b & 0x7F;
		if ((shift > 56) && (part >> (63 - shift))) ThrowCorrupt();
		v |= part << shift;
		shift += 7;
	}
	// no array holds 2^63 elements
	if (v >= (C_UInt64)numeric_limits<C_Int64>::max()) ThrowCorrupt();
	L = (C_Int64)v + 1;
	return n;
}

void CdRLEBase::RleLoad(CdReader &Reader, CdBlockStream *GDSStream,
	CdPipeMgrItem *PipeInfo, CdAllocator &Allocator)
{
	fIndexingStream = NULL;
	fNumIndex = 0;
	fArrayIndex.clear();
	fArrayPos.clear();
	fTotalStreamSize = 0;
	if (GDSStream)
	{
		if (Reader.HaveProperty(VAR_INDEX))
		{
			Reader[VAR_INDEX] >> fIndexingID;
			fIndexingStream = GDSStream->Collection()[fIndexingID];
			fNumIndex = fIndexingStream->GetSize() / CK_SIZE;
		}
		if (PipeInfo)
			fTotalStreamSize = PipeInfo->StreamTotalIn();
		else if (Allocator.BufStream())
			fTotalStreamSize = Allocator.BufStream()->GetSize();
	}
}

void CdRLEBase::RleSave(CdWriter &Writer, CdBlockStream *GDSStream)
{
	if (GDSStream)
	{
		if (!fIndexingStream)
			fIndexingStream = GDSStream->Collection().NewBlockStream();
		fIndexingID = fIndexingStream->ID();
		Writer[VAR_INDEX] << fIndexingID;
	}
}

void CdRLEBase::CkStore(C_Int64 Index, SIZE64 Pos)
{
	if (!fIndexingStream) return;
	// checkpoints come in increasing order of element, so the next one goes
	// at the end of the table
	fIndexingStream->SetPosition(fNumIndex * CK_SIZE);
	BYTE_LE<CdStream>(fIndexingStream) << Index << TdGDSPos(Pos);
	// a loaded cache is kept in step
	if ((C_Int64)fArrayIndex.size() == fNumIndex)
	{
		fArrayIndex.push_back(Index);
		fArrayPos.push_back(Pos);
	}
	fNumIndex ++;
}

void CdRLEBase::CkLoad()
{
	if ((C_Int64)fArrayIndex.size() != fNumIndex)
	{
		fArrayIndex.resize(fNumIndex);
		fArrayPos.resize(fNumIndex);
		BYTE_LE<CdStream> IS(fIndexingStream);
		IS.SetPosition(0);
		for (C_Int64 i=0; i < fNumIndex; i++)
		{
			TdGDSPos pos;
			IS >> fArrayIndex[i] >> pos;
			fArrayPos[i] = pos;
		}
	}
}

bool CdRLEBase::CkLookup(C_Int64 Index, C_Int64 &OutIndex, SIZE64 &OutPos)
{
	if (!fIndexingStream || (fNumIndex <= 0)) return false;
	CkLoad();
	// the first checkpoint after Index
	C_Int64 st = 0, ed = fNumIndex;
	while (st < ed)
	{
		C_Int64 mid = (st + ed) / 2;
		if (fArrayIndex[mid] <= Index)
			st = mid + 1;
		else
			ed = mid;
	}
	if (st <= 0) return false;
	OutIndex = fArrayIndex[st - 1];
	OutPos = fArrayPos[st - 1];
	return true;
}

void CdRLEBase::CkTruncate(C_Int64 Index)
{
	if (!fIndexingStream || (fNumIndex <= 0)) return;
	CkLoad();
	C_Int64 k = fNumIndex;
	while ((k > 0) && (fArrayIndex[k-1] >= Index)) k--;
	if (k < fNumIndex)
	{
		fIndexingStream->SetSize(k * CK_SIZE);
		fArrayIndex.resize(k);
		fArrayPos.resize(k);
		fNumIndex = k;
	}
}

void CdRLEBase::ThrowCorrupt()
{
	throw ErrArray("Invalid data stream of a run-length encoded array.");
}
