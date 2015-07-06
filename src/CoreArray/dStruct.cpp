// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dStruct.cpp: Data container - array, matrix, etc
//
// Copyright (C) 2007-2015    Xiuwen Zheng
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

#include "dStruct.h"
#include <memory>
#include <algorithm>


using namespace std;
using namespace CoreArray;
using namespace CoreArray::_INTERNAL;


// =====================================================================
// CdIterator
// =====================================================================

CdIterator& CdIterator::operator++()
{
	Handler->IterOffset(*this, +1);
	return *this;
}

CdIterator& CdIterator::operator--()
{
	Handler->IterOffset(*this, -1);
	return *this;
}

CdIterator& CdIterator::operator+= (SIZE64 offset)
{
	Handler->IterOffset(*this, offset);
	return *this;
}

CdIterator& CdIterator::operator-= (SIZE64 offset)
{
	Handler->IterOffset(*this, -offset);
	return *this;
}

C_Int64 CdIterator::GetInteger()
{
	return Handler->IterGetInteger(*this);
}

double CdIterator::GetFloat()
{
	return Handler->IterGetFloat(*this);
}

UTF16String CdIterator::GetString()
{
	return Handler->IterGetString(*this);
}

void CdIterator::SetInteger(C_Int64 val)
{
	Handler->IterSetInteger(*this, val);
}

void CdIterator::SetFloat(double val)
{
	Handler->IterSetFloat(*this, val);
}

void CdIterator::SetString(const UTF16String &val)
{
	Handler->IterSetString(*this, val);
}

void *CdIterator::ReadData(void *OutBuf, ssize_t n, C_SVType OutSV)
{
    return Handler->IterRData(*this, OutBuf, n, OutSV);
}

const void *CdIterator::WriteData(const void *InBuf, ssize_t n, C_SVType InSV)
{
    return Handler->IterWData(*this, InBuf, n, InSV);
}

void CdIterator::Copy(CdIterator &d, CdIterator &s, C_Int64 Count)
{
	#define ITER_COPY(TYPE, SV) \
		{ \
			const ssize_t SIZE = COREARRAY_STREAM_BUFFER / sizeof(TYPE); \
			TYPE Buffer[SIZE]; \
			while (Count > 0) \
			{ \
				ssize_t n = (Count <= SIZE) ? Count : SIZE; \
				s.ReadData(Buffer, n, SV); \
				d.WriteData(Buffer, n, SV); \
				Count -= n; \
			} \
			break; \
		}

	switch (d.Handler->SVType())
	{
		case svInt8:
			ITER_COPY(C_Int8, svInt8);
		case svUInt8:
			ITER_COPY(C_UInt8, svUInt8);
		case svInt16:
			ITER_COPY(C_Int16, svInt16);
		case svUInt16:
			ITER_COPY(C_UInt16, svUInt16);
		case svInt32:
			ITER_COPY(C_Int32, svInt32);
		case svUInt32:
			ITER_COPY(C_UInt32, svUInt32);
		case svInt64: case svCustomInt:
			ITER_COPY(C_Int64, svInt64);
		case svUInt64: case svCustomUInt:
			ITER_COPY(C_UInt64, svUInt64);
		case svFloat32:
			ITER_COPY(C_Float32, svFloat32);
		case svFloat64: case svCustomFloat:
			ITER_COPY(C_Float64, svFloat64);
		case svStrUTF8:
			ITER_COPY(UTF8String, svStrUTF8);
		case svStrUTF16: case svCustomStr:
			ITER_COPY(UTF16String, svStrUTF16);
		default:
			throw ErrContainer("Invalid SVType in destination.");
	}

	#undef ITER_COPY
}



// =====================================================================
// CdContainer
// =====================================================================

CdContainer::CdContainer(): CdGDSObjPipe() { }

CdContainer::~CdContainer() { }

void CdContainer::AssignOneEx(CdGDSObj &Source, bool Append, void *Param)
{
	_RaiseInvalidAssign(string(dName()) + " := " + Source.dName());
}

void CdContainer::Caching()
{
    // do nothing ...
}

SIZE64 CdContainer::GDSStreamSize()
{
	return -1;
}

void *CdContainer::IterRData(CdIterator &I, void *OutBuf, ssize_t n,
	C_SVType OutSV)
{
	#define ITER_READ_INT(TYPE) { \
			TYPE *p = (TYPE*)OutBuf; \
			for (; n > 0; n--, ++I) *p++ = I.GetInteger(); \
			return p; \
		}
	#define ITER_READ_FLOAT(TYPE) { \
			TYPE *p = (TYPE*)OutBuf; \
			for (; n > 0; n--, ++I) *p++ = I.GetFloat(); \
			return p; \
		}

	switch (OutSV)
	{
		case svInt8:
			ITER_READ_INT(C_Int8);
		case svUInt8:
			ITER_READ_INT(C_UInt8);
		case svInt16:
			ITER_READ_INT(C_Int16);
		case svUInt16:
			ITER_READ_INT(C_UInt16);
		case svInt32:
			ITER_READ_INT(C_Int32);
		case svUInt32:
			ITER_READ_INT(C_UInt32);
		case svInt64:
			ITER_READ_INT(C_Int64);
		case svUInt64:
			ITER_READ_INT(C_UInt64);
		case svFloat32:
			ITER_READ_FLOAT(C_Float32);
		case svFloat64:
			ITER_READ_FLOAT(C_Float64);
		case svStrUTF8:
			{
				UTF8String *p = (UTF8String*)OutBuf;
				for (; n > 0; n--, ++I) *p++ = UTF16ToUTF8(I.GetString());
				return p;
			}
		case svStrUTF16:
			{
				UTF16String *p = (UTF16String*)OutBuf;
				for (; n > 0; n--, ++I) *p++ = I.GetString();
				return p;
			}
		default:
			throw ErrContainer("Invalid SVType.");
	}

	return OutBuf;

	#undef ITER_READ_INT
	#undef ITER_READ_FLOAT
}

const void *CdContainer::IterWData(CdIterator &I, const void *InBuf,
	ssize_t n, C_SVType InSV)
{
	#define ITER_WRITE_INT(TYPE) { \
			const TYPE *p = (const TYPE*)InBuf; \
			for (; n > 0; n--, ++I) I.SetInteger(*p++); \
			return p; \
		}
	#define ITER_WRITE_FLOAT(TYPE) { \
			const TYPE *p = (const TYPE*)InBuf; \
			for (; n > 0; n--, ++I) I.SetFloat(*p++); \
			return p; \
		}

	switch (InSV)
	{
		case svInt8:
			ITER_WRITE_INT(C_Int8);
		case svUInt8:
			ITER_WRITE_INT(C_UInt8);
		case svInt16:
			ITER_WRITE_INT(C_Int16);
		case svUInt16:
			ITER_WRITE_INT(C_UInt16);
		case svInt32:
			ITER_WRITE_INT(C_Int32);
		case svUInt32:
			ITER_WRITE_INT(C_UInt32);
		case svInt64:
			ITER_WRITE_INT(C_Int64);
		case svUInt64:
			ITER_WRITE_INT(C_UInt64);
		case svFloat32:
			ITER_WRITE_FLOAT(C_Float32);
		case svFloat64:
			ITER_WRITE_FLOAT(C_Float64);
		case svStrUTF8:
			{
				const UTF8String *p = (const UTF8String*)InBuf;
				for (; n > 0; n--, ++I) I.SetString(UTF8ToUTF16(*p++));
				return p;
			}
		case svStrUTF16:
			{
				const UTF16String *p = (const UTF16String*)InBuf;
				for (; n > 0; n--, ++I) I.SetString(*p++);
				return p;
			}
		default:
			throw ErrContainer("Invalid SVType.");
	}

	return InBuf;

	#undef ITER_WRITE_INT
	#undef ITER_WRITE_FLOAT
}



// =====================================================================
// CdAbstractArray
// =====================================================================

