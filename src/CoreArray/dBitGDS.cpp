// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dBitGDS.cpp: Bit operators and classes of GDS format
//
// Copyright (C) 2007-2019    Xiuwen Zheng
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

#ifndef COREARRAY_NO_COMPILER_OPTIM_O3
#if defined(__clang__)
#pragma clang optimize on
#elif defined(__GNUC__) && ((__GNUC__>4) || (__GNUC__==4 && __GNUC_MINOR__>=4))
#pragma GCC optimize("O3")
#endif
#endif

#include "dBitGDS.h"


namespace CoreArray
{
	#ifdef COREARRAY_CC_SUNPRO
		// specific compiler
		#define PREFIX	extern
	#else
		#define PREFIX
	#endif

	// Bit Array

	PREFIX const C_UInt8 CoreArray_MaskBit1Array[8] =
		{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
	PREFIX const C_UInt8 CoreArray_MaskBit1ArrayNot[8] =
		{ 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F };

	PREFIX const C_UInt8 CoreArray_MaskBit2Array[4] =
		{ 0x03, 0x0C, 0x30, 0xC0 };
	PREFIX const C_UInt8 CoreArray_MaskBit2ArrayNot[4] =
		{ 0xFC, 0xF3, 0xCF, 0x3F };

	PREFIX const C_UInt8 CoreArray_MaskBit4Array[2] =
		{ 0x0F, 0xF0 };
	PREFIX const C_UInt8 CoreArray_MaskBit4ArrayNot[2] =
		{ 0xF0, 0x0F };


	// Class Names

	PREFIX const char *BitStreamNames[33] =
	{
		"dBit0",
		"dBit1",  "dBit2",  "dBit3",  "dBit4",
		"dBit5",  "dBit6",  "dBit7",  "dUInt8",
		"dBit9",  "dBit10", "dBit11", "dBit12",
		"dBit13", "dBit14", "dBit15", "dUInt16",
		"dBit17", "dBit18", "dBit19", "dBit20",
		"dBit21", "dBit22", "dBit23", "dUInt24",
		"dBit25", "dBit26", "dBit27", "dBit28",
		"dBit29", "dBit30", "dBit31", "dUInt32"
	};

	PREFIX const char *SBitStreamNames[33] =
	{
		"dSBit0",
		"dSBit1",  "dSBit2",  "dSBit3",  "dSBit4",
		"dSBit5",  "dSBit6",  "dSBit7",  "dInt8",
		"dSBit9",  "dSBit10", "dSBit11", "dSBit12",
		"dSBit13", "dSBit14", "dSBit15", "dInt16",
		"dSBit17", "dSBit18", "dSBit19", "dSBit20",
		"dSBit21", "dSBit22", "dSBit23", "dInt24",
		"dSBit25", "dSBit26", "dSBit27", "dSBit28",
		"dSBit29", "dSBit30", "dSBit31", "dInt32"
	};

	#undef PREFIX
}


using namespace std;
using namespace CoreArray;


// =====================================================================
// Bit operators
// =====================================================================

COREARRAY_DLL_DEFAULT void CoreArray::BitBinShr(void *Buf,
	size_t NByte, C_UInt8 NShr)
{
	C_UInt32 *p32, *p32a, D32;
	C_UInt8 *p8, *p8a, D8, xNShr, xNShr8;

	NShr &= 0x07;
	if (NShr == 0) return;
	xNShr = 32 - NShr; xNShr8 = 8 - NShr;

	p32 = (C_UInt32*)Buf; p32a = NULL;
	while (NByte >= 4)
	{
		D32 = *p32; *p32 = D32 >> NShr;
		if (p32a != NULL)
			*p32a |= D32 << xNShr;
		p32a = p32; ++p32; NByte-= 4;
	}

	p8 = (C_UInt8*)p32;
	p8a = (p32a) ? (((C_UInt8*)p32a)+3) : NULL;
	while (NByte > 0)
	{
		D8 = *p8; *p8 = D8 >> NShr;
		if (p8a)
			*p8a |= D8 << xNShr8;
		p8a = p8; ++p8; --NByte;
	}
}

COREARRAY_DLL_DEFAULT void CoreArray::BitBinShl(void *Buf,
	size_t NByte, C_UInt8 NShl)
{
	C_UInt32 *p32, D32a, D32;
	C_UInt8 *p8, D8a, D8, xNShl, xNShl8;

	NShl &= 0x07;
	if (NShl == 0) return;
	xNShl = 32 - NShl; xNShl8 = 8 - NShl;

	p32 = (C_UInt32*)Buf; D32a = 0;
	while (NByte >= 4)
	{
		D32 = *p32; *p32 = (D32 << NShl) | D32a;
		D32a = D32 >> xNShl;
		++p32; NByte -=4;
	}

	p8 = (C_UInt8*)p32; D8a = D32a;
	while (NByte > 0)
	{
		D8 = *p8; *p8 = (D8 << NShl) | D8a;
		D8a = D8 >> xNShl8;
		++p8; --NByte;
	}
}

COREARRAY_DLL_DEFAULT void CoreArray::BitClear(CdAllocator &alloc,
	SIZE64 p, SIZE64 Len)
{
	C_UInt8 k, kEnd, B;
	if (Len > 0)
	{
		// Head
		k = ((C_UInt8)p) & 0x07; p = p >> 3;
		alloc.SetPosition(p);
		if (k > 0)
		{
			B = alloc.R8b();
			kEnd = (7 < k+(ssize_t)Len-1) ? 7 : (k+(ssize_t)Len-1);
			for (; k <= kEnd; k++, Len--)
				B = B & CoreArray_MaskBit1ArrayNot[k];
			alloc.SetPosition(p);
			alloc.W8b(B);
		}
		// Middle
		if (Len >= 8)
		{
			B = ((C_UInt8)Len) & 0x07;
			Len = Len >> 3;
			alloc.ZeroFill(Len);
			p += Len; Len = B;
		}
		// End
		if (Len > 0)
		{
			B = alloc.R8b();
			B = B & (0xFF << ((C_UInt8)Len));
			alloc.SetPosition(alloc.Position()-1);
			alloc.W8b(B);
		}
	}
}

#define AND_BIT3(x)    (C_UInt8(x) & 0x07)

static size_t bit_cpy_to_buf(CdAllocator &alloc, const SIZE64 pS,
	void *Buffer, size_t L)
{
	SIZE64 ppS = pS >> 3;
	SIZE64 p = pS + L;
	L = (AND_BIT3(p) == 0) ? ((p >> 3) - ppS) : ((p >> 3) - ppS + 1);
	alloc.SetPosition(ppS);
	alloc.ReadData(Buffer, L);
	return L;
}

COREARRAY_DLL_DEFAULT void CoreArray::BitBufToCpy(CdAllocator &alloc,
	SIZE64 pD, void *Buffer, size_t L)
{
	C_UInt8 *pB, i, B, xpD, xpDL;
	SIZE64 p;

	pB = (C_UInt8*)Buffer; xpD = AND_BIT3(pD);
	p = pD >> 3;
	alloc.SetPosition(p);
	if (xpD > 0)
	{
		B = alloc.R8b();
    	xpDL = ((xpD+L-1) < 7) ? (xpD+L-1) : 7;
		for (i = xpD; i <= xpDL; i++)
		{
			B = (B & CoreArray_MaskBit1ArrayNot[i]) |
				(*pB & CoreArray_MaskBit1Array[i]);
			--L; pD++;
		}
		alloc.SetPosition(p);
		alloc.W8b(B);
		++ pB;
	}

	if (L >= 8)
	{
		B = L & 0x07; L >>= 3;
		alloc.WriteData((void*)pB, L);
		pB += L; pD += L; L = B;
	}

	if (L > 0)
	{
		B = alloc.R8b();
		for (i = 0; (size_t)i < L; i++)
		{
			B = (B & CoreArray_MaskBit1ArrayNot[i]) |
				(*pB & CoreArray_MaskBit1Array[i]);
		}
		alloc.SetPosition(alloc.Position() - 1);
		alloc.W8b(B);
	}
}

COREARRAY_DLL_DEFAULT void CoreArray::BitMoveBits(CdAllocator &alloc,
	SIZE64 pS, SIZE64 pD, SIZE64 Len)
{
	C_UInt8 Buf[COREARRAY_STREAM_BUFFER];
	size_t L, LD;

	if (pS < pD)
	{
		pS += Len; pD += Len;
		while (Len > 0)
		{
			L = (Len <= (SIZE64)(sizeof(Buf)*8-16)) ? Len : (sizeof(Buf)*8-16);
			pS -= L; pD -= L; Len -= L;
			LD = bit_cpy_to_buf(alloc, pS, Buf, L);
			if (AND_BIT3(pS) < AND_BIT3(pD))
				BitBinShl((void*)Buf, LD+1, AND_BIT3(pD) - AND_BIT3(pS));
			else if (AND_BIT3(pS) > AND_BIT3(pD))
				BitBinShr((void*)Buf, LD, AND_BIT3(pS) - AND_BIT3(pD));
			BitBufToCpy(alloc, pD, Buf, L);
		}
	} else if (pS > pD)
	{
		while (Len > 0)
		{
			L = (Len <= (SIZE64)(sizeof(Buf)*8-16)) ? Len : (sizeof(Buf)*8-16);
			LD = bit_cpy_to_buf(alloc, pS, Buf, L);
			if (AND_BIT3(pS) < AND_BIT3(pD))
				BitBinShl((void*)Buf, LD+1, AND_BIT3(pD) - AND_BIT3(pS));
			else if (AND_BIT3(pS) > AND_BIT3(pD))
				BitBinShr((void*)Buf, LD, AND_BIT3(pS) - AND_BIT3(pD));
			BitBufToCpy(alloc, pD, Buf, L);
			pS += L; pD += L; Len -= L;
		}
	}
}

COREARRAY_DLL_DEFAULT C_Int32 CoreArray::BitSet_IfSigned(C_Int32 val,
	unsigned nbit)
{
	static const C_UInt32 BitFlag[33] = {
		0, 0x1, 0x2, 0x4, 0x8,
		0x10, 0x20, 0x40, 0x80,
		0x100, 0x200, 0x400, 0x800,
		0x1000, 0x2000, 0x4000, 0x8000,
		0x10000, 0x20000, 0x40000, 0x80000,
		0x100000, 0x200000, 0x400000, 0x800000,
		0x1000000, 0x2000000, 0x4000000, 0x8000000,
		0x10000000, 0x20000000, 0x40000000, 0x80000000 };
	static const C_UInt32 BitNeg[33] = {
		0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFC, 0xFFFFFFF8, 0xFFFFFFF0,
		0xFFFFFFE0, 0xFFFFFFC0, 0xFFFFFF80, 0xFFFFFF00,
		0xFFFFFE00, 0xFFFFFC00, 0xFFFFF800, 0xFFFFF000,
		0xFFFFE000, 0xFFFFC000, 0xFFFF8000, 0xFFFF0000,
		0xFFFE0000, 0xFFFC0000, 0xFFF80000, 0xFFF00000,
		0xFFE00000, 0xFFC00000, 0xFF800000, 0xFF000000,
		0xFE000000, 0xFC000000, 0xF8000000, 0xF0000000,
		0xE0000000, 0xC0000000, 0x80000000, 0x00000000 };

	if (val & BitFlag[nbit]) val |= BitNeg[nbit];

	return val;
}


namespace CoreArray
{
	template<typename TClass> static CdObjRef *OnObjCreate()
	{
		return new TClass();
	}

