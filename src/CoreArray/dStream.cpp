// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dStream.cpp: Stream classes and functions
//
// Copyright (C) 2007-2016    Xiuwen Zheng
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

#include "dStream.h"
#include <cctype>
#include <limits>

#ifndef COREARRAY_NO_STD_IN_OUT
#   include <iostream>
#endif


using namespace std;
using namespace CoreArray;


// =====================================================================
// CdHandleStream

CdHandleStream::CdHandleStream(): CdStream()
{
	fHandle = NullSysHandle;
}

CdHandleStream::CdHandleStream(TSysHandle AHandle): CdStream()
{
	fHandle = AHandle;
}

ssize_t CdHandleStream::Read(void *Buffer, ssize_t Count)
{
	if (Count > 0)
		return SysHandleRead(fHandle, Buffer, Count);
	else
		return 0;
}

ssize_t CdHandleStream::Write(const void *Buffer, ssize_t Count)
{
	if (Count > 0)
		return SysHandleWrite(fHandle, Buffer, Count);
	else
		return 0;
}

SIZE64 CdHandleStream::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	SIZE64 rv = SysHandleSeek(fHandle, Offset, Origin);

	if (rv < 0)
	{
		RaiseLastOSError<ErrOSError>();
		return 0;
	} else
		return rv;
}

void CdHandleStream::SetSize(SIZE64 NewSize)
{
	if (!SysHandleSetSize(fHandle, NewSize))
    	RaiseLastOSError<ErrOSError>();
}


// =====================================================================
// CdFileStream

static const char *ErrFileCreate = "Can not create file '%s'. %s";
static const char *ErrFileOpen = "Can not open file '%s'. %s";
static TSysShareMode ShMode[4] = { saNone, saRead, saNone, saNone };

CdFileStream::CdFileStream(const char * const AFileName, TdOpenMode Mode):
	CdHandleStream()
{
	Init(AFileName, Mode);
}

CdFileStream::~CdFileStream()
{
	if (fHandle != NullSysHandle)
	{
		if (!SysCloseHandle(fHandle))
			RaiseLastOSError<ErrOSError>();
	}
}

void CdFileStream::Init(const char * const AFileName, TdOpenMode mode)
{
	if (mode == fmCreate)
	{
		fHandle = SysCreateFile(AFileName, 0);
		if (fHandle == NullSysHandle)
			throw ErrStream(ErrFileCreate, AFileName, LastSysErrMsg().c_str());
	} else{
		fHandle = SysOpenFile(AFileName, (TSysOpenMode)(mode-fmOpenRead),
			ShMode[mode]);
		if (fHandle == NullSysHandle)
			throw ErrStream(ErrFileOpen, AFileName, LastSysErrMsg().c_str());
	}

	fFileName = AFileName;
	fMode = mode;
}


// =====================================================================
// File stream for forked processes

CdForkFileStream::CdForkFileStream(const char * const AFileName,
	TdOpenMode Mode): CdFileStream()
{
#ifdef COREARRAY_PLATFORM_UNIX
	Current_PID = getpid();
#endif

	if (Mode == fmCreate)
		throw ErrStream("Not support create a file in a forked process.");
	Init(AFileName, Mode);
}

ssize_t CdForkFileStream::Read(void *Buffer, ssize_t Count)
{
	RedirectFile();
	return CdFileStream::Read(Buffer, Count);
}

ssize_t CdForkFileStream::Write(const void *Buffer, ssize_t Count)
{
	RedirectFile();
	return CdFileStream::Write(Buffer, Count);
}

SIZE64 CdForkFileStream::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	RedirectFile();
	return CdFileStream::Seek(Offset, Origin);
}

SIZE64 CdForkFileStream::GetSize()
{
	RedirectFile();
	return CdFileStream::GetSize();
}

void CdForkFileStream::SetSize(SIZE64 NewSize)
{
	RedirectFile();
	CdFileStream::SetSize(NewSize);
}

COREARRAY_INLINE void CdForkFileStream::RedirectFile()
{
#ifdef COREARRAY_PLATFORM_UNIX
	if (Current_PID != getpid())
	{
		Current_PID = getpid();
		SIZE64 p = 0;
		if (fHandle != NullSysHandle)
		{
			p = Position();
			SysCloseHandle(fHandle);
		}
		Init(fFileName.c_str(), fMode);
		SetPosition(p);
	}
#endif
}


// =====================================================================
// CdTempStream

CdTempStream::CdTempStream(): CdFileStream(
	TempFileName("tmp", ".").c_str(), CdFileStream::fmCreate)
{ }

CdTempStream::CdTempStream(const char *Path): CdFileStream(
	TempFileName("tmp", Path).c_str(), CdFileStream::fmCreate)
{ }

CdTempStream::~CdTempStream()
{
	if (fHandle != NullSysHandle)
	{
		if (!SysCloseHandle(fHandle))
		{
			fHandle = NullSysHandle;
			RaiseLastOSError<ErrOSError>();
        }
		fHandle = NullSysHandle;
		remove(fFileName.c_str());
	}
}


// =====================================================================
// CdMemoryStream

CdMemoryStream::CdMemoryStream(size_t Size): CdStream()
{
	fBuffer = NULL;
	fCapacity = fPosition = 0;
	SetSize(Size);
}

CdMemoryStream::~CdMemoryStream()
{
	if (fBuffer)
	{
		free(fBuffer);
		fBuffer = NULL;
	}
}

ssize_t CdMemoryStream::Read(void *Buffer, ssize_t Count)
{
	if (Count <= 0) return 0;
	if ((fPosition + Count) > fCapacity)
	{
		Count = fCapacity - fPosition;
		if (Count <= 0) return 0;
	}
	memmove(Buffer, (const C_UInt8*)fBuffer + fPosition, Count);
	fPosition += Count;
	return Count;
}

ssize_t CdMemoryStream::Write(const void *Buffer, ssize_t Count)
{
	if (Count <= 0) return 0;
	if ((fPosition + Count) > fCapacity)
		SetSize(fPosition + Count);
	memmove((C_UInt8*)fBuffer + fPosition, Buffer, Count);
	fPosition += Count;
	return Count;
}

SIZE64 CdMemoryStream::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	switch (Origin)
	{
		case soBeginning:
			fPosition = Offset; break;
		case soCurrent:
        	fPosition += Offset; break;
		case soEnd:
        	fPosition = fCapacity + Offset;
			break;
		default:
			return -1;
	}
	if ((fPosition < 0) || (fPosition > fCapacity))
		throw ErrStream("Invalid position (%d) in %s.", fPosition, __func__);
	return fPosition;
}

SIZE64 CdMemoryStream::GetSize()
{
	return fCapacity;
}

void CdMemoryStream::SetSize(SIZE64 NewSize)
{
	if (NewSize < 0) NewSize = 0;
	if (NewSize != fCapacity)
	{
		if (NewSize > 0)
		{
			fBuffer = realloc(fBuffer, NewSize);
			if (fBuffer == NULL)
				throw ErrStream("No enough memory in CdMemoryStream.");
			fCapacity = NewSize;
			if (fPosition > fCapacity)
				fPosition = fCapacity;
		} else {
			free(fBuffer);
			fBuffer = NULL;
			fCapacity = fPosition = 0;
		}
	}
}

void *CdMemoryStream::BufPointer()
{
	return fBuffer;
}


// =====================================================================
// CdStdInStream

#ifndef COREARRAY_NO_STD_IN_OUT

CdStdInStream::CdStdInStream(): CdStream()
{ }

CdStdInStream::~CdStdInStream()
{ }

ssize_t CdStdInStream::Read(void *Buffer, ssize_t Count)
{
	cin.read((char*)Buffer, Count);
	return Count;
}

ssize_t CdStdInStream::Write(const void *Buffer, ssize_t Count)
{
	throw ErrStream("Invalid CdStdInStream::Write.");
}

SIZE64 CdStdInStream::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	return 0;
}

SIZE64 CdStdInStream::GetSize()
{
	return 0;
}

void CdStdInStream::SetSize(SIZE64 NewSize)
{
	throw ErrStream("Invalid CdStdInStream::SetSize.");
}

// CdStdOutStream

CdStdOutStream::CdStdOutStream(): CdStream()
{ }

CdStdOutStream::~CdStdOutStream()
{ }

ssize_t CdStdOutStream::Read(void *Buffer, ssize_t Count)
{
	throw ErrStream("Invalid CdStdOutStream::Read.");
}

ssize_t CdStdOutStream::Write(const void *Buffer, ssize_t Count)
{
	cout.write((const char*)Buffer, Count);
	return Count;
}

SIZE64 CdStdOutStream::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	return 0;
}

SIZE64 CdStdOutStream::GetSize()
{
	return 0;
}

void CdStdOutStream::SetSize(SIZE64 NewSize)
{
	throw ErrStream("Invalid CdStdOutStream::SetSize.");
}

#endif


// =====================================================================
// The classes of transformed stream

// CdRecodeStream

CdRecodeStream::CdRecodeStream(CdStream &vStream): CdStream()
{
	fStream = &vStream;
	fStream->AddRef();
	fStreamBase = fStreamPos = vStream.Position();
	fTotalIn = fTotalOut = 0;
}

CdRecodeStream::~CdRecodeStream()
{
	if (fStream)
		fStream->Release();
}

inline void CdRecodeStream::UpdateStreamPosition()
{
	if (fStream->Position() != fStreamPos)
		fStream->SetPosition(fStreamPos);
}


CdRecodeLevel::CdRecodeLevel(CdRecodeStream::TLevel level)
{
	if ((level < CdRecodeStream::clMin) || (level > CdRecodeStream::clMax))
		throw ErrRecodeStream("Invalid compression level: %d", (int)level);

	fLevel = level;
}



// =====================================================================
// Algorithm of random access

// CdRandomAccessStream

static const ssize_t SIZE_RA_BLOCK_HEADER = 7;

static const int RA_BLOCK_SIZE_LIST[10] =
{
	16*1024,   32*1024,       64*1024,    128*1024,    256*1024,
	512*1024,  1024*1024, 2*1024*1024, 4*1024*1024, 8*1024*1024
};

CdRAAlgorithm::CdRAAlgorithm(CdRecodeStream &owner):
	fOwner(owner)
{
	fSizeType = raUnknown;
}


// CdRA_Read

CdRA_Read::CdRA_Read(CdRecodeStream *owner):
	CdRAAlgorithm(*owner)
{
	fVersion = 0;
	fBlockNum = fBlockIdx = 0;
	fCB_ZStart = fCB_ZSize = 0;
	fCB_UZStart = fCB_UZSize = 0;
	fBlockListStart = 0;
	fIndexingStart = 0;
	fIndex = NULL;
	fIndexSize = 0;
}

CdRA_Read::~CdRA_Read()
{
	if (fIndex) delete []fIndex;
}

void CdRA_Read::InitReadStream()
{
	static const char ErrUnsupport[] =
		"Unsupported stream version v%d.%d, you might need to upgrade the library";
	static const char ErrBlkNum[] =
		"the number of compression blocks should be defined.";

	// get the base position
	fOwner.fStreamBase = fOwner.fStream->Position();

	// read and check the magic number
	if (!ReadMagicNumber(*fOwner.fStream))
		throw ErrRecodeStream("Invalid stream header with random access.");

	// get the algorithm version
	fVersion = fOwner.fStream->R8b();
	if ((fVersion != 0x10) && (fVersion != 0x11))
	{
		throw ErrStream(ErrUnsupport, fVersion >> 4, fVersion & 0x0F);
	}
	// get size type
	C_Int8 b = fOwner.fStream->R8b();
	if ((b < raFirst) || (b > raLast)) b = raUnknown;
	fSizeType = (TBlockSize)b;

	// get the number of independent blocks
	BYTE_LE<CdStream>(fOwner.fStream) >> fBlockNum;
	fBlockListStart = fOwner.fStreamPos = fOwner.fStream->Position();
	// avoid integer overflow
	if ((fBlockNum >= 0) && (fBlockNum < 0x7FFFFFFF))
	{
		const size_t n = (size_t)fBlockNum + 1;
		fIndex = new TIndex[n];
		memset(fIndex, 0, sizeof(TIndex)*n);
	} else
		throw ErrStream(ErrBlkNum);

	// initialize the first compressed block
	fBlockIdx = 0;
	fCB_UZStart = fIndex[0].RawStart = 0;
	fIndexSize = 0;

	if (fVersion == 0x10)
	{
		// there is no pre-defined block information
		fCB_ZStart  = fIndex[0].CmpStart = fBlockListStart;
		if (fBlockNum > 0)
		{
			GetBlockHeader_v1_0();
			fIndex[1].RawStart = fIndex[0].RawStart + fCB_UZSize;
			fIndex[1].CmpStart = fIndex[0].CmpStart + fCB_ZSize;
			fIndexSize = 1;
		}
	} else if (fVersion == 0x11)
	{
		// pre-defined block information is stored after compressed data blocks
		TdGDSPos Len;
		BYTE_LE<CdStream>(fOwner.fStream) >> Len;
		fOwner.fStreamPos += GDS_POS_SIZE;
		fCB_ZStart = fIndex[0].CmpStart = fBlockListStart = fOwner.fStreamPos;
		fIndexingStart = fBlockListStart + Len;
		// read indexing information
		LoadIndexing();
		// set sizes
		if (fBlockNum >= 1)
		{
			fCB_UZSize = fIndex[1].RawStart - fIndex[0].RawStart;
			fCB_ZSize = fIndex[1].CmpStart - fIndex[0].CmpStart;
		} else {
			fCB_UZSize = fCB_ZSize = 0;
		}
	} else
		throw ErrStream(ErrUnsupport, fVersion >> 4, fVersion & 0x0F);
}