static const char *ERR_READ_INV_SV   = "ReadData: Invalid SVType.";
static const char *ERR_READEX_INV_SV = "ReadDataEx: Invalid SVType.";
static const char *ERR_WRITE_INV_SV  = "WriteData: Invalid SVType.";
static const char *ERR_INV_DIM_RECT  = "Invalid dimension 'Start' and 'Length'.";

namespace CoreArray
{
	namespace _INTERNAL
	{
		template<typename TYPE> struct COREARRAY_DLL_LOCAL ITER_INT
		{
			/// read an array from an iterator
			static TYPE *Read(CdIterator &I, TYPE *p, ssize_t n)
			{
				for (; n > 0; n--, ++I) *p++ = I.GetInteger();
				return p;
			}
			/// read an array with selection from an iterator
			static TYPE *ReadEx(CdIterator &I, TYPE *p, ssize_t n, const C_BOOL *Sel)
			{
				for (; n > 0; n--, ++I, Sel++)
					if (*Sel) *p++ = I.GetInteger();
				return p;
			}
			/// write an array to an iterator
			static const TYPE *Write(CdIterator &I, const TYPE *p, ssize_t n)
			{
				for (; n > 0; n--, ++I) I.SetInteger(*p++);
				return p;
			}
		};

		template<typename TYPE> struct COREARRAY_DLL_LOCAL ITER_FLOAT
		{
			/// read an array from an iterator
			static TYPE *Read(CdIterator &I, TYPE *p, ssize_t n)
			{
				for (; n > 0; n--, ++I) *p++ = I.GetFloat();
				return p;
			}
			/// read an array with selection from an iterator
			static TYPE *ReadEx(CdIterator &I, TYPE *p, ssize_t n, const C_BOOL *Sel)
			{
				for (; n > 0; n--, ++I, Sel++)
					if (*Sel) *p++ = I.GetFloat();
				return p;
			}
			/// write an array to an iterator
			static const TYPE *Write(CdIterator &I, const TYPE *p, ssize_t n)
			{
				for (; n > 0; n--, ++I) I.SetFloat(*p++);
				return p;
			}
		};

		/// read an array from an iterator
		static UTF8String *ITER_STR8_Read(CdIterator &I, UTF8String *p, ssize_t n)
		{
			for (; n > 0; n--, ++I) *p++ = UTF16ToUTF8(I.GetString());
			return p;
		}
		/// read an array with selection from an iterator
		static UTF8String *ITER_STR8_ReadEx(CdIterator &I, UTF8String *p, ssize_t n, const C_BOOL *Sel)
		{
			for (; n > 0; n--, ++I, Sel++)
				if (*Sel) *p++ = UTF16ToUTF8(I.GetString());
			return p;
		}

		/// read an array from an iterator
		static UTF16String *ITER_STR16_Read(CdIterator &I, UTF16String *p, ssize_t n)
		{
			for (; n > 0; n--, ++I) *p++ = I.GetString();
			return p;
		}
		/// read an array with selection from an iterator
		static UTF16String *ITER_STR16_ReadEx(CdIterator &I, UTF16String *p, ssize_t n, const C_BOOL *Sel)
		{
			for (; n > 0; n--, ++I, Sel++)
				if (*Sel) *p++ = I.GetString();
			return p;
		}

		/// write an array to an iterator
		static const UTF8String *ITER_STR8_Write(CdIterator &I, const UTF8String *p, ssize_t n)
		{
			for (; n > 0; n--, ++I) I.SetString(UTF8ToUTF16(*p++));
			return p;
		}
		/// write an array to an iterator
		static const UTF16String *ITER_STR16_Write(CdIterator &I, const UTF16String *p, ssize_t n)
		{
			for (; n > 0; n--, ++I) I.SetString(*p++);
			return p;
		}
	}
}

CdAbstractArray::CdAbstractArray(): CdContainer()
{ }

CdAbstractArray::~CdAbstractArray()
{ }

void CdAbstractArray::AssignOneEx(CdGDSObj &Source, bool Append, void *Param)
{
	if (dynamic_cast<CdContainer*>(&Source))
	{
		CdContainer &Array = *static_cast<CdContainer*>(&Source);
		C_Int64 Count = Array.TotalCount();
		if (!Append)
		{
			if (dynamic_cast<CdAbstractArray*>(&Source))
				static_cast<CdAbstractArray*>(&Source)->_AssignToDim(*this);
		}

		CdIterator I = Array.IterBegin();
		this->Append(I, Count);
	} else
		CdContainer::AssignOneEx(Source, Append, Param);
}

void CdAbstractArray::ReadData(const C_Int32 *Start, const C_Int32 *Length,
	void *OutBuffer, C_SVType OutSV)
{
	if ((Start != NULL) || (Length != NULL))
	{
		_CheckRect(Start, Length);
	}
	switch (OutSV)
	{
		case svInt8:
			ArrayRIterRect(Start, Length, DimCnt(), *this,
				(C_Int8*)OutBuffer, IIndex, ITER_INT<C_Int8>::Read);
			break;
		case svUInt8:
			ArrayRIterRect(Start, Length, DimCnt(), *this,
				(C_UInt8*)OutBuffer, IIndex, ITER_INT<C_UInt8>::Read);
			break;
		case svInt16:
			ArrayRIterRect(Start, Length, DimCnt(), *this,
				(C_Int16*)OutBuffer, IIndex, ITER_INT<C_Int16>::Read);
			break;
		case svUInt16:
			ArrayRIterRect(Start, Length, DimCnt(), *this,
				(C_UInt16*)OutBuffer, IIndex, ITER_INT<C_UInt16>::Read);
			break;
		case svInt32:
			ArrayRIterRect(Start, Length, DimCnt(), *this,
				(C_Int32*)OutBuffer, IIndex, ITER_INT<C_Int32>::Read);
			break;
		case svUInt32:
			ArrayRIterRect(Start, Length, DimCnt(), *this,
				(C_UInt32*)OutBuffer, IIndex, ITER_INT<C_UInt32>::Read);
			break;
		case svInt64:
			ArrayRIterRect(Start, Length, DimCnt(), *this,
				(C_Int64*)OutBuffer, IIndex, ITER_INT<C_Int64>::Read);
			break;
		case svUInt64:
			ArrayRIterRect(Start, Length, DimCnt(), *this,
				(C_UInt64*)OutBuffer, IIndex, ITER_INT<C_UInt64>::Read);
			break;
		case svFloat32:
			ArrayRIterRect(Start, Length, DimCnt(), *this,
				(C_Float32*)OutBuffer, IIndex, ITER_FLOAT<C_Float32>::Read);
			break;
		case svFloat64:
			ArrayRIterRect(Start, Length, DimCnt(), *this,
				(C_Float64*)OutBuffer, IIndex, ITER_FLOAT<C_Float64>::Read);
			break;
		case svStrUTF8:
			ArrayRIterRect(Start, Length, DimCnt(), *this,
				(UTF8String*)OutBuffer, IIndex, ITER_STR8_Read);
			break;
		case svStrUTF16:
			ArrayRIterRect(Start, Length, DimCnt(), *this,
				(UTF16String*)OutBuffer, IIndex, ITER_STR16_Read);
			break;
		default:
			throw ErrArray(ERR_READ_INV_SV);
	}
}

