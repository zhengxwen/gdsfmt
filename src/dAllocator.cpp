// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dAllocator.cpp: Storage allocation
//
// Copyright (C) 2007 - 2014	Xiuwen Zheng
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

#include <dAllocator.h>
#include <ctype.h>
#include <limits>

#ifndef COREARRAY_NO_STD_IN_OUT
#	include <iostream>
#endif


using namespace std;
using namespace CoreArray;


// =====================================================================
// Allocator
// =====================================================================

static const char *ERR_NOT_INIT = "'CdAllocator' has not been initialized.";

// Initial allocator functions

void CdAllocator::_InitFree(CdAllocator &Obj)
	{ }
SIZE64 CdAllocator::_InitGetSize(CdAllocator &Obj)
	{ throw ErrAllocator(ERR_NOT_INIT); }
void CdAllocator::_InitSetSize(CdAllocator &Obj, SIZE64 NewSize)
	{ throw ErrAllocator(ERR_NOT_INIT); }
SIZE64 CdAllocator::_InitGetPos(CdAllocator &Obj)
	{ throw ErrAllocator(ERR_NOT_INIT); }
void CdAllocator::_InitSetPos(CdAllocator &Obj, SIZE64 NewPos)
	{ throw ErrAllocator(ERR_NOT_INIT); }
void CdAllocator::_InitRead(CdAllocator &Obj, void *Buffer, ssize_t Count)
	{ throw ErrAllocator(ERR_NOT_INIT); }
C_UInt8 CdAllocator::_InitR8b(CdAllocator &Obj)
	{ throw ErrAllocator(ERR_NOT_INIT); }
C_UInt16 CdAllocator::_InitR16b(CdAllocator &Obj)
	{ throw ErrAllocator(ERR_NOT_INIT); }
C_UInt32 CdAllocator::_InitR32b(CdAllocator &Obj)
	{ throw ErrAllocator(ERR_NOT_INIT); }
C_UInt64 CdAllocator::_InitR64b(CdAllocator &Obj)
	{ throw ErrAllocator(ERR_NOT_INIT); }
void CdAllocator::_InitWrite(CdAllocator &Obj, const void *Buffer, ssize_t Count)
	{ throw ErrAllocator(ERR_NOT_INIT); }
void CdAllocator::_InitW8b(CdAllocator &Obj, C_UInt8 val)
	{ throw ErrAllocator(ERR_NOT_INIT); }
void CdAllocator::_InitW16b(CdAllocator &Obj, C_UInt16 val)
	{ throw ErrAllocator(ERR_NOT_INIT); }
void CdAllocator::_InitW32b(CdAllocator &Obj, C_UInt32 val)
	{ throw ErrAllocator(ERR_NOT_INIT); }
void CdAllocator::_InitW64b(CdAllocator &Obj, C_UInt64 val)
	{ throw ErrAllocator(ERR_NOT_INIT); }

// CdBufStream allocator functions

void CdAllocator::_BufFree(CdAllocator &Obj)
{
	if (Obj._BufStream)
	{
		Obj._BufStream->Release();
		Obj._BufStream = NULL;
	}
}

SIZE64 CdAllocator::_BufGetSize(CdAllocator &Obj)
	{ return Obj._BufStream->GetSize(); }
void CdAllocator::_BufSetSize(CdAllocator &Obj, SIZE64 NewSize)
	{ Obj._BufStream->SetSize(NewSize); }
SIZE64 CdAllocator::_BufGetPos(CdAllocator &Obj)
	{ return Obj._BufStream->Position(); }
void CdAllocator::_BufSetPos(CdAllocator &Obj, SIZE64 NewPos)
	{ Obj._BufStream->SetPosition(NewPos); }
void CdAllocator::_BufRead(CdAllocator &Obj, void *Buffer, ssize_t Count)
	{ Obj._BufStream->ReadData(Buffer, Count); }
C_UInt8 CdAllocator::_BufR8b(CdAllocator &Obj)
	{ return Obj._BufStream->R8b(); }
C_UInt16 CdAllocator::_BufR16b(CdAllocator &Obj)
	{ return Obj._BufStream->R16b(); }