bool CdRA_Read::SeekStream(SIZE64 Position)
{
	if (Position < 0)
	{
		throw ErrStream(
			"'Seek' out of the range: position (%lld) should be >= 0.",
			Position);
	}

	if (Position < fCB_UZStart)
	{
		BinSearch(Position, 0, fBlockIdx-1);
		return true;

	} else if (Position >= (fCB_UZStart + fCB_UZSize))
	{
		if (Position >= fIndex[fIndexSize].RawStart)
		{
			if (fVersion == 0x10)
			{
				// no target, go to the last
				fBlockIdx = fIndexSize - 1;
				TIndex *p = fIndex + fBlockIdx;
				fCB_UZStart = p[0].RawStart;
				fCB_UZSize  = p[1].RawStart - p[0].RawStart;
				fCB_ZStart  = p[0].CmpStart;
				fCB_ZSize   = p[1].CmpStart - p[0].CmpStart;

				while (Position >= (fCB_UZStart + fCB_UZSize))
				{
					// go to the next block
					if (!NextBlock())
					{
						if (Position > fCB_UZStart)
						{
							throw ErrStream(
								"'Seek' out of the range with position (%lld).",
								Position);
						}
					}
				}
			} else {
				throw ErrStream(
					"'Seek' out of the range with position (%lld).",
					Position);
			}
		} else {
			BinSearch(Position, fBlockIdx+1, fIndexSize-1);
		}
		return true;
	}

	return false;
}

void CdRA_Read::GetUpdated()
{
	if (fIndexSize < fBlockNum)
	{
		while (NextBlock());
	}
}

void CdRA_Read::GetBlockInfo(vector<SIZE64> &RawSize, vector<SIZE64> &CmpSize)
{
	GetUpdated();
	RawSize.resize(fIndexSize);
	CmpSize.resize(fIndexSize);
	for (ssize_t i=0; i < fIndexSize; i++)
	{
		RawSize[i] = fIndex[i+1].RawStart - fIndex[i].RawStart;
		CmpSize[i] = fIndex[i+1].CmpStart - fIndex[i].CmpStart;
	}
}

bool CdRA_Read::NextBlock()
{
	fCB_ZStart += fCB_ZSize;
	fCB_UZStart += fCB_UZSize;
	fBlockIdx ++;
	if (fBlockIdx < fBlockNum)
	{
		if (fBlockIdx < fIndexSize)
		{
			fOwner.fStreamPos = fCB_ZStart;
			if (fVersion == 0x10)
				 fOwner.fStreamPos += SIZE_RA_BLOCK_HEADER;
			TIndex *p = fIndex + fBlockIdx;
			fCB_ZSize = p[1].CmpStart - p[0].CmpStart;
			fCB_UZSize = p[1].RawStart - p[0].RawStart;
		} else {
			GetBlockHeader_v1_0();
			fIndexSize = fBlockIdx + 1;
			TIndex *p = fIndex + fIndexSize;
			p->RawStart = fCB_UZStart + fCB_UZSize;
			p->CmpStart = fCB_ZStart + fCB_ZSize;
		}
		return true;
	} else {
		fCB_ZSize = fCB_UZSize = 1; // avoid ZERO
		return false;
	}
}

void CdRA_Read::BinSearch(SIZE64 Position, ssize_t low, ssize_t high)
{
	TIndex *p = fIndex;
	while (low < high)
	{
		ssize_t mid = low + ((high - low) >> 1);
		if (Position >= p[mid].RawStart)
		{
			if (Position >= p[mid+1].RawStart)
				low = mid + 1;
			else
				high = low = mid;
		} else {
			high = mid - 1;
		}
	}

	// find the target
	fBlockIdx = low;
	p = fIndex + low;
	fCB_UZStart = p[0].RawStart;
	fCB_UZSize  = p[1].RawStart - p[0].RawStart;
	fCB_ZStart  = p[0].CmpStart;
	fCB_ZSize   = p[1].CmpStart - p[0].CmpStart;
}

void CdRA_Read::GetBlockHeader_v1_0()
{
	C_UInt8 BSZ[SIZE_RA_BLOCK_HEADER];
	fOwner.fStream->SetPosition(fCB_ZStart);
	fOwner.fStream->ReadData(BSZ, SIZE_RA_BLOCK_HEADER);
	fOwner.fStreamPos = fCB_ZStart + SIZE_RA_BLOCK_HEADER;

	fCB_ZSize = BSZ[0] | (C_UInt32(BSZ[1]) << 8) |
		(C_UInt32(BSZ[2]) << 16);
	fCB_UZSize = BSZ[3] | (C_UInt32(BSZ[4]) << 8) |
		(C_UInt32(BSZ[5]) << 16) | (C_UInt32(BSZ[6]) << 24);
}

void CdRA_Read::LoadIndexing()
{
	if (fIndexSize <= 0)
	{
		fOwner.fStream->SetPosition(fIndexingStart);
		C_UInt8 BSZ[SIZE_RA_BLOCK_HEADER];
		TIndex *p = fIndex;
		for (ssize_t i=0; i < fBlockNum; i++)
		{
			fOwner.fStream->ReadData(BSZ, SIZE_RA_BLOCK_HEADER);
			C_UInt32 CmpLen = BSZ[0] | (C_UInt32(BSZ[1]) << 8) |
				(C_UInt32(BSZ[2]) << 16);
			C_UInt32 RawLen = BSZ[3] | (C_UInt32(BSZ[4]) << 8) |
				(C_UInt32(BSZ[5]) << 16) | (C_UInt32(BSZ[6]) << 24);
			TIndex *n = p + 1;
			n->RawStart = p->RawStart + RawLen;
			n->CmpStart = p->CmpStart + CmpLen;
			p ++;
		}
		fIndexSize = fBlockNum;
		fOwner.fStream->SetPosition(fOwner.fStreamPos);
	}
}


// CdRA_Write

CdRA_Write::CdRA_Write(CdRecodeStream *owner, TBlockSize bs):
	CdRAAlgorithm(*owner)
{
	if ((bs < raFirst) || (bs > raLast))
		throw EZLibError("%s: invalid block size.", __func__);
	fVersion = 0x11;  // by default
	fBlockNum = 0;
	fCB_ZStart = fCB_UZStart = 0;
	fBlockListStart = 0;
	fHasInitWriteBlock = false;
}

void CdRA_Write::InitWriteStream()
{
	// get the base position
	fOwner.fStreamBase = fOwner.fStream->Position();

	// write the magic number
	WriteMagicNumber(*fOwner.fStream);
	// write the version of this algorithm, v1.0
	fOwner.fStream->W8b(fVersion);
	// write the parameter of block size
	fOwner.fStream->W8b(fSizeType);
	// write the number of independent blocks, -1 for unknown
	BYTE_LE<CdStream>(fOwner.fStream) << C_Int32(-1);

	// set values
	fBlockListStart = fOwner.fStreamPos = fOwner.fStream->Position();
	// version
	if (fVersion == 0x11)
	{
		BYTE_LE<CdStream>(fOwner.fStream) << TdGDSPos(0);
		fOwner.fStreamPos += GDS_POS_SIZE;
		fBlockListStart = fOwner.fStreamPos;
	}

	fOwner.fTotalOut = (fOwner.fStreamPos - fOwner.fStreamBase);
	fHasInitWriteBlock = false;
}

void CdRA_Write::DoneWriteStream()
{
	DoneWriteBlock();

	if (fVersion == 0x10)
	{
		// write the ending code, zero filling
		C_UInt64 Val = 0;
		fOwner.fStream->WriteData(&Val, SIZE_RA_BLOCK_HEADER);
		fOwner.fStreamPos += SIZE_RA_BLOCK_HEADER;
		fOwner.fTotalOut = fOwner.fStreamPos - fOwner.fStreamBase;
	}

	fHasInitWriteBlock = false;
	// save position
	SIZE64 OldPos = fOwner.fStreamPos;

	// write down the number of independent blocks
	if (fVersion == 0x10)
	{
		fOwner.fStream->SetPosition(fBlockListStart - sizeof(C_Int32));
		BYTE_LE<CdStream>(fOwner.fStream) << C_Int32(fBlockNum);
	} else if (fVersion == 0x11)
	{
		fOwner.fStream->SetPosition(fBlockListStart - sizeof(C_Int32) -
			GDS_POS_SIZE);
		BYTE_LE<CdStream>(fOwner.fStream) << C_Int32(fBlockNum) <<
			TdGDSPos(OldPos - fBlockListStart);
		// store indexing information
		fOwner.fStream->SetPosition(OldPos);
		for (ssize_t i=0; i < fBlockNum; i++)
		{
			C_UInt64 u = fBlockInfoList[i];
			C_UInt32 SC = u & 0xFFFFFFFF;
			C_UInt32 SU = u >> 32;
			C_UInt8 SZ[SIZE_RA_BLOCK_HEADER] =
			{
				C_UInt8(SC & 0xFF),
				C_UInt8((SC >> 8) & 0xFF),
				C_UInt8((SC >> 16) & 0xFF),
				C_UInt8(SU & 0xFF),
				C_UInt8((SU >> 8) & 0xFF),
				C_UInt8((SU >> 16) & 0xFF),
				C_UInt8((SU >> 24) & 0xFF)
			};
			fOwner.fStream->WriteData(SZ, SIZE_RA_BLOCK_HEADER);
		}
	}

	// reset stream position
	fOwner.fStream->SetPosition(fOwner.fStreamPos=OldPos);
}

void CdRA_Write::InitWriteBlock()
{
	if (!fHasInitWriteBlock)
	{
		fCB_ZStart = fOwner.fStreamPos;
		fCB_UZStart = fOwner.fTotalIn;
		if (fVersion == 0x10)
		{
			C_UInt64 Val = 0;
			fOwner.fStream->WriteData(&Val, SIZE_RA_BLOCK_HEADER);
			fOwner.fStreamPos += SIZE_RA_BLOCK_HEADER;
		}
		fHasInitWriteBlock = true;
	}
}

void CdRA_Write::DoneWriteBlock()
{
	if (fHasInitWriteBlock)
	{
		C_UInt32 SC = fOwner.fStreamPos - fCB_ZStart;
		C_UInt32 SU = fOwner.fTotalIn - fCB_UZStart;

		if (fVersion == 0x10)
		{
			// store indexing info together with the compression block
			C_UInt8 SZ[SIZE_RA_BLOCK_HEADER] =
			{
				C_UInt8(SC & 0xFF),
				C_UInt8((SC >> 8) & 0xFF),
				C_UInt8((SC >> 16) & 0xFF),
				C_UInt8(SU & 0xFF),
				C_UInt8((SU >> 8) & 0xFF),
				C_UInt8((SU >> 16) & 0xFF),
				C_UInt8((SU >> 24) & 0xFF)
			};
			fOwner.fStream->SetPosition(fCB_ZStart);
			fOwner.fStream->WriteData(SZ, SIZE_RA_BLOCK_HEADER);
			fOwner.fStream->SetPosition(fOwner.fStreamPos);
			fBlockNum ++;
		} else if (fVersion == 0x11)
		{
			// add indexing info to fBlockInfoList
			AddBlockInfo(SC, SU);
		}

		fHasInitWriteBlock = false;
	}
}

void CdRA_Write::AddBlockInfo(C_UInt32 CmpLen, C_UInt32 RawLen)
{
	if (fVersion == 0x11)
		fBlockInfoList.push_back(CmpLen | (C_UInt64(RawLen) << 32));
	fBlockNum ++;
}


// =====================================================================
// The classes of ZLIB stream

static const char *ErrZDeflateInvalid =
	"Invalid ZIP deflate stream operation '%s'!";
static const char *ErrZInflateInvalid =
	"Invalid ZIP inflate stream operation '%s'!";
static const char *ErrZDeflateClosed =
	"ZIP deflate stream has been closed.";

static short ZLevels[4] =
{
	Z_BEST_SPEED,           // clMin
	3,                      // clFast
	Z_DEFAULT_COMPRESSION,  // clDefault
	Z_BEST_COMPRESSION      // clMax
};

COREARRAY_INLINE static int ZCheck(int Code)
{
	if ((Code < 0) && (Code != Z_BUF_ERROR))
		throw EZLibError(Code);
	return Code;
}


// CdBaseZStream

CdBaseZStream::CdBaseZStream(CdStream &vStream): CdRecodeStream(vStream)
{
	memset((void*)&fZStream, 0, sizeof(z_stream));
}

// CdZEncoder

CdZEncoder::CdZEncoder(CdStream &Dest, TLevel Level):
	CdBaseZStream(Dest), CdRecodeLevel(Level)
{
	PtrExtRec = NULL;
	fZStream.next_out = fBuffer;
	fZStream.avail_out = sizeof(fBuffer);
	fHaveClosed = false;
	ZCheck( deflateInit_(&fZStream, ZLevels[Level],
		ZLIB_VERSION, sizeof(fZStream)) );
}

CdZEncoder::CdZEncoder(CdStream &Dest, TLevel Level, int windowBits,
	int memLevel, int Strategy): CdBaseZStream(Dest), CdRecodeLevel(Level)
{
	PtrExtRec = NULL;
	fZStream.next_out = fBuffer;
	fZStream.avail_out = sizeof(fBuffer);
	fHaveClosed = false;

	#define Z_DEFLATED 8
	ZCheck( deflateInit2_(&fZStream, ZLevels[Level],
		Z_DEFLATED, windowBits, memLevel, Strategy,
		ZLIB_VERSION, sizeof(fZStream)) );
	#undef Z_DEFLATED
}