void CdAbstractArray::ReadDataEx(const C_Int32 *Start, const C_Int32 *Length,
	const C_BOOL *const Selection[], void *OutBuffer, C_SVType OutSV)
{
	if (Selection == NULL)
	{
		ReadData(Start, Length, OutBuffer, OutSV);
		return;
	}
	_CheckRect(Start, Length);
	switch (OutSV)
	{
		case svInt8:
			ArrayRIterRectEx(Start, Length, Selection, DimCnt(), *this,
				(C_Int8*)OutBuffer, IIndex, ITER_INT<C_Int8>::ReadEx);
			break;
		case svUInt8:
			ArrayRIterRectEx(Start, Length, Selection, DimCnt(), *this,
				(C_UInt8*)OutBuffer, IIndex, ITER_INT<C_UInt8>::ReadEx);
			break;
		case svInt16:
			ArrayRIterRectEx(Start, Length, Selection, DimCnt(), *this,
				(C_Int16*)OutBuffer, IIndex, ITER_INT<C_Int16>::ReadEx);
			break;
		case svUInt16:
			ArrayRIterRectEx(Start, Length, Selection, DimCnt(), *this,
				(C_UInt16*)OutBuffer, IIndex, ITER_INT<C_UInt16>::ReadEx);
			break;
		case svInt32:
			ArrayRIterRectEx(Start, Length, Selection, DimCnt(), *this,
				(C_Int32*)OutBuffer, IIndex, ITER_INT<C_Int32>::ReadEx);
			break;
		case svUInt32:
			ArrayRIterRectEx(Start, Length, Selection, DimCnt(), *this,
				(C_UInt32*)OutBuffer, IIndex, ITER_INT<C_UInt32>::ReadEx);
			break;
		case svInt64:
			ArrayRIterRectEx(Start, Length, Selection, DimCnt(), *this,
				(C_Int64*)OutBuffer, IIndex, ITER_INT<C_Int64>::ReadEx);
			break;
		case svUInt64:
			ArrayRIterRectEx(Start, Length, Selection, DimCnt(), *this,
				(C_UInt64*)OutBuffer, IIndex, ITER_INT<C_UInt64>::ReadEx);
			break;
		case svFloat32:
			ArrayRIterRectEx(Start, Length, Selection, DimCnt(), *this,
				(C_Float32*)OutBuffer, IIndex, ITER_FLOAT<C_Float32>::ReadEx);
			break;
		case svFloat64:
			ArrayRIterRectEx(Start, Length, Selection, DimCnt(), *this,
				(C_Float64*)OutBuffer, IIndex, ITER_FLOAT<C_Float64>::ReadEx);
			break;
		case svStrUTF8:
			ArrayRIterRectEx(Start, Length, Selection, DimCnt(), *this,
				(UTF8String*)OutBuffer, IIndex, ITER_STR8_ReadEx);
			break;
		case svStrUTF16:
			ArrayRIterRectEx(Start, Length, Selection, DimCnt(), *this,
				(UTF16String*)OutBuffer, IIndex, ITER_STR16_ReadEx);
			break;
		default:
			throw ErrArray(ERR_READEX_INV_SV);
	}
}

void CdAbstractArray::WriteData(const C_Int32 *Start, const C_Int32 *Length,
	void const *InBuffer, C_SVType InSV)
{
	_CheckRect(Start, Length);
	switch (InSV)
	{
		case svInt8:
			ArrayWIterRect(Start, Length, DimCnt(), *this,
				(const C_Int8*)InBuffer, IIndex, ITER_INT<C_Int8>::Write);
			break;
		case svUInt8:
			ArrayWIterRect(Start, Length, DimCnt(), *this,
				(const C_UInt8*)InBuffer, IIndex, ITER_INT<C_UInt8>::Write);
			break;
		case svInt16:
			ArrayWIterRect(Start, Length, DimCnt(), *this,
				(const C_Int16*)InBuffer, IIndex, ITER_INT<C_Int16>::Write);
			break;
		case svUInt16:
			ArrayWIterRect(Start, Length, DimCnt(), *this,
				(const C_UInt16*)InBuffer, IIndex, ITER_INT<C_UInt16>::Write);
			break;
		case svInt32:
			ArrayWIterRect(Start, Length, DimCnt(), *this,
				(const C_Int32*)InBuffer, IIndex, ITER_INT<C_Int32>::Write);
			break;
		case svUInt32:
			ArrayWIterRect(Start, Length, DimCnt(), *this,
				(const C_UInt32*)InBuffer, IIndex, ITER_INT<C_UInt32>::Write);
			break;
		case svInt64:
			ArrayWIterRect(Start, Length, DimCnt(), *this,
				(const C_Int64*)InBuffer, IIndex, ITER_INT<C_Int64>::Write);
			break;
		case svUInt64:
			ArrayWIterRect(Start, Length, DimCnt(), *this,
				(const C_UInt64*)InBuffer, IIndex, ITER_INT<C_UInt64>::Write);
			break;
		case svFloat32:
			ArrayWIterRect(Start, Length, DimCnt(), *this,
				(const C_Float32*)InBuffer, IIndex, ITER_FLOAT<C_Float32>::Write);
			break;
		case svFloat64:
			ArrayWIterRect(Start, Length, DimCnt(), *this,
				(const C_Float64*)InBuffer, IIndex, ITER_FLOAT<C_Float64>::Write);
			break;
		case svStrUTF8:
			ArrayWIterRect(Start, Length, DimCnt(), *this,
				(const UTF8String*)InBuffer, IIndex, ITER_STR8_Write);
			break;
		case svStrUTF16:
			ArrayWIterRect(Start, Length, DimCnt(), *this,
				(const UTF16String*)InBuffer, IIndex, ITER_STR16_Write);
			break;
		default:
			throw ErrArray(ERR_WRITE_INV_SV);
	}
}

void CdAbstractArray::Append(CdIterator &I, C_Int64 Count)
{
	#define ITER_APPEND(TYPE, SV) \
		{ \
			const ssize_t SIZE = COREARRAY_STREAM_BUFFER / sizeof(TYPE); \
			TYPE Buffer[SIZE]; \
			while (Count > 0) \
			{ \
				ssize_t n = (Count <= SIZE) ? Count : SIZE; \
				I.ReadData(Buffer, n, SV); \
				this->Append(Buffer, n, SV); \
				Count -= n; \
			} \
			break; \
		}

	switch (SVType())
	{
		case svInt8:
			ITER_APPEND(C_Int8, svInt8);
		case svUInt8:
			ITER_APPEND(C_UInt8, svUInt8);
		case svInt16:
			ITER_APPEND(C_Int16, svInt16);
		case svUInt16:
			ITER_APPEND(C_UInt16, svUInt16);
		case svInt32:
			ITER_APPEND(C_Int32, svInt32);
		case svUInt32:
			ITER_APPEND(C_UInt32, svUInt32);
		case svInt64: case svCustomInt:
			ITER_APPEND(C_Int64, svInt64);
		case svUInt64: case svCustomUInt:
			ITER_APPEND(C_UInt64, svUInt64);
		case svFloat32:
			ITER_APPEND(C_Float32, svFloat32);
		case svFloat64: case svCustomFloat:
			ITER_APPEND(C_Float64, svFloat64);
		case svStrUTF8:
			ITER_APPEND(UTF8String, svStrUTF8);
		case svStrUTF16: case svCustomStr:
			ITER_APPEND(UTF16String, svStrUTF16);
		default:
			throw ErrContainer("Invalid SVType in destination.");
	}

	#undef ITER_APPEND
}

void CdAbstractArray::_CheckRect(const C_Int32 Start[],
	const C_Int32 Length[]) const
{
	if ((Start==NULL) || (Length==NULL))
		throw ErrArray(ERR_INV_DIM_RECT);
	for (int i=0; i < DimCnt(); i++)
	{
		if ((*Start < 0) || (*Length < 0) || ((*Start + *Length)>GetDLen(i)))
			throw ErrArray(ERR_INV_DIM_RECT);
		Start ++; Length ++;
	}
}

void CdAbstractArray::_AssignToDim(CdAbstractArray &Dest) const
{
	TArrayDim dims;
	GetDim(dims);
	if (DimCnt() > 0) dims[0] = 0;
	Dest.ResetDim(dims, DimCnt());
}

void CdAbstractArray::IIndex(CdAbstractArray &Obj, CdIterator &I,
	const C_Int32 DimI[])
{
	I = Obj.Iterator(DimI);
}


