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

#include "dStream.h"
#include <cctype>
#include <limits>


using namespace std;
using namespace CoreArray;

static const char *SFCreateErrorEx = "Can not create file '%s'. %s";
static const char *SFOpenErrorEx = "Can not open file '%s'. %s";

static const char *rsBlockInvalidPos = "Invalid Position: %lld in CdBlockStream.";
static const char *rsInvalidBlockLen = "Invalid length of Block!";


// CoreArray GDS Stream position mask
const C_Int64 CoreArray::GDS_STREAM_POS_MASK =
	(C_Int64(0x7FFF) << 32) | C_Int64(0xFFFFFFFF);  // 0x7FFF,FFFFFFFF
const C_Int64 CoreArray::GDS_STREAM_POS_MASK_HEAD_BIT =
	(C_Int64(0x8000) << 32) | C_Int64(0x00000000);  // 0x8000,00000000


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

// CdFileStream

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
			throw ErrStream(SFCreateErrorEx, AFileName, LastSysErrMsg().c_str());
	} else{
		fHandle = SysOpenFile(AFileName, (TSysOpenMode)(mode-fmOpenRead),
			ShMode[mode]);
		if (fHandle == NullSysHandle)
			throw ErrStream(SFOpenErrorEx, AFileName, LastSysErrMsg().c_str());
	}

	fFileName = AFileName;
	fMode = mode;
}


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

// CdMemoryStream

// TODO
CdMemoryStream::CdMemoryStream(size_t Size): CdStream()
{
//	InitMemAllocator(fAllocator, Size);
	fPosition = 0;
}

ssize_t CdMemoryStream::Read(void *Buffer, ssize_t Count)
{
//	fAllocator.Read(fPosition, Buffer, Count);
	fPosition += Count;
	return Count;
}

ssize_t CdMemoryStream::Write(const void *Buffer, ssize_t Count)
{
//	fAllocator.Write(fPosition, Buffer, Count);
	fPosition += Count;
	return Count;
}

SIZE64 CdMemoryStream::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
/*	switch (Origin)
	{
		case soBeginning:
			fPosition = Offset;
			break;
		case soCurrent:
        	fPosition += Offset;
			break;
		case soEnd:
        	fPosition += fAllocator.Capacity;
			break;
		default:
			return -1;
	}
	if ((fPosition < 0) || (fPosition > fAllocator.Capacity))
		throw ErrStream("Invalid position (%d).", fPosition);
*/	return fPosition;
}

SIZE64 CdMemoryStream::GetSize()
{
	return 0;
//    return fAllocator.Capacity;
}

void CdMemoryStream::SetSize(SIZE64 NewSize)
{
//	fAllocator.SetCapacity(NewSize);
}

void *CdMemoryStream::BufPointer()
{
	return NULL;
// return fAllocator.Base;
}


#ifndef COREARRAY_NO_STD_IN_OUT

// CdStdInStream

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
// =====================================================================

CdTransformStream::CdTransformStream(CdStream &vStream): CdStream()
{
	fStream = &vStream;
	fStream->AddRef();
	fStreamBase = fStreamPos = vStream.Position();
	fTotalIn = fTotalOut = 0;
}

CdTransformStream::~CdTransformStream()
{
	if (fStream)
		fStream->Release();
}



// =====================================================================
// The classes of ZLIB stream
// =====================================================================

static const char *ErrZDeflateInvalid =
	"Invalid Zip Deflate Stream operation '%s'!";
static const char *ErrZInflateInvalid =
	"Invalid Zip Inflate Stream operation '%s'!";
static const char *ErrZDeflateClosed =
	"ZIP deflate stream has been closed.";

static short ZLevels[13] =
{
	Z_NO_COMPRESSION,       // zcNone
	Z_BEST_SPEED,           // zcFastest
	Z_DEFAULT_COMPRESSION,  // zcDefault
	Z_BEST_COMPRESSION,     // zcMax
	1, 2, 3, 4, 5, 6, 7, 8, 9
};

static short ZStrategies[5] =
{
	Z_DEFAULT_STRATEGY,     // zsDefault
	Z_FILTERED,             // zsFiltered
	Z_HUFFMAN_ONLY,         // zsHuffman
	Z_RLE,                  // zsRLE
	Z_FIXED                 // zsFixed
};

COREARRAY_INLINE static int ZCheck(int Code)
{
	if (Code < 0) throw EZLibError(Code);
	return Code;
}