CdZEncoder::~CdZEncoder()
{
	Close();
	deflateEnd(&fZStream);
}

ssize_t CdZEncoder::Read(void *Buffer, ssize_t Count)
{
	throw EZLibError(ErrZDeflateInvalid, "Read");
}

ssize_t CdZEncoder::Write(const void *Buffer, ssize_t Count)
{
	if (fHaveClosed)
		throw EZLibError(ErrZDeflateClosed);

	fZStream.next_in = (Bytef*)Buffer;
	fZStream.avail_in = Count;
	ssize_t L = fZStream.avail_in;

	while (fZStream.avail_in > 0)
	{
		ZCheck(deflate(&fZStream, Z_NO_FLUSH));
		fTotalIn += L - fZStream.avail_in;
		L = fZStream.avail_in;
		if (fZStream.avail_out <= 0)
		{
			UpdateStreamPosition();
			fStream->WriteData(fBuffer, sizeof(fBuffer));
			fStreamPos += sizeof(fBuffer);
			fTotalOut += sizeof(fBuffer);
			fZStream.next_out = fBuffer;
			fZStream.avail_out = sizeof(fBuffer);
		}
	}

	return Count;
}

SIZE64 CdZEncoder::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	switch (Origin)
	{
		case soBeginning:
			if (Offset == fTotalIn) return fTotalIn;
			break;
		case soCurrent:
			if (Offset == 0) return fTotalIn;
			break;
		case soEnd:
			if (Offset == 0) return fTotalIn;
			break;
	}
	throw EZLibError(ErrZDeflateInvalid, "Seek");
}

void CdZEncoder::SetSize(SIZE64 NewSize)
{
	if (NewSize != fTotalIn)
		throw EZLibError(ErrZDeflateInvalid, "SetSize");
}

void CdZEncoder::Close()
{
	if (!fHaveClosed)
	{
		if (PtrExtRec)
		{
			if (PtrExtRec->Size > 0)
				WriteData((void*)PtrExtRec->Buf, PtrExtRec->Size);
			PtrExtRec = NULL;
		}
		SyncFinish();
		fHaveClosed = true;
	}
}

ssize_t CdZEncoder::Pending()
{
	return sizeof(fBuffer) - fZStream.avail_out;
}

void CdZEncoder::SyncFinish()
{
	fZStream.next_in = NULL;
	fZStream.avail_in = 0;
	UpdateStreamPosition();

	while (ZCheck(deflate(&fZStream, Z_FINISH)) != Z_STREAM_END)
	{
		ssize_t L = sizeof(fBuffer) - fZStream.avail_out;
		fStream->WriteData(fBuffer, L);
		fStreamPos += L;
		fZStream.next_out = fBuffer;
		fZStream.avail_out = sizeof(fBuffer);
	}

	if (fZStream.avail_out < sizeof(fBuffer))
	{
		ssize_t L = sizeof(fBuffer) - fZStream.avail_out;
		fStream->WriteData(fBuffer, L);
		fStreamPos += L;
	}

	fTotalOut = fStreamPos - fStreamBase;
}


// CdZDecoder

CdZDecoder::CdZDecoder(CdStream &Source):
	CdBaseZStream(Source)
{
	fZStream.next_in = fBuffer;
	fZStream.avail_in = 0;
	fCurPosition = 0;
	ZCheck(inflateInit_(&fZStream, ZLIB_VERSION, sizeof(fZStream)));
}

CdZDecoder::CdZDecoder(CdStream &Source, int windowBits):
	CdBaseZStream(Source)
{
	fZStream.next_in = fBuffer;
	fZStream.avail_in = 0;
	fCurPosition = 0;
	ZCheck(inflateInit2_(&fZStream, windowBits, ZLIB_VERSION, sizeof(fZStream)));
}

CdZDecoder::~CdZDecoder()
{
	inflateEnd(&fZStream);
}

ssize_t CdZDecoder::Read(void *Buffer, ssize_t Count)
{
	fZStream.next_out = (Bytef*)Buffer;
	const ssize_t OriCount = Count;
	int ZResult = Z_OK;

	while ((Count>0) && (ZResult!=Z_STREAM_END))
	{
		if (fZStream.avail_in <= 0)
		{
			UpdateStreamPosition();
			fZStream.avail_in = fStream->Read(fBuffer, sizeof(fBuffer));
			fStreamPos += fZStream.avail_in;
			if (fZStream.avail_in <= 0)
				return OriCount - Count;
			fZStream.next_in = fBuffer;
		}

		fZStream.avail_out = Count;
		ZResult = ZCheck(inflate(&fZStream, Z_NO_FLUSH));

		ssize_t L = Count - fZStream.avail_out;
		fCurPosition += L;
		Count -= L;
	}

	if ((ZResult==Z_STREAM_END) && (fZStream.avail_in>0))
	{
		fStreamPos -= fZStream.avail_in;
		fStream->SetPosition(fStreamPos);
		fZStream.avail_in = 0;
	}

	SIZE64 tmp = fStreamPos - fStreamBase;
	if (tmp > fTotalIn) fTotalIn = tmp;
	if (fCurPosition > fTotalOut) fTotalOut = fCurPosition;

	return OriCount - Count;
}

ssize_t CdZDecoder::Write(const void *Buffer, ssize_t Count)
{
	throw EZLibError(ErrZInflateInvalid, "Write");
}

SIZE64 CdZDecoder::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	if ((Offset==0) && (Origin==soBeginning))
	{
		if (fCurPosition == 0) return 0;
		ZCheck(inflateReset(&fZStream));
		fZStream.next_in = fBuffer;
		fZStream.avail_in = 0;
		fStream->SetPosition(fStreamPos = fStreamBase);
		return (fCurPosition = 0);
	} else if (Origin != soEnd)
	{
		if ((Offset==0) && (Origin==soCurrent))
			return fCurPosition;

		if (Origin == soCurrent) Offset += fCurPosition;
		if (Offset < fCurPosition)
			Seek(0, soBeginning);
		else
			Offset -= fCurPosition;

		C_UInt8 buffer[4096];
		SIZE64 DivI = Offset / sizeof(buffer);
		for (; DivI > 0; DivI--)
			ReadData(buffer, sizeof(buffer));
		ReadData(buffer, Offset % sizeof(buffer));
	} else
		throw EZLibError(ErrZInflateInvalid, "Seek");

	return fCurPosition;
}

SIZE64 CdZDecoder::GetSize()
{
	return -1;
}

void CdZDecoder::SetSize(SIZE64 NewSize)
{
	throw EZLibError(ErrZInflateInvalid, "SetSize");
}


// =====================================================================
// Input stream for zlib with the support of random access
// =====================================================================

#define ZRA_MAGIC_HEADER_SIZE     4
static const C_UInt8 ZRA_MAGIC_HEADER[ZRA_MAGIC_HEADER_SIZE] =
	{ 'Z', 'R', 'A', 0x10 };

// windowBits<0, indicating generating raw deflate data without zlib header
//               or trailer
#define ZRA_WINDOW_BITS_16K    -11
#define ZRA_WINDOW_BITS_32K    -12
#define ZRA_WINDOW_BITS_64K    -13
#define ZRA_WINDOW_BITS_128K   -14
#define ZRA_WINDOW_BITS        -15

// See: http://www.zlib.net/ChangeLog.txt
#ifdef ZLIB_VERNUM
#   if (ZLIB_VERNUM > 0x1250)
#       define ZLIB_DEFLATE_PENDING    deflatePending
#   else
#       define ZLIB_DEFLATE_PENDING(z, p, b)    0
#   endif
#else
#   define ZLIB_DEFLATE_PENDING(z, p, b)    0
#endif


CdZEncoder_RA::CdZEncoder_RA(CdStream &Dest, TLevel Level,
	TBlockSize BK): CdRA_Write(this, BK),
	CdZEncoder( Dest, Level,
		 BK==ra16KB  ? ZRA_WINDOW_BITS_16K :
		(BK==ra32KB  ? ZRA_WINDOW_BITS_32K :
		(BK==ra64KB  ? ZRA_WINDOW_BITS_64K :
		(BK==ra128KB ? ZRA_WINDOW_BITS_128K : ZRA_WINDOW_BITS))) )
{
	fBlockZIPSize = fCurBlockZIPSize = RA_BLOCK_SIZE_LIST[BK];
	InitWriteStream();
}

ssize_t CdZEncoder_RA::Write(const void *Buffer, ssize_t Count)
{
	if (fHaveClosed)
		throw EZLibError(ErrZDeflateClosed);
	if (Count <= 0) return 0;

	ssize_t OldCount = Count;
	C_UInt8 *pBuf = (C_UInt8*)Buffer;

	while (Count > 0)
	{
		InitWriteBlock();

		fZStream.next_in = (Bytef*)pBuf;
		fZStream.avail_in = Count;

		while (fZStream.avail_in > 0)
		{
			ZCheck(deflate(&fZStream, Z_NO_FLUSH));
			ssize_t L = Count - fZStream.avail_in;
			fTotalIn += L; pBuf += L;
			Count = fZStream.avail_in;

			// whether it is the end of specified block
			if (fZStream.avail_out <= 0)
			{
				UpdateStreamPosition();
				fStream->WriteData(fBuffer, sizeof(fBuffer));
				fStreamPos += sizeof(fBuffer);
				fZStream.next_out = fBuffer;
				fZStream.avail_out = sizeof(fBuffer);
				fCurBlockZIPSize -= sizeof(fBuffer);

				unsigned pending = 0;
				int bits = 0;
				ZCheck(ZLIB_DEFLATE_PENDING(&fZStream, &pending, &bits));
				if (bits > 0) pending ++;

				if ((fCurBlockZIPSize - (int)pending) <= 0)
				{
					// finish this block
					// 'fZStream.avail_in = 0' in SyncFinishBlock()
					SyncFinishBlock();
				}
			}
		}
	}

	fTotalOut = fStreamPos - fStreamBase;
	return OldCount - Count;
}

void CdZEncoder_RA::Close()
{
	if (!fHaveClosed)
	{
		if (PtrExtRec)
		{
			if (PtrExtRec->Size > 0)
				WriteData((void*)PtrExtRec->Buf, PtrExtRec->Size);
			PtrExtRec = NULL;
		}
		SyncFinishBlock();
		DoneWriteStream();
		fHaveClosed = true;
	}
}

void CdZEncoder_RA::WriteMagicNumber(CdStream &Stream)
{
	Stream.WriteData(ZRA_MAGIC_HEADER, ZRA_MAGIC_HEADER_SIZE);
}

void CdZEncoder_RA::SyncFinishBlock()
{
	if (fHasInitWriteBlock)
	{
		SyncFinish();
		DoneWriteBlock();
		fCurBlockZIPSize = fBlockZIPSize;
		fZStream.next_out = fBuffer;
		fZStream.avail_out = sizeof(fBuffer);
		ZCheck(deflateReset(&fZStream));
	}
}

void CdZEncoder_RA::CopyFrom(CdStream &Source, SIZE64 Pos, SIZE64 Count)
{
	if (dynamic_cast<CdZDecoder_RA*>(&Source))
	{
		CdZDecoder_RA *Src = static_cast<CdZDecoder_RA*>(&Source);
		if ((Src->SizeType() == SizeType()) && (Src->fVersion == fVersion))
		{
			Src->SetPosition(Pos);
			if (Count < 0)
				Count = Src->GetSize() - Pos;

			C_UInt8 Buffer[COREARRAY_STREAM_BUFFER];
			const ssize_t SIZE = sizeof(Buffer);

			// header
			if (Pos > Src->fCB_UZStart)
			{
				SIZE64 L = Src->fCB_UZStart + Src->fCB_UZSize - Pos;
				if (L > Count) L = Count;
				for (; L > 0; )
				{
					ssize_t N = (L <= SIZE) ? L : SIZE;
					Src->ReadData(Buffer, N);
					WriteData((void*)Buffer, N);
					Count -= N; Pos += N;
					L -= N;
				}
			}

			// body
			if (Count > 0)
			{
				Src->SeekStream(Pos);
				if ((Src->fCB_UZStart + Src->fCB_UZSize) <= (Pos + Count))
				{
					SyncFinishBlock();
					// determine start and size
					SIZE64 Start = Src->fCB_ZStart;
					SIZE64 ZSize = 0, USize = 0;
					while ((Src->fCB_UZStart + Src->fCB_UZSize) <= (Pos + Count))
					{
						ZSize += Src->fCB_ZSize;
						USize += Src->fCB_UZSize;
						AddBlockInfo(Src->fCB_ZSize, Src->fCB_UZSize);
						Count -= Src->fCB_UZSize;
						Pos += Src->fCB_UZSize;
						Src->NextBlock();
					}
					Src->Reset();
					// copying
					fStream->CopyFrom(*Src->fStream, Start, ZSize);
					fTotalIn += USize;
					fStreamPos += ZSize;
					fTotalOut = fStreamPos - fStreamBase;
				}
			}

			// tail
			if (Count > 0) Src->SetPosition(Pos);
			for (; Count > 0; )
			{
				ssize_t N = (Count <= SIZE) ? Count : SIZE;
				Src->ReadData(Buffer, N);
				WriteData((void*)Buffer, N);
				Count -= N;
			}

			return;
		}
	}

	CdStream::CopyFrom(Source, Pos, Count);
}