// fill selection, return true if it is a block
static bool fill_selection(C_Int32 DimSize, const C_BOOL Selection[],
	C_Int32 &OutStart, C_Int32 &OutCnt, C_Int32 &OutCntValid)
{
	if (Selection)
	{
		// find the first
		const C_BOOL *s = Selection;
		OutStart = 0;
		for (C_Int32 D = DimSize; D >= 0; D--)
		{
			if (*s) break;
			s ++; OutStart ++;
		}
		if (OutStart >= DimSize)
		{
			OutStart = 0; OutCnt = 0; OutCntValid = 0;
			return true;
		} 

		// find the last
		s = Selection + DimSize - 1;
		C_Int32 _st = DimSize - 1;
		for (C_Int32 D = DimSize; D >= 0; D--)
		{
			if (*s) break;
			s --; _st --;
		}

		// the block size
		OutCnt = _st - OutStart + 1;
		OutCntValid = 0;
		bool rv = true;

		s = &Selection[OutStart];
		for (C_Int32 i=0; i < OutCnt; i++)
		{
			if (*s)
				OutCntValid ++;
			else
				rv = false;
			s ++;
		}

		return rv;

	} else {
		OutStart = 0;
		OutCnt = DimSize;
		OutCntValid = DimSize;
		return true;
	}
}

void CdAbstractArray::GetInfoSelection(const C_BOOL *const Selection[],
	C_Int32 OutStart[], C_Int32 OutBlockLen[], C_Int32 OutValidCnt[])
{
	// no need a memory buffer
	if (Selection)
	{
		// with selection
		for (int i=0; i < DimCnt(); i++)
		{
			C_Int32 S, L, C;
			fill_selection(GetDLen(i), Selection[i], S, L, C);
			if (OutStart) OutStart[i] = S;
			if (OutBlockLen) OutBlockLen[i] = L;
			if (OutValidCnt) OutValidCnt[i] = C;
		}
	} else {
		// no selection, using all data
		for (int i=0; i < DimCnt(); i++)
		{
			if (OutStart) OutStart[i] = 0;
			C_Int32 L = GetDLen(i);
			if (OutValidCnt) OutValidCnt[i] = L;
			if (OutBlockLen) OutBlockLen[i] = L;
		}
	}
}

void CdAbstractArray::GetInfoSelection(const C_Int32 Start[],
	const C_Int32 Length[], const C_BOOL *const Selection[],
	C_Int32 OutStart[], C_Int32 OutBlockLen[], C_Int32 OutValidCnt[])
{
	// no need a memory buffer
	if (Selection)
	{
		// with selection
		for (int i=0; i < DimCnt(); i++)
		{
			C_Int32 S, L, C;
			fill_selection(Length[i], Selection[i], S, L, C);
			if (OutStart) OutStart[i] = Start[i]+S;
			if (OutBlockLen) OutBlockLen[i] = L;
			if (OutValidCnt) OutValidCnt[i] = C;
		}
	} else {
		// no selection, using all data
		for (int i=0; i < DimCnt(); i++)
		{
			if (OutStart) OutStart[i] = Start[i];
			if (OutBlockLen) OutBlockLen[i] = Length[i];
			if (OutValidCnt) OutValidCnt[i] = Length[i];
		}
	}
}



// =====================================================================
// CdAllocArray
// =====================================================================

static const char *VAR_DATA = "DATA";
static const char *VAR_DCNT = "DCNT";
static const char *VAR_DIM  = "DIM";

static const char *ERR_ELM_SIZE        = "%s: Invalid ElmSize (%d).";
static const char *ERR_INV_DIM_CNT     = "%s: Invalid number of dimensions (%d).";
static const char *ERR_INV_DIMLEN      = "%s: Invalid length of the %d dimension (%d).";
static const char *ERR_INV_DIM_INDEX   = "%s: Invalid index of dimentions (%d).";
static const char *ERR_DIM_INDEX       = "Invalid dimension index.";
static const char *ERR_DIM_INDEX_VALUE = "Invalid %d-th dimension size: %d.";
static const char *ERR_APPEND_SV       = "Invalid 'InSV' in 'CdAllocArray::Append'.";
static const char *ERR_PACKED_MODE     = "Invalid packed/compression method '%s'.";
static const char *ERR_READONLY        = "The GDS file is read-only!";
static const char *ERR_SETELMSIZE      = "CdAllocArray::SetElmSize, Invalid parameter.";


CdAllocArray::CdAllocArray(ssize_t vElmSize): CdAbstractArray()
{
	fElmSize = vElmSize;
	if (vElmSize <= 0)
    	throw ErrArray(ERR_ELM_SIZE, "CdAllocArray::CdAllocArray", vElmSize);

	fDimension.resize(1);
	fDimension[0].DimElmSize = fElmSize;
	fDimension[0].DimElmCnt = 1;
	fTotalCount = 0;

	vAllocID = 0;
	vAllocStream = NULL;
	vAlloc_Ptr = vCnt_Ptr = 0;
	fNeedUpdate = false;
	_OnFlushEvent = NULL;
}

CdAllocArray::~CdAllocArray()
{
	CloseWriter();
	if (fGDSStream) Synchronize();
}

bool CdAllocArray::Empty()
{
	return (fTotalCount <= 0);
}

C_Int64 CdAllocArray::TotalCount()
{
	return fTotalCount;
}

int CdAllocArray::DimCnt() const
{
	return fDimension.size();
}

void CdAllocArray::GetDim(C_Int32 DimLen[]) const
{
	vector<TDimItem>::const_iterator it;
	for (it=fDimension.begin(); it != fDimension.end();)
	{
		*DimLen = it->DimLen;
		DimLen ++;
		it ++;
	}
}

void CdAllocArray::ResetDim(const C_Int32 DimLen[], int DCnt)
{
	if ((DCnt <= 0) || (DCnt > (int)MAX_ARRAY_DIM))
		throw ErrArray(ERR_INV_DIM_CNT, "CdAllocArray::ResetDim", DCnt);

	for (int i=0; i < DCnt; i++)
	{
		if (DimLen[i] < 0)
		{
			throw ErrArray(ERR_INV_DIMLEN,
				"CdAllocArray::ResetDim", i, DimLen[i]);
		}
	}

	// the total count of DimLen
	C_Int64 TotCnt = 1;
	for (int i=0; i < DCnt; i++) TotCnt *= DimLen[i];

	if (TotCnt > fTotalCount)
	{
		CdIterator I = IterEnd();
		fAllocator.SetSize(AllocSize(TotCnt));
		IterInit(I, TotCnt - fTotalCount);
	} else if (TotCnt < fTotalCount)
	{
		CdIterator I = IterBegin();
		IterOffset(I, TotCnt);
		IterDone(I, fTotalCount - TotCnt);
		fAllocator.SetSize(AllocSize(TotCnt));
	}
	fTotalCount = TotCnt;

	// set fDimension
	_ResetDim(DimLen, DCnt);

	fChanged = true;
	if (fGDSStream) SaveToBlockStream();
}

C_Int32 CdAllocArray::GetDLen(int I) const
{
	if ((I < 0) || (I >= (int)fDimension.size()))
		throw ErrArray(ERR_INV_DIM_INDEX, "CdAllocArray::GetDLen", I);

	return fDimension[I].DimLen;
}

void CdAllocArray::SetDLen(int I, C_Int32 Value)
{
	_CheckSetDLen(I, Value);

	CdIterator it;
	C_Int64 MDimOld, MDimNew, LStep, DCnt, DResid;
	SIZE64 pS, pD;
	TDimItem &pDim = fDimension[I];

	if (pDim.DimLen != Value)
	{
		C_Int64 S = pDim.DimElmCnt * pDim.DimLen;
		if (fTotalCount > S)
		{
			it.Handler = this;
			it.Ptr = S * fElmSize;
			IterDone(it, fTotalCount - S);
		}

		if (pDim.DimElmSize > 0)
		{
			DCnt = 1;
			for (int i=I-1; i >= 0; i--)
				DCnt *= fDimension[i].DimLen;
			if (DCnt > 0)
			{
				MDimOld = pDim.DimLen * pDim.DimElmSize;
				MDimNew = Value * pDim.DimElmSize;
				if (pDim.DimLen < Value)
				{
					fAllocator.SetSize(DCnt * MDimNew);
					DResid = (Value - pDim.DimLen) * pDim.DimElmCnt;
					pS = (DCnt-1)*MDimOld; pD = (DCnt-1)*MDimNew;
					it.Handler = this;
					while (DCnt > 0)
					{
						fAllocator.Move(pS, pD, MDimOld);
						it.Ptr = pD + MDimOld;
						IterInit(it, DResid);
						pS -= MDimOld; pD -= MDimNew;
						DCnt --;
					}
				} else {
					LStep = MDimOld - MDimNew;
					DResid = (pDim.DimLen - Value) * pDim.DimElmCnt;
					it.Handler = this; it.Ptr = 0;
					pD = pS = 0;
					while (DCnt > 0)
					{
						it.Ptr += MDimNew;
						IterDone(it, DResid);
						it.Ptr += LStep;
						fAllocator.Move(pS, pD, MDimNew);
						pS += MDimOld; pD += MDimNew;
						DCnt --;
                    }
				}
			}
		}
		pDim.DimLen = Value;
		_SetDimAuto(I);

		// Notify32(mcDimLength, DimIndex);
		// Notify(mcDimChanged);

		fChanged = true;
		if (fGDSStream) SaveToBlockStream();
	}
}