C_UInt32 CdAllocator::_BufR32b(CdAllocator &Obj)
	{ return Obj._BufStream->R32b(); }
C_UInt64 CdAllocator::_BufR64b(CdAllocator &Obj)
	{ return Obj._BufStream->R64b(); }
void CdAllocator::_BufWrite(CdAllocator &Obj, const void *Buffer, ssize_t Count)
	{ Obj._BufStream->WriteData(Buffer, Count); }
void CdAllocator::_BufW8b(CdAllocator &Obj, C_UInt8 val)
	{ Obj._BufStream->W8b(val); }
void CdAllocator::_BufW16b(CdAllocator &Obj, C_UInt16 val)
	{ Obj._BufStream->W16b(val); }
void CdAllocator::_BufW32b(CdAllocator &Obj, C_UInt32 val)
	{ Obj._BufStream->W32b(val); }
void CdAllocator::_BufW64b(CdAllocator &Obj, C_UInt64 val)
	{ Obj._BufStream->W64b(val); }

// Read-Only or Write-Only

SIZE64 CdAllocator::_NoGetSize(CdAllocator &Obj)
	{ throw ErrAllocRead(); }
void CdAllocator::_NoSetSize(CdAllocator &Obj, SIZE64 NewSize)
	{ throw ErrAllocWrite(); }
void CdAllocator::_NoRead(CdAllocator &Obj, void *Buffer, ssize_t Count)
	{ throw ErrAllocRead(); }
C_UInt8 CdAllocator::_NoR8b(CdAllocator &Obj)
	{ throw ErrAllocRead(); }
C_UInt16 CdAllocator::_NoR16b(CdAllocator &Obj)
	{ throw ErrAllocRead(); }
C_UInt32 CdAllocator::_NoR32b(CdAllocator &Obj)
	{ throw ErrAllocRead(); }
C_UInt64 CdAllocator::_NoR64b(CdAllocator &Obj)
	{ throw ErrAllocRead(); }
void CdAllocator::_NoWrite(CdAllocator &Obj, const void *Buffer, ssize_t Count)
	{ throw ErrAllocWrite(); }
void CdAllocator::_NoW8b(CdAllocator &Obj, C_UInt8 val)
	{ throw ErrAllocWrite(); }
void CdAllocator::_NoW16b(CdAllocator &Obj, C_UInt16 val)
	{ throw ErrAllocWrite(); }
void CdAllocator::_NoW32b(CdAllocator &Obj, C_UInt32 val)
	{ throw ErrAllocWrite(); }
void CdAllocator::_NoW64b(CdAllocator &Obj, C_UInt64 val)
	{ throw ErrAllocWrite(); }


CdAllocator::CdAllocator()
{
	_BufStream = NULL;
	Initialize();
}

CdAllocator::~CdAllocator()
{
	(*_Free)(*this);
}

void CdAllocator::Initialize()
{
	_Free    = _InitFree;
	_GetSize = _InitGetSize;
	_SetSize = _InitSetSize;
	_GetPos  = _InitGetPos;
	_SetPos  = _InitSetPos;
	_Read    = _InitRead;
	_R8b     = _InitR8b;
	_R16b    = _InitR16b;
	_R32b    = _InitR32b;
	_R64b    = _InitR64b;
	_Write   = _InitWrite;
	_W8b     = _InitW8b;
	_W16b    = _InitW16b;
	_W32b    = _InitW32b;
	_W64b    = _InitW64b;
}