// =====================================================================
// Output stream for zlib with the support of random access
// =====================================================================

CdZDecoder_RA::CdZDecoder_RA(CdStream &Source):
	CdRA_Read(this), CdZDecoder(Source, ZRA_WINDOW_BITS)
{
	InitReadStream();
}

ssize_t CdZDecoder_RA::Read(void *Buffer, ssize_t Count)
{
	if (Count <= 0) return 0;
	if (fBlockIdx >= fBlockNum) return 0;

	C_UInt8 *pBuf = (C_UInt8*)Buffer;
	ssize_t OldCount = Count;

	while (Count > 0)
	{
		fZStream.next_out = (Bytef*)pBuf;
		int ZResult = Z_OK;

		while ((Count>0) && (ZResult!=Z_STREAM_END))
		{
			if (fZStream.avail_in <= 0)
			{
				ssize_t s = fCB_ZStart + fCB_ZSize - fStreamPos;
				if (s > (ssize_t)sizeof(fBuffer))
					s = sizeof(fBuffer);
				if (s > 0)
				{
					fStream->SetPosition(fStreamPos);
					fZStream.avail_in = fStream->Read(fBuffer, s);
					if (fZStream.avail_in <= 0)
						return OldCount - Count;

					fStreamPos += fZStream.avail_in;
					fZStream.next_in = fBuffer;
				}
			}

			fZStream.avail_out = Count;
			ZResult = ZCheck(inflate(&fZStream, Z_NO_FLUSH));

			ssize_t L = Count - fZStream.avail_out;
			fCurPosition += L;
			Count -= L;
			pBuf += L;
		}

		if (ZResult == Z_STREAM_END)
		{
			if ((fCurPosition-fCB_UZStart) != fCB_UZSize)
				throw EZLibError("Invalid ZIP block, inconsistent length.");

			// go to the next block
			if (NextBlock())
			{
				fZStream.next_in = fBuffer;
				fZStream.avail_in = 0;
				ZCheck(inflateReset(&fZStream));
			} else
				break;
		}
	}

	SIZE64 tmp = fStreamPos - fStreamBase;
	if (tmp > fTotalIn) fTotalIn = tmp;
	if (fCurPosition > fTotalOut) fTotalOut = fCurPosition;

	return OldCount - Count;
}

SIZE64 CdZDecoder_RA::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	if (Origin == soCurrent)
	{
		Offset += fCurPosition;
		if (Offset < 0)
			throw EZLibError(ErrZInflateInvalid, "Seek");
	} else if (Origin == soEnd)
		throw EZLibError(ErrZInflateInvalid, "Seek");

	bool flag = SeekStream(Offset);
	if (flag || (Offset < fCurPosition))
		Reset();

	Offset -= fCurPosition;
	if (Offset > 0)
	{
		C_UInt8 buffer[4096];
		SIZE64 DivI = Offset / sizeof(buffer);
		for (; DivI > 0; DivI--)
			ReadData(buffer, sizeof(buffer));
		ReadData(buffer, Offset % sizeof(buffer));
	} else if (Offset < 0)
		throw EZLibError(ErrZInflateInvalid, "Seek");

	return fCurPosition;
}

bool CdZDecoder_RA::ReadMagicNumber(CdStream &Stream)
{
	C_UInt8 Header[ZRA_MAGIC_HEADER_SIZE];
	Stream.SetPosition(fStreamBase);
	Stream.ReadData(Header, sizeof(Header));
	return (memcmp(Header, ZRA_MAGIC_HEADER, ZRA_MAGIC_HEADER_SIZE) == 0);
}

void CdZDecoder_RA::Reset()
{
	fZStream.next_in = fBuffer;
	fZStream.avail_in = 0;
	ZCheck(inflateReset(&fZStream));
	fStreamPos = fCB_ZStart;
	if (fVersion == 0x10)
		fStreamPos += SIZE_RA_BLOCK_HEADER;
	fCurPosition = fCB_UZStart;
}

// EZLibError

EZLibError::EZLibError(int Code): ErrRecodeStream()
{
	fErrCode = Code;
	fMessage = zError(Code);
}



// =====================================================================
// The classes of LZ4 stream
// =====================================================================

//   64KB,    256KB,         1MB,         4MB
static const ssize_t LZ4ChunkSize[4] =
	{ 64*1024, 256*1024, 1*1024*1024, 4*1024*1024 };
static const int LZ4DeflateLevel[4] =
	{ 0, 2, 8, 16 };
static const LZ4F_blockSizeID_t LZ4FrameInfoBlockSize[4] =
	{ LZ4F_max64KB, LZ4F_max256KB, LZ4F_max1MB, LZ4F_max4MB };

static const char *ErrLZ4DeflateInvalid =
	"Invalid LZ4 Deflate Stream operation '%s'!";
static const char *ErrLZ4InflateInvalid =
	"Invalid LZ4 Inflate Stream operation '%s'!";
static const char *ErrLZ4DeflateClosed =
	"LZ4 deflate stream has been closed.";

CdBaseLZ4Stream::CdBaseLZ4Stream(CdStream &vStream):
	CdRecodeStream(vStream)
{ }


// Compression of LZ4 algorithm

CdLZ4Encoder::CdLZ4Encoder(CdStream &Dest, CdRecodeStream::TLevel level,
	TLZ4Chunk chunk): CdBaseLZ4Stream(Dest), CdRecodeLevel(level)
{
	if ((chunk < bsFirst) || (chunk > bsLast))
		throw ELZ4Error("CdLZ4Encoder::CdLZ4Encoder, invalid chunk.");
	fChunk = chunk;

	memset((void*)&lz4_pref, 0, sizeof(lz4_pref));
	lz4_pref.compressionLevel = LZ4DeflateLevel[level];
	lz4_pref.frameInfo.blockSizeID = LZ4FrameInfoBlockSize[chunk];
	lz4_pref.frameInfo.blockMode = blockLinked;
	lz4_pref.frameInfo.contentChecksumFlag = contentChecksumEnabled;

	lz4_context = NULL;
	LZ4F_errorCode_t err = LZ4F_createCompressionContext(&lz4_context, LZ4F_VERSION);
	if (LZ4F_isError(err)) throw ELZ4Error(err);

	fChunkSize = LZ4ChunkSize[chunk];
	fRawChunkSize = LZ4F_compressBound(fChunkSize, &lz4_pref);
	fCompress = new C_UInt8[fRawChunkSize];
	fChunkUsed = 0;
	fHaveClosed = false;

	size_t s = LZ4F_compressBegin(lz4_context, fCompress, fRawChunkSize, &lz4_pref);
	if (LZ4F_isError(s))
	{
		LZ4F_freeCompressionContext(lz4_context);
		lz4_context = NULL;
		throw ELZ4Error((LZ4F_errorCode_t)s);
	}

	// write the header
	fStream->SetPosition(fStreamBase);
	fStream->WriteData(fCompress, s);
	fStreamPos = fStreamBase + (int)s;
	fTotalOut = (fStreamPos - fStreamBase);
}

CdLZ4Encoder::~CdLZ4Encoder()
{
	Close();
	if (fCompress)
	{
		delete[] fCompress;
		fCompress = NULL;
	}
	if (lz4_context)
	{
		LZ4F_errorCode_t err = LZ4F_freeCompressionContext(lz4_context);
		lz4_context = NULL;
		if (LZ4F_isError(err)) throw ELZ4Error(err);
	}
}

ssize_t CdLZ4Encoder::Read(void *Buffer, ssize_t Count)
{
	throw ELZ4Error(ErrLZ4DeflateInvalid, "Read");
}

ssize_t CdLZ4Encoder::Write(const void *Buffer, ssize_t Count)
{
	if (fHaveClosed)
		throw ELZ4Error(ErrLZ4DeflateClosed);

	ssize_t OldCount = Count;
	C_UInt8 *p = (C_UInt8*)Buffer;
	while (Count > 0)
	{
		ssize_t size = fChunkSize - fChunkUsed;
		if (Count < size) size = Count;
		Count -= size; fTotalIn += size;
		fChunkUsed += size;

		size_t s = LZ4F_compressUpdate(lz4_context, fCompress, fRawChunkSize,
			p, size, NULL);
		if (LZ4F_isError(s))
			throw ELZ4Error((LZ4F_errorCode_t)s);
		if (s > 0)
		{
			fStream->WriteData(fCompress, s);
			fStreamPos += s;
			fChunkUsed = 0;
		}

		p += size;
	}

	fTotalOut = (fStreamPos - fStreamBase);
	return OldCount;
}

SIZE64 CdLZ4Encoder::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	switch (Origin)
	{
		case soBeginning:
			if (Offset == fTotalIn) return fTotalIn;
			break;
		case soCurrent:
			if (Offset == 0) return fTotalIn;
			break;
		case soEnd:
			if (Offset == 0) return fTotalIn;
			break;
	}
	throw ELZ4Error(ErrLZ4DeflateInvalid, "Seek");
}

void CdLZ4Encoder::SetSize(SIZE64 NewSize)
{
	if (NewSize != fTotalIn)
		throw ELZ4Error(ErrLZ4DeflateInvalid, "SetSize");
}

void CdLZ4Encoder::Close()
{
	if (!fHaveClosed)
	{
		if (PtrExtRec)
		{
			if (PtrExtRec->Size > 0)
				WriteData((void*)PtrExtRec->Buf, PtrExtRec->Size);
			PtrExtRec = NULL;
		}

		size_t s = LZ4F_compressEnd(lz4_context, fCompress, fRawChunkSize, NULL);
		if (LZ4F_isError(s))
			throw ELZ4Error((LZ4F_errorCode_t)s);
		fStream->WriteData(fCompress, s);
		fStreamPos += s;
		fTotalOut = (fStreamPos - fStreamBase);

		fHaveClosed = true;
	}
}


// Decompression of LZ4 algorithm

CdLZ4Decoder::CdLZ4Decoder(CdStream &Source): CdBaseLZ4Stream(Source)
{
	lz4_context = NULL;
	LZ4F_errorCode_t err =
		LZ4F_createDecompressionContext(&lz4_context, LZ4F_VERSION);
	if (LZ4F_isError(err)) throw ELZ4Error(err);
	fCurPosition = 0;
	fBufPtr = fBufEnd = fBuffer;
}


CdLZ4Decoder::~CdLZ4Decoder()
{
	if (lz4_context)
	{
		LZ4F_errorCode_t err = LZ4F_freeDecompressionContext(lz4_context);
		lz4_context = NULL;
		if (LZ4F_isError(err)) throw ELZ4Error(err);
	}
}

ssize_t CdLZ4Decoder::Read(void *Buffer, ssize_t Count)
{
	ssize_t OldCount = Count;
	C_UInt8 *p = (C_UInt8*)Buffer;
	while (Count > 0)
	{
		if (fBufPtr >= fBufEnd)
		{
			UpdateStreamPosition();
			ssize_t Cnt = fStream->Read(fBuffer, sizeof(fBuffer));
			fStreamPos += Cnt;
			fBufPtr = fBuffer;
			fBufEnd = fBufPtr + Cnt;
			if (Cnt <= 0) break;
		}

		size_t DstCnt = Count;
		size_t ScrCnt = fBufEnd - fBufPtr;
		size_t s = LZ4F_decompress(lz4_context, p, &DstCnt,
			fBufPtr, &ScrCnt, NULL);
		if (LZ4F_isError(s))
			break;

		fBufPtr += ScrCnt;
		fCurPosition += DstCnt;
		p += DstCnt;
		Count -= DstCnt;
	}

	SIZE64 tmp = fStreamPos - fStreamBase;
	if (tmp > fTotalIn) fTotalIn = tmp;
	if (fCurPosition > fTotalOut) fTotalOut = fCurPosition;

	return (OldCount - Count);
}

ssize_t CdLZ4Decoder::Write(const void *Buffer, ssize_t Count)
{
	throw ELZ4Error(ErrLZ4InflateInvalid, "Write");
}

SIZE64 CdLZ4Decoder::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	if ((Offset==0) && (Origin==soBeginning))
	{
		if (fCurPosition == 0) return 0;
		LZ4F_freeDecompressionContext(lz4_context);
		lz4_context = NULL;
		LZ4F_errorCode_t err =
			LZ4F_createDecompressionContext(&lz4_context, LZ4F_VERSION);
		if (LZ4F_isError(err))
			throw ELZ4Error(err);
		fBufPtr = fBufEnd = fBuffer;
		fStream->SetPosition(fStreamPos = fStreamBase);
		return (fCurPosition = 0);
	} else if (Origin != soEnd)
	{
		if ((Offset==0) && (Origin==soCurrent))
			return fCurPosition;

		if (Origin == soCurrent) Offset += fCurPosition;
		if (Offset < fCurPosition)
			Seek(0, soBeginning);
		else
			Offset -= fCurPosition;

		C_UInt8 buffer[4096];
		SIZE64 DivI = Offset / sizeof(buffer);
		for (; DivI > 0; DivI--)
			ReadData(buffer, sizeof(buffer));
		ReadData(buffer, Offset % sizeof(buffer));
	} else
		throw ELZ4Error(ErrLZ4InflateInvalid, "Seek");

	return fCurPosition;
}

SIZE64 CdLZ4Decoder::GetSize()
{
	return -1;
}

void CdLZ4Decoder::SetSize(SIZE64 NewSize)
{
	throw ELZ4Error(ErrLZ4InflateInvalid, "SetSize");
}