C_Int64 CdAllocArray::TotalArrayCount()
{
	TDimItem &R = fDimension[0];
	return R.DimLen * R.DimElmCnt;
}

SIZE64 CdAllocArray::AllocSize(C_Int64 Num)
{
	return fElmSize * Num;
}

CdIterator CdAllocArray::IterBegin()
{
	CdIterator I;
	I.Allocator = &fAllocator;
	I.Ptr = 0;
	I.Handler = this;
	return I;
}

CdIterator CdAllocArray::IterEnd()
{
	CdIterator I;
	I.Allocator = &fAllocator;
	I.Ptr = fTotalCount * fElmSize;
	I.Handler = this;
	return I;
}

CdIterator CdAllocArray::Iterator(const C_Int32 DimIndex[])
{
	_CheckRange(DimIndex);
	CdIterator it;
	it.Handler = this;
	it.Ptr = _IndexPtr(DimIndex);
	return it;
}

void CdAllocArray::IterOffset(CdIterator &I, SIZE64 val)
{
	I.Ptr += val * fElmSize;
}

void CdAllocArray::IterInit(CdIterator &I, SIZE64 n)
{
	fAllocator.SetPosition(I.Ptr);
	fAllocator.ZeroFill(n * fElmSize);
}

void CdAllocArray::IterDone(CdIterator &I, SIZE64 n)
{
	// do nothing
}

void CdAllocArray::Clear()
{
	// TODO
	if (fPipeInfo == NULL)
	{
		C_Int32 ZERO = 0;
		ResetDim(&ZERO, 1);
	}
}

void CdAllocArray::Synchronize()
{
	CdAbstractArray::Synchronize();

	if (fGDSStream && (!fGDSStream->ReadOnly()) && fNeedUpdate)
	{
		if (fAllocator.BufStream())
			fAllocator.BufStream()->FlushWrite();
		UpdateInfo(NULL);
	}
}

void CdAllocArray::CloseWriter()
{
	if (fAllocator.BufStream())
	{
		fAllocator.BufStream()->OnFlush.Clear();
		fAllocator.BufStream()->FlushWrite();
		if (fPipeInfo)
		{
			if (fPipeInfo->WriteMode(*fAllocator.BufStream()))
			{
            	fPipeInfo->ClosePipe(*fAllocator.BufStream());
				fNeedUpdate = true;
				UpdateInfo(NULL);

				vAllocStream->AddRef();
				fAllocator.Free();
				vAllocStream->SetPosition(0);
				fAllocator.Initialize(*vAllocStream, true, false);

				if (fPipeInfo)
					fPipeInfo->PushReadPipe(*fAllocator.BufStream());
				vAllocStream->Release();
            }
		} else {
			fNeedUpdate = true;
			Synchronize();
		}
	}
}

void CdAllocArray::SetPackedMode(const char *Mode)
{
	if (fGDSStream && fGDSStream->ReadOnly())
		throw ErrArray(ERR_READONLY);

	if (fPipeInfo ? (!fPipeInfo->Equal(Mode)) : true)
	{
		if ((fTotalCount>0) && (vAllocStream!=NULL) && (fGDSStream!=NULL))
		{
			Synchronize();

			if (fPipeInfo) delete fPipeInfo;
			fPipeInfo = dStreamPipeMgr.Match(*this, Mode);
			if ((fPipeInfo==NULL) && (strcmp(Mode, "")!=0))
				throw ErrArray(ERR_PACKED_MODE, Mode);

			{
				// automatically release the temporary stream
				CdStream *TmpStream = new CdTempStream;
				TdAutoRef<CdBufStream> Output(new CdBufStream(TmpStream));
				if (fPipeInfo)
					fPipeInfo->PushWritePipe(*Output);

				fAllocator.CopyTo(*Output, 0, AllocSize(fTotalCount));
				Output.get()->FlushWrite();
				if (fPipeInfo)
				{
					fPipeInfo->ClosePipe(*Output);
					fPipeInfo->GetStreamInfo(Output.get());
				}

				// copy
				vAllocStream->SetPosition(0);
				vAllocStream->SetSizeOnly(0);
				vAllocStream->CopyFrom(*TmpStream);
			}

			vAllocStream->SetPosition(0);
			if (fPipeInfo)
			{
				// compressed, no writing
				fAllocator.Initialize(*vAllocStream, true, false);
				fPipeInfo->PushReadPipe(*fAllocator.BufStream());
			} else {
				// raw data, allow writing
				fAllocator.Initialize(*vAllocStream, true, true);
			}

			// save, since PipeInfo has been changed.
			SaveToBlockStream();
		} else {
			if (fPipeInfo)
				delete fPipeInfo;
			fPipeInfo = dStreamPipeMgr.Match(*this, Mode);
			if ((fPipeInfo==NULL) && (strcmp(Mode, "")!=0))
				throw ErrArray(ERR_PACKED_MODE, Mode);

			if ((fPipeInfo!=NULL) && (vAllocStream!=NULL))
			{
				vAllocStream->SetPosition(0);
				fAllocator.Initialize(*vAllocStream, false, true);
				fPipeInfo->PushWritePipe(*fAllocator.BufStream());
			}
		}
	}
}

void CdAllocArray::Append(const void *Buffer, ssize_t Cnt, C_SVType InSV)
{
	if (Cnt <= 0) return;
	if (!COREARRAY_SV_VALID(InSV)) throw ErrArray(ERR_APPEND_SV);

	// writing
	_SetLargeBuffer();
	fAllocator.SetPosition(fTotalCount*fElmSize);
	fAllocator.WriteData(Buffer, Cnt*fElmSize);

	// check
	TDimItem &R = fDimension.front();
	fTotalCount += Cnt;
	if (fTotalCount >= R.DimElmCnt*(R.DimLen+1))
	{
		R.DimLen = fTotalCount / R.DimElmCnt;
		_SetFlushEvent();
		fNeedUpdate = true;
	}
}

void CdAllocArray::Caching()
{
	if (vAllocStream)
	{
		C_UInt8 Buffer[STREAM_BUFFER_SIZE];

		SIZE64 SavePos = vAllocStream->Position();
		vAllocStream->SetPosition(0);
		SIZE64 p=0, size=vAllocStream->GetSize();

		while (p < size)
		{
			SIZE64 L = size - p;
			if (L > STREAM_BUFFER_SIZE)
				L = STREAM_BUFFER_SIZE;
			vAllocStream->ReadData(Buffer, L);
			p += L;
		}

		vAllocStream->SetPosition(SavePos);
	}
}

SIZE64 CdAllocArray::GDSStreamSize()
{
	if (vAllocStream)
	{
		return vAllocStream->GetSize();
	} else
		return -1;
}

void CdAllocArray::GetOwnBlockStream(vector<const CdBlockStream*> &Out)
{
	Out.clear();
	if (vAllocStream) Out.push_back(vAllocStream);
}

void CdAllocArray::_CheckRange(const C_Int32 DimI[])
{
	vector<TDimItem>::iterator it;
	for (it=fDimension.begin(); it != fDimension.end(); it++, DimI++)
	{
		if ((DimI[0] < 0) || (DimI[0] > it->DimLen))
			throw ErrArray(ERR_DIM_INDEX);
	}
}