// CdBaseZStream

CdBaseZStream::CdBaseZStream(CdStream &vStream): CdTransformStream(vStream)
{
	memset((void*)&fZStream, 0, sizeof(z_stream));
}

// CdZDeflate

CdZDeflate::CdZDeflate(CdStream &Dest, TZLevel DeflateLevel):
	CdBaseZStream(Dest)
{
	PtrExtRec = NULL;
	fZStream.next_out = fBuffer;
	fZStream.avail_out = sizeof(fBuffer);
	fHaveClosed = false;

	ZCheck( deflateInit_(&fZStream, ZLevels[DeflateLevel],
		ZLIB_VERSION, sizeof(fZStream)) );
}

CdZDeflate::CdZDeflate(CdStream &Dest, TZLevel DeflateLevel,
	int windowBits, int memLevel, TZStrategy Strategy): CdBaseZStream(Dest)
{
	PtrExtRec = NULL;
	fZStream.next_out = fBuffer;
	fZStream.avail_out = sizeof(fBuffer);
	fHaveClosed = false;

	#define Z_DEFLATED 8
	ZCheck( deflateInit2_(&fZStream, ZLevels[DeflateLevel],
		Z_DEFLATED, windowBits, memLevel, ZStrategies[Strategy],
		ZLIB_VERSION, sizeof(fZStream)) );
	#undef Z_DEFLATED
}

CdZDeflate::~CdZDeflate()
{
	Close();
	deflateEnd(&fZStream);
}

ssize_t CdZDeflate::Read(void *Buffer, ssize_t Count)
{
	throw EZLibError(ErrZDeflateInvalid, "Read");
}

ssize_t CdZDeflate::Write(const void *Buffer, ssize_t Count)
{
	if (fHaveClosed)
		throw EZLibError(ErrZDeflateClosed);

	fZStream.next_in = (Bytef*)Buffer;
	fZStream.avail_in = Count;
	if (fStream->Position() != fStreamPos)
		fStream->SetPosition(fStreamPos);

	ssize_t L = fZStream.avail_in;
	while (fZStream.avail_in > 0)
	{
		ZCheck(deflate(&fZStream, Z_NO_FLUSH));
		fTotalIn += L - fZStream.avail_in;
		L = fZStream.avail_in;
		if (fZStream.avail_out <= 0)
		{
			fStream->WriteData(fBuffer, sizeof(fBuffer));
			fStreamPos += sizeof(fBuffer);
			fTotalOut += sizeof(fBuffer);
			fZStream.next_out = fBuffer;
			fZStream.avail_out = sizeof(fBuffer);
		}
	}

	return Count;
}

SIZE64 CdZDeflate::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
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

void CdZDeflate::SetSize(SIZE64 NewSize)
{
	if (NewSize != fTotalIn)
		throw EZLibError(ErrZDeflateInvalid, "SetSize");
}

void CdZDeflate::Close()
{
	if (!fHaveClosed)
	{
		if (PtrExtRec)
		{
			WriteData((void*)PtrExtRec->Buf, PtrExtRec->Size);
			PtrExtRec = NULL;
		}
		SyncFinish();
		fHaveClosed = true;
	}
}

ssize_t CdZDeflate::Pending()
{
	return sizeof(fBuffer) - fZStream.avail_out;
}