// CdLZ4Encoder_RA

#define LZ4_MAGIC_HEADER_SIZE     7
static const C_UInt8 LZ4_MAGIC_HEADER[LZ4_MAGIC_HEADER_SIZE] =
	{ 'L', 'Z', '4', '_', 'R', 'A', 0x10 };
static const char *ErrLZ4Compressing =
	"Internal error in 'CdLZ4Encoder_RA::Compressing'.";

CdLZ4Encoder_RA::CdLZ4Encoder_RA(CdStream &Dest, TLevel Level, TBlockSize BK):
	CdRA_Write(this, BK), CdBaseLZ4Stream(Dest), CdRecodeLevel(Level)
{
	switch (Level)
	{
	case clFast:
		fLZ4Ptr = malloc(sizeof(LZ4_stream_t));
		memset(fLZ4Ptr, 0, sizeof(LZ4_stream_t));
		break;
	case clDefault: case clMax:
		fLZ4Ptr = LZ4_createStreamHC();
		break;
	default:
		fLZ4Ptr = NULL;
	}

	PtrExtRec = NULL;
	fHaveClosed = false;
	fUnusedRawSize = LZ4RA_RAW_BUFFER_SIZE;
	_IdxRaw = 0;

	fBlockLZ4Size = fCurBlockLZ4Size = RA_BLOCK_SIZE_LIST[BK];
	InitWriteStream();
}

CdLZ4Encoder_RA::~CdLZ4Encoder_RA()
{
	switch (fLevel)
	{
	case clFast:
		free(fLZ4Ptr); break;
	case clDefault: case clMax:
		LZ4_freeStreamHC((LZ4_streamHC_t*)fLZ4Ptr); break;
	default:
		break;
	}
	fLZ4Ptr = NULL;
}

ssize_t CdLZ4Encoder_RA::Read(void *Buffer, ssize_t Count)
{
	throw ELZ4Error(ErrLZ4InflateInvalid, "Read");
}

ssize_t CdLZ4Encoder_RA::Write(const void *Buffer, ssize_t Count)
{
	if (fHaveClosed)
		throw ELZ4Error(ErrLZ4DeflateClosed);
	if (Count <= 0) return 0;

	ssize_t OldCount = Count;
	C_UInt8 *pBuf = (C_UInt8*)Buffer;
	char *pRaw = fRawBuffer[_IdxRaw];

	while (Count > 0)
	{
		if (!fHasInitWriteBlock)
		{
			switch (fLevel)
			{
			case clFast:
				memset(fLZ4Ptr, 0, sizeof(LZ4_stream_t)); break;
			case clDefault:
				LZ4_resetStreamHC((LZ4_streamHC_t*)fLZ4Ptr,
					LZ4DeflateLevel[clDefault]); break;
			case clMax:
				LZ4_resetStreamHC((LZ4_streamHC_t*)fLZ4Ptr,
					LZ4DeflateLevel[clMax]); break;
			default:
				break;
			}
		}
		InitWriteBlock();

		ssize_t L = Count;
		if (L > fUnusedRawSize) L = fUnusedRawSize;
		memcpy(&pRaw[LZ4RA_RAW_BUFFER_SIZE - fUnusedRawSize], pBuf, L);
		pBuf += L;
		Count -= L;
		fTotalIn += L;
		fUnusedRawSize -= L;
		if (fUnusedRawSize <= 0)
		{
			Compressing(LZ4RA_RAW_BUFFER_SIZE);
			pRaw = fRawBuffer[_IdxRaw];
		}
	}

	return OldCount - Count;
}

SIZE64 CdLZ4Encoder_RA::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	switch (Origin)
	{
		case soBeginning:
			if (Offset == fTotalIn) return fTotalIn;
			break;
		case soCurrent:
			if (Offset == 0) return fTotalIn;
			break;
		case soEnd:
			if (Offset == 0) return fTotalIn;
			break;
	}
	throw ELZ4Error(ErrLZ4InflateInvalid, "Seek");
}

void CdLZ4Encoder_RA::SetSize(SIZE64 NewSize)
{
	if (NewSize != fTotalIn)
		throw ELZ4Error(ErrLZ4InflateInvalid, "SetSize");
}

void CdLZ4Encoder_RA::Close()
{
	if (!fHaveClosed)
	{
		if (PtrExtRec)
		{
			if (PtrExtRec->Size > 0)
				WriteData((void*)PtrExtRec->Buf, PtrExtRec->Size);
			PtrExtRec = NULL;
		}
		fCurBlockLZ4Size = 0;
		Compressing(LZ4RA_RAW_BUFFER_SIZE - fUnusedRawSize);
		DoneWriteStream();
		fHaveClosed = true;
	}
}

void CdLZ4Encoder_RA::WriteMagicNumber(CdStream &Stream)
{
	Stream.WriteData(LZ4_MAGIC_HEADER, LZ4_MAGIC_HEADER_SIZE);
	Stream.W8b(fLevel);
}

void CdLZ4Encoder_RA::Compressing(int bufsize)
{
	if (bufsize <= 0) return;

	// the buffer for compressed data
	char LZ4Buffer[LZ4RA_LZ4_BUFFER_SIZE + 2];
	int size, cmpBytes;

	UpdateStreamPosition();

	switch (fLevel)
	{
	case clMin:
		BYTE_LE<CdStream>(fStream) << (C_UInt16)(bufsize);
		fStream->WriteData(fRawBuffer[_IdxRaw], bufsize);
		size = sizeof(C_UInt16) + bufsize;
		break;

	case clFast:
		cmpBytes = LZ4_compress_continue((LZ4_stream_t*)fLZ4Ptr,
			fRawBuffer[_IdxRaw], LZ4Buffer+2, bufsize);
		if (cmpBytes <= 0)
			throw ELZ4Error(ErrLZ4Compressing);
		LZ4Buffer[0] = cmpBytes & 0xFF;
		LZ4Buffer[1] = (cmpBytes >> 8) & 0xFF;
		size = sizeof(C_UInt16) + cmpBytes;
		fStream->WriteData(LZ4Buffer, size);
		break;

	case clDefault: case clMax:
		cmpBytes = LZ4_compressHC_continue((LZ4_streamHC_t*)fLZ4Ptr,
			fRawBuffer[_IdxRaw], LZ4Buffer+2, bufsize);
		if (cmpBytes <= 0)
			throw ELZ4Error(ErrLZ4Compressing);
		LZ4Buffer[0] = cmpBytes & 0xFF;
		LZ4Buffer[1] = (cmpBytes >> 8) & 0xFF;
		size = sizeof(C_UInt16) + cmpBytes;
		fStream->WriteData(LZ4Buffer, size);
		break;

	default:
		throw ELZ4Error(ErrLZ4Compressing);
	}

	fStreamPos += size;
	fTotalOut = fStreamPos - fStreamBase;

	_IdxRaw = 1 - _IdxRaw;
	fUnusedRawSize = LZ4RA_RAW_BUFFER_SIZE;
	fCurBlockLZ4Size -= size;
	if (fCurBlockLZ4Size < 0)
	{
		fCurBlockLZ4Size = fBlockLZ4Size;
		DoneWriteBlock();
	}
}

void CdLZ4Encoder_RA::CopyFrom(CdStream &Source, SIZE64 Pos, SIZE64 Count)
{
	if (dynamic_cast<CdLZ4Decoder_RA*>(&Source))
	{
		CdLZ4Decoder_RA *Src = static_cast<CdLZ4Decoder_RA*>(&Source);
		if ((Src->SizeType() == SizeType()) && (Src->fVersion == fVersion))
		{
			Src->SetPosition(Pos);
			if (Count < 0)
				Count = Src->GetSize() - Pos;

			C_UInt8 Buffer[COREARRAY_STREAM_BUFFER];
			const ssize_t SIZE = sizeof(Buffer);

			// header
			if (Pos > Src->fCB_UZStart)
			{
				SIZE64 L = Src->fCB_UZStart + Src->fCB_UZSize - Pos;
				if (L > Count) L = Count;
				for (; L > 0; )
				{
					ssize_t N = (L <= SIZE) ? L : SIZE;
					Src->ReadData(Buffer, N);
					WriteData((void*)Buffer, N);
					Count -= N; Pos += N;
					L -= N;
				}
			}

			// body
			if (Count > 0)
			{
				Src->SeekStream(Pos);
				if ((Src->fCB_UZStart + Src->fCB_UZSize) <= (Pos + Count))
				{
					if (fHasInitWriteBlock)
					{
						fCurBlockLZ4Size = 0;
						Compressing(LZ4RA_RAW_BUFFER_SIZE - fUnusedRawSize);
						fHasInitWriteBlock = false;
					}
					// determine start and size
					SIZE64 Start = Src->fCB_ZStart;
					SIZE64 ZSize = 0, USize = 0;
					for (; (Src->fCB_UZStart + Src->fCB_UZSize) <= (Pos + Count); )
					{
						ZSize += Src->fCB_ZSize;
						USize += Src->fCB_UZSize;
						AddBlockInfo(Src->fCB_ZSize, Src->fCB_UZSize);
						Count -= Src->fCB_UZSize;
						Pos += Src->fCB_UZSize;
						Src->NextBlock();
					}
					Src->Reset();
					// copying
					fStream->CopyFrom(*Src->fStream, Start, ZSize);
					fTotalIn += USize;
					fStreamPos += ZSize;
					fTotalOut = fStreamPos - fStreamBase;
				}
			}

			// tail
			if (Count > 0) Src->SetPosition(Pos);
			for (; Count > 0; )
			{
				ssize_t N = (Count <= SIZE) ? Count : SIZE;
				Src->ReadData(Buffer, N);
				WriteData((void*)Buffer, N);
				Count -= N;
			}

			return;
		}
	}

	CdStream::CopyFrom(Source, Pos, Count);
}



// CdLZ4Decoder_RA

CdLZ4Decoder_RA::CdLZ4Decoder_RA(CdStream &Source):
	CdRA_Read(this), CdBaseLZ4Stream(Source)
{
	fLevel = clUnknown;
	InitReadStream();
	_IdxRaw = 1;
	fCurPosition = 0;
	memset(&lz4_body, 0, sizeof(lz4_body));
	iRaw = CntRaw = 0;
}

ssize_t CdLZ4Decoder_RA::Read(void *Buffer, ssize_t Count)
{
	if (Count <= 0) return 0;
	if (fBlockIdx >= fBlockNum) return 0;

	C_UInt8 *pBuf = (C_UInt8*)Buffer;
	ssize_t OldCount = Count;
	char *pRaw = fRawBuffer[_IdxRaw];

	while (Count > 0)
	{
		if (CntRaw <= 0)
		{
			UpdateStreamPosition();
			C_UInt16 Len;
			BYTE_LE<CdStream>(fStream) >> Len;

			if (fLevel != clMin)
			{
				char LZ4Buffer[65536];
				fStream->ReadData(LZ4Buffer, Len);
				fStreamPos += sizeof(Len) + Len;

	            _IdxRaw = 1 - _IdxRaw;
				int decBytes = LZ4_decompress_safe_continue(
					&lz4_body, LZ4Buffer, fRawBuffer[_IdxRaw], Len,
					LZ4RA_RAW_BUFFER_SIZE);
	            if(decBytes <= 0)
    	        	break;
	            CntRaw = decBytes;
	            pRaw = fRawBuffer[_IdxRaw];
	        } else {
				fStream->ReadData(fRawBuffer[_IdxRaw], Len);
				fStreamPos += sizeof(Len) + Len;
	            CntRaw = Len;
	        }
            iRaw = 0;
		}

		ssize_t L = Count;
		if (L > (CntRaw - iRaw)) L = CntRaw - iRaw;
		memcpy(pBuf, &pRaw[iRaw], L);
		fCurPosition += L;
		Count -= L;
		pBuf += L;

		iRaw += L;
		if (iRaw >= CntRaw)
		{
			CntRaw = 0;
			SIZE64 b = fCurPosition - fCB_UZStart;
			if (b == fCB_UZSize)
			{
				// go to the next block
				if (NextBlock())
					memset(&lz4_body, 0, sizeof(lz4_body));
				else
					break;
			} else if (b > fCB_UZSize)
				throw ELZ4Error("Invalid LZ4 block for random access");
		}
	}

	SIZE64 tmp = fStreamPos - fStreamBase;
	if (tmp > fTotalIn) fTotalIn = tmp;
	if (fCurPosition > fTotalOut) fTotalOut = fCurPosition;

	return OldCount - Count;
}

ssize_t CdLZ4Decoder_RA::Write(const void *Buffer, ssize_t Count)
{
	throw ELZ4Error(ErrLZ4InflateInvalid, "Write");
}

SIZE64 CdLZ4Decoder_RA::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	if (Origin == soCurrent)
	{
		Offset += fCurPosition;
		if (Offset < 0)
			throw ELZ4Error(ErrLZ4InflateInvalid, "Seek");
	} else if (Origin == soEnd)
		throw ELZ4Error(ErrLZ4InflateInvalid, "Seek");

	bool flag = SeekStream(Offset);
	if (flag || (Offset < fCurPosition))
		Reset();

	Offset -= fCurPosition;
	if (Offset > 0)
	{
		C_UInt8 buffer[4096];
		SIZE64 DivI = Offset / sizeof(buffer);
		for (; DivI > 0; DivI--)
			ReadData(buffer, sizeof(buffer));
		ReadData(buffer, Offset % sizeof(buffer));
	} else if (Offset < 0)
		throw ELZ4Error(ErrLZ4InflateInvalid, "Seek");

	return fCurPosition;
}