void CdAllocArray::_CheckRect(const C_Int32 *Start, const C_Int32 *Length)
{
	vector<TDimItem>::iterator it;
	for (it=fDimension.begin(); it != fDimension.end(); it++)
	{
		if ((Start[0]<0) || (Length[0]<0) || (Start[0]+Length[0] > it->DimLen))
			throw ErrArray(ERR_INV_DIM_RECT);
		++Start; ++Length;
	}
}

void CdAllocArray::_CheckSetDLen(int I, C_Int32 Value)
{
	if ((I < 0) || (I >= (int)fDimension.size()))
		throw ErrArray(ERR_INV_DIM_INDEX, "CdAllocArray::SetDLen", I);
	if (Value < 0)
		throw ErrArray(ERR_DIM_INDEX_VALUE, I, Value);
	if ((Value == 0) && (I > 0))
		throw ErrArray(ERR_DIM_INDEX_VALUE, I, Value);
}

SIZE64 CdAllocArray::_IndexPtr(const C_Int32 DimI[])
{
	vector<TDimItem>::const_iterator it;
	SIZE64 rv = 0;
	for (it=fDimension.begin(); it != fDimension.end(); it++)
	{
		rv += DimI[0] * (it->DimElmSize);
		DimI ++;
	}
	return rv;
}

void CdAllocArray::_ResetDim(const C_Int32 DimLen[], int DCnt)
{
	fDimension.resize(DCnt);
	SIZE64 TotCnt = 1;
	for (int i=DCnt-1; i >= 0; i--)
	{
		TDimItem &D = fDimension[i];
		D.DimLen = DimLen[i];
		D.DimElmSize = TotCnt * fElmSize;
		D.DimElmCnt = TotCnt;
		TotCnt *= D.DimLen;
	}
	fTotalCount = TotCnt;
}

void CdAllocArray::Loading(CdReader &Reader, TdVersion Version)
{
	CdAbstractArray::Loading(Reader, Version);

	// dimension
	C_UInt16 DCnt = 0;
	Reader[VAR_DCNT] >> DCnt;
	TArrayDim DimBuf;
	Reader[VAR_DIM].GetAutoArray(DimBuf, DCnt);
	_ResetDim(DimBuf, DCnt);

	if (fGDSStream ? fGDSStream->ReadOnly() : true)
		vCnt_Ptr = 0;
	else if (DCnt > 0)
		vCnt_Ptr = Reader.PropPosition(VAR_DIM);
	else
		vCnt_Ptr = 0;

	// load the content
	if (fGDSStream)
	{
		Reader[VAR_DATA] >> vAllocID;
		vAlloc_Ptr = Reader.PropPosition(VAR_DATA);
		vAllocStream = fGDSStream->Collection()[vAllocID];
		fAllocator.Initialize(*vAllocStream, true, !fGDSStream->ReadOnly());
		if (fPipeInfo)
			fPipeInfo->PushReadPipe(*fAllocator.BufStream());
	}

	fChanged = fNeedUpdate = false;
}

void CdAllocArray::Saving(CdWriter &Writer)
{
	CdAbstractArray::Saving(Writer);

	// save dimension
	TArrayDim DBuf;
	C_UInt16 D = fDimension.size();
	Writer[VAR_DCNT] << D;

	vector<TDimItem>::iterator it;
	C_Int32 *p = &DBuf[0];
	for (it=fDimension.begin(); it != fDimension.end(); it++)
		*p++ = it->DimLen;
	Writer[VAR_DIM].NewAutoArray(DBuf, D);
	vCnt_Ptr = Writer.PropPosition(VAR_DIM);

	// save data
	if (fGDSStream != NULL)
	{
		if (vAllocStream == NULL)
		{
			vAllocStream = fGDSStream->Collection().NewBlockStream();
			fAllocator.Initialize(*vAllocStream, true, true);
			if (fPipeInfo)
				fPipeInfo->PushWritePipe(*fAllocator.BufStream());
		}
		TdGDSBlockID Entry = vAllocStream->ID();
		Writer[VAR_DATA] << Entry;
		vAlloc_Ptr = Writer.PropPosition(VAR_DATA);
	}
}

void CdAllocArray::GetPipeInfo()
{
	if (fAllocator.BufStream())
	{
		if (_GetStreamPipeInfo(fAllocator.BufStream(), false))
			fNeedUpdate = true;
	}
}

void CdAllocArray::UpdateInfo(CdBufStream *Sender)
{
	if (fNeedUpdate)
	{
		// update pipe information
		if (fPipeInfo)
		{
			if (_GetStreamPipeInfo(fAllocator.BufStream(), false))
				_UpdateStreamPipeInfo(*fGDSStream);
        }
		// update dimension
		if (vCnt_Ptr != 0)
		{
			TArrayDim DBuf;
			C_Int32 *p = &DBuf[0];
			vector<TDimItem>::iterator it;
			for (it=fDimension.begin(); it != fDimension.end(); it++)
				*p++ = it->DimLen;
			BYTE_LE<CdStream> W(fGDSStream);
			W.SetPosition(vCnt_Ptr);
			W.W(DBuf, fDimension.size());
		}

		// call external function
		if (_OnFlushEvent)
			(*_OnFlushEvent)(this, Sender);

		fNeedUpdate = false;
	}

	// clear the event, because
	//   fAllocator.BufStream()->OnFlush.Set(this, &CdAllocArray::UpdateInfo)
	if (fAllocator.BufStream())
		fAllocator.BufStream()->OnFlush.Clear();
}

void CdAllocArray::SetElmSize(ssize_t NewSize)
{
	if (NewSize <= 0)
		throw ErrArray(ERR_SETELMSIZE);

	if (fElmSize != NewSize)
	{
		if (fTotalCount > 0)
		{
			SIZE64 pS, pD;
			if (NewSize > fElmSize)
			{
				pS = fElmSize * fTotalCount;
				pD = NewSize * fTotalCount;
				fAllocator.SetSize(pD);
				ssize_t Lx = NewSize - fElmSize;
				for (C_Int64 n=fTotalCount; n > 0; n--)
				{
					pS -= fElmSize; pD -= NewSize;

					fAllocator.Move(pS, pD, fElmSize);

					fAllocator.SetPosition(pD+fElmSize);
					fAllocator.ZeroFill(Lx);
				}
			} else {
				pS = pD = 0;
				for (C_Int64 n=fTotalCount; n > 0; n--)
				{
					pS += fElmSize; pD += NewSize;
					fAllocator.Move(pS, pD, NewSize);
				}
			}
		}
		fElmSize = NewSize;

		SIZE64 TotCnt = 1;
		for (int i=fDimension.size()-1; i >= 0; i--)
		{
			TDimItem &D = fDimension[i];
			D.DimElmSize = TotCnt * fElmSize;
			D.DimElmCnt = TotCnt;
			TotCnt *= D.DimLen;
		}

		fNeedUpdate = true;
	}
}

void CdAllocArray::_SetDimAuto(int DimIndex)
{
	vector<TDimItem>::iterator it = fDimension.begin() + DimIndex;
	SIZE64 LSize = it->DimElmSize;
	SIZE64 LCnt = it->DimElmCnt;
	for (; DimIndex >= 1; DimIndex--)
	{
		LSize = LSize * it->DimLen;
		LCnt = LCnt * it->DimLen;
		it--;
		it->DimElmSize = LSize;
		it->DimElmCnt = LCnt;
	}
	fTotalCount = it->DimLen * LCnt;
	fNeedUpdate = true;
}

void CdAllocArray::_SetSmallBuffer()
{
	if (fAllocator.BufStream())
	{
		if (fAllocator.BufStream()->BufSize() != STREAM_BUFFER_SMALL_SIZE)
		{
			fAllocator.BufStream()->SetBufSize(STREAM_BUFFER_SMALL_SIZE);
		}
	}
}