void CdAllocator::Initialize(CdStream &Stream, bool CanRead, bool CanWrite)
{
	(*_Free)(*this);
	Initialize();

	_BufStream = new CdBufStream(&Stream);
	_BufStream->AddRef();

	_Free = _BufFree;
	_GetPos  = _BufGetPos;
	_SetPos  = _BufSetPos;

	if (CanRead)
	{
		_GetSize = _BufGetSize;
		_Read = _BufRead;
		_R8b  = _BufR8b;  _R16b = _BufR16b;
		_R32b = _BufR32b; _R64b = _BufR64b;
	} else {
		_GetSize = _NoGetSize;
		_Read = _NoRead;
		_R8b  = _NoR8b;  _R16b = _NoR16b;
		_R32b = _NoR32b; _R64b = _NoR64b;
	}
	if (CanWrite)
	{
		_SetSize = _BufSetSize;
		_Write = _BufWrite;
		_W8b   = _BufW8b;  _W16b  = _BufW16b;
		_W32b  = _BufW32b; _W64b  = _BufW64b;
	} else {
		_SetSize = _NoSetSize;
		_Write = _NoWrite;
		_W8b   = _NoW8b;  _W16b  = _NoW16b;
		_W32b  = _NoW32b; _W64b  = _NoW64b;
	}
}

void CdAllocator::Free()
{
	(*_Free)(*this);
	Initialize();
	_BufStream = NULL;
}

void CdAllocator::Move(SIZE64 Src, SIZE64 Dst, SIZE64 Size)
{
	if ((Size > 0) && (Src != Dst))
	{
		C_UInt8 Buf[COREARRAY_STREAM_BUFFER];

		if ((Src>Dst) || (Src+Size<=Dst))
		{
			while (Size > 0)
			{
				ssize_t L = (Size >= (SIZE64)sizeof(Buf)) ?
					(ssize_t)sizeof(Buf) : Size;
				// Read
				SetPosition(Src); ReadData(Buf, L);
				// Write
				SetPosition(Dst); WriteData(Buf, L);
				// Iterate
				Src += L; Dst += L;
				Size -= L;
            }
		} else {
			Src += Size; Dst += Size;
			while (Size > 0)
			{
				ssize_t L = (Size >= (SIZE64)sizeof(Buf)) ?
					(ssize_t)sizeof(Buf) : Size;
				// Iterate
				Src -= L; Dst -= L; Size -= L;
				// Read
				SetPosition(Src); ReadData(Buf, L);
				// Write
				SetPosition(Dst); WriteData(Buf, L);
            }
		}
	}
}

void CdAllocator::ZeroFill(SIZE64 Size)
{
	C_UInt8 Buffer[4096];
	const ssize_t size = (Size <= (int)sizeof(Buffer)) ? Size : sizeof(Buffer);
	memset(Buffer, 0, size);

	while (Size > 0)
	{
		ssize_t n = (Size <= (int)sizeof(Buffer)) ? Size : sizeof(Buffer);
		WriteData(Buffer, n);
		Size -= n;
	}
}

void CdAllocator::ZeroFill(SIZE64 Pos, SIZE64 Size)
{
	SetPosition(Pos);

	C_UInt8 Buffer[4096];
	const ssize_t size = (Size <= (int)sizeof(Buffer)) ? Size : sizeof(Buffer);
	memset(Buffer, 0, size);

	while (Size > 0)
	{
		ssize_t n = (Size <= (int)sizeof(Buffer)) ? Size : sizeof(Buffer);
		WriteData(Buffer, n);
		Size -= n;
	}
}

void CdAllocator::CopyTo(CdBufStream &Obj, SIZE64 Pos, SIZE64 Count)
{
	C_UInt8 Buffer[COREARRAY_STREAM_BUFFER];
	if (Count < 0)
	{
		SetPosition(0);
		Count = GetSize();
	} else
		SetPosition(Pos);

	while (Count > 0)
	{
		ssize_t N = (Count >= (SIZE64)sizeof(Buffer)) ?
			(ssize_t)sizeof(Buffer) : Count;
		ReadData(Buffer, N);
		Obj.WriteData(Buffer, N);
		Count -= N;
	}
}



// =====================================================================
// Exception for Allocator
// =====================================================================

static const char *ERR_READ_ONLY =
	"The current allocator is in the Read-Only mode!";
static const char *ERR_WRITE_ONLY =
	"The current Allocator is in the Write-Only mode!";

ErrAllocRead::ErrAllocRead(): ErrAllocator(ERR_READ_ONLY)
{ }

ErrAllocWrite::ErrAllocWrite(): ErrAllocator(ERR_WRITE_ONLY)
{ }