	#define REG_CLASS(T, CLASS, CType, Desp)	\
		dObjManager().AddClass(TdTraits< T >::StreamName(), \
			OnObjCreate< CLASS >, CdObjClassMgr::CType, Desp)
	#define REG_CLASS_EX(Name, CLASS, CType, Desp)	\
		dObjManager().AddClass(Name, OnObjCreate< CLASS >, \
			CdObjClassMgr::CType, Desp)

	COREARRAY_DLL_LOCAL void RegisterClass_Bit_s24()
	{
		REG_CLASS(SBIT24, CdSBit24, ctArray, "signed integer of 24 bits");
	}

	COREARRAY_DLL_LOCAL void RegisterClass_Bit_u24()
	{
		REG_CLASS(BIT24, CdBit24, ctArray, "unsigned integer of 24 bits");
	}

	COREARRAY_DLL_LOCAL void RegisterClass_Bit()
	{
		// signed bit integers

		REG_CLASS(SBIT2, CdSBit2, ctArray, "signed integer of 2 bits");
		REG_CLASS(SBIT3, CdSBit3, ctArray, "signed integer of 3 bits");
		REG_CLASS(SBIT4, CdSBit4, ctArray, "signed integer of 4 bits");
		REG_CLASS(SBIT5, CdSBit5, ctArray, "signed integer of 5 bits");
		REG_CLASS(SBIT6, CdSBit6, ctArray, "signed integer of 6 bits");
		REG_CLASS(SBIT7, CdSBit7, ctArray, "signed integer of 7 bits");
		REG_CLASS_EX("dSBit8", CdSBit8, ctArray, "signed integer of 8 bits");

		REG_CLASS(SBIT9,  CdSBit9,  ctArray, "signed integer of 9 bits");
		REG_CLASS(SBIT10, CdSBit10, ctArray, "signed integer of 10 bits");
		REG_CLASS(SBIT11, CdSBit11, ctArray, "signed integer of 11 bits");
		REG_CLASS(SBIT12, CdSBit12, ctArray, "signed integer of 12 bits");
		REG_CLASS(SBIT13, CdSBit13, ctArray, "signed integer of 13 bits");
		REG_CLASS(SBIT14, CdSBit14, ctArray, "signed integer of 14 bits");
		REG_CLASS(SBIT15, CdSBit15, ctArray, "signed integer of 15 bits");

		REG_CLASS_EX("dSBit16", CdSBit16, ctArray, "signed integer of 16 bits");
		REG_CLASS_EX("dSBit24", CdSBit24, ctArray, "signed integer of 24 bits");
		REG_CLASS_EX("dSBit32", CdSBit32, ctArray, "signed integer of 32 bits");
		REG_CLASS_EX("dSBit64", CdInt64, ctArray, "signed integer of 64 bits");

		// unsigned bit integers

		REG_CLASS(BIT1, CdBit1, ctArray, "unsigned integer of 1 bit");
		REG_CLASS(BIT2, CdBit2, ctArray, "unsigned integer of 2 bits");
		REG_CLASS(BIT3, CdBit3, ctArray, "unsigned integer of 3 bits");
		REG_CLASS(BIT4, CdBit4, ctArray, "unsigned integer of 4 bits");
		REG_CLASS(BIT5, CdBit5, ctArray, "unsigned integer of 5 bits");
		REG_CLASS(BIT6, CdBit6, ctArray, "unsigned integer of 6 bits");
		REG_CLASS(BIT7, CdBit7, ctArray, "unsigned integer of 7 bits");
		REG_CLASS_EX("dBit8", CdBit8, ctArray, "unsigned integer of 8 bits");

		REG_CLASS(BIT9,  CdBit9,  ctArray, "unsigned integer of 9 bits");
		REG_CLASS(BIT10, CdBit10, ctArray, "unsigned integer of 10 bits");
		REG_CLASS(BIT11, CdBit11, ctArray, "unsigned integer of 11 bits");
		REG_CLASS(BIT12, CdBit12, ctArray, "unsigned integer of 12 bits");
		REG_CLASS(BIT13, CdBit13, ctArray, "unsigned integer of 13 bits");
		REG_CLASS(BIT14, CdBit14, ctArray, "unsigned integer of 14 bits");
		REG_CLASS(BIT15, CdBit15, ctArray, "unsigned integer of 15 bits");

		REG_CLASS_EX("dBit16", CdBit16, ctArray, "unsigned integer of 16 bits");
		REG_CLASS_EX("dBit24", CdBit24, ctArray, "unsigned integer of 24 bits");
		REG_CLASS_EX("dBit32", CdBit32, ctArray, "unsigned integer of 32 bits");
		REG_CLASS_EX("dBit64", CdUInt64, ctArray, "unsigned integer of 64 bits");
	}
}