void CdAllocArray::_SetLargeBuffer()
{
	if (fAllocator.BufStream())
	{
		if (fAllocator.BufStream()->BufSize() != STREAM_BUFFER_LARGE_SIZE)
		{
			fAllocator.BufStream()->SetBufSize(STREAM_BUFFER_LARGE_SIZE);
		}
	}
}

void CdAllocArray::_SetFlushEvent()
{
	fAllocator.BufStream()->OnFlush.Set(this, &CdAllocArray::UpdateInfo);
}




// ===========================================================
// Apply functions
// ===========================================================

// the size of memory buffer for reading dataset marginally
C_Int64 CoreArray::ARRAY_READ_MEM_BUFFER_SIZE = 1024*1024*1024;


// read an array-oriented object margin by margin

CdArrayRead::CdArrayRead()
{
	fObject = NULL;
	fMargin = 0;
	fSVType = svCustom;
	fIndex = fCount = 0;
	fMarginCount = 0;
	fMarginIndex = _MarginStart = _MarginEnd = 0;
	_Margin_Buf_IncCnt = 0;
	_Margin_Buf_Cnt = 0;
	_Call_rData = _Margin_Call_rData = true;
}

CdArrayRead::~CdArrayRead()
{
}
		
void CdArrayRead::Init(CdAbstractArray &vObj, int vMargin, C_SVType vSVType,
	const C_BOOL *const vSelection[], bool buf_if_need)
{
	// set object
	fObject = &vObj;

	// check
	int DCnt = vObj.DimCnt();
	fMargin = vMargin;
	if ((vMargin < 0) || (vMargin >= DCnt))
		throw ErrArray("Error margin %d: reading dataset marginally.", vMargin);

	// initialize ...
	CdAbstractArray::TArrayDim DimLen;
	vObj.GetDim(DimLen);

	// determine the size of element
	fSVType = vSVType;
	switch (vSVType)
	{
		case svInt8:         // Signed integer of 8 bits
		case svUInt8:        // Unsigned integer of 8 bits
			fElmSize = 1; break;
		case svInt16:        // Signed integer of 16 bits
		case svUInt16:       // Unsigned integer of 16 bits
			fElmSize = 2; break;
		case svInt32:        // Signed integer of 32 bits
		case svUInt32:       // Unsigned integer of 32 bits
			fElmSize = 4; break;
		case svInt64:        // Signed integer of 64 bits
		case svUInt64:       // Unsigned integer of 64 bits
			fElmSize = 8; break;
		case svFloat32:      // Float number of single precision (32 bits)
			fElmSize = 4; break;
		case svFloat64:      // Float number of double precision (64 bits)
			fElmSize = 8; break;
		case svStrUTF8:      // UTF-8 string
			fElmSize = sizeof(UTF8String);
			break;
		case svStrUTF16:     // UTF-16 string
			fElmSize = sizeof(UTF16String);
			break;
		default:
			if (dynamic_cast<CdAllocArray*>(&vObj))
			{
				fElmSize = dynamic_cast<CdAllocArray*>(&vObj)->ElmSize();
			} else {
				fElmSize = ((vObj.BitOf() & 0x7) > 0) ?
					(vObj.BitOf()/8 + 1) : (vObj.BitOf()/8);
			}
			if (fElmSize <= 0) fElmSize = 1;
			break;
	};

	// true for calling ReadData, false for calling ReadDataEx
	_Call_rData = true;
	_Margin_Call_rData = true;

	// selection determination
	_sel_array.clear();
	_Have_Selection = (vSelection != NULL);
	if (_Have_Selection)
	{
		// with selection
		_sel_array.resize(DCnt);
		// for - loop
		for (int i=0; i < DCnt; i++)
		{
			bool v = fill_selection(DimLen[i], vSelection[i],
				_DStart[i], _DCount[i], _DCntValid[i]);
			if (i != vMargin)
			{
				if (!v) _Call_rData = false;
			} else {
				if (!v) _Margin_Call_rData = false;
			}
			_sel_array[i].assign(vSelection[i] + _DStart[i],
				vSelection[i] + _DStart[i] + _DCount[i]);
			_Selection[i] = &(_sel_array[i][0]);
		}
		_Margin_Call_rData = _Margin_Call_rData && _Call_rData;
	} else {
		// no selection, using all data
		for (int i=0; i < DCnt; i++)
		{
			_DStart[i] = 0;
			_DCount[i] = DimLen[i];
			_DCntValid[i] = DimLen[i];
		}
	}

	// total number
	C_Int64 TotalCount = 1;
	for (int i=0; i < DCnt; i++)
		TotalCount *= _DCntValid[i];

	fIndex = 0; fCount = _DCntValid[vMargin];
	fMarginCount = (fCount > 0) ? (TotalCount / fCount) : 0;

	_MarginStart = _DStart[vMargin];
	fMarginIndex = _MarginStart;
	_MarginEnd = _DStart[vMargin] + _DCount[vMargin];


	// make a margin buffer
	if (vMargin > 0)
	{
		_Margin_Buf_Cnt = 0;

		_Margin_Buf_MajorCnt = 1;
		for (int i=0; i < vMargin; i++)
			_Margin_Buf_MajorCnt *= _DCntValid[i];

		_Margin_Buf_MinorSize = fElmSize;
		for (int i=vMargin+1; i < DCnt; i++)
			_Margin_Buf_MinorSize *= _DCntValid[i];

		// determine buffer
		if (buf_if_need)
		{
			// need a memory buffer to speed up
			_Margin_Buf_IncCnt = ARRAY_READ_MEM_BUFFER_SIZE /
				(fElmSize * fMarginCount);

			if (_Margin_Buf_IncCnt > 1)
			{
				if (_Margin_Buf_IncCnt > fCount)
					_Margin_Buf_IncCnt = fCount;
				switch (fSVType)
				{
				case svStrUTF8:      // UTF-8 string
					_Margin_Buffer_UTF8.resize(
						_Margin_Buf_IncCnt * fMarginCount);
					_Margin_Buffer_Ptr = &_Margin_Buffer_UTF8[0];
					break;
				case svStrUTF16:     // UTF-16 string
					_Margin_Buffer_UTF16.resize(
						_Margin_Buf_IncCnt * fMarginCount);
					_Margin_Buffer_Ptr = &_Margin_Buffer_UTF16[0];
					break;
				default:
					_Margin_Buffer.resize(
						fElmSize * _Margin_Buf_IncCnt * fMarginCount);
					_Margin_Buffer_Ptr = &_Margin_Buffer[0];
				}
			} else {
				_Margin_Buf_IncCnt = 1;
				_Margin_Buffer.clear();
				_Margin_Buffer_UTF8.clear();
				_Margin_Buffer_UTF16.clear();
				_Margin_Buffer_Ptr = NULL;
			}
		} else {
			_Margin_Buf_IncCnt = 1;
			_Margin_Buffer.clear();
			_Margin_Buffer_UTF8.clear();
			_Margin_Buffer_UTF16.clear();
			_Margin_Buffer_Ptr = NULL;
		}
	} else {
		_Margin_Buf_IncCnt = 1;
		_Margin_Buffer.clear();
		_Margin_Buffer_UTF8.clear();
		_Margin_Buffer_UTF16.clear();
		_Margin_Buffer_Ptr = NULL;
	}
}