SIZE64 CdLZ4Decoder_RA::GetSize()
{
	return -1;
}

void CdLZ4Decoder_RA::SetSize(SIZE64 NewSize)
{
	throw ELZ4Error(ErrLZ4InflateInvalid, "SetSize");
}

bool CdLZ4Decoder_RA::ReadMagicNumber(CdStream &Stream)
{
	C_UInt8 Header[LZ4_MAGIC_HEADER_SIZE];
	Stream.SetPosition(fStreamBase);
	Stream.ReadData(Header, sizeof(Header));
	if (memcmp(Header, LZ4_MAGIC_HEADER, LZ4_MAGIC_HEADER_SIZE) == 0)
	{
		fLevel = (CdRecodeStream::TLevel)((C_Int8)Stream.R8b());
		return true;
	} else
		return false;
}

void CdLZ4Decoder_RA::Reset()
{
	memset(&lz4_body, 0, sizeof(lz4_body));
	iRaw = CntRaw = 0;
	fStreamPos = fCB_ZStart;
	if (fVersion == 0x10)
		fStreamPos += SIZE_RA_BLOCK_HEADER;
	fCurPosition = fCB_UZStart;
}



// =====================================================================
// The classes of xz/lzma stream

static const char *ErrXZDeflateInvalid =
	"Invalid xz deflate Stream operation '%s'!";
static const char *ErrXZInflateInvalid =
	"Invalid xz inflate Stream operation '%s'!";
static const char *ErrXZDeflateClosed =
	"xz deflate stream has been closed.";

static uint32_t XZLevels[4] =
{
	0,  // clMin
	2,  // clFast
	6,  // clDefault
	9 | LZMA_PRESET_EXTREME  // clMax
};

COREARRAY_INLINE static void XZCheck(lzma_ret code)
{
	switch (code)
	{
		case LZMA_OK:
		case LZMA_BUF_ERROR:
		case LZMA_GET_CHECK:
			break;
		case LZMA_UNSUPPORTED_CHECK:
			throw EXZError("LZMA: cannot calculate the integrity check");
		case LZMA_MEM_ERROR:
			throw EXZError("LZMA: cannot allocate memory");
		case LZMA_MEMLIMIT_ERROR:
			throw EXZError("LZMA: memory usage limit was reached");
		case LZMA_FORMAT_ERROR:
			throw EXZError("LZMA: file format not recognized");
		case LZMA_OPTIONS_ERROR:
			throw EXZError("LZMA: invalid or unsupported options");
		case LZMA_DATA_ERROR:
			throw EXZError("LZMA: data is corrupt");
		case LZMA_PROG_ERROR:
			throw EXZError("LZMA: programming error");
		default:
			throw EXZError((int)code);
	}
}


// CdBaseXZStream

CdBaseXZStream::CdBaseXZStream(CdStream &vStream): CdRecodeStream(vStream)
{
	memset((void*)&fXZStream, 0, sizeof(fXZStream));
}


// CdXZEncoder

CdXZEncoder::CdXZEncoder(CdStream &Dest, TLevel Level):
	CdBaseXZStream(Dest), CdRecodeLevel(Level)
{
	PtrExtRec = NULL;
	fHaveClosed = false;
	XZCheck(lzma_easy_encoder(&fXZStream, XZLevels[Level], LZMA_CHECK_CRC32));
}

CdXZEncoder::~CdXZEncoder()
{
	Close();
	lzma_end(&fXZStream);
}

ssize_t CdXZEncoder::Read(void *Buffer, ssize_t Count)
{
	throw EXZError(ErrXZInflateInvalid, "Read");
}

ssize_t CdXZEncoder::Write(const void *Buffer, ssize_t Count)
{
	if (fHaveClosed)
		throw EXZError(ErrXZDeflateClosed);
	if (Count <= 0) return 0;

	C_UInt8 buf[65536];
	fXZStream.next_in = (const C_UInt8 *)Buffer;
	fXZStream.avail_in = Count;
	ssize_t L = fXZStream.avail_in;

	while (fXZStream.avail_in > 0)
	{
		fXZStream.avail_out = sizeof(buf);
		fXZStream.next_out = buf;

		XZCheck(lzma_code(&fXZStream, LZMA_RUN));
		fTotalIn += L - fXZStream.avail_in;
		L = fXZStream.avail_in;

		size_t nout = sizeof(buf) - fXZStream.avail_out;
		if (nout > 0)
		{
			UpdateStreamPosition();
			fStream->WriteData(buf, nout);
			fStreamPos += nout;
			fTotalOut += nout;
		}
    }

	return Count;
}

SIZE64 CdXZEncoder::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	switch (Origin)
	{
		case soBeginning:
			if (Offset == fTotalIn) return fTotalIn;
			break;
		case soCurrent:
			if (Offset == 0) return fTotalIn;
			break;
		case soEnd:
			if (Offset == 0) return fTotalIn;
			break;
	}
	throw EXZError(ErrXZInflateInvalid, "Seek");
}

void CdXZEncoder::SetSize(SIZE64 NewSize)
{
	if (NewSize != fTotalIn)
		throw EXZError(ErrXZDeflateInvalid, "SetSize");
}

void CdXZEncoder::Close()
{
	if (!fHaveClosed)
	{
		if (PtrExtRec)
		{
			if (PtrExtRec->Size > 0)
				WriteData((void*)PtrExtRec->Buf, PtrExtRec->Size);
			PtrExtRec = NULL;
		}
		SyncFinish();
		fHaveClosed = true;
	}
}

void CdXZEncoder::SyncFinish()
{
	C_UInt8 buffer[65536];
	lzma_ret ret = LZMA_OK;

	while (ret != LZMA_STREAM_END)
	{
		fXZStream.avail_out = sizeof(buffer);
		fXZStream.next_out = buffer;

		ret = lzma_code(&fXZStream, LZMA_FINISH);
		if (ret != LZMA_STREAM_END) XZCheck(ret);

		size_t nout = sizeof(buffer) - fXZStream.avail_out;
		if (nout > 0)
		{
			UpdateStreamPosition();
			fStream->WriteData(buffer, nout);
			fStreamPos += nout;
			fTotalOut += nout;
		}
	}
}

ssize_t CdXZEncoder::Pending()
{
	return 0;
}


// CdXZDecoder

#define XZ_DECODER_FLAG    LZMA_TELL_UNSUPPORTED_CHECK

CdXZDecoder::CdXZDecoder(CdStream &Source): CdBaseXZStream(Source)
{
	fXZStream.avail_in = 0;
	fCurPosition = 0;
	XZCheck(lzma_stream_decoder(&fXZStream, UINT64_MAX, XZ_DECODER_FLAG));
}

CdXZDecoder::~CdXZDecoder()
{
	lzma_end(&fXZStream);
}

ssize_t CdXZDecoder::Read(void *Buffer, ssize_t Count)
{
	ssize_t OriCount = Count;
	C_UInt8 *pBuffer = (C_UInt8 *)Buffer;
	lzma_ret ret = LZMA_OK;

	while ((Count > 0) && (ret != LZMA_STREAM_END))
	{
		if (fXZStream.avail_in <= 0)
		{
			UpdateStreamPosition();
			fXZStream.avail_in = fStream->Read(fBuffer, sizeof(fBuffer));
			if (fXZStream.avail_in <= 0)
				return OriCount - Count;
			fStreamPos += fXZStream.avail_in;
			fXZStream.next_in = fBuffer;
		}

		fXZStream.avail_out = Count;
		fXZStream.next_out = pBuffer;
		ret = lzma_code(&fXZStream, LZMA_RUN);
		if (ret != LZMA_STREAM_END)
			XZCheck(ret);

		ssize_t n = Count - fXZStream.avail_out;
		fCurPosition += n;
		pBuffer += n;
		Count -= n;
	}

	if ((ret==LZMA_STREAM_END) && (fXZStream.avail_in>0))
	{
		fStreamPos -= fXZStream.avail_in;
		fStream->SetPosition(fStreamPos);
		fXZStream.avail_in = 0;
	}

	SIZE64 tmp = fStreamPos - fStreamBase;
	if (tmp > fTotalIn) fTotalIn = tmp;
	if (fCurPosition > fTotalOut) fTotalOut = fCurPosition;

	return OriCount - Count;
}

ssize_t CdXZDecoder::Write(const void *Buffer, ssize_t Count)
{
	throw EXZError(ErrXZInflateInvalid, "Write");
}

SIZE64 CdXZDecoder::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	if ((Offset==0) && (Origin==soBeginning))
	{
		if (fCurPosition == 0) return 0;
		lzma_end(&fXZStream);
		XZCheck(lzma_stream_decoder(&fXZStream, UINT64_MAX, XZ_DECODER_FLAG));
		fXZStream.avail_in = 0;
		fStream->SetPosition(fStreamPos = fStreamBase);
		return (fCurPosition = 0);
	} else if (Origin != soEnd)
	{
		if ((Offset==0) && (Origin==soCurrent))
			return fCurPosition;

		if (Origin == soCurrent) Offset += fCurPosition;
		if (Offset < fCurPosition)
			Seek(0, soBeginning);
		else
			Offset -= fCurPosition;

		C_UInt8 buffer[4096];
		SIZE64 DivI = Offset / sizeof(buffer);
		for (; DivI > 0; DivI--)
			ReadData(buffer, sizeof(buffer));
		ReadData(buffer, Offset % sizeof(buffer));
	} else
		throw EXZError(ErrXZInflateInvalid, "Seek");

	return fCurPosition;
}

SIZE64 CdXZDecoder::GetSize()
{
	return -1;
}

void CdXZDecoder::SetSize(SIZE64 NewSize)
{
	throw EXZError(ErrXZInflateInvalid, "SetSize");
}



// =====================================================================
// Input stream for xz/lzma with the support of random access

#define XZ_RA_MAGIC_HEADER_SIZE     6
static const C_UInt8 XZ_RA_MAGIC_HEADER[XZ_RA_MAGIC_HEADER_SIZE] =
	{ 'X', 'Z', '_', 'R', 'A', 0x10 };


CdXZEncoder_RA::CdXZEncoder_RA(CdStream &Dest, TLevel Level,
	TBlockSize B): CdRA_Write(this, B), CdXZEncoder(Dest, Level)
{
	fBlockZIPSize = fCurBlockZIPSize = RA_BLOCK_SIZE_LIST[B];
	InitWriteStream();
}

ssize_t CdXZEncoder_RA::Write(const void *Buffer, ssize_t Count)
{
	if (fHaveClosed)
		throw EXZError(ErrZDeflateClosed);
	if (Count <= 0) return 0;

	C_UInt8 buf[8192];
	ssize_t OldCount = Count;
	C_UInt8 *pBuf = (C_UInt8*)Buffer;

	while (Count > 0)
	{
		InitWriteBlock();

		fXZStream.next_in = pBuf;
		fXZStream.avail_in = Count;
		fXZStream.next_out = buf;
		fXZStream.avail_out = sizeof(buf);

		while (fXZStream.avail_in > 0)
		{
			XZCheck(lzma_code(&fXZStream, LZMA_RUN));
			ssize_t L = Count - fXZStream.avail_in;
			fTotalIn += L;
			pBuf += L;
			Count = fXZStream.avail_in;

			// have something output
			ssize_t n  = sizeof(buf) - fXZStream.avail_out;
			if (n > 0)
			{
				UpdateStreamPosition();
				fStream->WriteData(buf, n);
				fStreamPos += n;
				fXZStream.next_out = buf;
				fXZStream.avail_out = sizeof(buf);
				fCurBlockZIPSize -= n;

				if ((fCurBlockZIPSize - Pending()) <= 0)
				{
					// finish this block
					// 'fZStream.avail_in = 0' in SyncFinishBlock()
					SyncFinishBlock();
				}
			}
		}
	}

	fTotalOut = fStreamPos - fStreamBase;
	return OldCount - Count;
}

void CdXZEncoder_RA::Close()
{
	if (!fHaveClosed)
	{
		if (PtrExtRec)
		{
			if (PtrExtRec->Size > 0)
				WriteData((void*)PtrExtRec->Buf, PtrExtRec->Size);
			PtrExtRec = NULL;
		}
		SyncFinishBlock();
		DoneWriteStream();
		fHaveClosed = true;
	}
}

void CdXZEncoder_RA::WriteMagicNumber(CdStream &Stream)
{
	Stream.WriteData(XZ_RA_MAGIC_HEADER, XZ_RA_MAGIC_HEADER_SIZE);
}

void CdXZEncoder_RA::SyncFinishBlock()
{
	if (fHasInitWriteBlock)
	{
		fXZStream.avail_in = 0;
		SyncFinish();
		DoneWriteBlock();
		fCurBlockZIPSize = fBlockZIPSize;
		lzma_end(&fXZStream);
		XZCheck(lzma_easy_encoder(&fXZStream, XZLevels[fLevel], LZMA_CHECK_CRC32));
	}
}