void CdZDeflate::SyncFinish()
{
	fZStream.next_in = NULL;
	fZStream.avail_in = 0;

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


// CdZInflate

CdZInflate::CdZInflate(CdStream &Source):
	CdBaseZStream(Source)
{
	fZStream.next_in = fBuffer;
	fZStream.avail_in = 0;
	fCurPosition = 0;
	ZCheck(inflateInit_(&fZStream, ZLIB_VERSION, sizeof(fZStream)));
}

CdZInflate::CdZInflate(CdStream &Source, int windowBits):
	CdBaseZStream(Source)
{
	fZStream.next_in = fBuffer;
	fZStream.avail_in = 0;
	fCurPosition = 0;
	ZCheck(inflateInit2_(&fZStream, windowBits, ZLIB_VERSION, sizeof(fZStream)));
}

CdZInflate::~CdZInflate()
{
	inflateEnd(&fZStream);
}

ssize_t CdZInflate::Read(void *Buffer, ssize_t Count)
{
	fZStream.next_out = (Bytef*)Buffer;
	if (fStream->Position() != fStreamPos)
		fStream->SetPosition(fStreamPos);

	const ssize_t OriCount = Count;
	int ZResult = Z_OK;

	while ((Count>0) && (ZResult!=Z_STREAM_END))
	{
		if (fZStream.avail_in <= 0)
		{
			fZStream.avail_in = fStream->Read(fBuffer, sizeof(fBuffer));
			if (fZStream.avail_in <= 0)
				return OriCount - Count;
			fStreamPos += fZStream.avail_in;
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

ssize_t CdZInflate::Write(const void *Buffer, ssize_t Count)
{
	throw EZLibError(ErrZInflateInvalid, "Write");
}

SIZE64 CdZInflate::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
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

SIZE64 CdZInflate::GetSize()
{
	return -1;
}

void CdZInflate::SetSize(SIZE64 NewSize)
{
	throw EZLibError(ErrZInflateInvalid, "SetSize");
}


// =====================================================================
// Input stream for zlib with the support of random access
// =====================================================================

#define ZRA_MAGIC_HEADER_SIZE    4
static const C_UInt8 ZRA_MAGIC_HEADER[ZRA_MAGIC_HEADER_SIZE] =
	{ 'Z', 'R', 'A', 0x10 };

#define ZRA_SIZE_BLOCK_Z       3
#define ZRA_SIZE_BLOCK_UZ      4
#define ZRA_SIZE_BLOCK_HEADER  (ZRA_SIZE_BLOCK_Z + ZRA_SIZE_BLOCK_UZ)

#define ZRA_WINDOW_BITS_16K    -11
#define ZRA_WINDOW_BITS_32K    -12
#define ZRA_WINDOW_BITS_64K    -13
#define ZRA_WINDOW_BITS_128K   -14
#define ZRA_WINDOW_BITS        -15

static const int ZRA_BK_SIZE[7] =
	{ 16*1024, 32*1024, 64*1024, 128*1024, 256*1024, 512*1024, 1024*1024 };

CdZRA_Deflate::CdZRA_Deflate(CdStream &Dest, TZLevel DeflateLevel,
		TRABlockSize BK):
	CdZDeflate(Dest, DeflateLevel,
		   BK==bs16K  ? ZRA_WINDOW_BITS_16K :
		  (BK==bs32K  ? ZRA_WINDOW_BITS_32K :
		  (BK==bs64K  ? ZRA_WINDOW_BITS_64K :
		  (BK==bs128K ? ZRA_WINDOW_BITS_128K : ZRA_WINDOW_BITS))),
		8, zsDefault)
	// windowBits < 0, indicates generating raw deflate data
	//                   without zlib header or trailer
	// memLevel = 8, the default value
{
	fNumZBlock = 0;
	fInitBlockHeader = false;
	fZBStart = fUBStart = 0;

	if ((BK < bs16K) || (BK > bs1M))
		throw EZLibError("CdZRA_Deflate::CdZRA_Deflate, Invalid block size.");
	fBlockZIPSize = fCurBlockZIPSize = ZRA_BK_SIZE[BK];

	// write the header
	fStream->SetPosition(fStreamBase);
	fStream->WriteData(ZRA_MAGIC_HEADER, sizeof(ZRA_MAGIC_HEADER));
	BYTE_LE<CdStream>(Dest) << C_Int32(-1);
	fStreamPos = fStream->Position();
	fTotalOut = (fStreamPos - fStreamBase);
}

ssize_t CdZRA_Deflate::Write(const void *Buffer, ssize_t Count)
{
	if (fHaveClosed)
		throw EZLibError(ErrZDeflateClosed);

	if (Count <= 0) return 0;
	if (fStream->Position() != fStreamPos)
		fStream->SetPosition(fStreamPos);

	ssize_t OldCount = Count;
	C_UInt8 *pBuf = (C_UInt8*)Buffer;

	while (Count > 0)
	{
		if (!fInitBlockHeader)
		{
			fZBStart = fStreamPos;
			fUBStart = fTotalIn;
			static const C_UInt64 ZRA_BLOCK_ZERO = 0;
			fStream->WriteData(&ZRA_BLOCK_ZERO, ZRA_SIZE_BLOCK_HEADER);
			fStreamPos += ZRA_SIZE_BLOCK_HEADER;
			fInitBlockHeader = true;
		}

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
				fStream->WriteData(fBuffer, sizeof(fBuffer));
				fStreamPos += sizeof(fBuffer);
				fZStream.next_out = fBuffer;
				fZStream.avail_out = sizeof(fBuffer);
				fCurBlockZIPSize -= sizeof(fBuffer);

				unsigned pending = 0;
				int bits = 0;
				ZCheck(deflatePending(&fZStream, &pending, &bits));
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

void CdZRA_Deflate::Close()
{
	if (!fHaveClosed)
	{
		if (PtrExtRec)
		{
			WriteData((void*)PtrExtRec->Buf, PtrExtRec->Size);
			PtrExtRec = NULL;
		}
		SyncFinishBlock();

		// write down the number of block
		fStream->SetPosition(fStreamBase + sizeof(ZRA_MAGIC_HEADER));
		BYTE_LE<CdStream>(fStream) << C_Int32(fNumZBlock);
		fStream->SetPosition(fStreamPos);

		fHaveClosed = true;
	}
}

void CdZRA_Deflate::SyncFinishBlock()
{
	if (fInitBlockHeader)
	{
		SyncFinish();

		C_UInt32 SC = fStreamPos - fZBStart;
		C_UInt32 SU = fTotalIn - fUBStart;

		C_UInt8 SZ[ZRA_SIZE_BLOCK_HEADER] = {
			C_UInt8(SC & 0xFF),
			C_UInt8((SC >> 8) & 0xFF),
			C_UInt8((SC >> 16) & 0xFF),
			C_UInt8(SU & 0xFF),
			C_UInt8((SU >> 8) & 0xFF),
			C_UInt8((SU >> 16) & 0xFF),
			C_UInt8((SU >> 24) & 0xFF)
		};
		fStream->SetPosition(fZBStart);
		fStream->WriteData(SZ, ZRA_SIZE_BLOCK_HEADER);
		fStream->SetPosition(fStreamPos);

		fZStream.next_out = fBuffer;
		fZStream.avail_out = sizeof(fBuffer);
		ZCheck(deflateReset(&fZStream));

		fCurBlockZIPSize = fBlockZIPSize;
		fNumZBlock ++;
		fInitBlockHeader = false;
	}
}


// =====================================================================
// Output stream for zlib with the support of random access
// =====================================================================

static const char *ErrZInflateHeader =
	"Invalid header for ZIP stream with random access.";

CdZRA_Inflate::CdZRA_Inflate(CdStream &Source):
	CdZInflate(Source, ZRA_WINDOW_BITS)
{
	C_UInt8 Header[ZRA_MAGIC_HEADER_SIZE];
	fStream->SetPosition(fStreamBase);
	fStream->ReadData(Header, sizeof(Header));
	if (memcmp(Header, ZRA_MAGIC_HEADER, ZRA_MAGIC_HEADER_SIZE) != 0)
		throw EZLibError(ErrZInflateHeader);

	BYTE_LE<CdStream>(Source) >> fNumZBlock;
	fStreamPos = fStream->Position();

	// whether need to identify the number of blocks
	if (fNumZBlock < 0)
	{
		if (fNumZBlock != -1)
			throw EZLibError(ErrZInflateHeader);
		// TODO
		throw EZLibError("It will be implemented in future!");
	}

	// the first compressed block
	fIdxZBlock = 0;
	fCB_ZStart = fStreamBase + sizeof(ZRA_MAGIC_HEADER) + sizeof(C_Int32);
	fCB_UZStart = 0;
	_ReadBHeader(fCB_ZSize, fCB_UZSize);
}

ssize_t CdZRA_Inflate::Read(void *Buffer, ssize_t Count)
{
	if (Count <= 0) return 0;
	if (fIdxZBlock >= fNumZBlock) return 0;

	C_UInt8 *pBuf = (C_UInt8*)Buffer;
	ssize_t OldCount = Count;
	if (fStream->Position() != fStreamPos)
		fStream->SetPosition(fStreamPos);

	while (Count > 0)
	{
		fZStream.next_out = (Bytef*)pBuf;
		int ZResult = Z_OK;

		while ((Count>0) && (ZResult!=Z_STREAM_END))
		{
			if (fZStream.avail_in <= 0)
			{
				ssize_t s = fCB_ZStart + fCB_ZSize - fStreamPos;
				if (s > (ssize_t)sizeof(fBuffer)) s = sizeof(fBuffer);
			
				fZStream.avail_in = fStream->Read(fBuffer, s);
				if (fZStream.avail_in <= 0)
					return OldCount - Count;

				fStreamPos += fZStream.avail_in;
				fZStream.next_in = fBuffer;
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
			if (fCurPosition-fCB_UZStart != fCB_UZSize)
				throw EZLibError("Invalid ZIP block for random access");

			// go to the next block
			fCB_ZStart += fCB_ZSize;
			fCB_UZStart += fCB_UZSize;
			fIdxZBlock ++;
			if (fIdxZBlock < fNumZBlock)
			{
				_ReadBHeader(fCB_ZSize, fCB_UZSize);
				fZStream.next_in = fBuffer;
				fZStream.avail_in = 0;
				ZCheck(inflateReset(&fZStream));
			} else {
				fCB_ZSize = fCB_UZSize = 1; // avoid ZERO
				break;
			}
		}
	}

	SIZE64 tmp = fStreamPos - fStreamBase;
	if (tmp > fTotalIn) fTotalIn = tmp;
	if (fCurPosition > fTotalOut) fTotalOut = fCurPosition;

	return OldCount - Count;
}

SIZE64 CdZRA_Inflate::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
{
	if (Origin == soCurrent)
	{
		Offset += fCurPosition;
		if (Offset < 0)
			throw EZLibError(ErrZInflateInvalid, "Seek");
	} else if (Origin == soEnd)
		throw EZLibError(ErrZInflateInvalid, "Seek");

	if (Offset < fCurPosition)
	{
		if (Offset < fCB_UZStart)
		{
			// the first compressed block
			fIdxZBlock = 0;
			fCB_ZStart = fStreamBase + sizeof(ZRA_MAGIC_HEADER) + sizeof(C_Int32);
			fCB_UZStart = 0;
			for (;;)
			{
				_ReadBHeader(fCB_ZSize, fCB_UZSize);
				if (fCB_UZStart+fCB_UZSize <= Offset)
				{
					// go to the next block
					fIdxZBlock ++;
					if (fIdxZBlock >= fNumZBlock)
						throw EZLibError(ErrZInflateInvalid, "Seek");
					fCB_ZStart += fCB_ZSize;
					fCB_UZStart += fCB_UZSize;
					_ReadBHeader(fCB_ZSize, fCB_UZSize);			
				} else
					break;
			}
		}

		fZStream.next_in = fBuffer;
		fZStream.avail_in = 0;
		ZCheck(inflateReset(&fZStream));
		fStreamPos = fCB_ZStart + ZRA_SIZE_BLOCK_HEADER;
		fCurPosition = fCB_UZStart;
	} else {
		if (Offset >= fCB_UZStart+fCB_UZSize)
		{
			do {
				// go to the next block
				fIdxZBlock ++;
				if (fIdxZBlock >= fNumZBlock)
					throw EZLibError(ErrZInflateInvalid, "Seek");
				fCB_ZStart += fCB_ZSize;
				fCB_UZStart += fCB_UZSize;
				_ReadBHeader(fCB_ZSize, fCB_UZSize);
			} while (fCB_UZStart+fCB_UZSize <= Offset);

			fZStream.next_in = fBuffer;
			fZStream.avail_in = 0;
			ZCheck(inflateReset(&fZStream));
			fStreamPos = fCB_ZStart + ZRA_SIZE_BLOCK_HEADER;
			fCurPosition = fCB_UZStart;
		}
	}
		
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

void CdZRA_Inflate::_ReadBHeader(SIZE64 &ZSize, SIZE64 &UZSize)
{
	C_UInt8 BSZ[ZRA_SIZE_BLOCK_HEADER];
	fStream->SetPosition(fCB_ZStart);
	fStream->ReadData(BSZ, ZRA_SIZE_BLOCK_HEADER);
	fStreamPos = fCB_ZStart + ZRA_SIZE_BLOCK_HEADER;

	ZSize = BSZ[0] | (C_UInt32(BSZ[1]) << 8) | (C_UInt32(BSZ[2]) << 16);
	UZSize = BSZ[ZRA_SIZE_BLOCK_Z + 0] |
		(C_UInt32(BSZ[ZRA_SIZE_BLOCK_Z + 1]) << 8) |
		(C_UInt32(BSZ[ZRA_SIZE_BLOCK_Z + 2]) << 16) |
		(C_UInt32(BSZ[ZRA_SIZE_BLOCK_Z + 3]) << 24);
}


// EZLibError

EZLibError::EZLibError(int Code): ErrStream()
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
static const blockSizeID_t LZ4FrameInfoBlockSize[4] =
	{ max64KB, max256KB, max1MB, max4MB };

static const char *ErrLZ4DeflateInvalid =
	"Invalid LZ4 Deflate Stream operation '%s'!";
static const char *ErrLZ4InflateInvalid =
	"Invalid LZ4 Inflate Stream operation '%s'!";
static const char *ErrLZ4DeflateClosed =
	"LZ4 deflate stream has been closed.";

CdBaseLZ4Stream::CdBaseLZ4Stream(CdStream &vStream):
	CdTransformStream(vStream)
{
	fUncompress = fCompress = NULL;
	fChunk = lzDefSize;
	fChunkSize = LZ4ChunkSize[fChunk];
	fRawChunkSize = 0;
}

CdBaseLZ4Stream::~CdBaseLZ4Stream()
{
	if (fUncompress) delete[] fUncompress;
	if (fCompress) delete[] fCompress;
}


// Compression of LZ4 algorithm

CdLZ4Deflate::CdLZ4Deflate(CdStream &Dest, TLZ4Chunk chunk):
	CdBaseLZ4Stream(Dest)
{
	if ((chunk < lz64KB) || (chunk > lz4MB))
		throw ELZ4Error("CdLZ4Deflate::CdLZ4Deflate, invalid chunk.");
	fChunk = chunk;

	memset((void*)&lz4_pref, 0, sizeof(lz4_pref));
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

CdLZ4Deflate::~CdLZ4Deflate()
{
	Close();
	if (lz4_context)
	{
		LZ4F_errorCode_t err = LZ4F_freeCompressionContext(lz4_context);
		lz4_context = NULL;
		if (LZ4F_isError(err)) throw ELZ4Error(err);
	}
}

ssize_t CdLZ4Deflate::Read(void *Buffer, ssize_t Count)
{
	throw ELZ4Error(ErrLZ4DeflateInvalid, "Read");
}

ssize_t CdLZ4Deflate::Write(const void *Buffer, ssize_t Count)
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

SIZE64 CdLZ4Deflate::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
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

void CdLZ4Deflate::SetSize(SIZE64 NewSize)
{
	if (NewSize != fTotalIn)
		throw ELZ4Error(ErrLZ4DeflateInvalid, "SetSize");
}

void CdLZ4Deflate::Close()
{
	if (!fHaveClosed)
	{
		if (PtrExtRec)
		{
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

CdLZ4Inflate::CdLZ4Inflate(CdStream &Source): CdBaseLZ4Stream(Source)
{
	lz4_context = NULL;
	LZ4F_errorCode_t err =
		LZ4F_createDecompressionContext(&lz4_context, LZ4F_VERSION);
	if (LZ4F_isError(err)) throw ELZ4Error(err);
	fCurPosition = 0;
	fBufPtr = fBufEnd = fBuffer;
}


CdLZ4Inflate::~CdLZ4Inflate()
{
	if (lz4_context)
	{
		LZ4F_errorCode_t err = LZ4F_freeDecompressionContext(lz4_context);
		lz4_context = NULL;
		if (LZ4F_isError(err)) throw ELZ4Error(err);
	}
}

ssize_t CdLZ4Inflate::Read(void *Buffer, ssize_t Count)
{
	ssize_t OldCount = Count;
	C_UInt8 *p = (C_UInt8*)Buffer;
	while (Count > 0)
	{
		if (fBufPtr >= fBufEnd)
		{
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
			throw ELZ4Error((LZ4F_errorCode_t)s);

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

ssize_t CdLZ4Inflate::Write(const void *Buffer, ssize_t Count)
{
	throw EZLibError(ErrLZ4InflateInvalid, "Write");
}

SIZE64 CdLZ4Inflate::Seek(SIZE64 Offset, TdSysSeekOrg Origin)
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
		throw EZLibError(ErrZInflateInvalid, "Seek");

	return fCurPosition;
}

SIZE64 CdLZ4Inflate::GetSize()
{
	return -1;
}

void CdLZ4Inflate::SetSize(SIZE64 NewSize)
{
	throw EZLibError(ErrLZ4InflateInvalid, "SetSize");
}



// =====================================================================
// GDS block stream
// =====================================================================

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

SIZE64 CdBlockStream::TBlockInfo::AbsStart()
{
	return StreamStart - (Head ? (HeadSize+2*GDS_POS_SIZE) : (2*GDS_POS_SIZE));
}

void CdBlockStream::TBlockInfo::SetSize(CdStream &Stream, SIZE64 _Size)
{
	BlockSize = _Size;
	SIZE64 L = Head ? (HeadSize + 2*GDS_POS_SIZE) : (2*GDS_POS_SIZE);
	Stream.SetPosition(StreamStart - L);
	BYTE_LE<CdStream>(Stream) <<
		TdGDSPos((_Size+L) | (Head ? GDS_STREAM_POS_MASK_HEAD_BIT : 0));
}

void CdBlockStream::TBlockInfo::SetNext(CdStream &Stream, SIZE64 _Next)
{
	StreamNext = _Next;
	Stream.SetPosition(StreamStart -
		(Head ? (HeadSize+GDS_POS_SIZE) : GDS_POS_SIZE));
	BYTE_LE<CdStream>(Stream) << TdGDSPos(_Next);
}

void CdBlockStream::TBlockInfo::SetSize2(CdStream &Stream,
	SIZE64 _Size, SIZE64 _Next)
{
	BlockSize = _Size;
	StreamNext = _Next;
	SIZE64 L = Head ? (HeadSize + 2*GDS_POS_SIZE) : (2*GDS_POS_SIZE);
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
		throw ErrStream(rsBlockInvalidPos, rv);
	fCurrent = _FindCur(rv);
	return (fPosition = rv);
}

SIZE64 CdBlockStream::GetSize()
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
			throw ErrStream(rsInvalidBlockLen);

	} else {
		// Need a new block
		CdBlockStream::TBlockInfo *n =
			_NeedBlock(NewCapacity - Block.fBlockCapacity, true);

		n->BlockStart = 0; n->Next = NULL;
		Block.fBlockCapacity = n->BlockSize;
		Block.fList = Block.fCurrent = n;

		fStream->SetPosition(n->StreamStart -
			CdBlockStream::TBlockInfo::HeadSize);
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
		Size += CdBlockStream::TBlockInfo::HeadSize;

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
			(Head ? CdBlockStream::TBlockInfo::HeadSize : 0);
		rv->Head = Head;
		rv->SetSize2(*fStream,
			Size - (Head ? CdBlockStream::TBlockInfo::HeadSize : 0), 0);

	} else {
		// Have such block
		rv->Head = Head;
		if (Head)
		{
			rv->BlockSize -= CdBlockStream::TBlockInfo::HeadSize;
			rv->StreamStart += CdBlockStream::TBlockInfo::HeadSize;
		}
		rv->SetSize2(*fStream, rv->BlockSize, 0);

		// Delete it
		if (qrv)
			qrv->Next = rv->Next;
		else
        	fUnuse = rv->Next;

		rv->Next = NULL;
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
		int L = n->Head ? CdBlockStream::TBlockInfo::HeadSize : 0;
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
		// Find the Head
		p = fUnuse; q = NULL;
		while (p != NULL) {
			if (p->Head) break;
			q = p; p = p->Next;
        }

		if (p != NULL)
		{
        	// Delete p
			if (q) q->Next = p->Next; else fUnuse = p->Next;
			//
			CdBlockStream *bs = new CdBlockStream(*this);
			bs->AddRef();
			fBlockList.push_back(bs);
			//
			fStream->SetPosition(p->StreamStart -
				CdBlockStream::TBlockInfo::HeadSize);
			BYTE_LE<CdStream>(fStream) >> bs->fID >> bs->fBlockSize;
			bs->fBlockCapacity = p->BlockSize;
			bs->fList = bs->fCurrent = p;
			p->Next = NULL;

			// Find a list
			n = fUnuse; q = NULL;
			while ((n != NULL) && (p->StreamNext != 0))
			{
				if ((p->StreamNext == n->AbsStart()) && (!n->Head))
				{
					if (q) q->Next = n->Next; else fUnuse = n->Next;
					p->Next = n;
					n->BlockStart = p->BlockStart + p->BlockSize;
					bs->fBlockCapacity += n->BlockSize;
					p = n; n = n->Next;
					p->Next = NULL;
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
					p->BlockSize += CdBlockStream::TBlockInfo::HeadSize;
					p->StreamStart -= CdBlockStream::TBlockInfo::HeadSize;
					p->Head = false;
				}
				p->SetSize2(*fStream, p->BlockSize, 0);
            	q = p; p = p->Next;
            }
			if (q) {
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