void CdArrayRead::AllocBuffer(C_Int64 buffer_size)
{
	if (fIndex >= fCount)
	{
		throw ErrArray("call CdArrayRead::Init first.");
	}

	if (fMargin > 0)
	{
		if (buffer_size < 0)
			buffer_size = ARRAY_READ_MEM_BUFFER_SIZE;

		// need a memory buffer to speed up
		_Margin_Buf_IncCnt = buffer_size / (fElmSize * fMarginCount);

		if (_Margin_Buf_IncCnt > 1)
		{
			if (_Margin_Buf_IncCnt > fCount)
				_Margin_Buf_IncCnt = fCount;
			switch (fSVType)
			{
				case svStrUTF8:      // UTF-8 string
					_Margin_Buffer_UTF8.resize(
						_Margin_Buf_IncCnt * fMarginCount);
					_Margin_Buffer_Ptr = &_Margin_Buffer_UTF8[0];
					break;
				case svStrUTF16:     // UTF-16 string
					_Margin_Buffer_UTF16.resize(
						_Margin_Buf_IncCnt * fMarginCount);
					_Margin_Buffer_Ptr = &_Margin_Buffer_UTF16[0];
					break;
				default:
					_Margin_Buffer.resize(
						fElmSize * _Margin_Buf_IncCnt * fMarginCount);
					_Margin_Buffer_Ptr = &_Margin_Buffer[0];
			}
		} else {
			_Margin_Buf_IncCnt = 1;
			_Margin_Buffer.clear();
			_Margin_Buffer_UTF8.clear();
			_Margin_Buffer_UTF16.clear();
			_Margin_Buffer_Ptr = NULL;
		}
	} else {
		_Margin_Buf_IncCnt = 1;
		_Margin_Buffer.clear();
		_Margin_Buffer_UTF8.clear();
		_Margin_Buffer_UTF16.clear();
		_Margin_Buffer_Ptr = NULL;
	}
}

void CdArrayRead::Read(void *Buffer)
{
	if (fIndex < fCount)
	{
		// whether it is the major dimension
		if (fMargin == 0)
		{
			// init
			_DStart[0] = fMarginIndex;
			_DCount[0] = 1;

			// read sub data
			if (_Call_rData)
			{
				fObject->ReadData(_DStart, _DCount, Buffer, fSVType);
			} else {
				_Selection[0] = &(_sel_array[0][fMarginIndex - _MarginStart]);
				fObject->ReadDataEx(_DStart, _DCount, _Selection, Buffer, fSVType);
			}

			// next ``Index'', ``MarginIndex''
			fIndex ++;
			fMarginIndex ++;
			if (_Have_Selection)
			{
				// skip unselected layout
				while ((fMarginIndex < _MarginEnd) &&
					!_sel_array[0][fMarginIndex - _MarginStart])
				{
					fMarginIndex ++;
				}
			}
		} else {

			// determine buffer size
			if (_Margin_Buf_Cnt <= 0)
			{
				// determine '_Margin_Buf_Cnt' first
				if (_Margin_Buf_IncCnt > 1)
				{
					if (_Have_Selection)
					{
						_DCount[fMargin] = 0;
						_Margin_Buf_Cnt = 0;

						C_Int32 Cnt = _Margin_Buf_IncCnt;
						for (C_Int32 k=fMarginIndex; (k < _MarginEnd) && (Cnt > 0); k++)
						{
							_DCount[fMargin] ++;
							if (_Selection[fMargin][k - _MarginStart])
							{
								_Margin_Buf_Cnt ++;
								Cnt --;
							}
						}
					} else {
						C_Int32 I = fMarginIndex + _Margin_Buf_IncCnt;
						if (I > _MarginEnd) I = _MarginEnd;
						_Margin_Buf_Cnt = I - fMarginIndex;
						_DCount[fMargin] = _Margin_Buf_Cnt;
					}
				} else {
					_Margin_Buf_Cnt = 1;
				}

				// read sub data to margin buffer
				_Margin_Buf_Need = (_Margin_Buf_Cnt > 1);
				_DStart[fMargin] = fMarginIndex;

				if (_Margin_Buf_Need)
				{
					if (_Margin_Call_rData)
					{
						fObject->ReadData(_DStart, _DCount, _Margin_Buffer_Ptr,
							fSVType);
					} else {
						// call reading with a selection
						_Selection[fMargin] =
							&(_sel_array[fMargin][fMarginIndex - _MarginStart]);
						fObject->ReadDataEx(_DStart, _DCount, _Selection,
							_Margin_Buffer_Ptr, fSVType);
					}
				} else {
					if (_Call_rData)
					{
						fObject->ReadData(_DStart, _DCount, Buffer, fSVType);
					} else {
						// call reading with a selection
						_Selection[fMargin] =
							&(_sel_array[fMargin][fMarginIndex - _MarginStart]);
						fObject->ReadDataEx(_DStart, _DCount, _Selection,
							Buffer, fSVType);
					}
				}

				if (_Margin_Buf_Need)
				{
					_Margin_Buf_MinorSize2 =
						_Margin_Buf_MinorSize * _Margin_Buf_Cnt;
				}

				_Margin_Buf_Old_Index = fIndex;
			}

			if (_Margin_Buf_Need)
			{
				C_UInt8 *s = ((C_UInt8*)_Margin_Buffer_Ptr) +
					_Margin_Buf_MinorSize * (fIndex - _Margin_Buf_Old_Index);
				C_UInt8 *p = (C_UInt8*)Buffer;

				switch (fSVType)
				{
				case svStrUTF8:      // UTF-8 string
					for (C_Int64 n=_Margin_Buf_MajorCnt; n > 0; n--)
					{
						{
							UTF8String *pl = (UTF8String*)p;
							UTF8String *sl = (UTF8String*)s;
							for (C_Int64 L=_Margin_Buf_MinorSize; L > 0;)
							{
								*pl = *sl; pl ++; sl ++;
								L -= fElmSize;
							}
						}
						p += _Margin_Buf_MinorSize;
						s += _Margin_Buf_MinorSize2;
					}
					break;
				case svStrUTF16:     // UTF-16 string
					for (C_Int64 n=_Margin_Buf_MajorCnt; n > 0; n--)
					{
						{
							UTF16String *pl = (UTF16String*)p;
							UTF16String *sl = (UTF16String*)s;
							for (C_Int64 L=_Margin_Buf_MinorSize; L > 0;)
							{
								*pl = *sl; pl ++; sl ++;
								L -= fElmSize;
							}
						}
						p += _Margin_Buf_MinorSize;
						s += _Margin_Buf_MinorSize2;
					}
					break;
				default:
					for (C_Int64 n=_Margin_Buf_MajorCnt; n > 0; n--)
					{
						memcpy(p, s, _Margin_Buf_MinorSize);
						p += _Margin_Buf_MinorSize;
						s += _Margin_Buf_MinorSize2;
					}
				}
			}

			_Margin_Buf_Cnt	--;

			// next ``Index'', ``MarginIndex''
			fIndex ++;
			fMarginIndex ++;
			if (_Have_Selection)
			{
				// skip unselected layout
				while ((fMarginIndex < _MarginEnd) &&
					!_sel_array[fMargin][fMarginIndex - _MarginStart])
				{
					fMarginIndex ++;
				}
			}
		}
	} else {
		throw ErrArray("Invalid CdArrayRead::Read.");	
	}
}

bool CdArrayRead::Eof()
{
	return (fIndex >= fCount);
}



void CoreArray::Balance_ArrayRead_Buffer(CdArrayRead *array[], int n,
	C_Int64 buffer_size)
{
	if (n <= 0)
		throw ErrArray("CoreArray::Balance_ArrayRead_Buffer !");

	if (buffer_size < 0)
		buffer_size = ARRAY_READ_MEM_BUFFER_SIZE;

	// calculate memory sizes
	vector<double> Mem(n);
	for (int i=0; i < n; i++)
	{
		Mem[i] = (array[i]->Margin() > 0) ?
			(double)array[i]->MarginSize() : 0.0;
	}

	// compute ratio
	double sum = 0;
	for (int i=0; i < n; i++) sum += Mem[i];
	if (sum > 0)
	{
		sum = 1.0 / sum;
		for (int i=0; i < n; i++) Mem[i] *= sum;
	} else {
		for (int i=0; i < n; i++) Mem[i] = 0;
	}

	// reallocate buffer
	for (int i=0; i < n; i++)
	{
		if (Mem[i] > 0)
		{
			C_Int64 size = (C_Int64)(buffer_size * Mem[i]);
			array[i]->AllocBuffer(size);
		}
	}
}

void CoreArray::Balance_ArrayRead_Buffer(CdArrayRead array[], int n,
	C_Int64 buffer_size)
{
	vector<CdArrayRead*> list(n);
	for (int i=0; i < n; i++)
		list[i] = &array[i];
	Balance_ArrayRead_Buffer(&list[0], n, buffer_size);
}