void CdXZEncoder_RA::CopyFrom(CdStream &Source, SIZE64 Pos, SIZE64 Count)
{
	if (dynamic_cast<CdXZDecoder_RA*>(&Source))
	{
		CdXZDecoder_RA *Src = static_cast<CdXZDecoder_RA*>(&Source);
		if ((Src->SizeType() == SizeType()) && (Src->fVersion == fVersion))
		{
			Src->SetPosition(Pos);
			if (Count < 0)
				Count = Src->GetSize() - Pos;

			C_UInt8 Buffer[COREARRAY_STREAM_BUFFER];
			const ssize_t SIZE = sizeof(Buffer);

			// header
			if (Pos > Src->fCB_UZStart)
			{
				SIZE64 L = Src->fCB_UZStart + Src->fCB_UZSize - Pos;
				if (L > Count) L = Count;
				for (; L > 0; )
				{
					ssize_t N = (L <= SIZE) ? L : SIZE;
					Src->ReadData(Buffer, N);
					WriteData((void*)Buffer, N);
					Count -= N; Pos += N;
					L -= N;
				}
			}

			// body
			if (Count > 0)
			{
				Src->SeekStream(Pos);
				if ((Src->fCB_UZStart + Src->fCB_UZSize) <= (Pos + Count))
				{
					SyncFinishBlock();
					// determine start and size
					SIZE64 Start = Src->fCB_ZStart;
					SIZE64 ZSize = 0, USize = 0;
					for (; (Src->fCB_UZStart + Src->fCB_UZSize) <= (Pos + Count); )
					{
						ZSize += Src->fCB_ZSize;
						USize += Src->fCB_UZSize;
						AddBlockInfo(Src->fCB_ZSize, Src->fCB_UZSize);
						Count -= Src->fCB_UZSize;
						Pos += Src->fCB_UZSize;
						Src->NextBlock();
					}
					Src->Reset();
					// copying
					fStream->CopyFrom(*Src->fStream, Start, ZSize);
					fTotalIn += USize;
					fStreamPos += ZSize;
					fTotalOut = fStreamPos - fStreamBase;
				}
			}

			// tail
			if (Count > 0) Src->SetPosition(Pos);
			for (; Count > 0; )
			{
				ssize_t N = (Count <= SIZE) ? Count : SIZE;
				Src->ReadData(Buffer, N);
				WriteData((void*)Buffer, N);
				Count -= N;
			}

			return;
		}
	}

	CdStream::CopyFrom(Source, Pos, Count);
}


// =====================================================================

CdXZDecoder_RA::CdXZDecoder_RA(CdStream &Source): CdRA_Read(this),
	CdXZDecoder(Source)
{
	InitReadStream();
}

ssize_t CdXZDecoder_RA::Read(void *Buffer, ssize_t Count)
{
	if (Count <= 0) return 0;
	if (fBlockIdx >= fBlockNum) return 0;

	ssize_t OriCount = Count;
	C_UInt8 *pBuffer = (C_UInt8 *)Buffer;

	while (Count > 0)
	{
		lzma_ret ret = LZMA_OK;

		while ((Count > 0) && (ret != LZMA_STREAM_END))
		{
			if (fXZStream.avail_in <= 0)
			{
				UpdateStreamPosition();
				ssize_t n = fCB_ZSize - (fStreamPos - fCB_ZStart);
				if (n > (ssize_t)sizeof(fBuffer)) n = sizeof(fBuffer);
				if (n < 0) n = 0;
				fXZStream.avail_in = fStream->Read(fBuffer, n);
				if (fXZStream.avail_in <= 0)
					return OriCount - Count;
				fStreamPos += fXZStream.avail_in;
				fXZStream.next_in = fBuffer;
			}

			fXZStream.avail_out = Count;
			fXZStream.next_out = pBuffer;
			ret = lzma_code(&fXZStream, LZMA_RUN);
			if (ret != LZMA_STREAM_END)
				XZCheck(ret);

			ssize_t n = Count - fXZStream.avail_out;
			fCurPosition += n;
			pBuffer += n;
			Count -= n;
		}

		if (ret == LZMA_STREAM_END)
		{
			if ((fCurPosition-fCB_UZStart) != fCB_UZSize)
				throw EXZError("Invalid XZ block, inconsistent length.");

			// go to the next block
			if (NextBlock())
			{
				lzma_end(&fXZStream);
				XZCheck(lzma_stream_decoder(&fXZStream, UINT64_MAX, XZ_DECODER_FLAG));
				fXZStream.avail_in = 0;
			} else
				break;
		}
	}

	SIZE64 tmp = fStreamPos - fStreamBase;
	if (tmp > fTotalIn) fTotalIn = tmp;
	if (fCurPosition > fTotalOut) fTotalOut = fCurPosition;

	return OriCount - Count;
}

SIZE64 CdXZDecoder_RA::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	if (Origin == soCurrent)
	{
		Offset += fCurPosition;
		if (Offset < 0)
			throw EXZError(ErrXZInflateInvalid, "Seek");
	} else if (Origin == soEnd)
		throw EXZError(ErrXZInflateInvalid, "Seek");

	bool flag = SeekStream(Offset);
	if (flag || (Offset < fCurPosition))
		Reset();

	Offset -= fCurPosition;
	if (Offset > 0)
	{
		C_UInt8 buffer[4096];
		SIZE64 DivI = Offset / sizeof(buffer);
		for (; DivI > 0; DivI--)
			ReadData(buffer, sizeof(buffer));
		ReadData(buffer, Offset % sizeof(buffer));
	} else if (Offset < 0)
		throw EXZError(ErrXZInflateInvalid, "Seek");

	return fCurPosition;
}

bool CdXZDecoder_RA::ReadMagicNumber(CdStream &Stream)
{
	C_UInt8 Header[XZ_RA_MAGIC_HEADER_SIZE];
	Stream.SetPosition(fStreamBase);
	Stream.ReadData(Header, sizeof(Header));
	return (memcmp(Header, XZ_RA_MAGIC_HEADER, XZ_RA_MAGIC_HEADER_SIZE) == 0);
}

void CdXZDecoder_RA::Reset()
{
	lzma_end(&fXZStream);
	XZCheck(lzma_stream_decoder(&fXZStream, UINT64_MAX, XZ_DECODER_FLAG));
	fXZStream.avail_in = 0;
	fStreamPos = fCB_ZStart;
	if (fVersion == 0x10)
		fStreamPos += SIZE_RA_BLOCK_HEADER;
	fCurPosition = fCB_UZStart;
}




// =====================================================================
// GDS block stream

static const char *ErrBlockInvalidPos =
	"Invalid Position: %lld in CdBlockStream.";
static const char *ErrInvalidBlockLength =
	"Invalid block length in CdBlockCollection!";

// CoreArray GDS Stream position mask
const C_Int64 CoreArray::GDS_STREAM_POS_MASK = 0x7FFFFFFFFFFFLL;
const C_Int64 CoreArray::GDS_STREAM_POS_MASK_HEAD_BIT = 0x800000000000LL;


// CdBlockStream

static void xClearList(CdBlockStream::TBlockInfo *Head)
{
	CdBlockStream::TBlockInfo *p = Head;
	while (p != NULL)
	{
		CdBlockStream::TBlockInfo *q = p;
		p = p->Next;
		delete q;
	}
}

CdBlockStream::TBlockInfo::TBlockInfo()
{
	Next = NULL;
	BlockStart = BlockSize = StreamStart = StreamNext = 0;
	Head = false;
}

SIZE64 CdBlockStream::TBlockInfo::AbsStart() const
{
	return StreamStart - (Head ? (HEAD_SIZE+2*GDS_POS_SIZE) : (2*GDS_POS_SIZE));
}

void CdBlockStream::TBlockInfo::SetSize(CdStream &Stream, SIZE64 _Size)
{
	BlockSize = _Size;
	SIZE64 L = Head ? (HEAD_SIZE + 2*GDS_POS_SIZE) : (2*GDS_POS_SIZE);
	Stream.SetPosition(StreamStart - L);
	BYTE_LE<CdStream>(Stream) <<
		TdGDSPos((_Size+L) | (Head ? GDS_STREAM_POS_MASK_HEAD_BIT : 0));
}

void CdBlockStream::TBlockInfo::SetNext(CdStream &Stream, SIZE64 _Next)
{
	StreamNext = _Next;
	Stream.SetPosition(StreamStart -
		(Head ? (HEAD_SIZE + GDS_POS_SIZE) : GDS_POS_SIZE));
	BYTE_LE<CdStream>(Stream) << TdGDSPos(_Next);
}

void CdBlockStream::TBlockInfo::SetSize2(CdStream &Stream,
	SIZE64 _Size, SIZE64 _Next)
{
	BlockSize = _Size;
	StreamNext = _Next;
	SIZE64 L = Head ? (HEAD_SIZE + 2*GDS_POS_SIZE) : (2*GDS_POS_SIZE);
	Stream.SetPosition(StreamStart - L);
	BYTE_LE<CdStream>(Stream)
		<< TdGDSPos((_Size+L) | (Head ? GDS_STREAM_POS_MASK_HEAD_BIT : 0))
		<< TdGDSPos(_Next);
}


CdBlockStream::CdBlockStream(CdBlockCollection &vCollection):
	CdStream(), fCollection(vCollection)
{
	fID = 0;
	fList = fCurrent = NULL;
	fPosition = fBlockCapacity = 0;
	fBlockSize = 0;
	fNeedSyncSize = false;
	if (vCollection.fStream)
	{
		vCollection.fStream->AddRef();
	}
}

CdBlockStream::~CdBlockStream()
{
	SyncSizeInfo();
	xClearList(fList);
	if (fCollection.fStream)
	{
		fCollection.fStream->Release();
	}
}

ssize_t CdBlockStream::Read(void *Buffer, ssize_t Count)
{
	SIZE64 LastPos = fPosition;
	if ((LastPos+Count) > fBlockSize)
		Count = fBlockSize - LastPos;

	if (fCurrent && (Count>0))
	{
		CdStream *vStream = fCollection.Stream();
		if (!vStream) return 0;

		char *p = (char*)Buffer;
		SIZE64 I, L;
		ssize_t RL;
		do {
			I = fPosition - fCurrent->BlockStart;
			L = fCurrent->BlockSize - I;
			if (Count < L)
			{
				vStream->SetPosition(fCurrent->StreamStart + I);
				RL = vStream->Read((void*)p, Count);
				fPosition += RL;
				break;
			} else {
				if (L > 0)
				{
					vStream->SetPosition(fCurrent->StreamStart + I);
					RL = vStream->Read((void*)p, L);
					Count -= RL; fPosition += RL; p += RL;
					if (RL != L) break;
                }
				fCurrent = fCurrent->Next;
				if ((fCurrent==NULL) || (Count<=0))
					break;
			}
		} while (true);
	}

	return fPosition - LastPos;
}

ssize_t CdBlockStream::Write(const void *Buffer, ssize_t Count)
{
	SIZE64 LastPos = fPosition;

	if (Count > 0)
	{
		SIZE64 L = fPosition + Count;
		if (L > fBlockCapacity)
			fCollection._IncStreamSize(*this, L);

		CdStream *vStream = fCollection.Stream();
		if (!vStream) return 0;

		C_UInt8 *p = (C_UInt8*)Buffer;
		ssize_t RL;
		do {
			SIZE64 I = fPosition - fCurrent->BlockStart;
			L = fCurrent->BlockSize - I;
			if (Count < L)
			{
				vStream->SetPosition(fCurrent->StreamStart + I);
				fPosition += vStream->Write(p, Count);
				break;
			} else {
				if (L > 0)
				{
					vStream->SetPosition(fCurrent->StreamStart + I);
					RL = vStream->Write(p, L);
					Count -= RL; fPosition += RL; p += RL;
					if (RL != L) break;
				}
				fCurrent = fCurrent->Next;
				if ((fCurrent==NULL) || (Count<=0))
					break;
			}
		} while (true);

		if (fPosition > fBlockSize)
		{
			fBlockSize = fPosition;
			fNeedSyncSize = true;
			SyncSizeInfo();
		}
	}
	return fPosition - LastPos;
}

SIZE64 CdBlockStream::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	SIZE64 rv = 0;
	switch (Origin)
	{
		case soBeginning:
			if (fPosition != Offset)
				rv = Offset;
			else
				return Offset;
			break;
		case soCurrent:
			if (Offset != 0)
				rv = fPosition + Offset;
			else
				return fPosition;
			break;
		case soEnd:
			if (Offset != 0)
				rv = fBlockSize + Offset;
			else
				return fBlockSize;
			break;
		default:
			return -1;
	}
	if ((rv < 0) || (rv > fBlockSize))
		throw ErrStream(ErrBlockInvalidPos, rv);
	fCurrent = _FindCur(rv);
	return (fPosition = rv);
}

SIZE64 CdBlockStream::GetSize()
{
	return fBlockSize;
}

SIZE64 CdBlockStream::GetSize() const
{
	return fBlockSize;
}

void CdBlockStream::SetSize(SIZE64 NewSize)
{
	if ((0<=NewSize) && (NewSize!=fBlockSize))
	{
		if (NewSize > fBlockCapacity)
			fCollection._IncStreamSize(*this, NewSize);
		else if (NewSize < fBlockCapacity)
			fCollection._DecStreamSize(*this, NewSize);
		fBlockSize = NewSize;
		if (fPosition > NewSize)
		{
			fPosition = NewSize;
			fCurrent = _FindCur(fPosition);
		}
		fNeedSyncSize = true;
		SyncSizeInfo();
	}
}

void CdBlockStream::SetSizeOnly(SIZE64 NewSize)
{
	if ((0<=NewSize) && (NewSize!=fBlockSize))
	{
		if (NewSize > fBlockCapacity)
		{
			SetSize(NewSize);
		} else {
			fBlockSize = NewSize;
			if (fPosition > NewSize)
			{
				fPosition = NewSize;
				fCurrent = _FindCur(fPosition);
			}
			fNeedSyncSize = true;
			SyncSizeInfo();
		}
	}
}

bool CdBlockStream::ReadOnly() const
{
    return fCollection.fReadOnly;
}

int CdBlockStream::ListCount() const
{
	int rv = 0;
	TBlockInfo *p = fList;
	while (p != NULL) { p = p->Next; rv++; }
	return rv;
}

void CdBlockStream::SyncSizeInfo()
{
	if (fNeedSyncSize)
	{
		if (fList)
		{
			CdStream *s = fCollection.Stream();
			s->SetPosition(fList->StreamStart - GDS_POS_SIZE);
			BYTE_LE<CdStream>(*s) << TdGDSPos(fBlockSize);
        }
    	fNeedSyncSize = false;
	}
}

CdBlockStream::TBlockInfo *CdBlockStream::_FindCur(const SIZE64 Pos)
{
	if (Pos < fBlockCapacity)
	{
		TBlockInfo *p = fCurrent;
		if (p == NULL) p = fList;
		if (p == NULL) return NULL;
		if (Pos < p->BlockStart) p = fList;

		TBlockInfo *n = p;
		while ((p != NULL) && (Pos >= p->BlockStart))
		{
			n = p; p = p->Next;
		}
		return n;
	} else
		return NULL;
}


// =====================================================================
// CdBlockCollection

CdBlockCollection::CdBlockCollection(const SIZE64 vCodeStart)
{
	fStream = NULL;
	fStreamSize = 0;
	fUnuse = NULL;
	vNextID = 1; // start from 1
	fCodeStart = vCodeStart;
	fClassMgr = &dObjManager();
	fReadOnly = false;
}

CdBlockCollection::~CdBlockCollection()
{
	Clear();
}

void CdBlockCollection::_IncStreamSize(CdBlockStream &Block,
	const SIZE64 NewCapacity)
{
	// NewCapacity > fBlockCapacity
	if (Block.fList != NULL)
	{
		CdBlockStream::TBlockInfo *p = Block.fList;
		while (p->Next) p = p->Next;

		SIZE64 L = p->BlockSize + p->StreamStart;

		// to check if it is the last block
		if (L == fStreamSize)
		{
			// immediately increase size of the original stream
			L += NewCapacity - Block.fBlockCapacity;
			fStream->SetSize(L); fStreamSize = L;
			// set size
			p->SetSize(*fStream, NewCapacity - p->BlockStart);
			Block.fBlockCapacity = NewCapacity;
			// check Block.fCurrent
			if (Block.fCurrent == NULL)
			{
				p = Block.fList;
				while (p->Next) p = p->Next;
				Block.fCurrent = p;
			}
		} else if (L < fStreamSize)
		{
			// Need a new block
			CdBlockStream::TBlockInfo *n =
				_NeedBlock(NewCapacity - Block.fBlockCapacity, false);

			n->BlockStart = p->BlockStart + p->BlockSize;
			p->Next = n; n->Next = NULL;
			p->SetNext(*fStream, n->AbsStart());

			Block.fBlockCapacity = n->BlockStart + n->BlockSize;
			if (Block.fCurrent == NULL)
				Block.fCurrent = n;
		} else
			throw ErrStream(ErrInvalidBlockLength);

	} else {
		// Need a new block
		CdBlockStream::TBlockInfo *n =
			_NeedBlock(NewCapacity - Block.fBlockCapacity, true);

		n->BlockStart = 0; n->Next = NULL;
		Block.fBlockCapacity = n->BlockSize;
		Block.fList = Block.fCurrent = n;

		fStream->SetPosition(n->StreamStart -
			CdBlockStream::TBlockInfo::HEAD_SIZE);
		BYTE_LE<CdStream>(fStream) << Block.fID << TdGDSPos(0);
	}
}

void CdBlockCollection::_DecStreamSize(CdBlockStream &Block,
	const SIZE64 NewSize)
{
	// NewSize < fBlockCapacity
	CdBlockStream::TBlockInfo *p, *q;

	p = Block.fList; q = NULL;
	while ((p!=NULL) && (p->BlockStart < NewSize))
	{
		q = p;
		p = p->Next;
	}

	if (p != NULL)
	{
		if (p == Block.fList)
		{
			// skip the header
			q = p;
			p = p->Next;
		}

		// delete the link
		q->Next = NULL;
		q->SetNext(*fStream, 0);

		// delete the unused parts
		while (p != NULL)
		{
			Block.fBlockCapacity -= p->BlockSize;
			p->SetSize2(*fStream, p->BlockSize, 0);
			q = p;
			p = p->Next;
			// insert into fUnuse
			q->Next = fUnuse;
			fUnuse = q;
		}
	}
}

CdBlockStream::TBlockInfo *CdBlockCollection::_NeedBlock(
	SIZE64 Size, bool Head)
{
	if (Head)
		Size += CdBlockStream::TBlockInfo::HEAD_SIZE;

	// First, find a suitable block for Unuse
	CdBlockStream::TBlockInfo *p = fUnuse;
	CdBlockStream::TBlockInfo *rv, *q, *qrv;
	rv = q = qrv = NULL;
	while (p != NULL)
	{
		if (p->BlockSize >= Size)
		{
			if (rv != NULL)
			{
				if (p->BlockSize < rv->BlockSize)
				{
					qrv = q; rv = p;
				}
			} else {
				qrv = q; rv = p;
			}
		}
		q = p; p = p->Next;
	}

	// Secend, no such block
	if (rv == NULL)
	{
		SIZE64 Pos = fStreamSize;
		fStreamSize += 2*GDS_POS_SIZE + Size;
		fStream->SetSize(fStreamSize);

		// Result
		rv = new CdBlockStream::TBlockInfo;
		rv->StreamStart = Pos + 2*GDS_POS_SIZE +
			(Head ? CdBlockStream::TBlockInfo::HEAD_SIZE : 0);
		rv->Head = Head;
		rv->SetSize2(*fStream,
			Size - (Head ? CdBlockStream::TBlockInfo::HEAD_SIZE : 0), 0);

	} else {
		// Remove it from the unused list
		if (qrv)
			qrv->Next = rv->Next;
		else
        	fUnuse = rv->Next;
		rv->Next = NULL;

		// Have such block
		rv->Head = Head;
		if (Head)
		{
			// any unused block with 'Head = false'
			rv->BlockSize -= CdBlockStream::TBlockInfo::HEAD_SIZE;
			rv->StreamStart += CdBlockStream::TBlockInfo::HEAD_SIZE;
		}
		rv->SetSize2(*fStream, rv->BlockSize, 0);
	}

	return rv;
}

CdBlockStream *CdBlockCollection::NewBlockStream()
{
	#ifdef COREARRAY_CODE_DEBUG
	if (!fStream)
		throw ErrStream("CdBlockCollection::fStream = NULL.");
	#endif

	// Need a new ID
	while (HaveID(vNextID)) ++vNextID;

	// New
	CdBlockStream *rv = new CdBlockStream(*this);
	rv->AddRef();
	rv->fID = vNextID; ++vNextID;
	fBlockList.push_back(rv);

	return rv;
}

bool CdBlockCollection::HaveID(TdGDSBlockID id)
{
	vector<CdBlockStream*>::const_iterator it;
	for (it=fBlockList.begin(); it != fBlockList.end(); it++)
		if ((*it)->fID == id)
			return true;
	return false;
}

int CdBlockCollection::NumOfFragment()
{
	int Cnt = 0;

	vector<CdBlockStream*>::const_iterator it;
	for (it=fBlockList.begin(); it != fBlockList.end(); it++)
		Cnt += (*it)->ListCount();

	CdBlockStream::TBlockInfo *p = fUnuse;
	while (p != NULL)
	{
		p = p->Next;
		Cnt ++;
	}

	return Cnt;
}

void CdBlockCollection::LoadStream(CdStream *vStream, bool vReadOnly)
{
	if (fStream)
		throw ErrStream("Call CdBlockCollection::Clear() first.");

	// Assign
	(fStream=vStream)->AddRef();
    fReadOnly = vReadOnly;

	// Start to screen
	CdBlockStream::TBlockInfo *p, *q, *n;

	fStream->SetPosition(fCodeStart);
	fStreamSize = fStream->GetSize();

	p = fUnuse;
	while (fStream->Position() < fStreamSize)
	{
		TdGDSPos sSize, sNext;
		BYTE_LE<CdStream>(fStream) >> sSize >> sNext;
		SIZE64 sPos = fStream->Position() +
			(sSize & GDS_STREAM_POS_MASK) - 2*GDS_POS_SIZE;

		CdBlockStream::TBlockInfo *n = new CdBlockStream::TBlockInfo;
		n->Head = (sSize & GDS_STREAM_POS_MASK_HEAD_BIT) != 0;
		int L = n->Head ? CdBlockStream::TBlockInfo::HEAD_SIZE : 0;
		n->BlockSize = (sSize & GDS_STREAM_POS_MASK) - L - 2*GDS_POS_SIZE;
		n->StreamStart = fStream->Position() + L;
		n->StreamNext = sNext;

		if (p) p->Next = n; else fUnuse = n;
		p = n;

		fStream->SetPosition(sPos);
	}

	// Reorganize Block
	while (fUnuse != NULL)
	{
		// find the head
		p = fUnuse; q = NULL;
		while (p != NULL)
		{
			if (p->Head) break;
			q = p; p = p->Next;
        }

		// find all blocks linked to the head
		if (p != NULL)
		{
        	// delete p
			if (q) q->Next = p->Next; else fUnuse = p->Next;

			// a new block stream
			CdBlockStream *bs = new CdBlockStream(*this);
			bs->AddRef();
			fBlockList.push_back(bs);

			// block list
			fStream->SetPosition(p->StreamStart -
				CdBlockStream::TBlockInfo::HEAD_SIZE);
			BYTE_LE<CdStream>(fStream) >> bs->fID >> bs->fBlockSize;
			bs->fBlockCapacity = p->BlockSize;
			bs->fList = bs->fCurrent = p;
			p->Next = NULL;

			// find a list of blocks linked to the head
			n = fUnuse; q = NULL;
			while ((n != NULL) && (p->StreamNext != 0))
			{
				if (p->StreamNext == n->AbsStart())
				{
					if  (!n->Head)
					{
						// remove 'n'
						if (q) q->Next = n->Next; else fUnuse = n->Next;
						p->Next = n;
						// update stream info
						n->BlockStart = p->BlockStart + p->BlockSize;
						bs->fBlockCapacity += n->BlockSize;
						p = n; p->Next = NULL;
						// restart searching
						n = fUnuse; q = NULL;
					} else {
						throw ErrStream("Internal Error: it should not be a head.");
					}
				} else {
					q = n; n = n->Next;
                }
			}

			// need checking codes
		} else
        	break;
	}
}

void CdBlockCollection::WriteStream(CdStream *vStream)
{
	if (fStream)
		throw ErrStream("Call CdBlockCollection::Clear() first.");
	// Assign
	(fStream=vStream)->AddRef();
    fReadOnly = false;

	fStream->SetSize(fStreamSize=fCodeStart);
}

void CdBlockCollection::Clear()
{
	vector<CdBlockStream*>::iterator it;
	for (it=fBlockList.begin(); it != fBlockList.end(); it++)
	{
		CdBlockStream *p = *it;
		if (p)
		{
		#ifdef COREARRAY_CODE_DEBUG
			if (p->Release() != 0)
				throw ErrStream("CdBlockStream::Release() should return 0 here.");
		#else
			p->Release();
		#endif
		}
	}
	fBlockList.clear();

	if (fStream)
	{
	#ifdef COREARRAY_CODE_DEBUG
		if (fStream->Release() != 0)
			throw ErrStream("CdBlockStream::Release() should return 0 here.");
	#else
		fStream->Release();
	#endif
		fStream = NULL;
	}

	xClearList(fUnuse);
	fUnuse = NULL;
}

void CdBlockCollection::DeleteBlockStream(TdGDSBlockID id)
{
	vector<CdBlockStream*>::iterator it;
	for (it=fBlockList.begin(); it != fBlockList.end(); it++)
	{
		if ((*it)->fID == id)
		{
			CdBlockStream::TBlockInfo *p, *q;
			p = (*it)->fList; q = NULL;
			while (p != NULL)
			{
				if (p->Head)
				{
					p->BlockSize += CdBlockStream::TBlockInfo::HEAD_SIZE;
					p->StreamStart -= CdBlockStream::TBlockInfo::HEAD_SIZE;
					p->Head = false;
				}
				p->SetSize2(*fStream, p->BlockSize, 0);
            	q = p; p = p->Next;
            }
			if (q)
			{
				q->Next = fUnuse;
				fUnuse = (*it)->fList;
				(*it)->fList = NULL;
            }

			(*it)->Release();
			fBlockList.erase(it);
			return;
		}
	}
	throw ErrStream("Invalid block with id: %x", id.Get());
}

CdBlockStream *CdBlockCollection::operator[] (const TdGDSBlockID &id)
{
	vector<CdBlockStream*>::iterator it;
	for (it=fBlockList.begin(); it != fBlockList.end(); it++)
	{
		if ((*it)->fID == id)
			return *it;
	}

	CdBlockStream *rv = new CdBlockStream(*this);
	rv->AddRef();
	rv->fID = id;
	fBlockList.push_back(rv);
	if (vNextID.Get() < id.Get())
		vNextID = id.Get() + 1;

	return rv;
}
